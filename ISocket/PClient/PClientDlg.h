
// PClientDlg.h : ͷ�ļ�
//

#pragma once
#include "resource.h"
#include "LoginDlg.h"


class CClientSocket;

// CPClientDlg �Ի���
class CPClientDlg : public CDialogEx
{
// ����
public:
	CPClientDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CPClientDlg();
	enum state
	{
		LOGIN,
		DOING,
		DONE,
		DISCON,
		UNKNOWN
	};
// �Ի�������
	enum { IDD = IDD_PCLIENT_DIALOG };

	CStatic m_ctlSep;
	CStatic m_ctlTime;
	CStatic m_ctlSN;
	CStatic m_ctlName;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

protected:
	BOOL ConnectServerAndGetPaper(void);
	void InitPaper(void);
	void BeginExam(void);
	void InitPaperTitle(void);
	void SendResult(void);
protected:
	CClientSocket* m_pClientSocket;
	CObList m_listItem;
	CRect m_rect;
	state m_state;
	CString m_strName;
	CLoginDlg m_loginDlg;
	BOOL m_bPulsePacket;

protected:
	int m_offsetHeight;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};
