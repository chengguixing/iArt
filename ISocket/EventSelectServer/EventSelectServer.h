
// EventSelectServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CEventSelectServerApp:
// �йش����ʵ�֣������ EventSelectServer.cpp
//

class CEventSelectServerApp : public CWinApp
{
public:
	CEventSelectServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CEventSelectServerApp theApp;