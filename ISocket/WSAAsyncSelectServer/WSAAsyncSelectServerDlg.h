
// WSAAsyncSelectServerDlg.h : ͷ�ļ�
//

#pragma once
class CListenSocket;
class CClientSocket;


// CWSAAsyncSelectServerDlg �Ի���
class CWSAAsyncSelectServerDlg : public CDialogEx
{
// ����
public:
	CWSAAsyncSelectServerDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CWSAAsyncSelectServerDlg();

// �Ի�������
	enum { IDD = IDD_SERVER_MAIN_DIALOG };
	CListCtrl m_ctlUserList;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


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


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
