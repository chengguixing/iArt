
// OverLapClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "LookupSocket.h"
#include "resource.h"


// COverLapClientDlg �Ի���
class COverLapClientDlg : public CDialogEx
{
// ����
public:
	COverLapClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_OVERLAPCLIENT_DIALOG };

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
	afx_msg void OnConnectButton();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
public:
	WSAOVERLAPPED* MapEventToOverlapped(int nIndex);
	void ShowMeaning(TCHAR* pMeaning);

protected:
	BOOL InitSocket(void);
	BOOL IsWord(char* pExp, CString& strWord);

protected:
	static DWORD WINAPI WorkerThread(void* pParam);


protected:
	WSAEVENT m_arrEvent[MAX_WAIT_EVENTS];
	DWORD m_nEventTotal;
	SOCKET m_sHost;
	BOOL m_bRunning;
	HANDLE m_hWorkerThread;
	CLookupSocket* m_pLookupSocket;

public:
	CIPAddressCtrl m_ctlServIP;
	CEdit m_ctlWord;
	CEdit m_ctlMeaning;
	short m_sServPort;

	afx_msg void OnLookupButton();
};
