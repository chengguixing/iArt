#include <iostream>
#include "MFC.h"

class CMyWinApp : public CWinApp
{
public:
	CMyWinApp()
	{
		;
	}
	~CMyWinApp()
	{
		;
	}
	virtual BOOL InitInstance();
};



class CMyFrameWnd : public CFrameWnd
{

	DECLARE_DYNCREATE(CMyFrameWnd)
public:
	CMyFrameWnd();
	~CMyFrameWnd()
	{
		;
	}
	void SayHello()
	{
		std::cout << "Hello CMyFrameWnd" << std::endl;
	}
};

class CMyDoc : public CDocument
{
	DECLARE_DYNCREATE(CMyDoc)
public:
	CMyDoc()
	{
		;
	}
	~CMyDoc()
	{
		;
	}
	void SayHello()
	{
		std::cout << "Hello CMyDoc" << std::endl;
	}
};

class CMyView : public CView
{
	DECLARE_DYNCREATE(CMyView)
public:
	CMyView()
	{
		;
	}
	~CMyView()
	{
		;
	}
	void SayHello()
	{
		std::cout << "Hello CMyView" << std::endl;
	}
};

void PrintAllClasses();