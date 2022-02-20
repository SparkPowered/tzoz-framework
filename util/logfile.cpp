#include "../stdafx.h"
#include "logfile.h"

#include "../win/imports.h"

namespace mu
{
	static logfile _applog;
	logfile *applog = &_applog;

	bool logfile::init (string path, string filename)
	{
		FILE *f;
		staticstring<MAX_PATH> buf(path);

		// write mode
		static xstring<4> enc_mode(/*w+*/ 0x02, 0x53, 0x247F0000);		

		m_path.reset();
		buf.append(filename);
		m_path.init(buf.str());

		f = imports->fopen(buf, enc_mode.decrypt());

		if (f != nullptr)
		{
			imports->fprintf(f, buf.append("\n"));
			imports->fclose(f);

			return true;
		}

		return false;
	}

	bool logfile::write (string fmt, ...)
	{
		FILE *f;
		va_list va;
		staticstring<512> buf;

		// append mode
		static xstring<4> enc_mode(/*a+*/ 0x02, 0xBD, 0xDC950000);
		
		f = imports->fopen(path(), enc_mode.decrypt());

		if (f != nullptr)
		{
			va_start(va, fmt);
			imports->vsnprintf(buf, sizeof buf, fmt, va);
			va_end(va);

			imports->fprintf(f, buf);
			imports->fclose(f);

			return true;
		}

		return false;
	}
};