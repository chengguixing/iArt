#include "stdafx.h"
#include "ClientView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientView

IMPLEMENT_DYNCREATE(CClientView, CView)

BEGIN_MESSAGE_MAP(CClientView, CView)
	//{{AFX_MSG_MAP(CClientView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_CLIENT_NEWGAME, OnClientNewgame)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_NEWGAME, OnUpdateClientNewgame)
	ON_COMMAND(ID_CLIENT_SURRENDER, OnClientSurrender)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_SURRENDER, OnUpdateClientSurrender)
	ON_COMMAND(ID_CLIENT_DISCONNECT, OnClientDisconnect)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_DISCONNECT, OnUpdateClientDisconnect)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_USER_CLIENTACCEPT, OnSocketAccept)
	ON_MESSAGE(WM_USER_CLIENTREAD, OnSocketRead)
	ON_MESSAGE(WM_USER_CLIENTCLOSE, OnSocketClose)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientView construction/destruction

CClientView::CClientView()
:m_nTotalEvent(0),
m_sListen(INVALID_SOCKET),
m_sServer(INVALID_SOCKET),
m_sClient(INVALID_SOCKET),
m_hEventExit(NULL),
m_bRuning(FALSE),
m_pServerSocket(NULL),
m_pListenSocket(NULL),
m_pClientSocket(NULL),
m_bLinked(FALSE),
m_bInGame(FALSE),
m_bOpponentWin(FALSE)
{
	// TODO: add construction code here

}

CClientView::~CClientView()
{
	
}

BOOL CClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CClientView drawing

void CClientView::OnDraw(CDC* pDC)
{
	CClientDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CClientView diagnostics

#ifdef _DEBUG
void CClientView::AssertValid() const
{
	CView::AssertValid();
}

void CClientView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CClientDoc* CClientView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CClientDoc)));
	return (CClientDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CClientView message handlers

/*
 * WM_PAINT消息响应函数
 */
void CClientView::OnPaint() 
{
	CPaintDC dc(this);				//设备环境
	
	CBitmap bitmap;
	bitmap.LoadBitmap (IDB_BKWOOD);	//载入棋盘位图
	
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);	//创建内存设备环境
	//将棋盘位图选入内存设备环境
	CBitmap* pOldBitmap = dcMem.SelectObject(&bitmap);

	//获取客户区域
	CRect rect(0, 0, 0, 0);
	GetClientRect(&rect);

	//画棋盘
	dc.StretchBlt(0, 0, rect.Width(), rect.Height(), 
					&dcMem, 0, 0, 640, 640, SRCCOPY);

	//画线
	for(int i = 1; i <= DIVISIONS; i++)
	{
		dc.MoveTo(CELLSIZE, i * CELLSIZE);
		dc.LineTo(DIVISIONS * CELLSIZE, i * CELLSIZE);//水平线
		dc.MoveTo(i * CELLSIZE, CELLSIZE);
		dc.LineTo(i * CELLSIZE, DIVISIONS * CELLSIZE);//垂直线
	}
	
	//画天元和星位
	dc.SelectStockObject(BLACK_BRUSH);//黑色刷子	
	int nRadius = 4;//半径
	dc.Ellipse(CELLSIZE + CELLSIZE * 3 - nRadius,
			   CELLSIZE + CELLSIZE * 3 - nRadius,
			   CELLSIZE + CELLSIZE * 3 + nRadius,
			   CELLSIZE + CELLSIZE * 3 + nRadius);

	dc.Ellipse(CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) - nRadius,
			   CELLSIZE + CELLSIZE * 3 - nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) + nRadius,
			   CELLSIZE + CELLSIZE * 3 + nRadius);
	
	dc.Ellipse(CELLSIZE + CELLSIZE * (DIVISIONS / 2) - nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS / 2) - nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS / 2) + nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS / 2) + nRadius);
	
	dc.Ellipse(CELLSIZE + CELLSIZE * 3 - nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) - nRadius,
			   CELLSIZE + CELLSIZE * 3 + nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) + nRadius);
	
	dc.Ellipse(CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) - nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) - nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) + nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) + nRadius);

	//画外框线
	dc.SelectStockObject(NULL_BRUSH);	
	CPen pen(PS_SOLID, 4, RGB(0, 0, 0));//黑色画笔
	CPen* pOldPen = dc.SelectObject(&pen);
	CRect rectFrame(CELLSIZE, CELLSIZE, DIVISIONS * CELLSIZE + 2, DIVISIONS * CELLSIZE + 2);
	rectFrame.InflateRect(5, 5);
	dc.Rectangle(&rectFrame);

	dc.SelectObject(pOldPen);	//将画笔选回设备环境
	dc.SelectObject(pOldBitmap);//将位图选回设备环境

	//画棋子
	DrawPieces();

}

/*
 *画棋子
 */
