#include "../stdafx.h"
#include "ntmodule.h"
#include "../stdafx.h"
#include "../hash/hash.h"

namespace mu
{
	void ntmodule::reset()
	{
		m_header = nullptr;

		m_base.reset();
		m_name.reset();
	}
	
	bool ntmodule::init (dword hash)
	{
		// searched the PEB linked list
		address base = moduleHandleFromHash(hash);

		// grab the plain name of this module
		if (!moduleFileName(base, m_name.str(), false))
			return false;

		// get the rest of the data
		return fill(base);
	}

	bool ntmodule::fill (address base)
	{
		// this will also check if the base address is null
		if ((m_header = ntHeader(base)) == nullptr)
			return false;

		m_base.set(base);
		return true;
	}

	address ntmodule::proc (dword hash) const
	{
		return procAddressFromHash(m_base.get(), hash);
	}

	void ntmodule::sectionBounds (dword section, address &start, size_t &length) const
	{
		auto scn = sectionHeaderFromHash(section, m_header);

		if (scn != nullptr)
		{
			start = m_base.get().get<ulong>(scn->VirtualAddress);
			length = section_size(scn);
		}
		else
		{
			start = m_base.get();
			length = m_header->OptionalHeader.SizeOfCode;
		}
	}

	// scan within PE section using traditional bytesig
	address ntmodule::byteScan (dword section, string signature) const
	{
		address start;
		size_t length;
		sectionBounds(section, start, length);
		return address::findpattern(start, signature, length);
	}

	// scan within PE section using hash
	address ntmodule::hashScan (dword section, dword signature, dword mask) const
	{
		address start;
		size_t length;
		sectionBounds(section, start, length);
		return findpattern(start, length, signature, mask);
	}
	
	// scan for reference to static data (ex: string located in .rdata)
	// credits: Badster
	address ntmodule::scanReference (address data, byte beginCode) const
	{
		byte ref[5];

		if (data == nullptr)
			return nullptr;

		// create our byte signature
		ref[0] = beginCode;
		*(ulong*)&ref[1] = data;
		
		return hashScan(SECTION_TEXT, hash_t().getsig(ref, 0x1f), 0x1f);
	}

	address ntmodule::hashScanFromReference (dword signature, dword mask, byte beginCode) const
	{
		address data = hashScan(SECTION_RDATA, signature, mask);
		return scanReference(data, beginCode);
	}

	address ntmodule::byteScanFromReference (string signature, byte beginCode) const
	{
		address data = byteScan(SECTION_RDATA, signature);
		return scanReference(data, beginCode);
	}

	// get the PE header for this module
	PIMAGE_NT_HEADERS ntHeader (address base)
	{
		if (base != nullptr)
		{
			auto dos = base.as<PIMAGE_DOS_HEADER>();

			if (dos->e_magic == IMAGE_DOS_SIGNATURE)
			{
				auto nt = base.get<word>(dos->e_lfanew).as<PIMAGE_NT_HEADERS>();

				if (nt->Signature == IMAGE_NT_SIGNATURE)
					return nt;
			}
		}
	
		return nullptr;
	}

	// credits: wav
	size_t calculateDllSize (PIMAGE_NT_HEADERS ntHdr)
	{
		size_t size = 0;

		if (ntHdr != nullptr)
		{
			size = sizeof IMAGE_DOS_HEADER + sizeof IMAGE_NT_HEADERS;
			size += sizeof IMAGE_SECTION_HEADER * ntHdr->FileHeader.NumberOfSections;
			size = ROUND_UP(size, ntHdr->OptionalHeader.SectionAlignment);
		
			auto first = IMAGE_FIRST_SECTION(ntHdr);

			for (auto scn = first; scn < (first + ntHdr->FileHeader.NumberOfSections); scn++)
			{
				size += ROUND_UP(scn->Misc.VirtualSize, ntHdr->OptionalHeader.SectionAlignment);
			}
		}
		
		return size;
	}

	address pointerFromRva (address rva, address image, PIMAGE_NT_HEADERS ntHdr /*= nullptr*/)
	{
		address ptr = rva.get<uintptr_t>(image);

		if (ntHdr != nullptr)
		{
			auto scn = sectionHeaderFromRva(rva, ntHdr);
			ptr = ptr.delta<int>((int)(scn->VirtualAddress - scn->PointerToRawData));
		}
	
		return ptr;
	}

	// section header from virtual address
	PIMAGE_SECTION_HEADER sectionHeaderFromRva (address rva, PIMAGE_NT_HEADERS ntHdr)
	{
		if (ntHdr != nullptr)
		{
			auto first = IMAGE_FIRST_SECTION(ntHdr);

			for (auto scn = first; scn < first + ntHdr->FileHeader.NumberOfSections; scn++)
			{
				if (rva.within(scn->VirtualAddress, section_size(scn)))
					return scn;
			}
		}
	
		return nullptr;
	}

