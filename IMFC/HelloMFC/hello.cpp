#include "stdafx.h"
#include "hello.h"
#include "resource.h"

CMyWinApp theApp;


BOOL CMyWinApp::InitInstance()
{
	m_pMainWnd = new CMyFrameWnd();
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

BOOL CMyWinApp::OnIdle(LONG lCount)
{
	CMyFrameWnd* pWnd = (CMyFrameWnd*)m_pMainWnd;
	pWnd->IdleTimeHandler(lCount);

	return TRUE;
}


CMyFrameWnd::CMyFrameWnd()
{
	Create(NULL, TEXT("Hello MFC"), WS_OVERLAPPEDWINDOW, rectDefault, NULL, TEXT("MainMenu"));

}

BEGIN_MESSAGE_MAP(CMyFrameWnd, CFrameWnd)
	ON_COMMAND(IDM_ABOUT, OnAbout)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CMyFrameWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;

	GetClientRect(rect);
	dc.SetTextAlign(TA_BOTTOM | TA_CENTER);

	::LineDDA(rect.right / 2, 0, rect.right / 2, rect.bottom / 2, (LINEDDAPROC)LineDDACallback, (LPARAM)(LPVOID)&dc);
}

VOID CALLBACK CMyFrameWnd::LineDDACallback(int x, int y, LPARAM lpdc)
{
	static TCHAR szText[] = TEXT("Hello, MFC");

	((CDC*)lpdc)->TextOut(x, y, szText, sizeof(szText) - 1);
	/*
	for (int i = 1; i < 50000; ++i)
	{
		;
	}
	*/
}

void CMyFrameWnd::OnAbout()
{
	CDialog about(TEXT("AboutBox"), this);
	about.DoModal();
}

void CMyFrameWnd::IdleTimeHandler(LONG lCount)
{
	CString str;
	CRect rect(10, 10, 200, 30);
	CDC* pDC = new CClientDC(this);

	str.Format(TEXT("%010d"), lCount);
	pDC->DrawText(str, &rect, DT_LEFT | DT_TOP);
}