#include "imageloader.h"

namespace ldr
{
	imageloader::imageloader()
	{
		reset();
	}

	imageloader::imageloader (string path)
	{
		reset();
		init(path);
	}

	imageloader::imageloader (string path, string exe, string dll)
	{
		reset();
		init(path, exe, dll);
	}

	imageloader::~imageloader()
	{
		shutdown();
	}

	void imageloader::reset()
	{
		m_process.reset();
		m_processName.reset();
		m_imageName.reset();
		m_appDir.reset();

		m_didInit = m_didSucceed = false;
	}

	void imageloader::init (string path)
	{
		staticstring<MAX_PATH> cfg(m_appDir = path);

		cfg.append("injector.ini");

		imports->getPrivateProfileString("injector", "dll", nullptr, m_imageName, sizeof m_imageName, cfg);
		imports->getPrivateProfileString("injector", "exe", nullptr, m_processName, sizeof m_processName, cfg);

		m_didInit = true;
	}
	
	void imageloader::init (string path, string exe, string dll)
	{
		m_appDir = path;
		m_processName = exe;
		m_imageName = dll;		
		m_didInit = true;
	}

	void imageloader::frame()
	{
		if (!m_didInit)
		{
			PRINT("error: no configuration set\n");
			return;
		}

		PRINT("waiting for %s\n", m_processName.str());

		do
		{
			// use NtQuerySystemInformation to get the list of running processes
			if (!procinfo->fill())
			{
				PRINT("failed to create process snapshot\n");
				return;
			}
			
			// if we found a match then attempt to open a handle to the process
		} while (!m_process.initByHash(hash_t(m_processName.str())));

		PRINT("%s -> id = %d, handle = 0x%x\n\n", m_processName.str(), m_process.pid(), m_process.phandle());

		if (map())
		{
			PRINT("injected %s into %s\n", m_imageName.str(), m_processName.str());
			m_didSucceed = true;
		}
		else
		{
			PRINT("failed to inject %s into %s\n", m_imageName.str(), m_processName.str());
			m_didSucceed = false;
		}

		// give the user some time to read
		imports->sleep(2000);
	}

	bool imageloader::map()
	{
		staticstring<MAX_PATH> dll(m_appDir);
		return m_process.createRemoteLoadLibraryThread(dll.append(m_imageName));
	}

	void imageloader::shutdown()
	{
		//if (m_didSucceed)
			//return;

		m_process.close();
		m_process.reset();
	}
};