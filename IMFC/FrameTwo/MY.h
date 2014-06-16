#include <iostream>
using namespace std;
#include "mfc.h"


class CMyWinApp : public CWinApp
{
public:
	CMyWinApp()
	{
		cout << "CMyWinApp Constructor" << endl;
	}
	~CMyWinApp()
	{
		cout << "CMyWinApp Destructor" << endl;
	}
	virtual BOOL InitInstance();
};


class CMyFrameWnd : public CFrameWnd
{
public:
	CMyFrameWnd();
	~CMyFrameWnd()
	{
		cout << "CMyFrameWnd Destructor" << endl;
	}
};