
// OverLapServer.h : OverLapServer Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// COverLapServerApp:
// �йش����ʵ�֣������ OverLapServer.cpp
//

class COverLapServerApp : public CWinApp
{
public:
	COverLapServerApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern COverLapServerApp theApp;
