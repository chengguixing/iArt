
// WSAAsyncSelectServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CWSAAsyncSelectServerApp:
// �йش����ʵ�֣������ WSAAsyncSelectServer.cpp
//

class CWSAAsyncSelectServerApp : public CWinApp
{
public:
	CWSAAsyncSelectServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CWSAAsyncSelectServerApp theApp;