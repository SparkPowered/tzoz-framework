#include "stdafx.h"

#include "win\imports.h"

using namespace mu;

EXTERN_C int _fltused = 1;

_PVFV __xc_a[], __xc_z[]; // C++ initializers
_PIFV __xi_a[], __xi_z[]; // C initializers
_PVFV __xp_a[], __xp_z[]; // Pre-terminators
_PVFV __xt_a[], __xt_z[]; // C terminators 

#pragma data_seg(".CRT$XCA")
_PVFV __xc_a[] = {0};

#pragma data_seg(".CRT$XCZ")
_PVFV __xc_z[] = {0};

#pragma data_seg(".CRT$XIA")
_PIFV __xi_a[] = {0};

#pragma data_seg(".CRT$XIZ")
_PIFV __xi_z[] = {0};

#pragma data_seg(".CRT$XPA")
_PVFV __xp_a[] = {0};

#pragma data_seg(".CRT$XPZ")
_PVFV __xp_z[] = {0};

#pragma data_seg(".CRT$XTA")
_PVFV __xt_a[] = {0};

#pragma data_seg(".CRT$XTZ")
_PVFV __xt_z[] = {0};

#pragma data_seg()

// merge these sections for our manual mapper
#pragma comment(linker, "/MERGE:.CRT=.rdata")
#pragma comment(linker, "/MERGE:.rdata=.data")

// "A non-zero error code if an initialization fails and throws an error; 0 if no error occurs."
int __cdecl _initterm_e (_PIFV *pfbegin, _PIFV *pfend)
{
	register int i;

	for (i = 0; pfbegin < pfend; pfbegin++)
	{
		if (*pfbegin != nullptr)
			i = (**pfbegin)();
	}
	
	return i;
}

// walk the routines and initialize the data for the program
void __cdecl _initterm (_PVFV *pfbegin, _PVFV *pfend)
{
	for (; pfbegin < pfend; pfbegin++)
	{
		if (*pfbegin != nullptr)
			(**pfbegin)();
	}
}

// exit callback
int atexit (_PVFV func)
{
	return -1;
}

#pragma function(memcpy, memset, memmove, memcmp)

EXTERN_C // optimized CRT routines
{
	// directly copy region of memory
	void *__cdecl memcpy (void *target, const void *source, size_t size)
	{
		__movsb((byte*)target, (byte*)source, size);
		return target;
	}

	// directly set region of memory (as bytecode)
	void *__cdecl memset (void *target, int value, size_t size)
	{
		__stosb((byte*)target, (byte)value, size);
		return target;
	}

	// copy memory with respect to overlapping regions
	void *__cdecl memmove (void *target, const void *source, size_t size)
	{
		register char *d = (char*)target;
		register const char *s = (const char*)source;

		 // check for overlapping memory
		if (d <= s || d >= (s + size))
		{
			memcpy(d, s, size);
		} 
		else
		{
			for (d += (size - 1), s += (size - 1); (size--) != 0; *d-- = *s--);
		}

		return target;
	}

	// compare bytes within a region
	int __cdecl memcmp (const void *a, const void *b, size_t size)
	{
		register const char *ra = (const char*)a;
		register const char *rb = (const char*)b;

		for (; size--; ra++, rb++)
		{
			if (*ra != *rb)
				return (*ra - *rb);
		}
		
		return 0;
	}

	// pure virtual method
	int __cdecl _purecall()
	{
		return 0;
	}
};

void* operator new (size_t size)
{
	return imports->malloc(size);
}

void* operator new[] (size_t size)
{
	return imports->malloc(size);
}

void operator delete (void *data)
{
	imports->free(data);
}

void operator delete[] (void *data)
{
	imports->free(data);
}
