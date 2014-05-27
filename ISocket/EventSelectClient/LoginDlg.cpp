
// EventSelectClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "LoginDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CEventSelectClientDlg 对话框



CEventSelectClientDlg::CEventSelectClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEventSelectClientDlg::IDD, pParent)
{
	m_servPort = SERVERPORT;
	m_strName = "izhuxin";
}

void CEventSelectClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLIENT_IPADDRESS, m_ctlServIP);
	DDX_Text(pDX, IDC_CLIENT_NAME_EDIT, m_strName);
	DDV_MaxChars(pDX, m_strName, 10);
	DDX_Text(pDX, IDC_CLIENT_PORT_EDIT, m_servPort);
	DDV_MinMaxInt(pDX, m_servPort, 5000, 8000);
}

BEGIN_MESSAGE_MAP(CEventSelectClientDlg, CDialogEx)
END_MESSAGE_MAP()


// CEventSelectClientDlg 消息处理程序

BOOL CEventSelectClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_ctlServIP.SetAddress(192, 168, 106, 77);

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CEventSelectClientDlg::OnOK(void)
{
	UpdateData(TRUE);
	CString strIP;
	m_ctlServIP.GetWindowText(strIP);

	if (strIP.IsEmpty() || m_servPort == 0 || m_strName.IsEmpty())
	{
		AfxMessageBox(TEXT("字段不能为空"));
		return ;
	}

	m_ctlServIP.GetAddress(m_lservIP);
	CDialogEx::OnOK();
}