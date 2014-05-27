
// EventSelectClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "LoginDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CEventSelectClientDlg �Ի���



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


// CEventSelectClientDlg ��Ϣ�������

BOOL CEventSelectClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_ctlServIP.SetAddress(192, 168, 106, 77);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CEventSelectClientDlg::OnOK(void)
{
	UpdateData(TRUE);
	CString strIP;
	m_ctlServIP.GetWindowText(strIP);

	if (strIP.IsEmpty() || m_servPort == 0 || m_strName.IsEmpty())
	{
		AfxMessageBox(TEXT("�ֶβ���Ϊ��"));
		return ;
	}

	m_ctlServIP.GetAddress(m_lservIP);
	CDialogEx::OnOK();
}