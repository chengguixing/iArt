#include <windows.h>
#include <assert.h>




#pragma data_seg(".iShare")
HINSTANCE g_Inst = NULL;
HHOOK g_hHookKbd = NULL;
HHOOK g_hHookMouse = NULL;
DWORD g_dwLastInputTick = 0;
#pragma data_seg()


#pragma comment(linker, "/SECTION:.iShare,RWS")



extern "C" __declspec(dllexport) DWORD GetLastInputTime()
{
	return g_dwLastInputTick;
}

LRESULT CALLBACK iMouseHook(int code, WPARAM wParam, LPARAM lParam)
{
	if (code == HC_ACTION)
	{
		g_dwLastInputTick = GetTickCount();
	}
	return ::CallNextHookEx(g_hHookMouse, code, wParam, lParam);
}

LRESULT CALLBACK iKbdHook(int code, WPARAM wParam, LPARAM lParam)
{
	if (code == HC_ACTION)
	{
		g_dwLastInputTick = GetTickCount();
	}
	return ::CallNextHookEx(g_hHookMouse, code, wParam, lParam);
}

extern "C" __declspec(dllexport) BOOL Init()
{
	if (g_hHookKbd == NULL)
	{
		HINSTANCE hInst = g_Inst;
		g_hHookKbd = SetWindowsHookEx(WH_KEYBOARD, iKbdHook, hInst, 0);
		g_hHookMouse = SetWindowsHookEx(WH_MOUSE, iMouseHook, hInst, 0);
		g_dwLastInputTick = GetTickCount();
	}

	assert(g_hHookKbd);
	assert(g_hHookMouse);
	return TRUE;
}

extern "C" __declspec(dllexport) void Term()
{
	BOOL bRet1 = UnhookWindowsHookEx(g_hHookKbd);
	BOOL bRet2 = UnhookWindowsHookEx(g_hHookMouse);
	assert(bRet1 && bRet2);
}

BOOL APIENTRY DllMain(HANDLE hModule,
					  DWORD ul_reason_for_call,
					  LPVOID lpReserved)
{
	g_Inst = (HINSTANCE)hModule;

	return TRUE;
}