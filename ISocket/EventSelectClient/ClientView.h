#pragma once
#include "stdafx.h"
#include "resource.h"
#include "Client.h"
#include "MainFrame.h"
#include "ClientDoc.h"
#include "ClientView.h"
#include "LoginDlg.h"
#include "UserView.h"
#include "ISocket.h"
#include "ServerSocket.h"
#include "UserInfo.h"

class CClientDoc;

class CMidi;class CClientView : public CView
{
protected: // create from serialization only
	CClientView();
	DECLARE_DYNCREATE(CClientView)

	// Attributes
public:
	CClientDoc* GetDocument();

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientView)
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CClientView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	void	UpdateUserList(CString strUserList);			//�����û��б�
	void	RequestPlay(const CUserInfo &userInfo);			//������Ϸ
	BOOL	HandleClientData(PCLIENTPACKET pClientPacket);	//����ͻ�������


protected:
	BOOL	BeginListen(void);								//��ʼ����
	BOOL	ConnectServer(void);							//���ӷ�����
	void	UnInit(void);									//����ʼ��

	CISocket*	FindRenjuSocket(SOCKET s);				//����CRenjuSocketָ��
	void	DeleteRenjuSocket(SOCKET s);					//ɾ��CRenjuSocketָ��
	void	ClearSocketAndEventFromArr(const int nIndex);	//ɾ���ͻ����׽���
	void	ClearSocketAndEventFromArr( SOCKET s );			//ɾ���ͻ����׽��ֺ��¼�����
	void	DestroyClientSocket();							//ɾ���ͻ���ָ��
	void	SendUserInfo(const CUserInfo &userInfo);		//�����û���Ϣ

	void	ResetCoords(void);								//��ʼ������
	void	AnalyseOpponent(int state, int x, int y);		//�����Է�
	void	DrawOneStep(int x, int y);						//���Է�����
	void	InviteNewGame(void);							//�Է������¿���
	BOOL	IsWin(int x, int y);							//�Ƿ��ʤ
	void	DrawPieces(void);								//������


	static DWORD WINAPI RunningThread(void *pParam);		//�����߳�
protected:

	int m_nCoords[DIVISIONS][DIVISIONS];			//��������
	CPoint m_ptLastStep;							//��һ������λ��
	BOOL m_bMyTurn;									//����˳��
	BOOL m_bLinked;									//�������û�������״̬
	BOOL m_bOpponentWin;							//�Է���ʤ
	BOOL m_bInGame;									//��������
	int	m_nColor;									//����ɫ

public:
	CEventSelectClientDlg		m_servDlg;						//��ַ�Ի���
	SOCKET			m_sListen;						//�����׽���
	SOCKET			m_sServer;						//���ӷ������׽���
	SOCKET			m_sClient;						//�ͻ����׽���

	CServerSocket	*m_pServerSocket;				//���ӷ�����ָ��
	CISocket	*m_pListenSocket;				//CRenjuSocketָ�����
	CISocket	*m_pClientSocket;				//CRenjuSocketָ��ͻ���

	WSAEVENT		m_arrEvent[MAX_NUM_EVENTS];		//�׽����¼���������
	SOCKET			m_arrSocket[MAX_NUM_EVENTS];	//�׽�������
	WORD			m_nTotalEvent;					//�����¼�����

	BOOL			m_bRuning;						//�ͻ�������״̬
	HANDLE			m_hEventExit;					//�ͻ����˳��¼�
	CUserInfo		m_UserInfo;						//�û���Ϣ

	// Generated message map functions
protected:
	//{{AFX_MSG(CClientView)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message); 
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClientNewgame();
	afx_msg void OnUpdateClientNewgame(CCmdUI* pCmdUI);
	afx_msg void OnClientSurrender();
	afx_msg void OnUpdateClientSurrender(CCmdUI* pCmdUI);

	afx_msg void OnClientDisconnect();
	afx_msg void OnUpdateClientDisconnect(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg LRESULT OnSocketAccept(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSocketRead(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSocketClose(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

};

#ifndef _DEBUG  // debug version in ClientView.cpp
inline CClientDoc* CClientView::GetDocument()
{ return (CClientDoc*)m_pDocument; }
#endif