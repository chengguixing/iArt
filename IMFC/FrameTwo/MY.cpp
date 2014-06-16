#include "my.h"

CMyWinApp theApp;


BOOL CMyWinApp::InitInstance()
{
	cout << "CMyWinApp::InitInstance" << endl;
	m_pMainWnd = new CMyFrameWnd;
	return TRUE;
}

CMyFrameWnd::CMyFrameWnd()
{
	cout << "CMyFrameWnd::CMyFrameWnd" << endl;
	Create();
}

int main(void)
{
	CWinApp* pApp = AfxGetApp();
	pApp->InitApplication();
	pApp->InitInstance();
	pApp->Run();

	system("pause");
	return 0;
}