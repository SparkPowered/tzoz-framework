#pragma once

#include "imageloader.h"

#define IMR_RELTYPE(x) ((x >> 12) & 0xf)
#define IMR_RELOFFSET(x) (x & 0xfff) 


namespace ldr
{
	struct dllmain_t
	{
		BOOL (__stdcall *fn)(HMODULE, DWORD, LPVOID);
		HMODULE dll;
		DWORD reason;
		LPVOID reserved;
	};

	// manually map an image into target process
	// credits: Ecksy, praydog, Darawk, s0beit
	class remoteimageloader : public imageloader {
	public:
		enum {k_allocType = MEM_COMMIT|MEM_RESERVE};

		remoteimageloader()
		{
			reset();
		}

		remoteimageloader (string path)
		{
			reset();
			init(path);
		}

		virtual void reset();
		virtual bool map();
		virtual void shutdown();

		bool remoteLoadLibrary (string dll);
		bool remoteDllMainCall (address start, HMODULE module, DWORD reason, LPVOID reserved);

	private:	
		bool create (string dll);
		bool relocate (size_t delta);
		bool resolveImports();
		bool mapSections();
		bool resolveTLS();

		template <typename T>
		T dllptr (uintptr_t rva)
		{
			return pointerFromRva(rva, m_dllBin, m_hdr).as<T>();
		}
		
		address hdrImageBase()
		{
			return (void*)m_hdr->OptionalHeader.ImageBase;
		}

		PIMAGE_DATA_DIRECTORY dataDirectory (int i)
		{
			return &m_hdr->OptionalHeader.DataDirectory[i];
		}

		// allocated memory
		address m_dllBin, m_remoteDllBin, m_remoteParam;

		// PE header
		PIMAGE_NT_HEADERS m_hdr;
		
		// complete size of image
		size_t m_dllSize;
	};
};