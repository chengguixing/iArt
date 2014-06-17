#include "my.h"

CMyWinApp theApp;


BOOL CMyWinApp::InitInstance()
{
	std::cout << "CMyWinApp::InitInstance" << std::endl;
	m_pMainWnd = new CMyFrameWnd;
	return TRUE;
}

CMyFrameWnd::CMyFrameWnd()
{
	Create();
}


BEGIN_MESSAGE_MAP(CMyWinApp, CWinApp)
ON_COMMAND(CMyWinAppid, 0)
END_MESSAGE_MAP()


BEGIN_MESSAGE_MAP(CMyFrameWnd, CFrameWnd)
	ON_COMMAND(CMyFrameWndid, 0)
END_MESSAGE_MAP()


BEGIN_MESSAGE_MAP(CMyDoc, CDocument)
	ON_COMMAND(CMyDocid, 0)
END_MESSAGE_MAP()


BEGIN_MESSAGE_MAP(CMyView, CView)
	ON_COMMAND(CMyViewid, 0)
END_MESSAGE_MAP()


void printlpEntries(AFX_MSGMAP_ENTRY* lpEntry)
{
	struct  
	{
		int classid;
		char* classname;
	}classinfo[] = 
	{
			CCmdTargetid, "CCmdTarget",
			CWinThreadid, "CWinThread",
			CWinAppid, "CWinApp",
			CMyWinAppid, "CMyWinApp",
			CWndid, "CWnd",
			CFrameWndid, "CFrameWnd",
			CMyFrameWndid, "CMyFrameWnd",
			CViewid, "CView",
			CMyViewid, "CMyView",
			CDocumentid, "CDocument",
			CMyDocid, "CMyDoc",
			0, " "
	};


	for (int i = 0; classinfo[i].classid != 0; ++i)
	{
		if (classinfo[i].classid == lpEntry->nID)
		{
			std::cout << lpEntry->nID << " ";
			std::cout << classinfo[i].classname << std::endl;
			break;
		}
	}
}


void MsgMapPrinting(AFX_MSGMAP* pMessageMap)
{
	for (; pMessageMap != NULL; pMessageMap = pMessageMap->pBaseMessageMap)
	{
		AFX_MSGMAP_ENTRY* lpEntry = pMessageMap->lpEntries;
		printlpEntries(lpEntry);
	}
}


int main(void)
{
	CWinApp* pApp = AfxGetApp();

	pApp->InitApplication();
	pApp->InitInstance();
	pApp->Run();

	CMyDoc* pMyDoc = new CMyDoc;
	CMyView* pMyView = new CMyView;
	CFrameWnd* pMyFrame = (CFrameWnd*)pApp->m_pMainWnd;


	AFX_MSGMAP* pMessageMap = pMyView->GetMessageMap();
	std::cout << std::endl << "CMyView Message Map : " << std::endl;
	MsgMapPrinting(pMessageMap);


	pMessageMap = pMyDoc->GetMessageMap();
	std::cout << std::endl << "CMyDoc Message Map : " << std::endl;
	MsgMapPrinting(pMessageMap);


	pMessageMap = pMyFrame->GetMessageMap();
	std::cout << std::endl << "CMyFrame Message Map : " << std::endl;
	MsgMapPrinting(pMessageMap);


	pMessageMap = pApp->GetMessageMap();
	std::cout << std::endl << "CMyView Message Map : " << std::endl;
	MsgMapPrinting(pMessageMap);


	system("pause");
	return 0;

}