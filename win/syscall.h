#pragma once

#include "../stdafx.h"

namespace mu
{
	class address;

	// FastSysCall wrapper for NT system
	class ntsyscall {
	public:
		virtual ~ntsyscall() {}

		virtual NTSTATUS callGate (DWORD id, ...) = 0;
		virtual NTSTATUS closeHandle (HANDLE object) = 0;
		virtual NTSTATUS queryInformationFile (HANDLE hfile, PIO_STATUS_BLOCK io, void *buffer, DWORD size, int infoClass) = 0;
		virtual NTSTATUS queryInformationProcess (HANDLE proc, int infoClass, void *buffer, DWORD inSize, PDWORD outSize) = 0;
		virtual NTSTATUS flushInstructionCache (HANDLE proc, void *base, size_t size) = 0;
		virtual NTSTATUS openProcess (HANDLE *handle, DWORD accessMask, POBJECT_ATTRIBUTES attr, PCLIENT_ID id) = 0;	
		virtual NTSTATUS queryInformationThread (HANDLE hthread, int infoClass, void *buffer, DWORD inSize, PDWORD outSize) = 0;
		virtual NTSTATUS setInformationThread (HANDLE hthread, int infoClass, void *buffer, DWORD length) = 0;
		virtual NTSTATUS terminateThread (HANDLE hthread, NTSTATUS exitStatus) = 0;
		virtual NTSTATUS suspendThread (HANDLE hthread, PDWORD returnCode) = 0;
		virtual NTSTATUS resumeThread (HANDLE hthread, PDWORD returnCode) = 0;
		virtual NTSTATUS getThreadContext (HANDLE hthread, PCONTEXT ctx) = 0;
		virtual NTSTATUS setThreadContext (HANDLE hthread, PCONTEXT ctx) = 0;
		virtual NTSTATUS openThread (PHANDLE hthread, DWORD access, POBJECT_ATTRIBUTES attrib, PCLIENT_ID cid) = 0;
		virtual NTSTATUS createThreadEx (PHANDLE hthread, DWORD access, POBJECT_ATTRIBUTES attrib, HANDLE proc, void *start, void *param, ULONG flags, ULONG_PTR zeroBits, SIZE_T commit, SIZE_T reserve, void *buffer) = 0;
		virtual NTSTATUS querySystemInformation (int infoClass, void *buffer, DWORD inSize, DWORD *outSize) = 0;
		virtual NTSTATUS readVirtualMemory (HANDLE proc, void *base, void *buffer, DWORD inSize, DWORD *outSize) = 0;
		virtual NTSTATUS writeVirtualMemory (HANDLE proc, void *base, void *buffer, DWORD inSize, DWORD *outSize) = 0;
		virtual NTSTATUS queryVirtualMemory (HANDLE proc, void *base, int infoClass, void *buffer, DWORD inSize, DWORD *outSize) = 0;
		virtual NTSTATUS protectVirtualMemory (HANDLE proc, void *base, size_t inSize, size_t *outSize, DWORD protect, DWORD *oldProtect) = 0;
		virtual NTSTATUS allocateVirtualMemory (HANDLE proc, void *base, ULONG_PTR zeroBits, size_t inSize, size_t *outSize, DWORD type, DWORD protect) = 0;
		virtual NTSTATUS freeVirtualMemory (HANDLE proc, void *base, size_t inSize, size_t *outSize, DWORD type) = 0;	
		virtual NTSTATUS openProcessTokenEx (HANDLE proc, DWORD access, DWORD attributes, PHANDLE handle) = 0;	
		virtual NTSTATUS openThreadTokenEx (HANDLE hthread, DWORD access, BOOLEAN as_self, DWORD attributes, PHANDLE handle) = 0;	
		virtual NTSTATUS adjustPrivilegesToken (HANDLE token, BOOLEAN disableAll, PTOKEN_PRIVILEGES newState, DWORD length, PTOKEN_PRIVILEGES prevState, PDWORD outLength) = 0;
			
		void *virtualAllocEx (HANDLE proc, void *adr, size_t size, DWORD type, DWORD protect);
		bool virtualFreeEx (HANDLE proc, void *adr, size_t size, DWORD type);
		bool virtualProtectEx (HANDLE proc, void *adr, size_t size, DWORD newProtect, PDWORD oldProtect);
		bool virtualQueryEx (HANDLE proc, void *adr, PMEMORY_BASIC_INFORMATION mbi, size_t length);
		bool canReadPointer (address ptr, HANDLE proc = PSEUDO_HANDLE);
		bool canWritePointer (address ptr, HANDLE proc = PSEUDO_HANDLE);
		bool adjustPrivilege (DWORD privilege, bool enable, bool currentThread);
		bool getFileSizeEx (HANDLE hfile, PLARGE_INTEGER size);
		bool getThreadExitStatus (HANDLE hthread, NTSTATUS *exitStatus);
		DWORD getFileSize (HANDLE hfile, LPDWORD fileSizeHigh);
		DWORD getProcessId (HANDLE proc);
		DWORD getThreadId (HANDLE hthread);
		HANDLE createRemoteThread (HANDLE proc, void *start, void *param, ULONG flags, LPDWORD threadId = nullptr);
		HANDLE openThreadX (DWORD access, DWORD threadId, bool inherit);		
	};

	extern ntsyscall *syscall;
};