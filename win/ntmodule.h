#pragma once

#include "../stdafx.h"
#include "../util/address.h"

namespace mu
{
	// wrapper for local dll
	class ntmodule {
	public:
		ntmodule()
		{
			reset();
		}

		ntmodule (dword hash)
		{
			reset();
			init(hash);
		}

		virtual ~ntmodule()
		{
			reset();
		}

		virtual void reset();
		virtual bool init (dword hash);
		virtual bool fill (address base);
		virtual address proc (dword hash) const;
		virtual address byteScan (dword section, string signature) const;
		virtual address hashScan (dword section, dword signature, dword mask) const;

		address scanReference (address data, byte beginCode) const;
		address hashScanFromReference (dword signature, dword mask, byte beginCode) const;
		address byteScanFromReference (string signature, byte beginCode) const;

		void sectionBounds (dword section, address &start, size_t &length) const;

		// basic null check
		bool valid() const;

		// pseudo-encrypted base address
		xaddress m_base;
		// the PE header
		PIMAGE_NT_HEADERS m_header;
		// base dll name
		staticstring<64> m_name;
	};

	inline bool ntmodule::valid() const
	{
		return m_base.get() != nullptr && m_header != nullptr;
	}

	extern PIMAGE_NT_HEADERS ntHeader (address base);
	extern size_t calculateDllSize (PIMAGE_NT_HEADERS ntHdr);
	extern address pointerFromRva (address rva, address image, PIMAGE_NT_HEADERS ntHdr = nullptr);
	extern PIMAGE_SECTION_HEADER sectionHeaderFromRva (address rva, PIMAGE_NT_HEADERS ntHdr);
	extern PIMAGE_SECTION_HEADER sectionHeaderFromHash (dword hash, PIMAGE_NT_HEADERS ntHdr);
	extern ulong sectionProtectionFlags (ulong characteristics);
	extern address moduleHandleFromHash (dword hash, string out = nullptr);
	extern bool moduleFileName (address base, string out, bool fullPath);
	extern address resolveForwarder (address method);
	extern address procAddressFromHash (address base, dword hash);

	// apply image base and/or file offset
	inline address pointerFromRva (uintptr_t rva, address image, PIMAGE_NT_HEADERS ntHdr = nullptr)
	{
		return pointerFromRva(address(rva), image, ntHdr);
	}

	// get the size of this section
	inline size_t section_size (PIMAGE_SECTION_HEADER scn)
	{
		return scn->Misc.VirtualSize != 0 ? scn->Misc.VirtualSize : scn->SizeOfRawData;
	}

	// number of relocations for this block
	inline ulong reloc_count (PIMAGE_BASE_RELOCATION reloc)
	{
		return (reloc->SizeOfBlock - sizeof IMAGE_BASE_RELOCATION) / sizeof word;
	}
};