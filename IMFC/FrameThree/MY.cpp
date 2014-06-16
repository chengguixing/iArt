#include "my.h"
#include <iostream>
using namespace std;


CMyWinApp theApp;

BOOL CMyWinApp::InitInstance()
{
	m_pMainWnd = new CMyFrameWnd;
	return TRUE;
}

CMyFrameWnd::CMyFrameWnd()
{
	Create();
}

IMPLEMENT_DYNCREATE(CMyFrameWnd, CFrameWnd)
IMPLEMENT_DYNCREATE(CMyDoc, CDocument)
IMPLEMENT_DYNCREATE(CMyView, CView)


void PrintAllClasses()
{
	CRuntimeClass* pClass;

	for (pClass = CRuntimeClass::pFirstClass; pClass != NULL; pClass = pClass->m_pNextClass)
	{
		cout << pClass->m_lpszClassName << endl;
		cout << pClass->m_nObjectSize << endl;
		cout << pClass->m_wSchema << endl;
	}
}


int main(void)
{
	/*
	CWinApp* pApp = AfxGetApp();

	pApp->InitApplication();
	pApp->InitInstance();
	pApp->Run();

	PrintAllClasses();


	CMyDoc* pMyDoc = new CMyDoc;
	CMyView* pMyView = new CMyView;

	cout << pMyDoc->IsKindOf(RUNTIME_CLASS(CMyDoc));
	cout << pMyDoc->IsKindOf(RUNTIME_CLASS(CDocument));
	cout << pMyDoc->IsKindOf(RUNTIME_CLASS(CCmdTarget));
	cout << pMyDoc->IsKindOf(RUNTIME_CLASS(CObject));
	cout << pMyDoc->IsKindOf(RUNTIME_CLASS(CWinApp));
	cout << pMyDoc->IsKindOf(RUNTIME_CLASS(CView));


	cout << pMyView->IsKindOf(RUNTIME_CLASS(CView));
	cout << pMyView->IsKindOf(RUNTIME_CLASS(CObject));
	cout << pMyView->IsKindOf(RUNTIME_CLASS(CWnd));
	cout << pMyView->IsKindOf(RUNTIME_CLASS(CFrameWnd));
	*/

	CRuntimeClass* pClassRef;
	CObject* pOb;
	while (1)
	{
		if ((pClassRef = CRuntimeClass::Load()) == NULL)
		{
			break;
		}
		pOb = pClassRef->CreateObject();
		if (NULL != pOb)
		{
			pOb->SayHello();
		}
	}

	system("pause");
	return 0;
}