
// EventSelectServerDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "resource.h"
#include "ServerAddrDlg.h"
#include "ClientSocket.h"


class CUserInfo;
class CClientSocket;

// CEventSelectServerDlg 对话框
class CEventSelectServerDlg : public CDialog
{
// 构造
public:
	CEventSelectServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_EVENTSELECTSERVER_DIALOG };
	CListCtrl m_ctlUserList;

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
	afx_msg void OnStartService();
	afx_msg void OnStopService();

	afx_msg LRESULT OnSocketAccept(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSocketRead(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSocketClose(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	BOOL Init(void);
	BOOL	UpdateUserList(const CString strUserInfo,CClientSocket* pClientSocket);		
	void UpdateListCtrl(void);
private:
	BOOL InitSocket(void);
	void InitListCtrlSetting(void);
	BOOL BeginListen(void);

	void ClearSocketAndEventFromArr(const int nIndex);
	void DeleteClientNode(SOCKET s);
	void DeleteAllClientNodes(void);
	CClientSocket* FindClientNode(const SOCKET s);

	void SendUserList(void);
	CUserInfo ParserUserInfo(const CString strUserInfo);
	void MakeSendUserString(CString& strUserInfoList);
	void HandleOverClientNum(SOCKET sClient);

	
	static DWORD WINAPI ServiceThread(void* pParam);

public:

	SOCKET m_sListen;
	WSAEVENT m_arrEvent[MAX_NUM_EVENTS];
	SOCKET m_arrSocket[MAX_NUM_EVENTS];
	WORD m_nTotalEvent;
	CImageList* m_pImageList;
	BOOL m_bRuning;
	HANDLE m_hEventExit;

private:
	CObList m_UserList;
	CServerAddrDlg m_ServAddrDlg;
	CCriticalSection m_csList;
};
