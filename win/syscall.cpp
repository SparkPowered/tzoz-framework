#include "../stdafx.h"
#include "syscall.h"

#include "../util/address.h"

namespace mu
{
	void *ntsyscall::virtualAllocEx (HANDLE proc, void *adr, size_t size, DWORD type, DWORD protect)
	{
		void *tmp = adr;
		size_t outSize = 0;
		NTSTATUS status = allocateVirtualMemory(proc, &tmp, 0, size, &outSize, type, protect);
		return NT_SUCCESS(status) && outSize != 0 ? tmp : nullptr;
	}

	bool ntsyscall::virtualFreeEx (HANDLE proc, void *adr, size_t size, DWORD type)
	{
		void *tmp = adr;
		size_t outSize = 0;
		NTSTATUS status = freeVirtualMemory(proc, &tmp, size, &outSize, type);
		return NT_SUCCESS(status) && outSize != 0 && tmp != nullptr;
	}

	bool ntsyscall::virtualProtectEx (HANDLE proc, void *adr, size_t size, DWORD newProtect, PDWORD oldProtect)
	{
		void *tmp = adr;
		size_t outSize = 0;
		NTSTATUS status = protectVirtualMemory(proc, &tmp, size, &outSize, newProtect, oldProtect);
		return NT_SUCCESS(status) && outSize != 0 && tmp != nullptr;
	}

	bool ntsyscall::virtualQueryEx (HANDLE proc, void *adr, PMEMORY_BASIC_INFORMATION mbi, size_t length)
	{
		void *tmp = adr;
		DWORD outSize = 0;
		NTSTATUS status = queryVirtualMemory(proc, &tmp, 0, mbi, length, &outSize);
		return NT_SUCCESS(status) && outSize != 0 && tmp != nullptr;
	}

	bool ntsyscall::canReadPointer (address ptr, HANDLE proc /*= PSEUDO_HANDLE*/)
	{
		MEMORY_BASIC_INFORMATION mbi;

		if (ptr == nullptr)
			return false;

		if (!virtualQueryEx(proc, ptr, &mbi, sizeof mbi))
			return false;

		if (mbi.Protect & k_page_offlimits)
			return false;

		return (mbi.Protect & k_page_readable);
	}

	bool ntsyscall::canWritePointer (address ptr, HANDLE proc /*= PSEUDO_HANDLE*/)
	{
		MEMORY_BASIC_INFORMATION mbi;

		if (ptr == nullptr)
			return false;

		if (!virtualQueryEx(proc, ptr, &mbi, sizeof mbi))
			return false;

		if (mbi.Protect & k_page_offlimits)
			return false;

		return (mbi.Protect & k_page_writable);
	}

	HANDLE ntsyscall::createRemoteThread (HANDLE proc, void *start, void *param, ULONG flags, LPDWORD threadId /*= nullptr*/)
	{
		CLIENT_ID cid;
		PS_ATTRIBUTE_LIST attr;

		HANDLE hthread = nullptr;

		attr.TotalLength = sizeof attr;
		attr.Attributes[0].Attribute = PS_ATTRIBUTE_CLIENT_ID;
		attr.Attributes[0].Size = sizeof cid;
		attr.Attributes[0].ValuePtr = &cid;
		attr.Attributes[0].ReturnLength = nullptr;	
	
		NTSTATUS status = createThreadEx(&hthread, STANDARD_RIGHTS_ALL|SPECIFIC_RIGHTS_ALL, nullptr, proc, start, param, flags, 0, 0, 0, &attr);

		if (!NT_SUCCESS(status))
			return nullptr;

		if (threadId != nullptr)
			*threadId = handle2ulong(cid.UniqueThread);

		return hthread;
	}

