
// HelloMFC.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CHelloMFCApp:
// �йش����ʵ�֣������ HelloMFC.cpp
//

class CHelloMFCApp : public CWinApp
{
public:
	CHelloMFCApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CHelloMFCApp theApp;