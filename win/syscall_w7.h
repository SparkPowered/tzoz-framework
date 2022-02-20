#pragma once

#include "../stdafx.h"
#include "syscall.h"

namespace mu
{
	// Windows 7 WOW64 ntsyscall wrapper
	class ntsyscall_7 : public ntsyscall {
	public:
		ntsyscall_7();

		virtual NTSTATUS callGate (DWORD id, ...);
		virtual NTSTATUS closeHandle (HANDLE object);
		virtual NTSTATUS queryInformationFile (HANDLE hfile, PIO_STATUS_BLOCK io, void *buffer, DWORD size, int infoClass);
		virtual NTSTATUS queryInformationProcess (HANDLE proc, int infoClass, void *buffer, DWORD inSize, PDWORD outSize);
		virtual NTSTATUS flushInstructionCache (HANDLE proc, void *base, size_t size);
		virtual NTSTATUS openProcess (HANDLE *handle, DWORD accessMask, POBJECT_ATTRIBUTES attr, PCLIENT_ID id);	
		virtual NTSTATUS queryInformationThread (HANDLE hthread, int infoClass, void *buffer, DWORD inSize, PDWORD outSize);
		virtual NTSTATUS setInformationThread (HANDLE hthread, int infoClass, void *buffer, DWORD length);
		virtual NTSTATUS terminateThread (HANDLE hthread, NTSTATUS exitStatus);
		virtual NTSTATUS suspendThread (HANDLE hthread, PDWORD returnCode);
		virtual NTSTATUS resumeThread (HANDLE hthread, PDWORD returnCode);
		virtual NTSTATUS getThreadContext (HANDLE hthread, PCONTEXT ctx);
		virtual NTSTATUS setThreadContext (HANDLE hthread, PCONTEXT ctx);
		virtual NTSTATUS openThread (PHANDLE hthread, DWORD access, POBJECT_ATTRIBUTES attrib, PCLIENT_ID cid);
		virtual NTSTATUS createThreadEx (PHANDLE hthread, DWORD access, POBJECT_ATTRIBUTES attrib, HANDLE proc, void *start, void *param, ULONG flags, ULONG_PTR zeroBits, SIZE_T commit, SIZE_T reserve, void *buffer);
		virtual NTSTATUS querySystemInformation (int infoClass, void *buffer, DWORD inSize, DWORD *outSize);
		virtual NTSTATUS readVirtualMemory (HANDLE proc, void *base, void *buffer, DWORD inSize, DWORD *outSize);
		virtual NTSTATUS writeVirtualMemory (HANDLE proc, void *base, void *buffer, DWORD inSize, DWORD *outSize);
		virtual NTSTATUS queryVirtualMemory (HANDLE proc, void *base, int infoClass, void *buffer, DWORD inSize, DWORD *outSize);
		virtual NTSTATUS protectVirtualMemory (HANDLE proc, void *base, size_t inSize, size_t *outSize, DWORD protect, DWORD *oldProtect);
		virtual NTSTATUS allocateVirtualMemory (HANDLE proc, void *base, ULONG_PTR zeroBits, size_t inSize, size_t *outSize, DWORD type, DWORD protect);
		virtual NTSTATUS freeVirtualMemory (HANDLE proc, void *base, size_t inSize, size_t *outSize, DWORD type);	
		virtual NTSTATUS openProcessTokenEx (HANDLE proc, DWORD access, DWORD attributes, PHANDLE handle);	
		virtual NTSTATUS openThreadTokenEx (HANDLE hthread, DWORD access, BOOLEAN as_self, DWORD attributes, PHANDLE handle);	
		virtual NTSTATUS adjustPrivilegesToken (HANDLE token, BOOLEAN disableAll, PTOKEN_PRIVILEGES newState, DWORD length, PTOKEN_PRIVILEGES prevState, PDWORD outLength);
		
	private:
		static DWORD m_param;
		static NTSTATUS __fastcall WOW64_FastSysCall (DWORD ecxId, va_list edxArgs);
	};
};