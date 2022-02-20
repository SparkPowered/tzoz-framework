#pragma once

#include "targetver.h"

// for debugging
//#define LOG_ENABLE
#define PRINT_ENABLE

// exclude unnecessary headers
#define WIN32_LEAN_AND_MEAN

// use this for current process handle
#define PSEUDO_HANDLE ((void*)-1) 

// size of an address
#define PTR_SIZE sizeof(void*)

#define ROUND_UP(p, align) (((unsigned long)(p) + (align) - 1) & ~((align) - 1))
#define ROUND_DOWN(p, align) ((unsigned long)(p) & ~((align) - 1))

// PE header sections
enum section_hash_t
{
	SECTION_TEXT = 0x05fd2116,
	SECTION_DATA = 0x05f4709b,
	SECTION_RDATA = 0xc57f3589,
	SECTION_RELOC = 0xc57f875c,
};

#include <Windows.h>
#include <intrin.h>

// e-z conversion
#define ulong2handle(ul) ((HANDLE)(ULONG_PTR)(ul))
#define handle2ulong(h) ((ULONG_PTR)h)

// cpu timestamp counter
#define TSC64 __rdtsc()
#define TSC32 (TSC64 & 0xffffffff)
#define TSC16 (TSC64 & 0xffff)
#define TSC8 (TSC64 & 0xff)

#include "type.h"

#include "util\mathlib.h"
#include "util\stringtools.h"
#include "util\logfile.h"

typedef void (__cdecl *_PVFV)();
typedef int (__cdecl *_PIFV)();

int __cdecl _initterm_e (_PIFV *pfbegin, _PIFV *pfend);
void __cdecl _initterm (_PVFV *pfbegin, _PVFV *pfend);

extern _PVFV __xc_a[], __xc_z[]; /* C++ initializers */
extern _PIFV __xi_a[], __xi_z[]; /* C initializers */
extern _PVFV __xp_a[], __xp_z[]; /* Pre-terminators */
extern _PVFV __xt_a[], __xt_z[]; /* C terminators */ 

#pragma function (memcpy, memset, memmove, memcmp)

void *__cdecl memcpy (void *target, const void *source, size_t size);
void *__cdecl memset (void *target, int value, size_t size);
void *__cdecl memmove (void *target, const void *source, size_t size);
int __cdecl memcmp (const void *a, const void *b, size_t size);
