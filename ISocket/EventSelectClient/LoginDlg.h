
// EventSelectClientDlg.h : ͷ�ļ�
//

#pragma once


// CEventSelectClientDlg �Ի���
class CEventSelectClientDlg : public CDialogEx
{
// ����
public:
	CEventSelectClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CLIENT_LOGIN_DIALOG };
	CIPAddressCtrl m_ctlServIP;
	short m_servPort;
	DWORD m_lservIP;
	CString m_strName;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	virtual void OnOK(void);
	DECLARE_MESSAGE_MAP()
};
