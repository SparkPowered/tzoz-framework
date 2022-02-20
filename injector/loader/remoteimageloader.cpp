#include "remoteimageloader.h"

#include "win\ntremotemodule.h"

namespace ldr
{
	// custom entry point for remote thread
	static BOOL APIENTRY DllMainStub (dllmain_t *p)
	{
		return p->fn(p->dll, p->reason, p->reserved);
	}

	static void DllMainStubEnd()
	{
		// ghetto method for getting the correct size of our stub
	}

	void remoteimageloader::reset()
	{
		m_dllBin.reset();
		m_remoteDllBin.reset();
		m_remoteParam.reset();

		m_hdr = nullptr;
		m_dllSize = 0;

		imageloader::reset();
	}

	bool remoteimageloader::map()
	{
		staticstring<MAX_PATH> dll(m_appDir);
		return remoteLoadLibrary(dll.append(m_imageName));
	}

	void remoteimageloader::shutdown()
	{
		if (m_dllBin != nullptr)
		{
			imports->free(m_dllBin);
			m_dllBin.reset();
		}

		imageloader::shutdown();
	}
	
	bool remoteimageloader::create (string dll)
	{
		ulong fileSize, readSize;

		// load the dll into memory
		HANDLE file = imports->createFile(
			dll,
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);

		if (file == INVALID_HANDLE_VALUE)
		{
			PRINT("CreateFileA() failed\n");
			return false;
		}

		// get the appropriate size
		if (imports->fileAttributes(dll) & FILE_ATTRIBUTE_COMPRESSED)
			fileSize = imports->compressedFileSize(dll, nullptr);
		else
			fileSize = syscall->getFileSize(file, nullptr);

		// allocate a temporary buffer for the dll and copy the contents
		if (!imports->readFile(file, m_dllBin = new byte[fileSize], fileSize, &readSize, nullptr))
		{
			PRINT("ReadFile() failed\n");
			return false;
		}

		PRINT("\tfile = 0x%x\n"
			"\tfileSize = 0x%x\n"			
			"\treadSize = 0x%x\n"
			"\tm_dllBin = 0x%x\n", file, fileSize, readSize, m_dllBin.ptr());
		
		syscall->closeHandle(file);

		// we definitely need the PE header... lol
		if ((m_hdr = ntHeader(m_dllBin)) == nullptr)
		{
			PRINT("ntHeader() failed\n");
			return false;
		}
		
		// aligned size of the dll file
		m_dllSize = calculateDllSize(m_hdr);
		
		// try to reserve the default image base so we can avoid repairing the base relocations
		m_remoteDllBin = m_process.alloc(hdrImageBase(), m_dllSize, k_allocType, PAGE_EXECUTE_READWRITE);

		if (m_remoteDllBin == nullptr)
		{
			// oh well...
			m_remoteDllBin = m_process.alloc(nullptr, m_dllSize, k_allocType, PAGE_EXECUTE_READWRITE);

			if (m_remoteDllBin == nullptr)
			{
				PRINT("NtAllocateVirtualMemory() failed for m_remoteDllBin\n");
				return false;
			}
		}
		
		// our dll won't be linked in the PEB list, nor will it have a PE header
		// we will pass the current directory to our DllMain
		m_remoteParam = m_process.alloc(nullptr, MAX_PATH, k_allocType, PAGE_READWRITE);

		if (m_remoteParam == nullptr)
		{
			PRINT("NtAllocateVirtualMemory() failed for m_remoteParam\n");
			return false;
		}

		// write the path
		if (!m_process.write(m_remoteParam, m_appDir.str(), MAX_PATH))
		{
			PRINT("NtWriteVirtualMemory() failed for m_remoteParam\n");
			return false;
		}

		PRINT("\tm_hdr = 0x%x\n"
			"\tm_dllSize = 0x%x\n"
			"\tm_remoteDllBin = 0x%x\n"
			"\tm_remoteParam = 0x%x\n\n", m_hdr, m_dllSize, m_remoteDllBin.ptr(), m_remoteParam.ptr());

		return true;
	}

	bool remoteimageloader::remoteLoadLibrary (string dll)
	{
		// setup our image
		if (!create(dll))
		{
			PRINT("failed to create image\n");
			return false;
		}

		PRINT("image created\n\n");
		
		// if this is zero then our relocs are correct
		size_t delta = m_remoteDllBin - m_hdr->OptionalHeader.ImageBase;

		PRINT("\tdelta = 0x%x\n\n", delta);

		if (delta && !(m_hdr->FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED))
		{
			if (!relocate(delta))
			{
				PRINT("failed to repair base relocations\n");
				return false;
			}

			PRINT("base relocations repaired\n");
		}
		
		if (!resolveImports())
		{
			PRINT("failed to resolve imports\n");
			return false;
		}

		PRINT("imports resolved\n");
				
		if (!mapSections())
		{
			PRINT("failed to map sections\n");
			return false;
		}

		PRINT("sections mapped\n");

		if (!resolveTLS())
		{
			PRINT("failed to run TLS callbacks\n");
			return false;
		}

		PRINT("TLS resolved\n");
		
		return remoteDllMainCall(
			m_remoteDllBin.get<ulong>(m_hdr->OptionalHeader.AddressOfEntryPoint),
			m_remoteDllBin.as<HMODULE>(),
			DLL_PROCESS_ATTACH,
			m_remoteParam);
	}

