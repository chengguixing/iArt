
// SServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "resource.h"


// CSServerDlg �Ի���

class CClient;


class CSServerDlg : public CDialogEx
{
// ����
public:
	CSServerDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CSServerDlg();

// �Ի�������
	enum { IDD = IDD_SSERVER_DIALOG };
	CIPAddressCtrl m_ctlIP;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	void ShowClientNumberInfo(int nNum);
	void ShowReqDirInfor(CString strDir);
	void ShowFileULInfor(CString strFileName);

	static DWORD WINAPI DirAndFileSizeServiceThread(void* pParam);
	BOOL IsConnecting(void)
	{
		return m_bConning;
	}
protected:
	void InitSocket(void);
	void UnInitSocket(void);

	CClient* GetClient(SOCKET s);
	void DeleteClient(SOCKET s);
	void AddClient(CClient* pClient);
	void DeleteAllClient(void);


	SOCKET GetSocket(void)
	{
		return m_sServer;
	}

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnButtonStartup();
	afx_msg void OnClose();
	BOOL m_bConning;
	CStatic m_ctlTip;
	SOCKET m_sServer;
	HANDLE m_hReqAndData;
	CRITICAL_SECTION m_cs;
	list<CClient*> m_listClient;
};
