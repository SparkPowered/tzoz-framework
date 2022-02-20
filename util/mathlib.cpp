#include "../stdafx.h"

namespace mu
{
	float sqrt (float x)
	{
		float r = 0.0f;

		__asm
		{
			sqrtss xmm0, x;
			movss r, xmm0;
		};

		return r;
	}

	__declspec (naked)
	float sin (float f)
	{
		__asm
		{
			fld dword ptr [esp + 0x4];
			fsin;
			retn;
		};
	}

	__declspec (naked)
	float cos (float f)
	{
		__asm
		{
			fld dword ptr [esp + 0x4];
			fcos;
			retn;
		};
	}
};