void CClientView::DrawPieces()
{
	CClientDC dc(this);					//设备环境

	CBitmap bmWhite, bmBlack, bmMask;
	bmWhite.LoadBitmap(IDB_WHITEPIECE);	//载入白棋位图
	bmBlack.LoadBitmap(IDB_BLACKPIECE);	//载入黑棋位图
	bmMask.LoadBitmap(IDB_MASK);		//载入掩码位图

	//创建白棋内存设备环境
	CDC dcMemWhite;
	dcMemWhite.CreateCompatibleDC(&dc);
	//将白棋位图选入内存设备环境
	CBitmap* pOld1 = dcMemWhite.SelectObject(&bmWhite);

	//创建黑棋内存设备环境
	CDC dcMemBlack;
	dcMemBlack.CreateCompatibleDC(&dc);
	//将黑棋选入内存设备环境
	CBitmap* pOld2 = dcMemBlack.SelectObject(&bmBlack);

	//创建掩码内存设备环境
	CDC dcMemMask;
	dcMemMask.CreateCompatibleDC(&dc);
	//将掩码位图选入内存设备环境
	CBitmap* pOld3 = dcMemMask.SelectObject(&bmMask);

	//用与方式画白棋 
	dcMemWhite.BitBlt(0, 0, 36, 36, &dcMemMask, 0, 0, SRCAND);
	//用与方式画黑棋 
	dcMemBlack.BitBlt(0, 0, 36, 36, &dcMemMask, 0, 0, SRCAND);

	//画棋子
	for(int i = 0; i < DIVISIONS; i++)
		for(int j = 0; j < DIVISIONS; j++)
		{
			if(m_nCoords[i][j] == -1)
			{
				
				dc.BitBlt((i + 1) * CELLSIZE - 18, (j + 1) * CELLSIZE - 18, 
						36, 36, &dcMemMask, 0, 0, 0x220326);

				//用或方式画白棋 
				dc.BitBlt((i + 1) * CELLSIZE - 18, (j + 1) * CELLSIZE - 18,
						  36, 36, &dcMemWhite, 0, 0, SRCPAINT);
			}
			
			if(m_nCoords[i][j] == 1)
			{
				dc.BitBlt((i + 1) * CELLSIZE - 18, (j + 1) * CELLSIZE - 18,
						  36, 36, &dcMemMask, 0, 0, 0x220326);
				//用或方式画黑棋 
				dc.BitBlt((i + 1) * CELLSIZE - 18, (j + 1) * CELLSIZE - 18,
						  36, 36, &dcMemBlack, 0, 0, SRCPAINT);
			}
			//画上一步棋子的位置
			if(i == m_ptLastStep.x && 
			   j == m_ptLastStep.y &&
			   m_nCoords[i][j] != 0)
			{
				CPen pen(PS_SOLID, 1, RGB(192, 0, 0));
				CPen* pOldPen = dc.SelectObject(&pen);
				dc.SelectStockObject(NULL_BRUSH);

				dc.Rectangle((i + 1) * CELLSIZE - 18,(j + 1) * CELLSIZE - 18,
							 (i + 1) * CELLSIZE + 18,(j + 1) * CELLSIZE + 18);
				dc.SelectObject(pOldPen);
			}
		}
	//将位图选回内存设备环境
	dcMemWhite.SelectObject(pOld1);
	dcMemBlack.SelectObject(pOld2);
	dcMemMask.SelectObject(pOld3);
}

int CClientView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


/*
 * 初始化
 */
void CClientView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	//连接服务器
	if(FALSE == ConnectServer())
	{
		GetParent()->GetParent()->PostMessage(WM_NCDESTROY);
		return;
	}

	//开始监听
	if(FALSE == BeginListen())
	{
		GetParent()->GetParent()->PostMessage(WM_NCDESTROY);		
		return;
	}

	//设置客户端运行状态
	m_bRuning = TRUE;

	//创建线程退出事件对象
	m_hEventExit = CreateEvent(NULL, TRUE, FALSE, NULL);//手动设置，初始“未传信”状态

	//创建线程参数
	PTHREADPARAM pThreadParam = new THREADPARAM;
	pThreadParam->hClientHwnd = this->GetSafeHwnd();
	pThreadParam->pRunning = &m_bRuning;
	pThreadParam->pTotalEvent = &m_nTotalEvent;
	pThreadParam->pArrEvent = m_arrEvent;
	pThreadParam->pArrSocket = m_arrSocket;
	pThreadParam->hEventExit = m_hEventExit;
	//创建工作线程
	DWORD dwThread;	
	HANDLE hServiceThread = CreateThread(NULL, 0, RunningThread, pThreadParam, 0, &dwThread);
	CloseHandle(hServiceThread);

	//获取客户端地址和端口
	SOCKADDR_IN clientAddr;
	int			clientLen = sizeof(clientAddr);
	getsockname(m_sServer, (SOCKADDR*)&clientAddr, &clientLen);	//获取IP
	m_UserInfo.m_lIP = clientAddr.sin_addr.S_un.S_addr;			
	getsockname(m_sListen, (SOCKADDR*)&clientAddr, &clientLen);	//获取监听端口
	m_UserInfo.m_uPort = clientAddr.sin_port;
	
	//用户信息
	m_UserInfo.m_strName = m_servDlg.m_strName;	//名字
	m_UserInfo.m_state = CUserInfo::LOGIN;		//状态

	SendUserInfo(m_UserInfo);					//向服务器发送用户信息
}


/*
 * 客户端退出
 */
void CClientView::OnDestroy() 
{
	CView::OnDestroy();
	//反初始化
	UnInit();
}

/*
 * 开始监听
 */
BOOL CClientView::BeginListen( void )
{
							
	int		reVal;//返回值
	m_sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//创建套接字
	if (INVALID_SOCKET == m_sListen)
	{
		WSACleanup();
		return FALSE;
	}

	//绑定
	SOCKADDR_IN clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	clientAddr.sin_port = INADDR_ANY;
	reVal = bind(m_sListen, (SOCKADDR*)&clientAddr, sizeof(clientAddr));
	if (SOCKET_ERROR == reVal)
		return FALSE;
	
	//监听
	reVal = listen(m_sListen, SOMAXCONN);
	if (SOCKET_ERROR == reVal)
		return FALSE;

	m_arrSocket[m_nTotalEvent]= m_sListen;			//将监听套接字加入集合
	m_arrEvent[m_nTotalEvent] = WSACreateEvent();	//创建套接字事件对象
	//为监听套接字注册FD_ACCEPT和 FD_CLOSE网络事件
	if(SOCKET_ERROR == WSAEventSelect(m_sListen, m_arrEvent[m_nTotalEvent], 
		FD_ACCEPT | FD_CLOSE))
		return FALSE;
	
	m_pListenSocket = new CISocket(this, m_sListen, m_arrEvent[m_nTotalEvent]);
	m_nTotalEvent++;//套接字事件对象加	

	return TRUE;
}
/*
 * 连接服务器
 */
