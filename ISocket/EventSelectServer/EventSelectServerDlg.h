
// EventSelectServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "resource.h"


class CUserInfo;

// CEventSelectServerDlg �Ի���
class CEventSelectServerDlg : public CDialog
{
// ����
public:
	CEventSelectServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_EVENTSELECTSERVER_DIALOG };
	CListCtrl m_ctlUserList;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStartService();
	afx_msg void OnStopService();

	afx_msg LRESULT OnSocketAccept(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSocketRead(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSocketClose(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	BOOL Init(void);
	BOOL UpdateUserList(const CString strUserInfo, CClientSocket* pClientSocket);
	void UpdateListCtrl(void);
	
};
