#ifndef _HOOKFUNCTION_H_E575EAEF_13F7_48DD_8408_687D737D567F
#define _HOOKFUNCTION_H_E575EAEF_13F7_48DD_8408_687D737D567F


#include <windows.h>
#include <tchar.h>

class HookFunction
{
public:
	static BOOL Init();
	static void Term();
	static DWORD GetLastInputTime();
public:
	typedef BOOL (*pInit)();
	typedef void (*pTerm)();
	typedef DWORD (*pGetLastInputTime)();

private:
	static pInit m_Init;
	static pTerm m_Term;
	static pGetLastInputTime m_GetLastInputTime;
};

#endif