#include "../stdafx.h"
#include "syscall_w7.h"

namespace mu
{
	static ntsyscall_7 _syscall7;
	ntsyscall *syscall = &_syscall7;

	ntsyscall_7::ntsyscall_7()
	{
	}

	DWORD ntsyscall_7::m_param = 0;

	__declspec(naked)
	NTSTATUS __fastcall ntsyscall_7::WOW64_FastSysCall (DWORD ecxId, va_list edxArgs)
	{
		__asm
		{
			mov eax, ecx;
			mov ecx, m_param;
			call dword ptr fs:[0xc0];
			add esp, 0x4;
			retn;
		};
	}

	NTSTATUS ntsyscall_7::callGate(DWORD id, ...)
	{
		va_list valist;
		va_start(valist, id);
		return WOW64_FastSysCall(id, valist);
	}
	
	NTSTATUS ntsyscall_7::closeHandle (HANDLE object)
	{
		m_param = 0;
		return callGate(0xc, object);
	}

	NTSTATUS ntsyscall_7::queryInformationFile (HANDLE hfile, PIO_STATUS_BLOCK io, void *buffer, DWORD size, int infoClass)
	{
		m_param = 0;
		return callGate(0xe, hfile, io, buffer, size, infoClass);
	}

	NTSTATUS ntsyscall_7::queryInformationProcess (HANDLE proc, int infoClass, void *buffer, DWORD inSize, PDWORD outSize)
	{
		m_param = 0;
		return callGate(0x16, proc, infoClass, buffer, inSize, outSize);
	}

	NTSTATUS ntsyscall_7::flushInstructionCache (HANDLE proc, void *base, size_t size)
	{
		m_param = 0xc;
		return callGate(0xc2, proc, base, size);
	}

	NTSTATUS ntsyscall_7::openProcess (HANDLE *handle, DWORD accessMask, POBJECT_ATTRIBUTES attr, PCLIENT_ID id)
	{
		m_param = 0;
		return callGate(0x23, handle, accessMask, attr, id);
	}
	
	NTSTATUS ntsyscall_7::queryInformationThread (HANDLE hthread, int infoClass, void *buffer, DWORD inSize, PDWORD outSize)
	{
		m_param = 0;
		return callGate(0x22, hthread, infoClass, buffer, inSize, outSize);
	}

	NTSTATUS ntsyscall_7::setInformationThread (HANDLE hthread, int infoClass, void *buffer, DWORD length)
	{
		m_param = 0;
		return callGate(0xa, hthread, infoClass, buffer, length);
	}

	NTSTATUS ntsyscall_7::terminateThread (HANDLE hthread, NTSTATUS exitStatus)
	{
		m_param = 0;
		return callGate(0x50, hthread, exitStatus);
	}

	NTSTATUS ntsyscall_7::suspendThread (HANDLE hthread, PDWORD returnCode)
	{
		m_param = 0x7;
		return callGate(0x17b, hthread, returnCode);
	}

	NTSTATUS ntsyscall_7::resumeThread (HANDLE hthread, PDWORD returnCode)
	{
		m_param = 0x7;
		return callGate(0x4f, hthread, returnCode);
	}

	NTSTATUS ntsyscall_7::getThreadContext (HANDLE hthread, PCONTEXT ctx)
	{
		m_param = 0;
		return callGate(0xca, hthread, ctx);
	}

	NTSTATUS ntsyscall_7::setThreadContext (HANDLE hthread, PCONTEXT ctx)
	{
		m_param = 0;
		return callGate(0x150, hthread, ctx);
	}

	NTSTATUS ntsyscall_7::openThread (PHANDLE hthread, DWORD access, POBJECT_ATTRIBUTES attrib, PCLIENT_ID cid)
	{
		m_param = 0;
		return callGate(0xfe, hthread, access, attrib, cid);
	}

	NTSTATUS ntsyscall_7::createThreadEx (PHANDLE hthread, DWORD access, POBJECT_ATTRIBUTES attrib, HANDLE proc, void *start, void *param, ULONG flags, ULONG_PTR zeroBits, SIZE_T commit, SIZE_T reserve, void *buffer)
	{
		m_param = 0;
		return callGate(0xa5, hthread, access, attrib, proc, start, param, flags, zeroBits, commit, reserve, buffer);
	}

	NTSTATUS ntsyscall_7::querySystemInformation (int infoClass, void *buffer, DWORD inSize, DWORD *outSize)
	{
		m_param = 0;
		return callGate(0x33, infoClass, buffer, inSize, outSize);
	}

	NTSTATUS ntsyscall_7::readVirtualMemory (HANDLE proc, void *base, void *buffer, DWORD inSize, DWORD *outSize)
	{
		m_param = 0;
		return callGate(0x3c, proc, base, buffer, inSize, outSize);
	}

	NTSTATUS ntsyscall_7::writeVirtualMemory (HANDLE proc, void *base, void *buffer, DWORD inSize, DWORD *outSize)
	{
		m_param = 0;
		return callGate(0x37, proc, base, buffer, inSize, outSize);
	}

	NTSTATUS ntsyscall_7::queryVirtualMemory (HANDLE proc, void *base, int infoClass, void *buffer, DWORD inSize, DWORD *outSize)
	{
		m_param = 0;
		return callGate(0x20, proc, base, infoClass, buffer, inSize, outSize);
	}

	NTSTATUS ntsyscall_7::protectVirtualMemory (HANDLE proc, void *base, size_t inSize, size_t *outSize, DWORD protect, DWORD *oldProtect)
	{
		size_t size = inSize;
		m_param = 0;
		NTSTATUS status = callGate(0x40, proc, base, &size, protect, oldProtect);
		if (outSize != nullptr) *outSize = size;
		return status;
	}

	NTSTATUS ntsyscall_7::allocateVirtualMemory (HANDLE proc, void *base, ULONG_PTR zeroBits, size_t inSize, size_t *outSize, DWORD type, DWORD protect)
	{
		size_t size = inSize;
		m_param = 0;
		NTSTATUS status = callGate(0x15, proc, base, zeroBits, &size, type, protect);
		if (outSize != nullptr) *outSize = size;
		return status;
	}

	NTSTATUS ntsyscall_7::freeVirtualMemory (HANDLE proc, void *base, size_t inSize, size_t *outSize, DWORD type)
	{
		size_t size = inSize;
		m_param = 0;
		NTSTATUS status = callGate(0x1b, proc, base, &size, type);
		if (outSize != nullptr) *outSize = size;
		return status;
	}
	
	NTSTATUS ntsyscall_7::openProcessTokenEx (HANDLE proc, DWORD access, DWORD attributes, PHANDLE handle)
	{
		m_param = 0;
		return callGate(0x2d, proc, access, attributes, handle);
	}
	
	NTSTATUS ntsyscall_7::openThreadTokenEx (HANDLE hthread, DWORD access, BOOLEAN as_self, DWORD attributes, PHANDLE handle)
	{
		m_param = 0;
		return callGate(0x2c, hthread, access, as_self, attributes, handle);
	}
	
	NTSTATUS ntsyscall_7::adjustPrivilegesToken (HANDLE token, BOOLEAN disableAll, PTOKEN_PRIVILEGES newState, DWORD length, PTOKEN_PRIVILEGES prevState, PDWORD outLength)
	{
		m_param = 0;
		return callGate(0x3e, token, disableAll, newState, length, prevState, outLength);
	}
};