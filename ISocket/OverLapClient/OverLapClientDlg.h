
// OverLapClientDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "LookupSocket.h"
#include "resource.h"


// COverLapClientDlg 对话框
class COverLapClientDlg : public CDialogEx
{
// 构造
public:
	COverLapClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_OVERLAPCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
