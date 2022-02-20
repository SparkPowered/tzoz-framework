#pragma once

#include "../stdafx.h"

#include "../win/ntmodule.h"

#include "../util/staticlist.h"

namespace mu
{
	class importlist {
	public:
		// static locations of each import
		enum index_t
		{
			// kernel32.dll
			k_Sleep,
			k_GetPrivateProfileStringA,
			k_WritePrivateProfileStringA,
			k_WaitForMultipleObjectsEx,
			k_LoadLibraryExA,
			k_CreateFileA,
			k_GetCompressedFileSizeA,
			k_GetFileAttributesA,
			k_ReadFile,
			k_GetTickCount,

			// msvcrt.dll
			k_printf,
			k_fopen,
			k_fclose,
			k_fprintf,
			k_vsnprintf,
			k_malloc,
			k_free,
			k_atan,
			k_atan2,
			k_exp,
			k_log,
			k_system,

			// user32.dll
			k_GetActiveWindow,
			k_GetWindowTextA,
			k_GetCursorPos,
			k_MessageBoxA,
			k_FindWindowExA,
			k_SetWindowLongA,
			k_CallWindowProcA,
			k_CreateWindowExA,
			k_CreateMenu,
			k_CreatePopupMenu,
			k_DestroyWindow,
			k_DestroyMenu,

			// winmm.dll
			k_PlaySoundA,

			// num imports
			k_importCount
		};

		importlist()
		{
			reset();
		}

		~importlist()
		{
			reset();
		}

		void init();
		void reset();

		address calloc (size_t num, size_t size);

		float bias (float x, float amt);
		float gain (float x, float amt);
		
		int snprintf (char *dest, int maxLength, const char *fmt, ...);
		int printf (const char *fmt, ...);
		int fprintf (FILE *file, const char *fmt, ...);
		void splitpath (char *inpath, char *drive, char *dir, char *fname, char *ext);
	
		// register import
		void reg_entry (int index, ntmodule &m, DWORD hash)
		{
			m_list.insert(index, xaddress(m.proc(hash)));
		}

		// get import address
		address get_entry (dword index)
		{
			return m_list[index].get();
		}

		// allocate data on the heap
		address malloc (size_t size)
		{
			return m_list[k_malloc].as<void *(*)(size_t)>()(size);
		}

		// free data from the heap
		void free (address data)
		{
			if (data == nullptr)
				return;

			m_list[k_free].as<void (*)(void*)>()(data);
		}

		// execute a command
		int system (const char *cmd)
		{
			return m_list[k_system].as<int (*)(const char*)>()(cmd);
		}

		// write formatted output using a pointer to a list of arguments
		int vsnprintf (char *dest, size_t length, const char *fmt, va_list valist)
		{
			return m_list[k_vsnprintf].as<int (*)(char*, size_t, const char*, va_list)>()(dest, length, fmt, valist);
		}

		// inverse tangent
		double atan (double x)
		{
			return m_list[k_atan].as<double (*)(double)>()(x);
		}
		
		double atan2 (double y, double x)
		{
			return m_list[k_atan2].as<double (*)(double, double)>()(y, x);
		}
		
		// inverse sine
		double asin (double x)
		{
			return atan(x / (1 + sqrt(1 - x*x))) * 2.0;
		}

		// inverse cosine
		double acos (double x)
		{
			register double y = sqrt((float)(1.-x*x));
			return atan2(y, x);
		}

		// e^x
		double exp (double x)
		{
			return m_list[k_exp].as<double (*)(double)>()(x);
		}

		// ln(x)
		double log (double x)
		{
			return m_list[k_log].as<double (*)(double)>()(x);
		}

		// e^(y * ln(x)) = e^ln(x^y) = x^y
		double pow (double x, double y)
		{
			return exp(y * log(x));
		}

		// create a stdstream file
		FILE *fopen (const char *file, const char *mode)
		{
			return m_list[k_fopen].as<FILE *(*)(const char*, const char*)>()(file, mode);
		}

		// destroy a stdstream file
		int fclose (FILE *file)
		{
			return m_list[k_fclose].as<int (*)(FILE*)>()(file);
		}

		// delay execution
		void sleep (DWORD ms)
		{
			m_list[k_Sleep].as<void (__stdcall*)(DWORD)>()(ms);
		}
	
		// get value from file config
		DWORD getPrivateProfileString (char *app, char *key, char *def, char *buffer, DWORD size, char *fileName)
		{
			return m_list[k_GetPrivateProfileStringA].as<DWORD (__stdcall*)(char*, char*, char*, char*, DWORD, char*)>()
				(app, key, def, buffer, size, fileName);
		}

		// write value to file config
		DWORD writePrivateProfileString (char *app, char *key, char *buffer, char *fileName)
		{
			return m_list[k_WritePrivateProfileStringA].as<DWORD (__stdcall*)(char*, char*, char*, char*)>()
				(app, key, buffer, fileName);
		}

		DWORD waitForMultipleObjectsEx (DWORD count, const HANDLE *handles, BOOL wait_all, DWORD timeout, BOOL alertable)
		{
			return m_list[k_WaitForMultipleObjectsEx].as<DWORD (__stdcall*)(DWORD, const HANDLE*, BOOL, DWORD, BOOL)>()
				(count, handles, wait_all, timeout, alertable);
		}

