
// WSAAsyncSelectServerDlg.h : 头文件
//

#pragma once
class CListenSocket;
class CClientSocket;


// CWSAAsyncSelectServerDlg 对话框
class CWSAAsyncSelectServerDlg : public CDialogEx
{
// 构造
public:
	CWSAAsyncSelectServerDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CWSAAsyncSelectServerDlg();

// 对话框数据
	enum { IDD = IDD_SERVER_MAIN_DIALOG };
	CListCtrl m_ctlUserList;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


public:
	BOOL Init(void);
	BOOL StartService(void);
	BOOL ProcessPendingRead(CArchive* pArchiveIn, CArchive* pArchiveOut, CClientSocket* pClientSocket);
	void ProcessPendingAccept(void);

	void ReleaseChatter(CClientSocket* pClientSocket);
	void DeleteChatter(CClientSocket* pClientSocket);
	void DeleteAllChatter(void);

protected:
	void InitListCtrlSetting(void);
	void InitUserList(void);

	void SaveUserList(CObject& obList);
	void LoadUserList(CObject& obList);

	void SaveOfflineMsg(CObject& obList);
	void LoadOfflineMsg(CObject& obList);


	void SendUserList(void);
	void CopyUserList(CObject& obList);
	void DeleteTempUserList(CObject& obList);


	BOOL UpdateUserList(const CUserInfo& userInfo, CClientSocket* pClientSocket);
	void TransmitMsg(const CChatPacket& packet, CClientSocket* pClientSocket);
	void UpdateServerListCtl(const CObject& obList);


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


	CListenSocket* m_pListenSocket;
	CImageList* m_pImageList;

	CObject m_UserList;
	CObject m_OfflineMsgList;
	CObject m_ChatterList;

	CCriticalSection m_csUserList;
	CCriticalSection m_csOfflineList;
	CCriticalSection m_csChatterList;
public:
	afx_msg void OnDestroy();
};
