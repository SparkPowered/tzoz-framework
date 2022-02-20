#pragma once

#include "../../util/vec3.h"
#include "../../util/vmthook.h"

using namespace mu;

namespace css
{
	class CDebugOverlay : public vmthook {
	public:
		CDebugOverlay()
		{
			reset();
		}

		int ScreenPosition (const vec3 &point, vec3 &screen)
		{
			return old_method<int (__thiscall*)(void*, const vec3&, vec3&)>(10)(inst(), point, screen);
		}
	};
};