		DWORD waitForSingleObject (HANDLE object, DWORD timeout)
		{
			return waitForMultipleObjectsEx(1, &object, FALSE, timeout, FALSE);
		}
	
		HMODULE loadLibraryEx (LPCSTR libname, HANDLE hfile, DWORD flags)
		{
			return m_list[k_LoadLibraryExA].as<HMODULE (__stdcall*)(LPCSTR, HANDLE, DWORD)>()(libname, hfile, flags);
		}

		HANDLE createFile (char *filename, DWORD access, DWORD sharing, void *sec, DWORD creation, DWORD attrib, HANDLE htemplate)
		{
			return m_list[k_CreateFileA].as<HANDLE (__stdcall*)(char*, DWORD, DWORD, void*, DWORD, DWORD, HANDLE)>()
				(filename, access, sharing, sec, creation, attrib, htemplate);
		}
	
		DWORD compressedFileSize (LPCSTR name, LPDWORD fileSizeHigh)
		{
			return m_list[k_GetCompressedFileSizeA].as<DWORD (__stdcall*)(LPCSTR, LPDWORD)>()(name, fileSizeHigh);
		}

		DWORD fileAttributes (LPCSTR name)
		{
			return m_list[k_GetFileAttributesA].as<DWORD (__stdcall*)(LPCSTR)>()(name);
		}

		// store contents of file in buffer
		BOOL readFile (HANDLE hfile, LPVOID buffer, DWORD inSize, LPDWORD outSize, LPOVERLAPPED overlapped)
		{
			return m_list[k_ReadFile].as<BOOL (__stdcall*)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED)>()
				(hfile, buffer, inSize, outSize, overlapped);
		}

		DWORD tickCount()
		{
			return m_list[k_GetTickCount].as<DWORD (__stdcall*)()>()();
		}

		HWND activeWindow()
		{
			return m_list[k_GetActiveWindow].as<HWND (__stdcall*)()>()();
		}

		BOOL cursorPos (LPPOINT point)
		{
			return m_list[k_GetCursorPos].as<BOOL (__stdcall*)(LPPOINT)>()(point);
		}

		int windowText (HWND window, LPSTR str, int max)
		{
			return m_list[k_GetWindowTextA].as<int (__stdcall*)(HWND, LPSTR, int)>()(window, str, max);
		}
		
		// display dialog box
		int messageBox (HWND wnd, LPCSTR txt, LPCSTR caption, UINT type)
		{
			return m_list[k_MessageBoxA].as<int (__stdcall*)(HWND, LPCSTR, LPCSTR, UINT)>()(wnd, txt, caption, type);
		}

		// get handle to window
		HWND findWindowEx (HWND parent, HWND child, LPCSTR className, LPCSTR title)
		{
			return m_list[k_FindWindowExA].as<HWND (__stdcall*)(HWND, HWND, LPCSTR, LPCSTR)>()(parent, child, className, title);
		}

		// change window attribute (x64 compatible)
		LONG setWindowLong (HWND wnd, int index, LONG newLong)
		{
			return m_list[k_SetWindowLongA].as<LONG_PTR (__stdcall*)(HWND, int, LONG)>()(wnd, index, newLong);
		}
		
		// pass to the window
		LRESULT callWindowProc (WNDPROC prevFunc, HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
		{
			return m_list[k_CallWindowProcA].as<LRESULT (__stdcall*)(WNDPROC, HWND, UINT, WPARAM, LPARAM)>()
				(prevFunc, wnd, msg, wparam, lparam);
		}

		HWND createWindowEx (DWORD exStyle, LPCSTR className, LPCSTR windowName, DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE inst, LPVOID param)
		{
			return m_list[k_CreateWindowExA].as<HWND (__stdcall*)(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID)>()
				(exStyle, className, windowName, style, x, y, w, h, parent, menu, inst, param);
		}

		HMENU createMenu()
		{
			return m_list[k_CreateMenu].as<HMENU (__stdcall*)()>()();
		}

		HMENU createPopupMenu()
		{
			return m_list[k_CreatePopupMenu].as<HMENU (__stdcall*)()>()();
		}

		BOOL destroyWindow (HWND wnd)
		{
			return m_list[k_DestroyWindow].as<BOOL (__stdcall*)(HWND)>()(wnd);
		}

		BOOL destroyMenu (HMENU menu)
		{
			return m_list[k_DestroyMenu].as<BOOL (__stdcall*)(HMENU)>()(menu);
		}

		BOOL playSound (LPCSTR sound, HMODULE module, DWORD flags)
		{
			return m_list[k_PlaySoundA].as<BOOL (__stdcall*)(LPCSTR, HMODULE, DWORD)>()(sound, module, flags);
		}

	private:
		staticlist<xaddress, k_importCount> m_list;
	};

	extern importlist *imports;
};

#ifndef PRINT
	#ifndef PRINT_ENABLE
		#define PRINT
	#else
		#define PRINT mu::imports->printf
	#endif
#endif