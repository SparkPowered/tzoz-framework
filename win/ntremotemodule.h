#pragma once

#include "ntmodule.h"
#include "process.h"

namespace mu
{
	extern bool remoteNtHeader (const process &proc, address base, PIMAGE_NT_HEADERS ntHdr);
	extern address remoteModuleHandleFromHash (const process &proc, DWORD hash, string out = nullptr);
	extern bool remoteModuleFileName (const process &proc, address moduleBase, string out, bool fullPath);
	extern address remoteProcAddressFromHash (const process &proc, address remoteBase, string moduleName, dword hash);
	
	class ntremotemodule : public ntmodule {
	public:
		ntremotemodule()
		{
			reset();
		}

		ntremotemodule (const process &p, dword hash)
		{
			reset();
			init(p, hash);
		}

		virtual ~ntremotemodule()
		{
			delete m_header;
			reset();
		}

		virtual bool init (dword hash);
		virtual bool fill (address base);
		virtual address proc (dword hash) const;
		virtual address byteScan (dword section, string signature) const;
		virtual address hashScan (dword section, dword signature, dword mask) const;
		virtual void reset();

		bool init (const process &p, dword hash);

		void setprocess (const process &p)
		{
			m_process = p;
		}

		const process &parentprocess() const
		{
			return m_process;
		}

	protected:
		process m_process;
	};
};