#ifndef _CRASHHANDLER_H_B8E485D3_E3F5_4BD9_BCB7_0C4770FD21F1
#define _CRASHHANDLER_H_B8E485D3_E3F5_4BD9_BCB7_0C4770FD21F1

#include <windows.h>
#include <signal.h>
#include <exception>
#include <new.h>
#include <dbghelp.h>
#include <Shellapi.h>
#include <psapi.h>
#include <rtcapi.h>
#include <sys/stat.h>
#include <tchar.h>

class iCrashHandler
{
public:
	iCrashHandler();

	virtual ~iCrashHandler();

	void SetProcessExceptionHandlers();

	void SetThreadExceptionHandlers();

	static void GetExceptionPointers(DWORD dwExceptionCode, EXCEPTION_POINTERS** pExceptionPointers);

	static void CreateMiniDump(EXCEPTION_POINTERS* pExcPtrs);

	static LONG WINAPI SehHandler(PEXCEPTION_POINTERS pExceptionPtrs);

	static void __cdecl TerminateHandler();
	
	static void __cdecl UnexpectedHandler();

	static void __cdecl PureCallHandler();

	static void __cdecl InvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved);

	static int __cdecl NewHandler(size_t);

	static void SigabrtHandler(int);

	static void SigfpeHandler(int /*code*/, int subcode);

	static void SigintHandler(int);

	static void SigillHandler(int);

	static void SigsegvHandler(int);

	static void SigtermHandler(int);
};


#endif