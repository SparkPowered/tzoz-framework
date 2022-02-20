#include "stdafx.h"

#include "loader\remoteimageloader.h"

using namespace mu;

BOOL mu_framework_main (bool start);

void Load (string path)
{
	ldr::remoteimageloader ldr(path);

	PRINT("injector v2.0\n"
		"author: tzoz_\n\n");

	ldr.frame();
}

EXTERN_C int mainCRTStartup()
{
	staticstring<MAX_PATH> path;

	// path to this process
	currentDirectoryPath(path.str());

	// initialize
	mu_framework_main(true);

	// inject
	Load(path.str());
	
	// terminate
	mu_framework_main(false);

	return 0;
}

// setup framework dependencies for DLL
BOOL mu_framework_main (bool start)
{
	BOOL result = FALSE;

	if (start)
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

				result = TRUE;
			}
		}
	}
	else
	{
		// call the terminators on exit
		_initterm(__xp_a, __xp_z); 
		_initterm(__xt_a, __xt_z);
		
		result = TRUE;
	}

	return result;
}