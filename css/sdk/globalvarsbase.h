#pragma once

#include "../../util/address.h"

using namespace mu;

namespace css
{
	class CGlobalVarsBase : public xaddress {
	public:
		CGlobalVarsBase()
		{
			reset();
		}

		float GetCurtime()
		{
			return get().get<size_t>(0xc).to<float>();
		}

		void SetCurtime (float curtime)
		{
			*get().get<size_t>(0xc).as<float*>() = curtime;
		}

		float GetFrametime()
		{
			return get().get<size_t>(0x10).to<float>();
		}

		void SetFrametime (float frametime)
		{
			*get().get<size_t>(0x10).as<float*>() = frametime;
		}

		int GetMaxClients()
		{
			return get().get<size_t>(0x14).to<int>();
		}

		int GetTickCount()
		{
			return get().get<size_t>(0x18).to<int>();
		}

		float GetIntervalPerTick()
		{
			return get().get<size_t>(0x1c).to<float>();
		}
	};
};