#include "my.h"

extern CMyWinApp theApp;
extern void printlpEntries(AFX_MSGMAP_ENTRY* lpEntry);

BOOL CCmdTarget::OnCmdMsg(UINT nID, int nCode)
{
	AFX_MSGMAP* pMessageMap;
	AFX_MSGMAP_ENTRY* lpEntry;
	for (pMessageMap = GetMessageMap(); pMessageMap != NULL; pMessageMap = pMessageMap->pBaseMessageMap)
	{
		lpEntry = pMessageMap->lpEntries;
		printlpEntries(lpEntry);
	}

	return FALSE;
}

BOOL CWnd::Create()
{
	cout << "CWnd::Create" << endl;
	return TRUE;
}

BOOL CWnd::CreateEx()
{
	cout << "CWnd::CreateEx" << endl;
	PreCreateWindow();
	return TRUE;
}

BOOL CWnd::PreCreateWindow()
{
	cout << "CWnd::PreCreateWindow" << endl;
	return TRUE;
}


LRESULT CWnd::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	AFX_MSGMAP* pMessageMap;
	AFX_MSGMAP_ENTRY* lpEntry;

	if (nMsg == WM_COMMAND)
	{
		if (OnCommand(wParam, lParam))
		{
			return 1L;
		}
		else
		{
			return (LRESULT)DefWindowProc(nMsg, wParam, lParam);
		}
	}

	pMessageMap = GetMessageMap();


	for (;pMessageMap != NULL; pMessageMap = pMessageMap->pBaseMessageMap)
	{
		lpEntry = pMessageMap->lpEntries;
		printlpEntries(lpEntry);
	}
	return 0;
}

LRESULT CWnd::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

BOOL CWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return OnCmdMsg(0, 0);
}

BOOL CFrameWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return CWnd::OnCommand(wParam, lParam);
}

BOOL CFrameWnd::Create()
{
	cout << "CFrameWnd::Create" << endl;
	CreateEx();
	return TRUE;
}

BOOL CFrameWnd::PreCreateWindow()
{
	cout << "CFrameWnd::PreCreateWindow" << endl;
	return TRUE;
}

CView* CFrameWnd::GetActiveView() const
{
	return m_pViewActive;
}


BOOL CFrameWnd::OnCmdMsg(UINT nID, int nCode)
{
	CView* pView = GetActiveView();
	if (pView->OnCmdMsg(nID, nCode))
	{
		return TRUE;
	}
	if (CWnd::OnCmdMsg(nID, nCode))
	{
		return TRUE;
	}
	CWinApp* pApp = AfxGetApp();
	if (pApp->OnCmdMsg(nID, nCode))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CDocument::OnCmdMsg(UINT nID, int nCode)
{
	if (CCmdTarget::OnCmdMsg(nID, nCode))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CView::OnCmdMsg(UINT nID, int nCode)
{
	if (CWnd::OnCmdMsg(nID, nCode))
	{
		return TRUE;
	}

	BOOL bHandled = FALSE;
	bHandled = m_pDocument->OnCmdMsg(nID, nCode);
	return bHandled;
}

AFX_MSGMAP* CCmdTarget::GetMessageMap() const
{
	return &CCmdTarget::messageMap;
}

AFX_MSGMAP CCmdTarget::messageMap = {
	NULL,
	&CCmdTarget::_messageEntries[0]
};


AFX_MSGMAP_ENTRY CCmdTarget::_messageEntries[] = 
{
	{0, 0, CCmdTargetid, 0, AfxSig_end, 0}
};

BEGIN_MESSAGE_MAP(CWnd, CCmdTarget)
	ON_COMMAND(CWndid, 0)
END_MESSAGE_MAP()


BEGIN_MESSAGE_MAP(CFrameWnd, CWnd)
	ON_COMMAND(CFrameWndid, 0)
END_MESSAGE_MAP()


BEGIN_MESSAGE_MAP(CDocument, CCmdTarget)
	ON_COMMAND(CDocumentid, 0)
END_MESSAGE_MAP()


BEGIN_MESSAGE_MAP(CView, CWnd)
	ON_COMMAND(CViewid, 0)
END_MESSAGE_MAP()


BEGIN_MESSAGE_MAP(CWinApp, CCmdTarget)
	ON_COMMAND(CWinAppid, 0)
END_MESSAGE_MAP()

CWinApp* AfxGetApp()
{
	return theApp.m_pCurrentWinApp;
}

LRESULT AfxWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam, CWnd* pWnd)
{
	return AfxCallWndProc(pWnd, hWnd, nMsg, wParam, lParam);
}

LRESULT AfxCallWndProc(CWnd* pWnd, HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = pWnd->WindowProc(nMsg, wParam, lParam);
	return lResult;
}