BOOL CClientView::ConnectServer( void )
{
	int	reVal;					//返回值
	WORD	wVersionRequested;	//请求的Windows Sockets 实现版本
	WSADATA	wsaData;			//返回协商结果

	wVersionRequested = MAKEWORD(2, 2);	
	reVal = WSAStartup( wVersionRequested, &wsaData );//初始化套接字动态库
	if ( 0 != reVal ) 
	{
		return FALSE;
	}

	//创建套接字
	m_sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//创建套接字
	if (INVALID_SOCKET == m_sServer)
	{
		WSACleanup();
		return FALSE;
	}
	
		//登录对话框
	if (IDOK != m_servDlg.DoModal())
	{	
		closesocket(m_sServer);
		WSACleanup();
		return FALSE;
	}

	//服务器套接字地址
	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(m_servDlg.m_lservIP);
	servAddr.sin_port = htons(m_servDlg.m_servPort);
	int servAddrLen = sizeof(servAddr);

	reVal = connect(m_sServer, (SOCKADDR*)&servAddr, servAddrLen);
	if (SOCKET_ERROR == reVal)
	{
		AfxMessageBox("连接服务失败");
		closesocket(m_sServer);
		WSACleanup();
		return FALSE;
	}

	m_arrSocket[m_nTotalEvent]= m_sServer;			//将监听套接字加入集合
	m_arrEvent[m_nTotalEvent] = WSACreateEvent();	//创建套接字事件对象
	//为监听套接字注册FD_READ和 FD_CLOSE网络事件
	if(SOCKET_ERROR == WSAEventSelect(m_sServer, m_arrEvent[m_nTotalEvent], 
		FD_READ | FD_CLOSE))
		return FALSE;

	m_pServerSocket = new CServerSocket(this, m_sServer, m_arrEvent[m_nTotalEvent]);
	m_nTotalEvent++;//套接字事件对象加1
	return TRUE;
}

/*
 * 工作线程
 */
DWORD WINAPI CClientView::RunningThread( void *pParam )
{
	PTHREADPARAM pThreadParam = (PTHREADPARAM)pParam;
	while (*pThreadParam->pRunning)
	{
		
		DWORD	dwIndex = 0;				//返回值
		SOCKET	sAccept = INVALID_SOCKET;	//接受其他客户端连接套接字
		
		//等待网络事件
		if ((dwIndex = WSAWaitForMultipleEvents(*pThreadParam->pTotalEvent, 
											pThreadParam->pArrEvent, FALSE, 
											CLIENT_WAIT_TIMEOUT, FALSE)) 
											== WSA_WAIT_FAILED)
		{
			AfxMessageBox("等待事件错误！");
			break;
		}		
		
		if (WSA_WAIT_TIMEOUT == dwIndex)//超时
		{
			Sleep(CLIENT_WAIT_TIMEOUT);
			continue;
		}
		//查看在套接字上发生的网络事件
		WSANETWORKEVENTS networkEvents;//网络事件结构
		if (WSAEnumNetworkEvents(pThreadParam->pArrSocket[dwIndex - WSA_WAIT_EVENT_0], 
								pThreadParam->pArrEvent[dwIndex - WSA_WAIT_EVENT_0], 
								&networkEvents) == SOCKET_ERROR)
		{
			AfxMessageBox("网络事件结构错误！");
			break;
		}
		
		//FD_ACCEPT网络事件
		if (networkEvents.lNetworkEvents & FD_ACCEPT)
		{
			if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)//发生网络错误
			{
				AfxMessageBox("FD_ACCEPT_BIT 网络事件错误！");
				break;
			}
			//向主窗口发送WM_USER_CLIENTACCEPT消息
			::SendMessage(pThreadParam->hClientHwnd, 
						WM_USER_CLIENTACCEPT, dwIndex, NULL);
		}		
		
		//发生FD_READ网络事件	
		if (networkEvents.lNetworkEvents & FD_READ )
		{
			if (networkEvents.iErrorCode[FD_READ_BIT] != 0)//发生网络错误
			{
				AfxMessageBox("FD_READ_BIT 网络事件错误！");
				break;
			}else
			{
				//向主窗口发送WM_USER_CLIENTREAD消息
				::SendMessage(pThreadParam->hClientHwnd, 
							WM_USER_CLIENTREAD, dwIndex, NULL);		
			}
		}
		
		//发生FD_CLOSE网络事件	
		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			//向主窗口发送WM_USER_CLIENTCLOSE消息
			::SendMessage(pThreadParam->hClientHwnd, 
						WM_USER_CLIENTCLOSE, dwIndex, NULL);	
		}		
	}

	//设置退出事件
	SetEvent(pThreadParam->hEventExit);	
	delete pThreadParam;
	pThreadParam = NULL;
	return 0;
}

/*
 * 清除套接字和事件对象
 */
