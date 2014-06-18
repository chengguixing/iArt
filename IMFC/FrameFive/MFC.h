#define TRUE 1
#define FALSE 0

typedef char* LPSTR;
typedef const char* LPCSTR;


typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef int INT;
typedef unsigned int UINT;
typedef long LONG;


typedef UINT WPARAM;
typedef LONG LPARAM;
typedef LONG LRESULT;
typedef int HWND;

#define WM_COMMAND 0x0111
#define WM_CREATE 0x0001
#define WM_PAINT 0x000f
#define WM_NOTIFY 0x004e


#define CObjectid 0xffff
#define CCmdTargetid 1
#define CWinThreadid 11
#define CWinAppid 111
#define CMyWinAppid 1111

#define CWndid 12
#define CFrameWndid 121
#define CMyFrameWndid 1211
#define CViewid 122
#define CMyViewid 1221
#define CDocumentid 13
#define CMyDocid 131


#include <iostream>
using namespace std;


struct AFX_MSGMAP_ENTRY;

struct AFX_MSGMAP
{
	AFX_MSGMAP* pBaseMessageMap;
	AFX_MSGMAP_ENTRY* lpEntries;
};


#define DECLARE_MESSAGE_MAP() \
	static AFX_MSGMAP_ENTRY _messageEntries[]; \
	static AFX_MSGMAP messageMap; \
	virtual AFX_MSGMAP* GetMessageMap() const;


#define BEGIN_MESSAGE_MAP(theClass, baseClass) \
	AFX_MSGMAP* theClass::GetMessageMap() const \
{return &theClass::messageMap;} \
	AFX_MSGMAP theClass::messageMap = \
{ \
	&(baseClass::messageMap), \
	(AFX_MSGMAP_ENTRY*)&(theClass::_messageEntries)}; \
	AFX_MSGMAP_ENTRY theClass::_messageEntries[] = \
{

#define END_MESSAGE_MAP() \
{0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0} \
};

#include "AFXMSG_.h"


class CObject
{
public:
	CObject()
	{
		;
	}
	~CObject()
	{
		;
	}
};

class CCmdTarget : public CObject
{
public:
	CCmdTarget()
	{
		;
	}
	~CCmdTarget()
	{
		;
	}
	virtual BOOL OnCmdMsg(UINT nID, int nCode);

	DECLARE_MESSAGE_MAP()
};

typedef void (CCmdTarget::*AFX_PMSG)(void);

struct AFX_MSGMAP_ENTRY
{
	UINT nMessage;
	UINT nCode;
	UINT nID;
	UINT nLastID;
	UINT nSig;
	AFX_PMSG pfn;
};

class CWinThread : public CCmdTarget
{
public:
	CWinThread()
	{
		;
	}
	~CWinThread()
	{
		;
	}
	virtual BOOL InitInstance()
	{
		cout << "CWinThread::InitInstance" << endl;
		return TRUE;
	}

	virtual int Run()
	{
		cout << "CWinThread::Run" << endl;
		return 1;
	}
};

class CWnd;

class CWinApp : public CWinThread
{
public:
	CWinApp* m_pCurrentWinApp;
	CWnd* m_pMainWnd;
public:
	CWinApp()
	{
		m_pCurrentWinApp = this;
	}
	~CWinApp()
	{
		;
	}
	virtual BOOL InitApplication()
	{
		cout << "CWinApp::InitApplication" << endl;
		return TRUE;
	}
	virtual BOOL InitInstance()
	{
		cout << "CWinApp::InitInstance" << endl;
		return TRUE;
	}
	virtual int Run()
	{
		cout << "CWinApp::Run" << endl;
		return CWinThread::Run();
	}
	DECLARE_MESSAGE_MAP()
};


typedef void (CWnd::*AFX_PMSGW)(void);


class CDocument : public CCmdTarget
{
public:
	CDocument()
	{
		;
	}
	~CDocument()
	{
		;
	}
	virtual BOOL OnCmdMsg(UINT nID, int nCode);

	DECLARE_MESSAGE_MAP()
};

class CWnd : public CCmdTarget
{
public:
	CWnd()
	{
		;
	}
	~CWnd()
	{
		;
	}
	virtual BOOL Create();
	BOOL CreateEx();
	virtual BOOL PreCreateWindow();
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParem);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};


class CView;

class CFrameWnd : public CWnd
{
public:
	CView* m_pViewActive;

public:
	CFrameWnd()
	{
		;
	}
	~CFrameWnd()
	{
		;
	}
	BOOL Create();
	CView* GetActiveView() const;
	virtual BOOL PreCreateWindow();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCmdMsg(UINT nID, int nCode);

	DECLARE_MESSAGE_MAP()

	friend CView;
};

class CView : public CWnd
{
public:
	CDocument* m_pDocument;

public:
	CView()
	{
		;
	}
	~CView()
	{
		;
	}
	virtual BOOL OnCmdMsg(UINT nID, int nCode);

	DECLARE_MESSAGE_MAP()

	friend CFrameWnd;
};

CWinApp* AfxGetApp();

LRESULT AfxWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam, CWnd* pWnd);
LRESULT AfxCallWndProc(CWnd* pWnd, HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);