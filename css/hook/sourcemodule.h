#pragma once

#include "../../win/ntmodule.h"
#include "../../util/vmthook.h"

using namespace mu;

namespace css
{
	struct interfacereg_t
	{
		void *(*createFn)();
		const char *name;
		interfacereg_t *next;
	};

	// ntmodule class extended for implementing CreateInterface
	class sourcemodule : public ntmodule {
	public:
		sourcemodule()
		{
			reset();
		}

		sourcemodule (dword hash)
		{
			reset();
			init(hash);
		}

		virtual bool fill (address base);
		virtual void reset();
		
		interfacereg_t *getList();
		address createInterface (dword hash, vmthook *iface, bool shouldcpy = false, bool includeversion = false);

	private:
		xaddress m_list;
	};
};