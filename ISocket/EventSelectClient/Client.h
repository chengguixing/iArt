

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"  



class CClientApp : public CWinApp
{
public:
	CClientApp();

public:
	virtual BOOL InitInstance();



	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};
