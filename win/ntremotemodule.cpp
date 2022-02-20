#include "../stdafx.h"
#include "ntremotemodule.h"
#include "imports.h"
#include "../hash/hash.h"

namespace mu
{
	bool ntremotemodule::init (const process &p, dword hash)
	{
		setprocess(p);
		return init(hash);
	}

	bool ntremotemodule::init (dword hash)
	{
		return m_process.isvalid()
			? fill(remoteModuleHandleFromHash(m_process, hash, m_name.str()))
			: nullptr;
	}

	bool ntremotemodule::fill (address imgBase)
	{
		// always do a sanity check!
		if (!m_process.isvalid() || imgBase == nullptr)
			return false;
	
		// allocate once per object
		if (m_header == nullptr)
			m_header = new IMAGE_NT_HEADERS;

		// read pe header
		if (!remoteNtHeader(m_process, imgBase, m_header))
			return false;

		m_base.set(imgBase);
		return true;
	}

	address ntremotemodule::proc (dword hash) const
	{
		return remoteProcAddressFromHash(m_process, m_base.get(), m_name.str(), hash);
	}

	address ntremotemodule::byteScan (dword section, string signature) const
	{
		address start;
		size_t length;
		sectionBounds(section, start, length);
		return address::remotefindpattern(m_process, start, length, signature);
	}

	address ntremotemodule::hashScan (dword section, dword scan, dword mask) const
	{
		hash_t h;
		address start;
		size_t length;
		sectionBounds(section, start, length);
		return remotefindpattern(m_process, start, length, scan, mask);
	}

	void ntremotemodule::reset()
	{
		ntmodule::reset();
		m_process.reset();
	}

	// read PE header from remote module base
	bool remoteNtHeader (const process &proc, address moduleBase, PIMAGE_NT_HEADERS ntHdr)
	{
		IMAGE_DOS_HEADER dosHdr;

		if (proc.read(moduleBase, &dosHdr, sizeof dosHdr))
		{
			if (dosHdr.e_magic == IMAGE_DOS_SIGNATURE)
			{
				address ntHdrBase = moduleBase + dosHdr.e_lfanew;

				if (proc.read(ntHdrBase, ntHdr, sizeof IMAGE_NT_HEADERS))
					return ntHdr->Signature == IMAGE_NT_SIGNATURE;
			}
		}	

		return false;
	}

	// get the module handle from a remote process
	address remoteModuleHandleFromHash (const process &proc, DWORD hash, string out /*= nullptr*/)
	{
		staticstring<64> moduleName;
		staticwstring<64> moduleNameWide;	
	
		if (proc.isvalid())
		{
			PROCESS_BASIC_INFORMATION pbi;

			if (proc.pquery(ProcessBasicInformation, &pbi, sizeof pbi))
			{
				address listBase, ldrBase = pbi.PebBaseAddress;

				ldrBase = ldrBase.get<LONG>(FIELD_OFFSET(PEB, LoaderData));
				
				if (proc.read(ldrBase, &listBase, sizeof listBase))
				{
					LIST_ENTRY list;
					
					listBase = listBase.get<LONG>(FIELD_OFFSET(PEB_LDR_DATA, InLoadOrderModuleList));
					
					if (proc.read(listBase, &list, sizeof list))
					{
						LDR_MODULE ldrModule;
						
						for (address entryBase = list.Flink; entryBase != listBase; entryBase = ldrModule.InLoadOrderModuleList.Flink)
						{
							// read this LDR_MODULE entry
							if (!proc.read(entryBase, &ldrModule, sizeof ldrModule))
								break;
						
							// read the module name as unicode
							if (!proc.read(ldrModule.BaseDllName.Buffer, moduleNameWide.str(), ldrModule.BaseDllName.Length))
								break;
						
							// truncate to ANSI and check the hash
							wcstombs(moduleName, moduleNameWide, ldrModule.BaseDllName.Length >> 1);

							if (hash_t(moduleName.lower()) == hash)
							{
								if (out != nullptr)
									moduleName.copyto(out);

								return ldrModule.BaseAddress;
							}
						}
					}
				}
			}
		}	
	
		return nullptr;
	}

	bool remoteModuleFileName (const process &proc, address moduleBase, string out, bool fullPath)
	{	
		staticstring<MAX_PATH> moduleFileName;
		staticwstring<MAX_PATH> moduleFileNameWide;	
	
		if (proc.isvalid())
		{
			PROCESS_BASIC_INFORMATION pbi;

			if (proc.pquery(ProcessBasicInformation, &pbi, sizeof pbi))
			{
				address listBase, ldrBase = pbi.PebBaseAddress;

				ldrBase = ldrBase.get<LONG>(FIELD_OFFSET(PEB, LoaderData));

				if (proc.read(ldrBase, &listBase, sizeof listBase))
				{
					LIST_ENTRY list;

					listBase = listBase.get<LONG>(FIELD_OFFSET(PEB_LDR_DATA, InLoadOrderModuleList));

					if (proc.read(listBase, &list, sizeof list))
					{
						LDR_MODULE ldrModule;

						for (address entryBase = list.Flink; entryBase != listBase; entryBase = ldrModule.InLoadOrderModuleList.Flink)
						{
							// read this LDR_MODULE entry
							if (!proc.read(entryBase, &ldrModule, sizeof ldrModule))
								break;

							if (ldrModule.BaseAddress == moduleBase)
							{
								UNICODE_STRING &dllname = fullPath ? ldrModule.FullDllName : ldrModule.BaseDllName;

								// read the module name as unicode
								if (!proc.read(dllname.Buffer, moduleFileNameWide.str(), dllname.Length))
									break;

								// truncate to ANSI and copy
								wcstombs(moduleFileName, moduleFileNameWide, dllname.Length >> 1);

								if (!fullPath)
									moduleFileName.lower();

								moduleFileName.copyto(out);
								return true;
							}
						}
					}
				}
			}
		}
	
		return false;
	}
	
	// this is really cheap
	address remoteProcAddressFromHash (const process &proc, address remoteBase, string moduleName, dword hash)
	{		
		address localBase = moduleHandleFromHash(hash_t(moduleName));
		
		if (localBase == nullptr)
		{
			// load it if necessary
			localBase = imports->loadLibraryEx(moduleName, nullptr, DONT_RESOLVE_DLL_REFERENCES);
			
			if (localBase == nullptr)
				return nullptr;
		}
		
		// get the delta
		uintptr_t offset = procAddressFromHash(localBase, hash).as<uintptr_t>() - localBase.as<uintptr_t>();
		
		// add to remote base
		return remoteBase.get<uintptr_t>(offset);
	}
};