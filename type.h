#pragma once

#include "win\ntddk.h"

namespace mu
{
	typedef long long int64;
	typedef unsigned char byte;
	typedef unsigned short word;
	typedef unsigned int dword;
	typedef unsigned long ulong;
	typedef unsigned long long qword;

	void *getEBP();
	void *getESI();
	void *getEDI();
		
	// acquire from relative JMP
	template <typename T>
	inline T jmpaddress (uintptr_t base)
	{
		return (T)(*(uintptr_t*)(base + 0x1) + base + 0x5);
	}

	// x64 relative address
	template <typename T>
	inline T absaddress (uintptr_t base)
	{
		return (T)(*(uintptr_t*)base + base + 0x4);
	}

	// thread environment block
	inline PTEB getTEB()
	{
		return (PTEB)__readfsdword(0x18);
	}

	// process environment block
	inline PPEB getPEB()
	{
		return getTEB()->Peb;
	}

	inline DWORD currentProcessId()
	{
		return __readfsdword(0x20);
	}

	inline DWORD currentThreadId()
	{
		return __readfsdword(0x24);
	}

	inline DWORD getLastError()
	{
		return __readfsdword(0x34);
	}

	inline bool isConsoleHandle (HANDLE h)
	{
		return h != PSEUDO_HANDLE && ((UINT_PTR)h & 3) == 3;
	}
};