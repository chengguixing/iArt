#pragma once
#include "atlwin.h"

class CMyWindow :
	public CWindowImpl<CMyWindow>
{
public:
	DECLARE_WND_CLASS(_T("MyName"))

	BEGIN_MSG_MAP(CMyWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	LRESULT OnCreate(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
public:
	CMyWindow(void);
	~CMyWindow(void);
};
