#pragma once

#include "cstring.h"

namespace mu
{
	class logfile {
	public:
		logfile()
		{
			m_path.reset();
		}

		logfile (string path, string filename)
		{
			init(path, filename);
		}

		~logfile()
		{
			m_path.reset();
		}

		string path()
		{
			return m_path.decrypt();
		}

		bool init (string path, string filename);
		bool write (string fmt, ...);

	private:
		xstring<MAX_PATH> m_path;
	};

	extern logfile *applog;
};

#ifndef LOG
	#ifndef LOG_ENABLE
		#define LOG
	#else
		#define LOG mu::applog->write
	#endif
#endif