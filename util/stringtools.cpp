#include "../stdafx.h"

namespace mu
{
	int strcmp (const char *s1, const char *s2, size_t n)
	{
		for (; n > 0; s1++, s2++, --n)
		{
			if (*s1 != *s2)
				return *(byte*)s1 - *(byte*)s2;
			else if (*s1 == '\0')
				return 0;
		}

		return 0;
	}

	char *strstr (const char *str, const char *substr)
	{
		size_t len;

		char c, sc;

		if ((c = *substr++) == '\0')
			return (char*)str;

		len = strlen(substr);

		do
		{
			do
			{
				if ((sc = *str++) == '\0')
					return nullptr;
			} while (sc != c);
		} while (strcmp(str, substr, len));

		return (char*)(str - 1);
	}

	char *strchr (const char *str, int c)
	{
		for (const char ch = c; *str != ch; str++)
		{
			if (*str == '\0')
				return nullptr;
		}

		return (char*)str;
	}

	char *strlwr (char *str)
	{   
		register char *s = str;

		for (char c; *str != '\0'; *str++ = c)
		{ 
			c = *str;

			if (c >= 'A' && c <= 'Z')
				c += 32;
		}
		
		return s; 
	}

	char *strupr (char *str)
	{   
		register char *s = str;

		for (char c; *str != '\0'; *str++ = c)
		{   
			c = *str;

			if (c >= 'a' && c <= 'z')
				c -= 32;
		}
		
		return s; 
	}

	void wcstombs (char *dest, wchar_t *src, size_t length)
	{
		for (size_t i = 0; i < length; i++)
		{
			dest[i] = src[i] & 0xff;
		}

		dest[length] = '\0';
	}

	void wcstombs (char *dest, UNICODE_STRING &src)
	{
		wcstombs(dest, src.Buffer, src.Length >> 1);
	}

	int atoi (const char *str)
	{
		int v, d, c;

		for (v = 0; (c = *str++) != '\0'; v = (v * 10) + d)
		{
			if (c < '0' || c > '9')
				break;

			d = c - '0';
		}

		return v;
	}
	
	int wcscmp (const wchar_t *s1, const wchar_t *s2, size_t n)
	{
		for (; n > 0; s1++, s2++, --n)
		{
			if (*s1 != *s2)
				return *(word*)s1 - *(word*)s2;
			else if (*s1 == L'\0')
				return 0;
		}

		return 0;
	}

	wchar_t *wcsstr (const wchar_t *str, const wchar_t *substr)
	{
		size_t len;

		wchar_t c, sc;

		if ((c = *substr++) == L'\0')
			return (wchar_t*)str;

		len = wcslen(substr);

		do
		{
			do
			{
				if ((sc = *str++) == L'\0')
					return nullptr;
			} while (sc != c);
		} while (wcscmp(str, substr, len));

		return (wchar_t*)(str - 1);
	}

	wchar_t *wcschr (const wchar_t *str, int c)
	{
		for (const wchar_t ch = c; *str != ch; str++)
		{
			if (*str == L'\0')
				return nullptr;
		}

		return (wchar_t*)str;
	}

	wchar_t *wcslwr (wchar_t *str)
	{
		register wchar_t *s = str;

		for (wchar_t c; *str; *str++ = c)
		{
			c = *str;

			if (c >= L'A' && c <= L'Z')
				c += 32;
		}
		
		return s;
	}

	wchar_t *wcsupr (wchar_t *str)
	{
		register wchar_t *s = str;

		for (wchar_t c; *str != L'\0'; *str++ = c)
		{
			c = *str;

			if (c >= L'a' && c <= L'z')
				c -= 32;
		}
		
		return s;
	}

	void mbstowcs (wchar_t *dest, char *src, size_t length)
	{
		for (size_t i = 0; i < length; i++)
		{
			dest[i] = src[i];
		}

		dest[length] = L'\0';
	}
};