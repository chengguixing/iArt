#include <windows.h>
#ifndef   WS_EX_LAYERED 
#define   WS_EX_LAYERED                       0x00080000
#define   LWA_COLORKEY                         0x00000001 
#define   LWA_ALPHA                               0x00000002 
#endif   //   ndef   WS_EX_LAYERED 

HINSTANCE  g_hInstance=NULL;

void OnCreate(HWND hwnd)
{
	//当设置另一种透明方式时，为方便测试，加个静态文本框
	CreateWindowEx(WS_EX_TRANSPARENT, TEXT("STATIC"), TEXT("Hello Static"), 0x80000 | WS_CHILD | WS_VISIBLE | WS_DLGFRAME,
		100, 100, 80, 30, hwnd, (HMENU)1001, g_hInstance, NULL);
	//设置窗口扩展风格
	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | 0x80000);
	typedef BOOL (WINAPI *FSetLayeredWindowAttributes)(HWND, COLORREF, BYTE, DWORD);
	FSetLayeredWindowAttributes SetLayeredWindowAttributes ;
	HINSTANCE hInst = LoadLibrary(TEXT("User32.DLL"));
	SetLayeredWindowAttributes = (FSetLayeredWindowAttributes)GetProcAddress(hInst, "SetLayeredWindowAttributes");
	if (SetLayeredWindowAttributes)
		SetLayeredWindowAttributes(hwnd, RGB(255, 255, 255), 100, 2);//这里设置透明度和透明方式1,2两种
	FreeLibrary(hInst);
}

void OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps = {0};
	HDC hdc = BeginPaint(hwnd, &ps);
	//TextOut(hdc, 50, 50, TEXT("Test transparent"), strlen("Test transparent"));
	EndPaint(hwnd, &ps);
}

void OnLbuttonDown(HWND hwnd,LPARAM lParam)
{
	//无边框窗口的移动
	SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(100);
		break;
	case WM_CREATE:
		OnCreate(hwnd);	
		break;
	case WM_PAINT:
		OnPaint(hwnd);
		break;
	case WM_LBUTTONDOWN:
		OnLbuttonDown(hwnd,lParam);
		break;
	}
	return DefWindowProc(hwnd,uMsg,wParam,lParam);
}

BOOL Register(WNDPROC fWndProc,HINSTANCE hInstance,LPCTSTR szClassName)
{
	WNDCLASSEX wce = {0};
	wce.cbSize = sizeof(wce);
	wce.style = CS_HREDRAW|CS_VREDRAW;
	wce.lpfnWndProc = fWndProc;
	wce.cbClsExtra = 0;
	wce.cbWndExtra = 0;
	wce.hInstance = hInstance;
	wce.hIcon = NULL;
	wce.hCursor = NULL;
	wce.hbrBackground = (HBRUSH)(6);//(HBRUSH)(COLOR_WINDOW+1);
	wce.lpszMenuName = NULL;
	wce.lpszClassName = szClassName;
	wce.hIconSm = NULL;
	ATOM nAtom = RegisterClassEx(&wce);
	if(nAtom == 0) return false;
	return true;
}

HWND Create(LPCTSTR lpClassName, LPCTSTR lpWindowName, HINSTANCE hInstance)
{
	return CreateWindowEx(WS_EX_TOPMOST,
		lpClassName, lpWindowName, WS_POPUP | WS_SYSMENU,
		100, 100, 200, 150,
		NULL, NULL, hInstance, NULL);

}

void Display(HWND hwnd)
{
	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);
}

void Message()
{
	MSG msg={0};
	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
	g_hInstance = hInstance;
	Register(WindowProc, hInstance, TEXT("transparent"));
	HWND hwnd = Create(TEXT("transparent"), TEXT("TEST"), hInstance);
	Display(hwnd);
	Message();
	return 0;
}
