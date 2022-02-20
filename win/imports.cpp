#include "../stdafx.h"
#include "imports.h"

#include "../util/cstring.h"

namespace mu
{
	static importlist _imports;
	importlist *imports = &_imports;

	void importlist::init()
	{
		ntmodule kernel32, msvcrt, user32, winmm;
	
		// kernel32.dll
		kernel32.init(0x3e003875);
		reg_entry(k_Sleep, kernel32, 0x0dce270a);
		reg_entry(k_GetPrivateProfileStringA, kernel32, 0x2bad33a1);
		reg_entry(k_WritePrivateProfileStringA, kernel32, 0xec21be2a);
		reg_entry(k_WaitForMultipleObjectsEx, kernel32, 0x5a76d87e);
		reg_entry(k_LoadLibraryExA, kernel32, 0x33bfa5e0);
		reg_entry(k_CreateFileA, kernel32, 0xcdf70c26);
		reg_entry(k_GetCompressedFileSizeA, kernel32, 0x3c5a0536);
		reg_entry(k_GetFileAttributesA, kernel32, 0x5a27021b);
		reg_entry(k_ReadFile, kernel32, 0x245d06b1);
		reg_entry(k_GetTickCount, kernel32, 0xa57242e5);

		// if we're compiling as a console app then we'll have to load the rest of the DLLs into the process
	#ifdef _CONSOLE	

		static xstring<12> enc_msvcrt(/*msvcrt.dll*/ 0x0A, 0xAE, 0xC3DCC6D2, 0xC0C79AD1, 0xDADB0000);
		static xstring<12> enc_user32(/*user32.dll*/ 0x0A, 0x23, 0x56574054, 0x141A074E, 0x47400000);
		static xstring<12> enc_winmm(/*winmm.dll*/ 0x09, 0x39, 0x4E535551, 0x50105B2C, 0x2D000000);

		loadLibraryEx(enc_msvcrt.decrypt(), nullptr, 0);
		loadLibraryEx(enc_user32.decrypt(), nullptr, 0);
		loadLibraryEx(enc_winmm.decrypt(), nullptr, 0);

	#endif	
	
		// msvcrt.dll
		msvcrt.init(0x184238a2);
		reg_entry(k_printf, msvcrt, 0x6fde90b2);
		reg_entry(k_fopen, msvcrt, 0x0a341a17);
		reg_entry(k_fclose, msvcrt, 0x4fdbfe35);
		reg_entry(k_fprintf, msvcrt, 0x79d87974);
		reg_entry(k_vsnprintf, msvcrt, 0xb81f1399);
		reg_entry(k_malloc, msvcrt, 0x573e6185);
		reg_entry(k_free, msvcrt, 0x7c6e3af1);
		reg_entry(k_atan, msvcrt, 0x7c6dbe1f);
		reg_entry(k_atan2, msvcrt, 0x0a2581cd);
		reg_entry(k_exp, msvcrt, 0x0b8724c8);
		reg_entry(k_log, msvcrt, 0x0b873f21);
		reg_entry(k_system, msvcrt, 0x806c14a0);

		// user32.dll
		user32.init(0xcba985ff);
		reg_entry(k_GetActiveWindow, user32, 0x2f7692f3);
		reg_entry(k_GetWindowTextA, user32, 0x4ad3b143);
		reg_entry(k_GetCursorPos, user32, 0x22a5b635);
		reg_entry(k_MessageBoxA, user32, 0x3ef073da);
		reg_entry(k_FindWindowExA, user32, 0xc0b7cb30);
		reg_entry(k_SetWindowLongA, user32, 0xa836a660);
		reg_entry(k_CallWindowProcA, user32, 0x42a08f04);
		reg_entry(k_CreateWindowExA, user32, 0x36089591);
		reg_entry(k_CreateMenu, user32, 0x1d8971b2);
		reg_entry(k_CreatePopupMenu, user32, 0x90ec2598);
		reg_entry(k_DestroyWindow, user32, 0xb617428b);
		reg_entry(k_DestroyMenu, user32, 0x6bcfbff4);

		// winmm.dll
		winmm.init(0xa43139df);
		reg_entry(k_PlaySoundA, winmm, 0x9d7083c3);
	}

	// initialize encoded pointer list
	void importlist::reset()
	{
		for (int i = 0; i < m_list.max_count(); i++)
		{
			m_list[i].reset();
			m_list.push();
		}
	}

	// allocate a zero-initialized block of memory
	address importlist::calloc (size_t num, size_t size)
	{
		size_t length = size * num;
		address a = malloc(length);
		return memset(a, 0, length);
	}
	
	float importlist::bias (float x, float amt)
	{
		static float prevamt = -1.0f;
		static float prevexp = 0.0f;

		if (prevamt != amt)
			prevexp = log(amt) * -1.4427;

		return pow(x, prevexp);
	}

	float importlist::gain (float x, float amt)
	{
		return x < 0.5f // not thread safe
			? 0.5f * bias(2.0f * x, 1.0f - amt)
			: 1.0f - 0.5f * bias(2.0f - (2.0f * x), 1.0f - amt);
	}

	// write formatted output to buffer
	int importlist::snprintf (char *dest, int maxLength, const char *fmt, ...)
	{
		int length;
		va_list valist;

		va_start(valist, fmt);
		length = vsnprintf(dest, maxLength, fmt, valist);
		va_end(valist);

		if (length < 0)
			dest[(length = maxLength) - 1] = '\0';

		return length;
	}
	
	// print formatted output to console
	int importlist::printf (const char *fmt, ...)
	{
		va_list valist;

		static staticstring<1024> buf;
		
		va_start(valist, fmt);
		vsnprintf(buf, sizeof buf, fmt, valist);
		va_end(valist);
		
		return m_list[k_printf].as<int (*)(const char*)>()(buf);
	}

	// print formatted output to file
	int importlist::fprintf (FILE *file, const char *fmt, ...)
	{
		va_list valist;
		static staticstring<1024> buf;
		
		va_start(valist, fmt);
		vsnprintf(buf, sizeof buf, fmt, valist);
		va_end(valist);

		return m_list[k_fprintf].as<int (*)(FILE*, const char*)>()(file, buf);
	}
};