	// section header from hash
	PIMAGE_SECTION_HEADER sectionHeaderFromHash (dword hash, PIMAGE_NT_HEADERS ntHdr)
	{
		if (ntHdr != nullptr)
		{
			auto first = IMAGE_FIRST_SECTION(ntHdr);

			for (auto scn = first; scn < first + ntHdr->FileHeader.NumberOfSections; scn++)
			{
				if (hash == hash_t((char*)scn->Name))
					return scn;
			}
		}
	
		return nullptr;
	}

	// page protection for a section
	ulong sectionProtectionFlags (ulong characteristics)
	{
		ulong protect = 0;

		if (characteristics & IMAGE_SCN_MEM_EXECUTE)
		{
			if (characteristics & IMAGE_SCN_MEM_READ)
			{
				if (characteristics & IMAGE_SCN_MEM_WRITE)
					protect |= PAGE_EXECUTE_READWRITE;
				else
					protect |= PAGE_EXECUTE_READ;
			}
			else
			{
				if (characteristics & IMAGE_SCN_MEM_WRITE)
					protect |= PAGE_EXECUTE_WRITECOPY;
				else
					protect |= PAGE_EXECUTE;
			}
		}
		else
		{
			if (characteristics & IMAGE_SCN_MEM_READ)
			{
				if (characteristics & IMAGE_SCN_MEM_WRITE)
					protect |= PAGE_READWRITE;
				else
					protect |= PAGE_READONLY;
			}
			else
			{
				if (characteristics & IMAGE_SCN_MEM_WRITE)
					protect |= PAGE_WRITECOPY;
				else
					protect |= PAGE_NOACCESS;
			}
		}

		if (characteristics & IMAGE_SCN_MEM_NOT_CACHED)
			protect |= PAGE_NOCACHE;

		return protect;
	}
	
	// get the local module handle
	address moduleHandleFromHash (dword hash, string out /*= nullptr*/)
	{
		PLDR_MODULE ldr;
		staticstring<64> name;

		// access the PEB linked list
		auto list = &(getPEB()->LoaderData->InLoadOrderModuleList);

		// iterate each entry
		for (auto entry = list->Flink; entry != list; entry = entry->Flink)
		{
			// current module in the list
			ldr = CONTAINING_RECORD(entry, LDR_MODULE, InLoadOrderModuleList);

			wcstombs(name, ldr->BaseDllName);

			if (hash == hash_t(name.lower()))
			{
				if (out != nullptr)
					out.copy(name);

				return ldr->BaseAddress;
			}
		}
	
		return nullptr;
	}

	// full or shortened name of this module
	bool moduleFileName (address base, string out, bool fullPath)
	{
		PLDR_MODULE ldr;
		staticstring<MAX_PATH> name;

		// access the PEB linked list
		auto list = &(getPEB()->LoaderData->InLoadOrderModuleList);

		if (base != nullptr && out != nullptr)
		{
			// iterate
			for (auto entry = list->Flink; entry != list; entry = entry->Flink)
			{
				// match base address
				ldr = CONTAINING_RECORD(entry, LDR_MODULE, InLoadOrderModuleList);

				if (ldr->BaseAddress == base)
				{
					wcstombs(name, fullPath ? ldr->FullDllName : ldr->BaseDllName);
					out.copy(fullPath ? name : name.lower());
					return true;
				}
			}
		}

		return false;
	}

	address resolveForwarder (address method)
	{
		string pos;
		address base;

		static xstring<4> ext(/*.dll*/ 0x04, 0xEF, 0xC1949D9E);

		if (method != nullptr)
		{
			staticstring<256> fwd(method.as<char*>());			

			// find the bridge between the module/export names
			if ((pos = fwd.chr('.')) != nullptr)
			{
				// copy the function name
				staticstring<64> fn(pos + 1);

				// set up the module name
				fwd -= pos;
				fwd.append(ext.decrypt());

				// get the correct data
				base = moduleHandleFromHash(hash_t(fwd.lower()));
				method = procAddressFromHash(base, hash_t(fn.str()));
			}
		}

		return method;
	}

	// export from local module
	address procAddressFromHash (address base, dword hash)
	{
		address method, rva;

		auto ntHdr = ntHeader(base);

		if (ntHeader != nullptr)
		{
			// verify base directory
			auto data = &ntHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

			if (data->VirtualAddress && data->Size)
			{
				// get export data
				auto dir = base.get<ulong>(data->VirtualAddress).as<PIMAGE_EXPORT_DIRECTORY>();
				auto ordinalList = base.get<ulong>(dir->AddressOfNameOrdinals).as<word*>();
				auto methodList = base.get<ulong>(dir->AddressOfFunctions).as<uintptr_t*>();
				auto nameList = base.get<ulong>(dir->AddressOfNames).as<uintptr_t*>();

				for (dword i = 0; i < dir->NumberOfNames; i++)
				{
					if (hash == hash_t(base.get<uintptr_t>(nameList[i]).as<char*>()))
					{
						rva = methodList[ordinalList[i]];
						method = base.get<uintptr_t>(rva);

						// check if this export is forwarded
						if (rva.within(data->VirtualAddress, data->Size))
							method = resolveForwarder(method);

						return method;
					}
				}
			}
		}
	
		return nullptr;
	}
};