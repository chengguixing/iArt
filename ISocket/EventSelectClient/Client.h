

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
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
