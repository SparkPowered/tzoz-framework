#pragma once

#include "stdafx.h"

#include "hash\hash.h"

#include "win\process.h"
#include "win\imports.h"

using namespace mu;

namespace ldr
{
	class imageloader {
	public:
		imageloader();
		imageloader (string path);
		imageloader (string path, string exe, string dll);

		virtual ~imageloader();
		
		virtual void reset(); // default values	
		virtual void init (string path); // read from .ini file
		virtual void init (string path, string exe, string dll); // configure manually
		virtual void frame(); // main routine
		virtual bool map(); // perform the actual loading process		
		virtual void shutdown(); // cleanup

	protected:
		// target process
		process m_process;

		// target bins
		staticstring<64> m_processName, m_imageName;

		// main directory
		staticstring<MAX_PATH> m_appDir;

		// verification
		bool m_didInit, m_didSucceed;
	};
};

