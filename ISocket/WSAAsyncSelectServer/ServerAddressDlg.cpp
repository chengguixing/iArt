// ServerAddressDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WSAAsyncSelectServer.h"
#include "ServerAddressDlg.h"
#include "afxdialogex.h"


// CServerAddressDlg �Ի���


CServerAddressDlg::CServerAddressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerAddressDlg::IDD, pParent)
{

}


void CServerAddressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_IPADDRESS, m_ctlServIP);
	DDX_Control(pDX, IDC_SERVER_PORT_EDIT, m_ctlServPort);
}


BEGIN_MESSAGE_MAP(CServerAddressDlg, CDialog)
END_MESSAGE_MAP()


void CServerAddressDlg::OnOK() 
{
	//��ȡ������IP�Ͷ˿�
	m_ctlServIP.GetAddress(m_strServIP);

	CString strServPort;
	m_ctlServPort.GetWindowText(strServPort);
	m_shServPort = atoi(strServPort);		//���ַ���ת��ΪSHORT

	CDialog::OnOK();
}

BOOL CServerAddressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctlServIP.SetAddress(127, 0, 0, 1);
	CString strServIP;
	strServIP.Format("%d", SERVERPORT);
	m_ctlServPort.SetWindowText(strServIP);

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