	HANDLE ntsyscall::openThreadX (DWORD access, DWORD threadId, bool inherit)
	{
		CLIENT_ID cid;
		OBJECT_ATTRIBUTES attr;

		HANDLE hthread = nullptr;

		cid.UniqueProcess = nullptr;
		cid.UniqueThread = ulong2handle(threadId);

		attr.Length = sizeof OBJECT_ATTRIBUTES;
		attr.RootDirectory = nullptr;
		attr.ObjectName = nullptr;
		attr.Attributes = inherit ? OBJ_INHERIT : 0;
		attr.SecurityDescriptor = nullptr;
		attr.SecurityQualityOfService = nullptr;

		NTSTATUS status = openThread(&hthread, access, &attr, &cid);
		return NT_SUCCESS(status) ? hthread : nullptr;
	}

	bool ntsyscall::getThreadExitStatus (HANDLE hthread, NTSTATUS *exitStatus)
	{
		THREAD_BASIC_INFORMATION tbi;

		NTSTATUS status = queryInformationThread(hthread, ThreadBasicInformation, &tbi, sizeof tbi, nullptr);

		if (!NT_SUCCESS(status))
		{
			*exitStatus = 0;
			return false;
		}
		else
		{
			*exitStatus = tbi.ExitStatus;
			return true;
		}
	}

	DWORD ntsyscall::getThreadId (HANDLE hthread)
	{
		THREAD_BASIC_INFORMATION tbi;
		NTSTATUS status = queryInformationThread(hthread, ThreadBasicInformation, &tbi, sizeof tbi, nullptr);
		return NT_SUCCESS(status) ? handle2ulong(tbi.ClientId.UniqueThread) : 0;
	}

	bool ntsyscall::adjustPrivilege (DWORD privilege, bool enable, bool currentThread)
	{
		HANDLE token;
		NTSTATUS status;
		ULONG retnLength;	
		TOKEN_PRIVILEGES newState, oldState;

		// open token handle
		status = currentThread
			? openThreadTokenEx(PSEUDO_HANDLE, TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, FALSE, 0, &token)
			: openProcessTokenEx(PSEUDO_HANDLE, TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, 0, &token);

		if (!NT_SUCCESS(status))
			return false;
		
		// set privileges for this token
		oldState.PrivilegeCount = 1;
		newState.PrivilegeCount = 1;
		newState.Privileges[0].Luid.LowPart = privilege;
		newState.Privileges[0].Luid.HighPart = 0;
		newState.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;
		status = adjustPrivilegesToken(token, FALSE, &newState, sizeof TOKEN_PRIVILEGES, &oldState, &retnLength);

		closeHandle(token);

		if (!NT_SUCCESS(status))
			return false;

		return oldState.PrivilegeCount != 0 ? oldState.Privileges[0].Attributes & SE_PRIVILEGE_ENABLED : enable;
	}

	bool ntsyscall::getFileSizeEx (HANDLE hfile, PLARGE_INTEGER size)
	{
		IO_STATUS_BLOCK io;
		FILE_STANDARD_INFO info;

		if (!isConsoleHandle(hfile))
		{
			NTSTATUS status = queryInformationFile(hfile, &io, &info, sizeof info, FileStandardInformation);

			if (NT_SUCCESS(status))
			{
				*size = info.EndOfFile;
				return true;
			}
		}

		return false;
	}

	DWORD ntsyscall::getFileSize (HANDLE hfile, LPDWORD fileSizeHigh)
	{
		LARGE_INTEGER size;

		if (!getFileSizeEx(hfile, &size))
			return INVALID_FILE_SIZE;

		if (fileSizeHigh)
			*fileSizeHigh = size.u.HighPart;

		return size.u.LowPart;
	}

	DWORD ntsyscall::getProcessId (HANDLE proc)
	{
		PROCESS_BASIC_INFORMATION pbi;
		memset(&pbi, 0, sizeof pbi);

		NTSTATUS status = queryInformationProcess(proc, 0, &pbi, sizeof pbi, nullptr);
		return NT_SUCCESS(status) ? pbi.UniqueProcessId : 0;
	}
};