#include "CrashHandler.h"

#ifndef _AddressOfReturnAddress

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC void* _AddressOfReturnAddress(void);
EXTERNC void* _ReturnAddress(void);

#endif


iCrashHandler::iCrashHandler()
{
	NULL;
}

iCrashHandler::~iCrashHandler()
{
	NULL;
}

void iCrashHandler::SetProcessExceptionHandlers()
{
	SetUnhandledExceptionFilter(SehHandler);

	_set_purecall_handler(PureCallHandler);

	_set_new_handler(NewHandler);

	_set_invalid_parameter_handler(InvalidParameterHandler);

	_set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);
	
	signal(SIGABRT, SigabrtHandler);

	signal(SIGINT, SigintHandler);

	signal(SIGTERM, SigtermHandler);
}

void iCrashHandler::SetThreadExceptionHandlers()
{
	set_terminate(TerminateHandler);

	set_unexpected(UnexpectedHandler);

	typedef void (*sign)(int);

	signal(SIGFPE, (sign)SigfpeHandler);

	signal(SIGILL, SigillHandler);

	signal(SIGSEGV, SigsegvHandler);
}


void iCrashHandler::GetExceptionPointers(DWORD dwExceptionCode, EXCEPTION_POINTERS** ppExceptionPointers)
{
	EXCEPTION_RECORD ExceptionRecord;
	CONTEXT ContextRecord;
	memset(&ContextRecord, 0, sizeof(CONTEXT));

#ifdef _X86
	__asm{
		mov dword ptr[ContextRecord.Eax], eax
			mov dword ptr[ContextRecord.Ecx], ecx
			mov dword ptr[ContextRecord.Edx], edx
			mov dword ptr[ContextRecord.Ebx], ebx
			mov dword ptr[ContextRecord.Esi], esi
			mov dword ptr[ContextRecord.Edi], edi
			mov dword ptr[ContextRecord.SegSs], ss
			mov dword ptr[ContextRecord.SegCs], cs
			mov dword ptr[ContextRecord.SegDs], ds
			mov dword ptr[ContextRecord.SegEs], es
			mov dword ptr[ContextRecord.SegFs], fs
			mov dword ptr[ContextRecord.SegGs], gs
			pushfd
			pop [ContextRecord.EFlags]
	}

	ContextRecord.ContextFlags = CONTEXT_CONTROL;
#pragma warning(push)
#pragma warning(disable : 4311)
		ContextRecord.Eip = (ULONG)_ReturnAddress();
		ContextRecord.Esp = (ULONG)_AddressOfReturnAddress();
#pragma warning(pop)
		ContextRecord.Ebp = *((ULONG *)_AddressOfReturnAddress() - 1);

#elif defined(_IA64_) || defined(_AMD64_)
	RtlCaptureContext(&ContextRecord);
#else
	ZeroMemory(&ContextRecord, sizeof(ContextRecord));
#endif

	ZeroMemory(&ExceptionRecord, sizeof(EXCEPTION_RECORD));

	ExceptionRecord.ExceptionCode = dwExceptionCode;
	ExceptionRecord.ExceptionAddress == _ReturnAddress();

	EXCEPTION_RECORD* pExceptionRecord = new EXCEPTION_RECORD;
	memcpy(pExceptionRecord, &ExceptionRecord, sizeof(EXCEPTION_RECORD));
	CONTEXT* pContextRecord = new CONTEXT;
	memcpy(pContextRecord, &ContextRecord, sizeof(CONTEXT));

	*ppExceptionPointers = new EXCEPTION_POINTERS;
	(*ppExceptionPointers)->ExceptionRecord = pExceptionRecord;
	(*ppExceptionPointers)->ContextRecord = pContextRecord;
}

