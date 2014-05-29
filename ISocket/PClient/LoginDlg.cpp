#include "stdafx.h"
#include "LoginDlg.h"

CLoginDlg::CLoginDlg(CWnd* pParent) : CDialog(CLoginDlg::IDD, pParent)
{
	m_servPort = SERVERPORT;
	m_strSN = "001";
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLIENT_IPADDRESS, m_ctlServIP);
	DDX_Text(pDX, IDC_CLIENT_PORT_EDIT, m_servPort);
	DDV_MinMaxInt(pDX, m_servPort, 5000, 8000);
	DDX_Text(pDX, IDC_CLIENT_SN_EDIT, m_strSN);
	DDV_MaxChars(pDX, m_strSN, 4);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
END_MESSAGE_MAP()

BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ctlServIP.SetAddress(127, 0, 0, 1);

	return TRUE;
}
void CLoginDlg::OnOK(void)
{
	UpdateData(TRUE);
	CString strIP;
	m_ctlServIP.GetWindowText(strIP);

	if (strIP.IsEmpty() || m_servPort == 0 || m_strSN.IsEmpty())
	{
		AfxMessageBox(TEXT("字段不能为空"));
		return ;
	}
	if (SNLEN != m_strSN.GetLength())
	{
		AfxMessageBox(TEXT("学号为4位数字"));
		return ;
	}

	m_ctlServIP.GetAddress(m_lservIP);
	CDialog::OnOK();
}