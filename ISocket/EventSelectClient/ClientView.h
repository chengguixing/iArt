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
	void	UpdateUserList(CString strUserList);			//更新用户列表
	void	RequestPlay(const CUserInfo &userInfo);			//请求游戏
	BOOL	HandleClientData(PCLIENTPACKET pClientPacket);	//处理客户端数据


protected:
	BOOL	BeginListen(void);								//开始监听
	BOOL	ConnectServer(void);							//连接服务器
	void	UnInit(void);									//反初始化

	CISocket*	FindRenjuSocket(SOCKET s);				//查找CRenjuSocket指针
	void	DeleteRenjuSocket(SOCKET s);					//删除CRenjuSocket指针
	void	ClearSocketAndEventFromArr(const int nIndex);	//删除客户端套接字
	void	ClearSocketAndEventFromArr( SOCKET s );			//删除客户端套接字和事件对象
	void	DestroyClientSocket();							//删除客户端指针
	void	SendUserInfo(const CUserInfo &userInfo);		//发送用户信息

	void	ResetCoords(void);								//初始化棋盘
	void	AnalyseOpponent(int state, int x, int y);		//分析对方
	void	DrawOneStep(int x, int y);						//画对方棋子
	void	InviteNewGame(void);							//对方请求新开局
	BOOL	IsWin(int x, int y);							//是否获胜
	void	DrawPieces(void);								//画棋子


	static DWORD WINAPI RunningThread(void *pParam);		//工作线程
protected:

	int m_nCoords[DIVISIONS][DIVISIONS];			//棋盘数组
	CPoint m_ptLastStep;							//上一步棋子位置
	BOOL m_bMyTurn;									//下棋顺序
	BOOL m_bLinked;									//与其它用户的连接状态
	BOOL m_bOpponentWin;							//对方获胜
	BOOL m_bInGame;									//正在下棋
	int	m_nColor;									//棋颜色

public:
	CEventSelectClientDlg		m_servDlg;						//地址对话框
	SOCKET			m_sListen;						//监听套接字
	SOCKET			m_sServer;						//连接服务器套接字
	SOCKET			m_sClient;						//客户端套接字

	CServerSocket	*m_pServerSocket;				//连接服务器指针
	CISocket	*m_pListenSocket;				//CRenjuSocket指针监听
	CISocket	*m_pClientSocket;				//CRenjuSocket指针客户端

	WSAEVENT		m_arrEvent[MAX_NUM_EVENTS];		//套接字事件对象数组
	SOCKET			m_arrSocket[MAX_NUM_EVENTS];	//套接字数组
	WORD			m_nTotalEvent;					//网络事件数量

	BOOL			m_bRuning;						//客户端运行状态
	HANDLE			m_hEventExit;					//客户端退出事件
	CUserInfo		m_UserInfo;						//用户信息

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