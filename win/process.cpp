#include "../stdafx.h"
#include "process.h"
#include "imports.h"
#include "../hash/hash.h"

namespace mu
{
	static processinfo _procinfo;
	processinfo *procinfo = &_procinfo;
	
	// path to parent process
	void currentDirectoryPath(string out)
	{
		// read from process parameters
		wcstombs(out, getPEB()->ProcessParameters->CurrentDirectoryPath);
	}

	// reset data
	void processinfo::reset()
	{
		m_data.reset();
		m_length = 0;
	}

	// clean up snapshot
	void processinfo::free()
	{
		if (m_data.get() != nullptr)
			syscall->virtualFreeEx(PSEUDO_HANDLE, m_data.get(), m_length, MEM_DECOMMIT);
		
		reset();
	}

	// get a snapshot of the process list
	bool processinfo::fill()
	{
		ulong length, outSize = 0;

		// get the total buffer size
		NTSTATUS status = syscall->querySystemInformation(SystemProcessInformation, nullptr, 0, &length);
		
		if (length)
		{
			if (m_data.get() != nullptr)
				free();

			// create and fill our buffer
			m_data = syscall->virtualAllocEx(PSEUDO_HANDLE, nullptr, length, MEM_COMMIT, PAGE_READWRITE);
			status = syscall->querySystemInformation(SystemProcessInformation, m_data.get(), length, &outSize);

			if (NT_SUCCESS(status))
			{
				m_length = length;
				return outSize != 0;
			}
		}
	
		return false;
	}

	// iterate the process info list
	processinfo::iter processinfo::spi_next (iter info)
	{
		if (info == nullptr)
			return nullptr;

		return !info->NextEntryDelta
			? nullptr
			: (iter)((byte*)info + info->NextEntryDelta);
	}
	
	// use hash of process name
	processinfo::const_iter processinfo::findByHash (dword hash) const
	{
		staticstring<64> procname;

		for (iter it = m_data.as<iter>(); it != nullptr; it = spi_next(it))
		{
			// get the proc name
			wcstombs(procname, it->ProcessName.Buffer, it->ProcessName.Length >> 1);

			if (hash_t(procname.lower()) == hash)
				return it;
		}

		return nullptr;
	}

	// use process id
	processinfo::const_iter processinfo::findById (ulong id) const
	{
		for (iter it = m_data.as<iter>(); it != nullptr; it = spi_next(it))
		{
			if (it->ProcessId == id)
				return it;
		}

		return nullptr;
	}

	// most recent remote thread
	ulong process::s_remoteThreadId = 0;

	// copy instance
	process &process::operator= (const process &p)
	{
		m_processId = p.m_processId;
		m_process = p.m_process;

		return (*this);
	}

	// reset data
	void process::reset()
	{
		m_processId = 0;
		m_process = nullptr;
	}

	// always close open handles after use
	void process::close()
	{
		if (!isvalid()) 
			return;

		syscall->closeHandle(m_process);
	}

	// use existing handle
	bool process::initByHandle (HANDLE proc)
	{
		return (m_processId = syscall->getProcessId(m_process = proc)) != 0;
	}

	// use hash of process name
	bool process::initByHash (dword hash)
	{
		auto info = procinfo->findByHash(hash);

		if (info != nullptr)
		{
			m_processId = info->ProcessId;
			return openProcess(PROCESS_ALL_ACCESS, false) != nullptr;
		}

		return false;
	}

	// use process id
	bool process::initById (ulong id)
	{
		if (procinfo->findById(id) != nullptr)
		{
			m_processId = id;
			return openProcess(PROCESS_ALL_ACCESS, false) != nullptr;
		}

		return false;
	}
	
	// allocate page
	address process::alloc (address base, size_t size, DWORD type, DWORD protect) const
	{
		if (!isvalid())
			return nullptr;

		return syscall->virtualAllocEx(m_process, base, size, type, protect);
	}

	// free allocated page
	bool process::free (address base, size_t size, DWORD type) const
	{
		if (!isvalid())
			return false;

		return syscall->virtualFreeEx(m_process, base, size, type);
	}

	// protect region
	bool process::protect (address base, size_t size, DWORD newProtect, PDWORD oldProtect) const
	{
		if (!isvalid())
			return false;

		return syscall->virtualProtectEx(m_process, base, size, newProtect, oldProtect);
	}

	// flush instruction cache
	bool process::flush (address base, size_t size) const
	{
		if (!isvalid())
			return false;

		NTSTATUS status = syscall->flushInstructionCache(m_process, base, size);
		return NT_SUCCESS(status);
	}

