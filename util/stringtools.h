#pragma once

#include "stdafx.h"

namespace mu
{
	extern int strcmp (const char *s1, const char *s2, size_t n);
	extern char *strstr (const char *in, const char *str);
	extern char *strchr (const char *str, int c);
	extern char *strlwr (char *str);
	extern char *strupr (char *str);
	extern void wcstombs (char *dest, wchar_t *src, size_t length);
	extern void wcstombs (char *dest, UNICODE_STRING &src);
	extern int atoi (const char *str);

	extern int wcscmp (const wchar_t *s1, const wchar_t *s2, size_t n);
	extern wchar_t *wcsstr (const wchar_t *str, const wchar_t *substr);
	extern wchar_t *wcschr (const wchar_t *str, int c);
	extern wchar_t *wcslwr (wchar_t *str);
	extern wchar_t *wcsupr (wchar_t *str);
	extern void mbstowcs (wchar_t *dest, char *src, size_t length);
};