void CClientView::ClearSocketAndEventFromArr( const int nIndex )
{
	//将该套接字和事件对象从数组中清除
	for (int i = nIndex - WSA_WAIT_EVENT_0; i < m_nTotalEvent; i++)
	{
		m_arrEvent[i] = m_arrEvent[i + 1];	
		m_arrSocket[i] = m_arrSocket[i + 1];
	}
	//总数减1		
	m_nTotalEvent--;	
}
/*
 * 清除套接字和事件对象
 */
void CClientView::ClearSocketAndEventFromArr( SOCKET s )
{
	int nIndex;
	for (nIndex = 0; nIndex < m_nTotalEvent; ++nIndex)
	{
		if (m_arrSocket[nIndex] == s)
		{
			ClearSocketAndEventFromArr(nIndex);	//将该套接字和事件对象从数组中清除
			break;
		}
	}
}

/*
 * 向服务器发送用户信息
 */
void CClientView::SendUserInfo(const CUserInfo &userInfo)
{
	CString strUserInfo;//用户信息
	CString strUserName;//名称
	strUserName= userInfo.m_strName;					//名称
	CString	strUserIP;
	strUserIP.Format("%ld",userInfo.m_lIP);				//ip
	CString strUserPort;
	strUserPort.Format("%d",userInfo.m_uPort);			//端口
	CString StrUserState;
	StrUserState.Format("%d",(int)userInfo.m_state);	//状态
	strUserInfo = "<" +
			strUserName + 
			"|"	+
			strUserIP + 
			"|"	+ 
			strUserPort + 
			"|"	+
			StrUserState+
			">";

	m_pServerSocket->Send(USERINFOR, strUserInfo);//发送用户信息
}

/*
 * 更新用户列表
 */
void CClientView::UpdateUserList( CString strUserList )
{
	//获取框架指针
	CMainFrame *pMF = (CMainFrame*)AfxGetApp()->GetMainWnd();
	//获取用户列表窗口指针
	CUserView *pUserView = pMF->GetUserView();
	//更新用户列表
	pUserView->UpdateUserList(strUserList);
}

/*
 * FD_ACCEPT网络事件响应函数
 */
afx_msg LRESULT CClientView::OnSocketAccept( WPARAM wParam, LPARAM lParam )
{
	SOCKET sAccept = INVALID_SOCKET;//接受套结字
	ASSERT(m_sListen == m_arrSocket[wParam - WSA_WAIT_EVENT_0]);	

	//接受连接请求
	if ((sAccept = accept(m_arrSocket[wParam - WSA_WAIT_EVENT_0],
							NULL, NULL)) == INVALID_SOCKET)
	{		
		AfxMessageBox("接受连接请求错误！");
		return -1;
	}
	
	if(!m_bLinked)//没有与其他用户连接
	{	
		int result = AfxMessageBox(_T("可以和你下棋吗?"), MB_YESNO);		
		if(result == IDYES)
		{
			ASSERT(m_sClient == INVALID_SOCKET);			
			//发送接受消息
			CLIENTPACKET clientPacket;
			clientPacket.type = ACCEPT;
			clientPacket.xPos = 0;
			clientPacket.yPos = 0;
			int reVal = send(sAccept, (char*)&clientPacket, CLIENTPACKETLEN, 0);
			if (reVal == SOCKET_ERROR)//失败处理
			{
				
				AfxMessageBox(_T("连接被断开\n不能打开游戏。"));
				m_bLinked = FALSE;							//未连接
				m_bInGame = FALSE;							//没有游戏
				closesocket(sAccept);						//关闭套接字
				GetDocument()->SetTitle("五子棋---准备");	//标题
				return -1;
			}		
			//创建套接字事件对象
			if ((m_arrEvent[m_nTotalEvent] = WSACreateEvent())
										== WSA_INVALID_EVENT)
			{
				AfxMessageBox("为新建套接字创建套接字事件对象错误！");
				return -1;
			}
			m_arrSocket[m_nTotalEvent] = sAccept;//加入套接字数组
			m_sClient = sAccept;
			
			//注册FD_READ FD_CLOSE网络事件
			if (WSAEventSelect(sAccept, m_arrEvent[m_nTotalEvent],
								FD_READ|FD_CLOSE) == SOCKET_ERROR)
			{
				AfxMessageBox("为新建套接字注册网络事件错误！");
				return -1;
			}
			
			//创建CClientSocket对象
			m_pClientSocket = new CISocket(this, sAccept, m_arrEvent[m_nTotalEvent]);
			m_nTotalEvent++;//总数加1			
			
			//给服务器发送消息
			m_UserInfo.m_state = CUserInfo::PLAY;				
			SendUserInfo(m_UserInfo);		
			
			//改变自己的状态
			m_bLinked = TRUE;		//连接状态
			m_bMyTurn = TRUE;		//开始下棋
			m_bOpponentWin = FALSE;	//对方未胜
			m_bInGame = TRUE;		//游戏中
			ResetCoords();			//设置棋牌
			m_nColor = 1;			//黑棋先下
			GetDocument()->SetTitle("五子棋---轮你下棋");		
		}
		else
		{
			//发送拒绝消息
			CLIENTPACKET  clientPacket;
			clientPacket.type = DECLINE;
			clientPacket.xPos = 0;
			clientPacket.yPos = 0;
			send(sAccept,(char*)&clientPacket, CLIENTPACKETLEN, 0);
			GetDocument()->SetTitle("五子棋---等待");	
			closesocket(sAccept);
			
		}
	}
	else// 已经与其他用户连接
	{
		//发送拒绝消息
		CLIENTPACKET  clientPacket;
		clientPacket.type = DECLINE;
		clientPacket.xPos = 0;
		clientPacket.yPos = 0;
		send(sAccept,(char*)&clientPacket, CLIENTPACKETLEN, 0);
		closesocket(sAccept);
	}	
	return 0;
}



