#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CServerAddressDlg �Ի���

class CServerAddressDlg : public CDialog
{

public:
	CServerAddressDlg(CWnd* pParent = NULL);   // ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SERVER_ADDRESS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
