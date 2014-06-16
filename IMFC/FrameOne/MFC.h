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
};

class CWinApp : public CWinThread
{
public:
	CWinApp* m_pCurrentWinApp;

public:
	CWinApp()
	{
		m_pCurrentWinApp = this;
		cout << "CWinThread Constructor" << endl;
	}
	~CWinApp()
	{
		cout << "CWinThread Destructor" << endl;
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