#define BOOL int
#define TRUE 1
#define FALSE 0

#include <iostream>
using namespace std;


class CObject
{
public:
	CObject()
	{
		cout << "CObject Constructor" << endl;
	}
	~CObject()
	{
		cout << "CObject Destructor" << endl;
	}
};


class CCmdTarget : public CObject
{
public:
	CCmdTarget()
	{
		cout << "CCmdTarget Constructor" << endl;
	}
	~CCmdTarget()
	{
		cout << "CCmdTarget Destructor" << endl;
	}
};

class CWinThread : public CCmdTarget
{
public:
	CWinThread()
	{
		cout << "CWinThread Constructor" << endl;
	}
	~CWinThread()
	{
		cout << "CWinThread Destructor" << endl;
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
		cout << "CWinApp Constructor" << endl;
	}
	~CWinApp()
	{
		cout << "CWinApp Destructor" << endl;
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
		return 1;
	}
};

class CDocument : public CCmdTarget
{
public:
	CDocument()
	{
		cout << "CDocument Constructor" << endl;
	}
	~CDocument()
	{
		cout << "CDocument Destructor" << endl;
	}

};

class CWnd : public CCmdTarget
{
public:
	CWnd()
	{
		cout << "CWnd Constructor" << endl;
	}
	~CWnd()
	{
		cout << "CWnd Destructor" << endl;
	}

	virtual BOOL Create();
	BOOL CreateEx();
	virtual BOOL PreCreateWindow();
};

class CFrameWnd : public CWnd
{
public:
	CFrameWnd()
	{
		cout << "CFrameWnd Constructor" << endl;
	}
	~CFrameWnd()
	{
		cout << "CFrameWnd Destructor" << endl;
	}
	BOOL Create();
	virtual BOOL PreCreateWindow();
};

class CView : public CWnd
{
public:
	CView()
	{
		cout << "CView Constructor" << endl;
	}
	~CView()
	{
		cout << "CView Destructor" << endl;
	}
};

CWinApp* AfxGetApp();