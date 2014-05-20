// ATLTest.cpp : WinMain µÄÊµÏÖ


#include "stdafx.h"
#include "resource.h"
#include "ATLTest_i.h"
#include "MyWindow.h"


class CATLTestModule : public CAtlExeModuleT< CATLTestModule >
{
public :
	DECLARE_LIBID(LIBID_ATLTestLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATLTEST, "{A6D316A2-3D66-47D3-8C57-4B6029064FBC}")
};

CATLTestModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	CMyWindow Window;
	Window.Create(NULL, CWindow::rcDefault, _T("hello"), WS_OVERLAPPEDWINDOW);
	Window.ShowWindow(nShowCmd);
	Window.UpdateWindow();
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
    return _AtlModule.WinMain(nShowCmd);
}

