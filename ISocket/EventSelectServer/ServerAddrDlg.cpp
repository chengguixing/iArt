#include "stdafx.h"
#include "ServerAddrDlg.h"


CServerAddrDlg::CServerAddrDlg(CWnd* pParent) : CDialog(CServerAddrDlg::IDD, pParent)
{
	;
}

void CServerAddrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_IPADDRESS, m_ctlServIP);
	DDX_Control(pDX, IDC_SERVER_PORT_EDIT, m_ctlServPort);
}


BEGIN_MESSAGE_MAP(CServerAddrDlg, CDialog)
END_MESSAGE_MAP()


void CServerAddrDlg::OnOK()
{
	m_ctlServIP.GetAddress(m_dwServIP);

	CString strServPort;
	m_ctlServPort.GetWindowText(strServPort);
	m_shServPort = _ttoi(strServPort);

	CDialog::OnOK();
}

BOOL CServerAddrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctlServIP.SetAddress(192, 168, 1, 166);
	CString strServIP;
	strServIP.Format(TEXT("%d"), SERVERPORT);
	m_ctlServPort.SetWindowText(strServIP);

	return TRUE;
}