/*
 * FD_READ网络事件响应函数
 */
afx_msg LRESULT CClientView::OnSocketRead( WPARAM wParam, LPARAM lParam )
{
	//服务器套接字
	if (m_sServer == m_arrSocket[wParam - WSA_WAIT_EVENT_0])
	{
		if(!m_pServerSocket->Recv())//接收服务器数据
			return -1;
	}else//客户端套接字
	{	
		ASSERT(m_sClient == m_arrSocket[wParam - WSA_WAIT_EVENT_0]);
		if(FALSE == m_pClientSocket->Recv())		//接收客户端数据
		{
			ClearSocketAndEventFromArr(m_sClient);	//清除客户端套接字
			delete m_pClientSocket;					//删除指针
			m_pClientSocket = NULL;
			m_sClient = INVALID_SOCKET;
			return -1;
		}
	}
	return 0;	
}

/*
 * FD_CLOSE网络事件响应函数
 */
afx_msg LRESULT CClientView::OnSocketClose( WPARAM wParam, LPARAM lParam )
{
	//客户端发起连接被拒绝或者对方退出游戏
	if (m_sClient == m_arrSocket[wParam - WSA_WAIT_EVENT_0])
	{
		ClearSocketAndEventFromArr(m_sClient);//清除套接字
		delete m_pClientSocket;
		m_pClientSocket = NULL;	
		m_sClient = INVALID_SOCKET;

		m_UserInfo.m_state = CUserInfo::WAIT;//改变状态
		SendUserInfo(m_UserInfo);

	}else if (m_sServer == m_arrSocket[wParam - WSA_WAIT_EVENT_0])//服务器套接字
	{
		AfxMessageBox("客户端退出");
		CUserView *pUserView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetUserView();
		pUserView->UnInit();
		PostMessage(WM_DESTROY);
	}else{}
	return 0;
}


/*
 * 向对方发出邀请
 */
void CClientView::RequestPlay( const CUserInfo &userInfo )
{
	if (m_sClient != INVALID_SOCKET || TRUE == m_bLinked)
	{
		AfxMessageBox("你已经在和别人下棋了！");
		return;
	}
	ASSERT(NULL == m_pClientSocket);	
	//创建套接字
	m_sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sClient)
		return ;	
	
	//客户端地址
	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = userInfo.m_lIP;
	servAddr.sin_port =userInfo.m_uPort;
	int servAddrLen = sizeof(servAddr);
	
	//连接
	int reVal = connect(m_sClient, (SOCKADDR*)&servAddr, servAddrLen);
	if (SOCKET_ERROR == reVal)
	{
		AfxMessageBox("连接客户端失败");
		closesocket(m_sClient);
		return ;
	}
	
	m_arrSocket[m_nTotalEvent]= m_sClient;			//将套接字加入集合
	m_arrEvent[m_nTotalEvent] = WSACreateEvent();	//创建套接字事件对象
	//为监听套接字注册FD_READ和 FD_CLOSE网络事件
	if(SOCKET_ERROR == WSAEventSelect(m_sClient, m_arrEvent[m_nTotalEvent],
						FD_READ | FD_CLOSE))
		return ;
	
	//创建CRenjuSocket对象
	m_pClientSocket = new CISocket(this, m_sClient, m_arrEvent[m_nTotalEvent]);
	m_nTotalEvent++;//套接字事件对象加1
	
	//在m_sClient上等待消息,然后更改自身状态
	//向服务器发送消息更改状态
	return ;	
}


/*
 * 刷新棋盘
 */
void CClientView::ResetCoords()
{
	//设置棋盘为位置值为0
	for(int i = 0; i < DIVISIONS; i++)
	{
		for(int j = 0; j < DIVISIONS; j++)
		{
			m_nCoords[i][j] = 0;
		}
	}
	//更新界面
 	Invalidate(TRUE);
 	UpdateWindow();
}

/*
 * 处理客户端数据
 */
BOOL CClientView::HandleClientData( PCLIENTPACKET pClientPacket )
{	
	switch(pClientPacket->type)
	{
	case NEWGAME://新开局请求
		{
			InviteNewGame();
			break;
		}
	case NONEWGAME://拒绝新开局
		{
			GetDocument()->SetTitle(_T("五子棋---游戏结束"));
			AfxMessageBox(_T("对方现在不想再重新开局了。"));
			break;
		}
	case DISCONNECT://退出游戏
		{		
			m_bLinked = FALSE;
			m_bInGame = FALSE;
			AfxMessageBox(_T("对方退出游戏"));
			ResetCoords();
			GetDocument()->SetTitle(_T("五子棋---等待"));			
			break;
		}
	case ACCEPT://对方接受邀请或者新开局
		{		
			m_bMyTurn = FALSE;
			m_bLinked = TRUE;
			m_bOpponentWin = FALSE;
			m_bInGame = TRUE;
			ResetCoords();
			m_nColor = -1;//白棋
			GetDocument()->SetTitle(_T("五子棋---对方下棋"));

			//发送服务器信息
			m_UserInfo.m_state = CUserInfo::PLAY;
			SendUserInfo( m_UserInfo);
			break;
		}
	case DECLINE://对方拒绝邀请 
		{
			AfxMessageBox(_T("对方拒绝你的邀请"));
			GetDocument()->SetTitle(_T("五子棋---等待"));
			break;
		}
	case SURRENDER://对方投降
		{
			m_bMyTurn = FALSE;
			m_bInGame = FALSE;
			GetDocument()->SetTitle(_T("五子棋---游戏结束"));
			AfxMessageBox(_T("对方投降\n你赢了!"));				
			break;
		}
	default:AnalyseOpponent(pClientPacket->type, pClientPacket->xPos, pClientPacket->yPos);			
	}	
	return TRUE;
}

