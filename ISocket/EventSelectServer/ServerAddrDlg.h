#pragma once
#include "afxwin.h"
#include "resource.h"
class CServerAddrDlg :
	public CDialog
{
public:
	CServerAddrDlg(CWnd* pParent = NULL);

	enum{IDD = IDD_SERVER_ADDRESS_DLG};
	CIPAddressCtrl m_ctlServIP;
	CEdit m_ctlServPort;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	DWORD m_dwServIP;
	SHORT m_shServPort;
};