void iCrashHandler::CreateMiniDump(EXCEPTION_POINTERS* pExcPtrs)
{
	HMODULE hDbgHelp = NULL;
	HANDLE hFile = NULL;
	MINIDUMP_EXCEPTION_INFORMATION mei;
	MINIDUMP_CALLBACK_INFORMATION mci;

	hDbgHelp = LoadLibrary(_T("dbghelp.dll"));
	if (NULL == hDbgHelp)
	{
		return ;
	}
	hFile = CreateFile(_T("crashdump.dmp"),
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE)
	{
		return ;
	}

	mei.ThreadId = GetCurrentThreadId();
	mei.ExceptionPointers = pExcPtrs;
	mei.ClientPointers = FALSE;
	mci.CallbackRoutine = NULL;
	mci.CallbackParam = NULL;

	typedef BOOL (WINAPI* LPMINIDUMPWRITEDUMP)(
		HANDLE hProcess,
		DWORD Process,
		HANDLE hFile,
		MINIDUMP_TYPE DumpType,
		CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
		CONST PMINIDUMP_USER_STREAM_INFORMATION UserEncoderParam,
		CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
		);

	LPMINIDUMPWRITEDUMP pfnMiniDumpWriteDump = (LPMINIDUMPWRITEDUMP)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
	if (!pfnMiniDumpWriteDump)
	{
		return ;
	}

	HANDLE hProcess = GetCurrentProcess();
	DWORD dwProcessId = GetCurrentProcessId();

	BOOL bWriteDump = pfnMiniDumpWriteDump(hProcess,
		dwProcessId,
		hFile,
		MiniDumpNormal,
		&mei,
		NULL,
		&mci);
	if (!bWriteDump)
	{
		return ;
	}
	CloseHandle(hFile);
	FreeLibrary(hDbgHelp);
}

LONG WINAPI iCrashHandler::SehHandler(PEXCEPTION_POINTERS pExceptionPtrs)
{
	CreateMiniDump(pExceptionPtrs);

	TerminateProcess(GetCurrentProcess(), 1);

	return EXCEPTION_EXECUTE_HANDLER;
}


void __cdecl iCrashHandler::TerminateHandler()
{
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	CreateMiniDump(pExceptionPtrs);

	TerminateProcess(GetCurrentProcess(), 1);
}


void __cdecl iCrashHandler::UnexpectedHandler()
{
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	CreateMiniDump(pExceptionPtrs);

	TerminateProcess(GetCurrentProcess(), 1);
}


void __cdecl iCrashHandler::PureCallHandler()
{
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	CreateMiniDump(pExceptionPtrs);

	TerminateProcess(GetCurrentProcess(), 1);
}



void __cdecl iCrashHandler::InvalidParameterHandler(const wchar_t* expression,
													const wchar_t* function,
													const wchar_t* file,
													unsigned int line,
													uintptr_t pReserved)
{
	pReserved;

	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	CreateMiniDump(pExceptionPtrs);

	TerminateProcess(GetCurrentProcess(), 1);
}

int __cdecl iCrashHandler::NewHandler(size_t)
{
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	CreateMiniDump(pExceptionPtrs);

	TerminateProcess(GetCurrentProcess(), 1);

	return 0;
}

void iCrashHandler::SigabrtHandler(int)
{
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	CreateMiniDump(pExceptionPtrs);

	TerminateProcess(GetCurrentProcess(), 1);
}

void iCrashHandler::SigfpeHandler(int /*code*/, int subcode)
{
	EXCEPTION_POINTERS* pExceptionPtrs = (PEXCEPTION_POINTERS)_pxcptinfoptrs;

	CreateMiniDump(pExceptionPtrs);

	TerminateProcess(GetCurrentProcess(), 1);
}

void iCrashHandler::SigillHandler(int)
{
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	CreateMiniDump(pExceptionPtrs);

	TerminateProcess(GetCurrentProcess(), 1);
}

void iCrashHandler::SigintHandler(int)
{
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	CreateMiniDump(pExceptionPtrs);

	TerminateProcess(GetCurrentProcess(), 1);
}
void iCrashHandler::SigsegvHandler(int)
{
	PEXCEPTION_POINTERS pExceptionPtrs = (PEXCEPTION_POINTERS)_pxcptinfoptrs;

	CreateMiniDump(pExceptionPtrs);

	TerminateProcess(GetCurrentProcess(), 1);
}

void iCrashHandler::SigtermHandler(int)
{
	EXCEPTION_POINTERS* pExceptionPtrs = NULL;
	GetExceptionPointers(0, &pExceptionPtrs);

	CreateMiniDump(pExceptionPtrs);

	TerminateProcess(GetCurrentProcess(), 1);
}