/*
 * 重新开局的请求
 */
void CClientView::InviteNewGame(void)
{
	//消息框
	int result = AfxMessageBox(_T("你想再玩一局吗?"),	MB_YESNO);

	if(result == IDYES)//接受邀请
	{
		//定义数据包
		CLIENTPACKET	clientPacket;
		clientPacket.type = ACCEPT;
		clientPacket.xPos = 0;
		clientPacket.yPos = 0;	
		//发送数据包
		if (FALSE == m_pClientSocket->Send(&clientPacket))
		{
			//失败处理
			AfxMessageBox(_T("网络问题\n 不能开局."));
			m_bLinked = FALSE;
			m_bInGame = FALSE;
			DestroyClientSocket();
			GetDocument()->SetTitle(_T("五子棋---等待中"));
			return;;
		}
		
		//设置游戏状态
		m_bMyTurn = TRUE;			//先下棋
		m_bLinked = TRUE;			//连接状态
		m_bOpponentWin = FALSE;		//对方未获胜
		m_bInGame = TRUE;			//游戏中

		ResetCoords();				//刷新棋盘
		m_nColor = 1;				//黑棋
		GetDocument()->SetTitle(_T("五子棋---轮你下棋"));		
	}
	else//拒绝新开局,但不断开网络
	{
		//定义数据包
		CLIENTPACKET	clientPacket;
		clientPacket.type = NONEWGAME;
		clientPacket.xPos = 0;
		clientPacket.yPos = 0;
		//发送数据包
		if (FALSE == m_pClientSocket->Send(&clientPacket))
		{
			//失败处理
			AfxMessageBox(_T("网络问题\n 不能开局."));
			m_bLinked = FALSE;
			m_bInGame = FALSE;
			DestroyClientSocket();
			GetDocument()->SetTitle(_T("五子棋---等待中"));
			return;
		}
		GetDocument()->SetTitle(_T("五子棋---游戏结束"));		
	}
	return ;
}

/*
 * 分析对方
 */
void	CClientView::AnalyseOpponent(int state, int x, int y)
{
	DrawOneStep(x, y);//画对方棋子
	
	switch(state)
	{
	case EVEN://平棋
		{
			m_bMyTurn = FALSE;
			m_bInGame = FALSE;
			GetDocument()->SetTitle(_T("五子棋---游戏结束"));
			AfxMessageBox(_T("平局!"));
			break;
		}
	case ULOSE://对方获胜
		{
			m_bMyTurn = FALSE;
			m_bInGame = FALSE;
			GetDocument()->SetTitle(_T("五子棋---游戏结束"));		
			AfxMessageBox(_T("你输了。"));
			break;
		}
	case UWIN://对方输局
		{
			m_bMyTurn = FALSE;
			m_bInGame = FALSE;
			GetDocument()->SetTitle(_T("五子棋---游戏结束"));
			AfxMessageBox(_T("你赢了。"));
			break;
		}			
	case IWIN://对方暂时获胜
		{
			m_bMyTurn = TRUE;
			m_bOpponentWin = TRUE;
			GetDocument()->SetTitle(_T("五子棋---该你走了"));
			break;
		}
	
	case COORDS://不分胜负
		{
			m_bMyTurn = TRUE;
			GetDocument()->SetTitle(_T("五子棋---该你走了"));
			break;
		}
	default: break;
	}	
}

/*
 * 画对方棋子
 */
void CClientView::DrawOneStep(int x, int y)
{
	CRect invalidOldRect((m_ptLastStep.x + 1) * CELLSIZE - CELLSIZE / 2,
						 (m_ptLastStep.y + 1) * CELLSIZE - CELLSIZE / 2,
						 (m_ptLastStep.x + 1) * CELLSIZE + CELLSIZE / 2,
						 (m_ptLastStep.y + 1) * CELLSIZE + CELLSIZE / 2);

	InvalidateRect(&invalidOldRect, FALSE);

	m_ptLastStep.x = x;
	m_ptLastStep.y = y;
	m_nCoords[x][y] = -m_nColor;
				
	PlaySound(_T("down.wav"), NULL, SND_FILENAME | SND_ASYNC);
	CRect invalidRect((x + 1) * CELLSIZE - CELLSIZE / 2,
					  (y + 1) * CELLSIZE - CELLSIZE / 2,
					  (x + 1) * CELLSIZE + CELLSIZE / 2,
					  (y + 1) * CELLSIZE + CELLSIZE / 2);
	InvalidateRect(&invalidRect, FALSE);
}

//网络出现问题删除指针，删除数组
void CClientView::DestroyClientSocket()
{
	ClearSocketAndEventFromArr(m_sClient);
	
	delete m_pClientSocket;
	m_pClientSocket = NULL;
	m_sClient = INVALID_SOCKET;
}

/*
 * 设置鼠标形状
 */
BOOL CClientView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(nHitTest == HTCLIENT && m_bLinked && m_bInGame)
	{
		if((m_bMyTurn && (m_nColor == 1)) ||//我被邀请下棋--我的棋子黑色
			(!m_bMyTurn && (m_nColor == -1)))//我邀请对方下棋，对方棋子黑色
			::SetCursor(AfxGetApp()->LoadCursor(IDC_BLACK));
		else
			::SetCursor(AfxGetApp()->LoadCursor(IDC_WHITE));
		
		return TRUE;
	}


	return CView::OnSetCursor(pWnd, nHitTest, message);
}