	// read remote memory
	bool process::read (address base, address buffer, DWORD size, bool verifySize /*= false*/) const
	{
		DWORD outSize = 0;

		if (!isvalid())
			return false;

		NTSTATUS status = syscall->readVirtualMemory(m_process, base, buffer, size, &outSize);
		
		if (!NT_SUCCESS(status) || !outSize || (verifySize && size != outSize))
			return false;

		return true;
	}

	// write remote memory
	bool process::write (address base, address buffer, DWORD size, bool verifySize /*= false*/) const
	{
		DWORD outSize = 0;

		if (!isvalid())
			return false;

		NTSTATUS status = syscall->writeVirtualMemory(m_process, base, buffer, size, &outSize);
		
		if (!NT_SUCCESS(status) || !outSize || (verifySize && size != outSize))
			return false;

		return true;
	}

	// query process memory
	bool process::mquery (address base, PMEMORY_BASIC_INFORMATION mbi) const
	{
		if (!isvalid())
			return false;

		return syscall->virtualQueryEx(m_process, base, mbi, sizeof MEMORY_BASIC_INFORMATION);
	}

	// query process info
	bool process::pquery (int infoClass, address buffer, DWORD size) const
	{
		DWORD outSize = 0;

		if (!isvalid())
			return false;

		NTSTATUS status = syscall->queryInformationProcess(m_process, infoClass, buffer, size, &outSize);
		return NT_SUCCESS(status) && outSize != 0;
	}

	// halt or resume thread execution
	bool process::suspendThreads (bool state, ulong ignoreId /*= s_remoteThreadId*/)
	{
		// SYSTEM_PROCESS_INFORMATION contains thread info
		auto info = procinfo->findById(m_processId);

		if (info == nullptr)
			return false;
		
		for (ulong i = 0; i < info->ThreadCount; i++)
		{
			// get the unique identifier
			ulong threadId = handle2ulong(info->Threads[i].ClientId.UniqueThread);

			if (threadId != ignoreId)
			{
				HANDLE hthread = syscall->openThreadX(THREAD_ALL_ACCESS, threadId, false);

				if (hthread != nullptr)
				{
					if (state)
						syscall->suspendThread(hthread, nullptr);
					else
						syscall->resumeThread(hthread, nullptr);

					syscall->closeHandle(hthread);
				}
			}
		}

		return true;
	}

	// use secure technique to create thread in remote process
	bool process::executeRemoteThread (address start, LPVOID param, LPDWORD threadId /*= &s_remoteThreadId*/)
	{
		bool result = false;
		
		NTSTATUS status, exit;

		if (isvalid() && start != nullptr)
		{
			// create a suspended thread
			HANDLE hthread = syscall->createRemoteThread(
				m_process,
				start,
				param,
				THREAD_CREATE_FLAGS_CREATE_SUSPENDED|THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER,
				threadId);

			if (hthread != nullptr)
			{
				// make sure it will complete quickly
				DWORD pri = THREAD_PRIORITY_TIME_CRITICAL;
				status = syscall->setInformationThread(hthread, ThreadPriority, &pri, sizeof pri);

				if (NT_SUCCESS(status))
				{
					// begin execution
					status = syscall->resumeThread(hthread, nullptr);

					if (NT_SUCCESS(status))
					{
						// wait for the thread to complete
						imports->waitForSingleObject(hthread, INFINITE);

						// see if we can query the handle
						if (syscall->getThreadExitStatus(hthread, &exit))
							result = true;
					}
				}

				syscall->closeHandle(hthread);
			}
		}

		return result;
	}
	
	bool process::createRemoteLoadLibraryThread (string dll)
	{
		bool result = false;
		
		address param, start;

		if (isvalid() && dll != nullptr)
		{
			size_t length = dll.length();

			param = alloc(nullptr, length + 1, MEM_COMMIT, PAGE_READWRITE);

			if (param != nullptr)
			{
				if (write(param, dll.str(), length))
				{
					start = ntmodule(0x3e003875/*kernel32.dll*/).proc(0x01ed9add/*LoadLibraryA*/);
					result = executeRemoteThread(start, param);
				}

				free(param, length + 1, MEM_DECOMMIT);
			}
		}

		return result;
	}

	// create handle
	HANDLE process::openProcess (ulong access, bool inherit)
	{
		// required to specify the unique process
		CLIENT_ID cid = {
			ulong2handle(m_processId),
			nullptr
		};

		// we use this structure for the OBJ_INHERIT parameter
		OBJECT_ATTRIBUTES attr = {
			sizeof OBJECT_ATTRIBUTES,
			nullptr,
			nullptr,
			inherit ? OBJ_INHERIT : 0,
			nullptr,
			nullptr
		};

		NTSTATUS status = syscall->openProcess(&m_process, access, &attr, &cid);
		return NT_SUCCESS(status) ? m_process : nullptr;
	}
};