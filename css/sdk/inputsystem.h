#pragma once

#include "../../util/vmthook.h"
#include "buttoncode.h"

using namespace mu;

namespace css
{
	class CInputSystem : public vmthook {
	public:
		CInputSystem()
		{
			reset();
		}

		bool IsButtonDown (ButtonCode_t code)
		{
			return old_method<bool (__thiscall*)(void*, ButtonCode_t)>(11)(inst(), code);
		}
	};
};