	bool remoteimageloader::remoteDllMainCall (address start, HMODULE module, DWORD reason, LPVOID reserved)
	{
		bool result = false;
		
		dllmain_t dllMain;
		
		// allocate memory for stub params
		address param = m_process.alloc(nullptr, sizeof dllMain, k_allocType, PAGE_READWRITE);

		if (param != nullptr)
		{
			// set the stub params
			dllMain.fn = start.as<BOOL (APIENTRY*)(HMODULE, DWORD, LPVOID)>();
			dllMain.dll = module;
			dllMain.reason = reason;
			dllMain.reserved = reserved;

			// write the params
			if (m_process.write(param, &dllMain, sizeof dllMain))
			{
				// get the stub size
				size_t stubSize = (size_t)((byte*)&DllMainStubEnd - (byte*)&DllMainStub);

				// allocate memory for the stub
				address stub = m_process.alloc(nullptr, stubSize, k_allocType, PAGE_READWRITE);
				
				if (stub != nullptr)
				{
					// write the stub
					if (m_process.write(stub, &DllMainStub, stubSize))
					{
						// "Applications should call FlushInstructionCache if they generate or modify code in memory.
						// The CPU cannot detect the change, and may execute the old code it cached."
						if (m_process.flush(stub, stubSize))
						{
							PRINT("\n\tstart = 0x%x\n"
								"\tparam = 0x%x\n"
								"\tstub = 0x%x\n"
								"\tstubSize = 0x%x\n\n", start.ptr(), param.ptr(), stub.ptr(), stubSize);

							// inject!
							result = m_process.executeRemoteThread(stub, param);
						}
					}

					m_process.free(stub, stubSize, MEM_DECOMMIT);
				}
			}

			m_process.free(param, sizeof dllMain, MEM_DECOMMIT);
		}

		return result;
	}

	bool remoteimageloader::relocate (size_t delta)
	{
		address base;

		ulong numrelocs;

		word *data, offset;

		auto dir = dataDirectory(IMAGE_DIRECTORY_ENTRY_BASERELOC);

		// any relocs?
		if (dir->Size)
		{
			// invalid
			if (!dir->VirtualAddress)
				return false;
		}
		else
		{
			return true;
		}

		// rebase image
		m_hdr->OptionalHeader.ImageBase = m_remoteDllBin;

		// start of base relocations
		auto reloc = dllptr<PIMAGE_BASE_RELOCATION>(dir->VirtualAddress);
		
		// complete the entire directory
		for (ulong bytes = 0; bytes < dir->Size;)
		{
			base = dllptr<void*>(reloc->VirtualAddress);
			data = (word*)((byte*)reloc + sizeof IMAGE_BASE_RELOCATION);

			numrelocs = reloc_count(reloc);
			
			for (ulong i = 0; i < numrelocs; ++i, ++data)
			{
				offset = IMR_RELOFFSET(*data);

				switch(IMR_RELTYPE(*data)) {
				case IMAGE_REL_BASED_HIGH:
					*base.get<word>(offset).as<word*>() += HIWORD(delta);
					break;
				case IMAGE_REL_BASED_LOW:
					*base.get<word>(offset).as<word*>() += LOWORD(delta);
					break;
				case IMAGE_REL_BASED_HIGHLOW:
					*base.get<word>(offset).as<ulong*>() += delta;
					break;
				case IMAGE_REL_BASED_DIR64:
					*base.get<word>(offset).as<qword*>() += delta;
					break;
				default:
					break;
				}
			}

			// continue to the next block
			bytes += reloc->SizeOfBlock;
			reloc = (PIMAGE_BASE_RELOCATION)data;
		}

		return true;
	}

