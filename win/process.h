#pragma once

#include "syscall_w7.h"

#include "../util/address.h"
#include "../util/wstring.h"

namespace mu
{
	// process iteration without CreateToolhelp32Snapshot
	// uses NtQuerySystemInformation instead
	class processinfo {
	public:
		typedef PSYSTEM_PROCESS_INFORMATION iter;
		typedef const PSYSTEM_PROCESS_INFORMATION const_iter;
	
		processinfo()
		{
			reset();
		}

		~processinfo()
		{
			free();
		}

		void reset();
		void free();
		bool fill();

		const_iter findByHash (dword hash) const;
		const_iter findById (ulong id) const;

		static iter spi_next (iter info);

	private:
		// buffer size... this may exceed the size of a single page
		ulong m_length;
		// virtual memory for our data
		xaddress m_data;
	};

	extern processinfo *procinfo;

	// wrapper for an nt process
	class process {
	public:
		enum {k_basicinfo = 0, /*PROCESS_BASIC_INFORMATION*/};

		process()
		{
			reset();
		}

		// init by hash of proc name
		process (dword hash)
		{
			reset();
			initByHash(hash);
		}

		explicit process (HANDLE proc)
		{
			reset();
			initByHandle(proc);
		}

		explicit process (const process &p)
		{
			*this = p;
		}

		// clone instance
		process &operator= (const process &p);

		void reset();
		void close();
		bool initByHandle (HANDLE proc);
		bool initByHash (dword hash);
		bool initById (ulong id);
		
		address alloc (address base, size_t size, DWORD type, DWORD protect) const;
		bool free (address base, size_t size, DWORD type) const;
		bool protect (address base, size_t size, DWORD newProtect, PDWORD oldProtect) const;
		bool flush (address base, size_t size) const;
		bool read (address base, address buffer, DWORD size, bool verifySize = false) const;
		bool write (address base, address buffer, DWORD size, bool verifySize = false) const;
		bool mquery (address base, PMEMORY_BASIC_INFORMATION mbi) const;
		bool pquery (int infoClass, address buffer, DWORD size) const;

		bool suspendThreads (bool state, ulong ignoreId = s_remoteThreadId);
		bool executeRemoteThread (address start, LPVOID param, LPDWORD threadId = &s_remoteThreadId);
		bool createRemoteLoadLibraryThread (string dll);
		
		HANDLE openProcess (ulong access, bool inherit);

		// unique identifier
		ulong pid() const
		{
			return m_processId;
		}

		// handle to open process
		HANDLE phandle() const
		{
			return m_process;
		}

		// holding valid process
		bool isvalid() const
		{
			return m_process != nullptr && m_processId != 0;
		}

		// using pseudo handle (-1)
		bool ispseudo() const
		{
			return m_process == PSEUDO_HANDLE;
		}

		static ulong s_remoteThreadId;

	private:
		ulong m_processId;
		HANDLE m_process;
	};

	extern void currentDirectoryPath(string out);
};