/*
 * 鼠标左键响应函数
 */
void CClientView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_bLinked && m_bMyTurn)
	{
		int x = (point.x - CELLSIZE / 2) / CELLSIZE;			//转换坐标为棋盘坐标
		int y = (point.y - CELLSIZE / 2) / CELLSIZE;
		if(x >= 0 && x < DIVISIONS && y >= 0 && y < DIVISIONS)	//在棋盘范围内
		{
			if(m_nCoords[x][y] == 0)//还没有棋子
			{	
				//清除上一步棋子标记
				CRect invalidOldRect((m_ptLastStep.x + 1) * CELLSIZE - CELLSIZE / 2,
									 (m_ptLastStep.y + 1) * CELLSIZE - CELLSIZE / 2,
									 (m_ptLastStep.x + 1) * CELLSIZE + CELLSIZE / 2,
									 (m_ptLastStep.y + 1) * CELLSIZE + CELLSIZE / 2);

				InvalidateRect(&invalidOldRect, FALSE);
				//播放声音
				PlaySound(_T("down.wav"), NULL, SND_FILENAME | SND_ASYNC);
				m_ptLastStep.x = x;
				m_ptLastStep.y = y;
				m_nCoords[x][y] = m_nColor;
				m_bMyTurn = FALSE;
				
				//画出当前棋子标记
				CRect invalidRect((x + 1) * CELLSIZE - CELLSIZE / 2,
								  (y + 1) * CELLSIZE - CELLSIZE / 2,
								  (x + 1) * CELLSIZE + CELLSIZE / 2,
								  (y + 1) * CELLSIZE + CELLSIZE / 2);

				InvalidateRect(&invalidRect, FALSE);

				CLIENTPACKET clientPacket;
				clientPacket.xPos = x;
				clientPacket.yPos = y;
				if(IsWin(x, y))//判断胜负结果
				{
					if(m_nColor == -1)//白棋,
					{
						if(m_bOpponentWin)//对方应经暂时获胜
						{
							clientPacket.type = EVEN;//平局
							m_bInGame = FALSE;
							GetDocument()->SetTitle(_T("五子棋---游戏结束"));
							AfxMessageBox("游戏结束");
						}else//黑棋战败
						{
							clientPacket.type = ULOSE;// white win and game over
							m_bInGame = FALSE;
							GetDocument()->SetTitle(_T("五子棋---游戏结束"));
							AfxMessageBox("祝贺，你赢了！");
						}
					}
					else//黑棋暂时获胜
					{
						clientPacket.type = IWIN;
						GetDocument()->SetTitle(_T("五子棋---对方下棋"));
					}
				}
				else//FALSE
				{
					if(m_bOpponentWin)//对方获胜
					{
						clientPacket.type = UWIN;
						m_bInGame = FALSE;
						GetDocument()->SetTitle(_T("五子棋---游戏结束"));
						AfxMessageBox("你输了!");
					}
					else//为分胜负
					{
						clientPacket.type = COORDS;					
						GetDocument()->SetTitle(_T("五子棋---对方下棋"));
					}
				}

				//向对方发送数据
				BOOL bRet = m_pClientSocket->Send(&clientPacket);
				if (FALSE == bRet)
				{
					AfxMessageBox(_T("网络断开\n不能游戏"));
					m_bLinked = FALSE;
 					m_bInGame = FALSE;
 					DestroyClientSocket();
 					GetDocument()->SetTitle(_T("五子棋---等待"));				
					//向服务器发送消息
					m_UserInfo.m_state = CUserInfo::WAIT;
					SendUserInfo(m_UserInfo);
 					return;
				}
			}
		}
		else
		{
			::MessageBeep(MB_ICONEXCLAMATION);//下棋位置错误声音
		}
	}
	CView::OnLButtonDown(nFlags, point);
}
/*
 * 判断胜负
 */
BOOL CClientView::IsWin(int x, int y)
{
	int xMin=max(0, x - 4);				//水平位置最小值
	int xMax=min(DIVISIONS - 1, x + 4);	//水平位置最大值
	int yMin=max(0, y - 4);				//垂直位置最小值
	int yMax=min(DIVISIONS - 1, y + 4);	//垂直位置最大值

	int i, j, p, q, sum1, sum2, sum3, sum4;

	//从当前棋子左边最远位置（x-4）开始循环，共计5个循环。
	//每循环一次向右移动一个位置，。
	for(j=xMin; j<=xMax-4; j++)
	{
		for(i=j, sum1=0; i<j+5; i++)//连续五个水平位置是否具有相同颜色棋子
			if(m_nCoords[i][y]==m_nCoords[x][y])
				sum1++;
			else
				break;

		if(sum1==5)
			return TRUE;
	}
	
	//从当前棋子上面最远位置（y-4）开始循环，共计5个循环。
	for(i=yMin; i<=yMax-4; i++)
	{
		for(j=i, sum2=0; j<i+5; j++)
			if(m_nCoords[x][j]==m_nCoords[x][y])
				sum2++;
			else
				break;
					

		if(sum2==5)
			return TRUE;
	}
	
	//左上角位置
	for(i=x, j=y; i>xMin && j>yMin; i--, j--);

	//从左上角到右下角
	for(; i<=xMax-4 && j<=yMax-4; i++, j++)
	{
		for(sum3=0, p=i, q=j; p<i+5; p++, q++)
			if(m_nCoords[p][q]==m_nCoords[x][y])
				sum3++;
			else
				break;

		if(sum3==5)
			return TRUE;
	}

	//左下角位置
	for(i=x, j=y; i>xMin && j<yMax; i--, j++);

	//从左下角到右上角
	for(; i<=xMax-4 && j>=yMin+4; i++, j--)
	{
		for(sum4=0, p=i, q=j; p<i+5; p++, q--)
			if(m_nCoords[p][q]==m_nCoords[x][y])
				sum4++;
			else
				break;

		if(sum4==5)
			return TRUE;
	}
	return FALSE;
}