	bool remoteimageloader::resolveImports()
	{
		hash_t h;

		ntremotemodule module;

		uintptr_t *thunk, *func, f;

		staticstring<MAX_PATH> moduleName;

		auto dir = dataDirectory(IMAGE_DIRECTORY_ENTRY_IMPORT);

		if (dir->Size)
		{
			if (!dir->VirtualAddress)
				return false;
		}
		else
		{
			return true;
		}

		auto desc = dllptr<PIMAGE_IMPORT_DESCRIPTOR>(dir->VirtualAddress);

		for (; syscall->canReadPointer(desc) && desc->Name; desc++)
		{
			moduleName = dllptr<char*>(desc->Name);		
			h.getstr(moduleName.lower());
						
			// is this dependency loaded already?
			if (!module.init(m_process, h))
			{
				// let's load it ourselves
				if (m_process.createRemoteLoadLibraryThread(moduleName.str()))
				{
					// give up if this fails
					if (!module.init(m_process, h))
					{
						PRINT("failed to initialize remote module %s\n", moduleName.str());
						return false;
					}
				}
			}

			if (desc->OriginalFirstThunk)
				thunk = dllptr<uintptr_t*>(desc->OriginalFirstThunk);
			else
				thunk = dllptr<uintptr_t*>(desc->FirstThunk);

			func = dllptr<uintptr_t*>(desc->FirstThunk);
			
			for (char *funcname; *thunk; ++thunk, ++func)
			{
				if (!IMAGE_SNAP_BY_ORDINAL(*thunk))
				{
					auto import = dllptr<PIMAGE_IMPORT_BY_NAME>(*thunk);
					funcname = (char*)(&import->Name[0]);
				}
				else
				{
					funcname = (char*)IMAGE_ORDINAL(*thunk);
				}

				f = module.proc(h.getstr(funcname)).as<uintptr_t>();;

				if (!f)
				{
					PRINT("failed to resolve %s in %s\n", funcname, moduleName.str());
					return false;
				}

				*func = f;
			}
		}

		return true;
	}

	bool remoteimageloader::mapSections()
	{
		hash_t h;		

		address remote, start;

		ulong size, newSize, protect, oldProtect;
		
		for (auto first = IMAGE_FIRST_SECTION(m_hdr), scn = first; scn < (first + m_hdr->FileHeader.NumberOfSections); scn++)
		{
			h.getstr((char*)&scn->Name[0]);

			// lol nope
			if (h == SECTION_RELOC)
				continue;

			// try to obtain the correct size for this section
			if (!(size = scn->SizeOfRawData))
			{
				if (scn->Characteristics & IMAGE_SCN_CNT_CODE)
					size = m_hdr->OptionalHeader.SizeOfCode;
				else if (scn->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
					size = m_hdr->OptionalHeader.SizeOfInitializedData;
				else if (scn->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
					size = m_hdr->OptionalHeader.SizeOfUninitializedData;
			
				if (!size)
					continue;
			}

			// remote start of section
			remote = m_remoteDllBin + scn->VirtualAddress;

			if (scn->Characteristics & IMAGE_SCN_MEM_DISCARDABLE)
			{
				m_process.free(remote, size, MEM_DECOMMIT);
				continue;
			}

			// local start of section
			start = m_dllBin + scn->PointerToRawData;

			if (h == SECTION_TEXT)
			{
				for (ulong i = 0; i < size; i++)
				{
					// find all INT 3 page alignments
					int count = start.get<ulong>(i).ccbyte(0xcc);

					// randomize these so we can effectively change the hash of each page
					if (count > 1)
						mt19937::mem(start.get<ulong>(i + 1), count - 1);
				}
			}

			// now we write the section
			if (!m_process.write(remote, start, size))
			{
				PRINT("failed to map %s\n", scn->Name);
				return false;
			}

			ulong protect = sectionProtectionFlags(scn->Characteristics);

			PRINT("\n\tscn->Name = %s\n"
				"\tscn->Characteristics = 0x%x\n"
				"\tsize = 0x%x\n"
				"\tremote = 0x%x\n"
				"\tstart = 0x%x\n"
				"\tprotect = 0x%x\n", scn->Name, scn->Characteristics, size, remote.ptr(), start.ptr(), protect);

			// set protection flags (may or may not work)
			m_process.protect(remote, size, protect, &oldProtect);
		}

		PRINT("\n");

		return true;
	}

	bool remoteimageloader::resolveTLS()
	{
		address base, callback;

		auto dir = dataDirectory(IMAGE_DIRECTORY_ENTRY_TLS);

		if (dir->Size)
		{
			if (!dir->VirtualAddress)
				return false;
		}
		else
		{
			return true;
		}

		auto tls = dllptr<PIMAGE_TLS_DIRECTORY>(dir->VirtualAddress);

		base = tls->AddressOfCallBacks;

		if (base != nullptr)
		{
			for (;;)
			{
				if (!m_process.read(base, &callback, sizeof callback))
					return false;

				PRINT("\n\tbase = 0x%x\n"
					"\tcallback = 0x%x\nn", base.ptr(), callback.ptr());

				// we reached the end
				if (callback == nullptr)
					break;

				// call that shit
				if (!remoteDllMainCall(callback, nullptr, DLL_PROCESS_ATTACH, nullptr))
					return false;

				// proceed
				base = base.get<ulong>(sizeof ulong);
			}
		}

		PRINT("\n");

		return true;
	}
};