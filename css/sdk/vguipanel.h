#pragma once

#include "../../util/vmthook.h"

using namespace mu;

namespace css
{
	class Panel : public vmthook {
	public:
		Panel()
		{
			reset();
		}

		string GetName (dword panel)
		{
			return old_method<char *(__thiscall*)(void*, dword)>(36)(inst(), panel);
		}

		void PaintTraverse (void *thisptr, dword panel, bool forceRepaint, bool allowForce)
		{
			old_method<void (__thiscall*)(void*, dword, bool, bool)>(41)(thisptr, panel, forceRepaint, allowForce);
		}
	};
};