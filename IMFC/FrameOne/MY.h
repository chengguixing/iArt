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
};


class CMyFrameWnd : public CFrameWnd
{
public:
	CMyFrameWnd()
	{
		cout << "CMyFrameWnd Constructor" << endl;
	}
	~CMyFrameWnd()
	{
		cout << "CMyFrameWnd Destructor" << endl;
	}
};