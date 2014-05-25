#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CServerAddressDlg 对话框

class CServerAddressDlg : public CDialog
{

public:
	CServerAddressDlg(CWnd* pParent = NULL);   // 标准构造函数

// 对话框数据
	enum { IDD = IDD_SERVER_ADDRESS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


protected:
	virtual void OnOK();
	virtual BOOL OnInitDialog();
public:
	CIPAddressCtrl m_ctlServIP;
	CEdit m_ctlServPort;
public:
	DWORD m_strServIP;
	SHORT m_shServPort;
};
