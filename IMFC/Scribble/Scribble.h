#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"


class CScribbleApp : public CWinApp
{
public:
	CScribbleApp();
public:
	virtual BOOL InitInstance();

	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()
};