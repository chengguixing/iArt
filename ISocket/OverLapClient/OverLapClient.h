
// OverLapClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// COverLapClientApp:
// �йش����ʵ�֣������ OverLapClient.cpp
//

class COverLapClientApp : public CWinApp
{
public:
	COverLapClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern COverLapClientApp theApp;