/*
 * 请求新开局
 */
void CClientView::OnClientNewgame() 
{
	CLIENTPACKET clientPacket;
	clientPacket.type = NEWGAME;
	clientPacket.xPos = 0;
	clientPacket.yPos = 0;
	BOOL bRet = m_pClientSocket->Send(&clientPacket);
	if (FALSE == bRet)
	{
		AfxMessageBox(_T("连接已经断开"));
		m_bLinked = FALSE;
		m_bInGame = FALSE;
		m_bMyTurn = FALSE;
		m_bOpponentWin = FALSE;
	
		ClearSocketAndEventFromArr(m_sClient);

		delete m_pClientSocket;
		m_pClientSocket = NULL;
		m_sClient = INVALID_SOCKET;
		GetDocument()->SetTitle(_T("五子棋---等待"));

		m_UserInfo.m_state = CUserInfo::WAIT;
		SendUserInfo( m_UserInfo);
		return;
	}
	GetDocument()->SetTitle(_T("五子棋---对方等待答复"));
}

/*
 * 更新菜单项
 */
void CClientView::OnUpdateClientNewgame(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bLinked && !m_bInGame);	
}

/*
 * 认输
 */
void CClientView::OnClientSurrender() 
{
	//消息框
	int result = AfxMessageBox(_T("你想认输？"),MB_YESNO);
	//认输
	if(result == IDYES)
	{
		//设置状态
		m_bMyTurn = FALSE;
		m_bInGame = FALSE;
		
		//定义数据包
		CLIENTPACKET clientPacket;
		clientPacket.type = SURRENDER;
		clientPacket.xPos = 0;
		clientPacket.yPos = 0;;
	
		//发送数据
		BOOL bRet = m_pClientSocket->Send(&clientPacket);
		if(FALSE == bRet)//错误处理
		{
			AfxMessageBox(_T("连接已经断开"));
			//设置状态
			m_bLinked = FALSE;
			m_bInGame = FALSE;
			m_bMyTurn = FALSE;
			m_bOpponentWin = FALSE;	
			
			//删除套接字
			ClearSocketAndEventFromArr(m_sClient);
		
			//删除对象
			delete m_pClientSocket;
			m_pClientSocket = NULL;
			m_sClient = INVALID_SOCKET;
			GetDocument()->SetTitle(_T("五子棋---等待"));
			
			//发送消息
			m_UserInfo.m_state = CUserInfo::WAIT;
			SendUserInfo( m_UserInfo);
			return;
		}
		GetParent()->SetWindowText(_T("五子棋---游戏结束"));
	}
}

/*
 * 更新菜单项
 */
void CClientView::OnUpdateClientSurrender(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bLinked && m_bInGame);		
}

/*
 * 退出游戏
 */
afx_msg void CClientView::OnClientDisconnect()
{
	//消息框
	int reVal = AfxMessageBox(_T("你不想和对方下棋了吗？"), MB_YESNO);

	if(reVal == IDYES)
	{
		//发送DISCONNECT数据包
		CLIENTPACKET clientPacket;
		clientPacket.type = DISCONNECT;
		clientPacket.xPos = 0;
		clientPacket.yPos = 0;
		m_pClientSocket->Send(&clientPacket);

		//清除套接字
		ASSERT(m_sClient != INVALID_SOCKET);
		ClearSocketAndEventFromArr(m_sClient);

		//删除指针
		delete m_pClientSocket;
		m_pClientSocket = NULL;
		m_sClient = INVALID_SOCKET;

		//修改状态
		m_bLinked = FALSE;
		m_bInGame = FALSE;	
		GetDocument()->SetTitle(_T("五子棋---等待"));
		//刷新棋盘
		ResetCoords();
		//发送消息
		m_UserInfo.m_state = CUserInfo::WAIT;
		SendUserInfo(m_UserInfo);
	}
}

/*
 * 更新菜单项
 */
afx_msg void CClientView::OnUpdateClientDisconnect( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(m_bLinked);
}

/*
 * 反初始化
 */
void CClientView::UnInit( void )
{
	m_bRuning = FALSE;//设置客户端运行状态

	WaitForSingleObject(m_hEventExit, INFINITE);//等待事件
	//如果还在游戏则通知对方
	if (m_bLinked && m_pClientSocket)
	{
		//发送DISCONNECT数据包
		CLIENTPACKET clientPacket;
		clientPacket.type = DISCONNECT;
		clientPacket.xPos = 0;
		clientPacket.yPos = 0;
		m_pClientSocket->Send(&clientPacket);
	
		//删除客户端套接字
		ASSERT(m_sClient != INVALID_SOCKET);
		ClearSocketAndEventFromArr(m_sClient);
		delete m_pClientSocket;
		m_pClientSocket = NULL;
		m_sClient = INVALID_SOCKET;
	
	}	
	delete m_pServerSocket;	//关闭服务器套接字
	m_pServerSocket = NULL;	
	delete m_pListenSocket;	//关闭监听套接字
	m_pListenSocket = NULL;
	WSACleanup();			//释放套接字动态库	
}
