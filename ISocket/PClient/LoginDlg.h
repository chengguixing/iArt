#pragma once
#include "afxwin.h"
#include "resource.h"
class CLoginDlg :
	public CDialog
{
public:
	CLoginDlg(CWnd* pParent = NULL);


	enum{IDD = IDD_CLIENT_LOGIN_DIALOG};
	CIPAddressCtrl m_ctlServIP;
	short m_servPort;
	CString m_strSN;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK(void);

public:
	DWORD m_lservIP;
};

