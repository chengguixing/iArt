
// WSAAsyncSelectServerDlg.h : ͷ�ļ�
//

#pragma once
#include "resource.h"
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

	void SaveUserList(CObList& obList);
	void LoadUserList(CObList& obList);

	void SaveOfflineMsg(CObList& obList);
	void LoadOfflineMsg(CObList& obList);


	void SendUserList(void);
	void CopyUserList(CObList& obList);
	void DeleteTempUserList(CObList& obList);


	BOOL UpdateUserList(const CUserInfo& userInfo, CClientSocket* pClientSocket);
	void TransmitMsg(const CChatPacket& packet, CClientSocket* pClientSocket);
	void UpdateServerListCtl(const CObList& obList);


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnRclickClientList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnServerListDeleteMenuitem();
	DECLARE_MESSAGE_MAP()


	CListenSocket* m_pListenSocket;
	CImageList* m_pImageList;

	CObList m_UserList;
	CObList m_OfflineMsgList;
	CObList m_ChatterList;

	CCriticalSection m_csUserList;
	CCriticalSection m_csOfflineList;
	CCriticalSection m_csChatterList;
};
