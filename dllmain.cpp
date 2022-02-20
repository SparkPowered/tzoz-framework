#include "stdafx.h"

#include "win\syscall_w7.h"
#include "win\imports.h"

#include "css\hook\core.h"
#include "css\hook\netvars.h"
#include "css\hook\drawpanel.h"
#include "css\hook\config.h"
#include "css\hook\gui.h"
#include "css\hook\client.h"
#include "css\hook\settings.h"

using namespace mu;

BOOL mu_framework_dllmain (DWORD reason, LPVOID param);

void setup_config (string dir)
{
	static xstring<4> cfgname(/*CSS*/ 0x03, 0xF7, 0xB4ABAA00);
	static xstring<8> cfgfile(/*CSS.ini*/ 0x07, 0x11, 0x5241403A, 0x7C787E00);

	css::cfg->init(cfgname.decrypt(), dir, cfgfile.decrypt());
}

DWORD WINAPI MainThread (LPVOID param)
{
	LOG("start MainThread()\n");
	
	// scan for modules, pointers, etc.
	if (!css::core->init())
	{
		LOG("gamecore::init() failed\n");
		return -1;
	}

	// cache netvar offsets
	css::netvars->init();

	// initialize config
	setup_config((char*)param);
	
	// initialize fonts
	if (!css::draw->init())
	{
		LOG("drawpanel::init() failed\n");
		return -1;
	}

	// initialize cvars
	css::settings->init();

	// initialize client manager
	css::move->init();
	
	// now load the last saved config
	css::cfg->load();

	// setup menu
	css::gui->init();
	css::gui->keyHook();
	
	// hook vtables
	css::core->hook();

	return 0;
}

EXTERN_C BOOL APIENTRY _DllMainCRTStartup (HMODULE module, DWORD reason, LPVOID param)
{
	// configure the framework
	BOOL result = mu_framework_dllmain(reason, param);

	if (result == TRUE && reason == DLL_PROCESS_ATTACH)
	{
		// use NtCreateThreadEx
		HANDLE h = syscall->createRemoteThread(PSEUDO_HANDLE, MainThread, param, THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER);

		// wait until we're finished initializing, then clean up
		imports->waitForSingleObject(h, INFINITE);
		syscall->closeHandle(h);
	}

	return result;
}

#ifdef LOG_ENABLE

void init_main_log (string path)
{
	static xstring<16> enc_output(/*dll_output.log*/ 0x0E, 0x23, 0x47484979, 0x485D5D5A, 0x5E580342, 0x40570000);
	applog->init(path, enc_output.decrypt());
}

#endif

// setup framework dependencies for DLL
BOOL mu_framework_dllmain (DWORD reason, LPVOID param)
{
	BOOL result = FALSE;

	if (reason == DLL_PROCESS_ATTACH)
	{
		// call c initializers
		if (!_initterm_e(__xi_a, __xi_z))
		{
			// call c++ initializers
			_initterm(__xc_a, __xc_z);

			// set debug privilege
			if (syscall->adjustPrivilege(SE_DEBUG_PRIVILEGE, true, false))
			{
				// initialize the PRNG
				mt19937::set(TSC16);

				// cache and encode our needed imports
				imports->init();

			#ifdef LOG_ENABLE
				init_main_log((char*)param);
			#endif

				result = TRUE;
			}
		}
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		// call the terminators on exit
		_initterm(__xp_a, __xp_z); 
		_initterm(__xt_a, __xt_z);
		
		result = TRUE;
	}

	return result;
}