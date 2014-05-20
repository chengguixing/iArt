#include "StdAfx.h"
#include "MyWindow.h"

CMyWindow::CMyWindow(void)
{
}

CMyWindow::~CMyWindow(void)
{
}
LRESULT CMyWindow::OnPaint(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps;
	HDC hDC =GetDC();
	BeginPaint(&ps);
	TextOut(hDC, 0, 0, _T("hello world"), 11);
	EndPaint(&ps);
	return 0;
}

LRESULT CMyWindow::OnCreate(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}


LRESULT CMyWindow::OnDestroy(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PostQuitMessage(0);
	return 0;
}