#include "stdafx.h"

namespace mu
{
	__declspec (naked)
	void *getEBP()
	{
		__asm
		{
			mov eax, ebp;
			retn;
		};
	}

	__declspec (naked)
	void *getESI()
	{
		__asm
		{
			mov eax, esi;
			retn;
		};
	}

	__declspec (naked)
	void *getEDI()
	{
		__asm
		{
			mov eax, edi;
			retn;
		};
	}
};