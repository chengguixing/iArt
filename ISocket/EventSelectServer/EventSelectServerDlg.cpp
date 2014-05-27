
// EventSelectServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "EventSelectServer.h"
#include "EventSelectServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CEventSelectServerDlg 对话框



CEventSelectServerDlg::CEventSelectServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventSelectServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	m_sListen = INVALID_SOCKET;
	memset(m_arrEvent, 0, MAX_NUM_EVENTS);
	memset(m_arrSocket, 0, MAX_NUM_EVENTS);
	m_nTotalEvent = 0;
	m_bRuning = FALSE;
	m_hEventExit = NULL;
}

void CEventSelectServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_LIST, m_ctlUserList);
}

BEGIN_MESSAGE_MAP(CEventSelectServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CEventSelectServerDlg::OnStartService)
	ON_BN_CLICKED(IDCANCEL, &CEventSelectServerDlg::OnStopService)
	ON_MESSAGE(WM_USER_ACCEPT, OnSocketAccept)
	ON_MESSAGE(WM_USER_READ, OnSocketRead)
	ON_MESSAGE(WM_USER_CLOSE, OnSocketClose)
END_MESSAGE_MAP()


// CEventSelectServerDlg 消息处理程序

BOOL CEventSelectServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	int len = sizeof(byte);
	len = sizeof(u_short);
	len = sizeof(PACKETHDR);

	if (FALSE == Init())
	{
		EndDialog(IDCANCEL);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CEventSelectServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CEventSelectServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CEventSelectServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CEventSelectServerDlg::Init(void)
{
	InitListCtrlSetting();
	if (!InitSocket())
	{
		return FALSE;
	}

	return TRUE;
}



void CEventSelectServerDlg::InitListCtrlSetting(void)
{
	m_pImageList = new CImageList();

	m_pImageList->Create(32, 32, ILC_COLOR8 | ILC_MASK, 2, 2);
	m_pImageList->Add(AfxGetApp()->LoadIconA(IDI_USER_PLAY_ICON));
	m_pImageList->Add(AfxGetApp()->LoadIconA(IDI_USER_WAIT_ICON));


	m_ctlUserList.SetImageList(m_pImageList, LVSIL_SMALL);
	m_ctlUserList.SetExtendedStyle(m_ctlUserList.GetExtendedStyle() | LVS_EX_SUBITEMIMAGES);


	m_ctlUserList.InsertColumn(0, TEXT("名称"), LVCFMT_CENTER);
	m_ctlUserList.InsertColumn(1, TEXT("状态"), LVCFMT_CENTER);
	m_ctlUserList.InsertColumn(2, TEXT("地址"), LVCFMT_CENTER);
	m_ctlUserList.InsertColumn(3, TEXT("端口"), LVCFMT_CENTER);


	CRect rect;
	m_ctlUserList.GetClientRect(&rect);
	int nWidth = rect.Width();
	m_ctlUserList.SetColumnWidth(0, nWidth * 25 / 100);
	m_ctlUserList.SetColumnWidth(1, nWidth * 20 / 100);
	m_ctlUserList.SetColumnWidth(2, nWidth * 35 / 100);
	m_ctlUserList.SetColumnWidth(3, nWidth * 20 / 100);
}


BOOL CEventSelectServerDlg::InitSocket(void)
{
	int reVal;
	WORD wVersionRequested;
	WSADATA wsaData;


	wVersionRequested = MAKEWORD(2, 2);
	reVal = WSAStartup(wVersionRequested, &wsaData);
	if (0 != reVal)
	{
		return FALSE;
	}

	m_sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sListen)
	{
		WSACleanup();
		return FALSE;
	}

	if (IDOK != m_ServAddrDlg.DoModal())
	{
		closesocket(m_sListen);
		WSACleanup();
		return FALSE;
	}

	return TRUE;
}
void CEventSelectServerDlg::OnStartService()
{
	if (FALSE == BeginListen())//服务器套接字开始监听
	{
		AfxMessageBox("启动服务器失败！");
		closesocket(m_sListen);
		WSACleanup();
		EndDialog(IDCANCEL);
		return;
	}

	m_bRuning = TRUE;									//设置服务器运行状态
	//创建服务线程退出事件对象
	m_hEventExit = CreateEvent(NULL, TRUE, FALSE, NULL);//手动设置，初始“未传信”状态

	//线程函数参数
	PTHREADPARAM pParam = new THREADPARAM;
	pParam->hServHwnd = this->GetSafeHwnd();
	pParam->bRunning = &m_bRuning;
	pParam->pTotalEvent = &m_nTotalEvent;
	pParam->pArrEvent = m_arrEvent;
	pParam->pArrSocket = m_arrSocket;
	pParam->hEventExit = m_hEventExit;

	//创建服务线程
	DWORD dwThread;	
	HANDLE hServiceThread = CreateThread(NULL, 0, ServiceThread, pParam, 0, &dwThread);
	CloseHandle(hServiceThread);

	//设置启动服务器按钮无效
	(CButton*)GetDlgItem(IDOK)->EnableWindow(FALSE);	
}


void CEventSelectServerDlg::OnStopService()
{
	//设置服务器状态
	m_bRuning = FALSE;

	WaitForSingleObject(m_hEventExit, INFINITE);//等待线程退出

	DeleteAllClientNodes();						//删除所有客户端节点

	delete m_pImageList;
	m_pImageList = NULL;
	CloseHandle(m_hEventExit);		//关闭线程退出事件对象 
	WSACloseEvent(m_arrEvent[0]);	//关闭监听套接字对应事件
	closesocket(m_sListen);			//关闭监听套接字
	WSACleanup();					//释放套接字资源
	CDialog::OnCancel();
}


/*
 *	服务器开始监听
 */
BOOL CEventSelectServerDlg::BeginListen( void )
{
	int	reVal;										//返回值

	m_arrSocket[m_nTotalEvent]= m_sListen;//将监听套接字加入集合
	m_arrEvent[m_nTotalEvent] = WSACreateEvent();	//创建套接字事件对象
	
	//为监听套接字注册FD_ACCEPT和 FD_CLOSE网络事件
	if(SOCKET_ERROR == WSAEventSelect(m_sListen, m_arrEvent[m_nTotalEvent], 
						FD_ACCEPT | FD_CLOSE))
		return FALSE;
	++m_nTotalEvent;//套接字事件对象加1
	
	//服务器套接字地址
	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(m_ServAddrDlg.m_dwServIP);
	servAddr.sin_port = htons(m_ServAddrDlg.m_shServPort);
	
	//绑定
	reVal = bind(m_sListen,(sockaddr*)&servAddr,sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == reVal)
		return FALSE;
	
	//监听
	reVal = listen(m_sListen, SOMAXCONN);
	if (SOCKET_ERROR == reVal)
		return FALSE;
	
	return TRUE;
}

/*
 *	清除套接字和事件对象
 */
void CEventSelectServerDlg::ClearSocketAndEventFromArr( const int nIndex )
{	
	//将该套接字和事件对象从数组中清除
	for (int i = nIndex - WSA_WAIT_EVENT_0; i < m_nTotalEvent; i++)
	{
		m_arrEvent[i] = m_arrEvent[i + 1];	
		m_arrSocket[i] = m_arrSocket[i + 1];
	}
	//总数减1		
	--m_nTotalEvent;
}

/*
 * 从管理客户端链表中查找对应的CClientSocket指针
 */

CClientSocket* CEventSelectServerDlg::FindClientNode( const SOCKET s )
{
	m_csList.Lock();
	POSITION pos = NULL;
	for (pos = m_UserList.GetHeadPosition(); pos != NULL;)
	{
		CClientSocket *pClientSocket = (CClientSocket*)m_UserList.GetNext(pos);
		if (pClientSocket->m_s == s)
		{
			m_csList.Unlock();
			return pClientSocket;
		}
	}
	m_csList.Unlock();
	return NULL;
}

/*
 * 从管理客户端链表删除节点
 */
void CEventSelectServerDlg::DeleteClientNode( SOCKET s )
{
	POSITION		pos1 = NULL;			//临时变量
	POSITION		pos2 = NULL;			//临时变量
	CClientSocket	*pClientSocket = NULL;	//CClientSocket指针
	BOOL			bFinder = FALSE;		//是否找到该套接字

	m_csList.Lock();//进入临界区
	//遍历整个链表找到套接字对应的节点
	for (pos1 = m_UserList.GetHeadPosition(); (pos2 = pos1) != NULL;)
	{
		pClientSocket = (CClientSocket*)m_UserList.GetNext(pos1);
		if (pClientSocket->m_s == s)
		{
			bFinder = TRUE;
			break;
		}
	}
	ASSERT(TRUE == bFinder);
	m_UserList.RemoveAt(pos2);	//从链表中删除该节点
	delete pClientSocket;		//释放内存
	pClientSocket = NULL;
	m_csList.Unlock();			//离开临界区
	
	UpdateListCtrl();			//更新服务器用户列表
	SendUserList();				//发送用户列表	
}



/*
 * 发送用户列表
 */
void CEventSelectServerDlg::SendUserList( void )
{
	CString strUserInfoList;			//临时变量
	MakeSendUserString(strUserInfoList);//准备用户列表
	m_csList.Lock();

	//发送用户列表
	POSITION pos = NULL;
	for (pos = m_UserList.GetHeadPosition(); NULL != pos ;)
	{
		CClientSocket *pClientSocket = (CClientSocket*)m_UserList.GetNext(pos);
		pClientSocket->Send(USERINFOR, strUserInfoList);	
	}
	m_csList.Unlock();
}



/*
 * 更新用户列表
 */

BOOL CEventSelectServerDlg::UpdateUserList(const CString strUserInfo, CClientSocket* pClientSocket)
{
	//解析用户信息
	CUserInfo userInfo = ParserUserInfo(strUserInfo);
	
	//判断是否重复登录
	if (CUserInfo::LOGIN == userInfo.m_state)//登录
	{
		m_csList.Lock();
		//不能名字重复
		POSITION pos1 = NULL;
		POSITION pos2 = NULL;
		for (pos1 = m_UserList.GetHeadPosition(); (pos2 = pos1)!=NULL;)
		{
			CClientSocket *pClient = (CClientSocket*)m_UserList.GetNext(pos1);
			if (pClient->m_UserInfo.m_strName == userInfo.m_strName)//名字重复
			{
				pClientSocket->Send(SERVMESSAGE, "用户重复登录！");	//发送消息
				m_csList.Unlock();
				return FALSE;			
			}			
		}
		m_csList.Unlock();
		pClientSocket->SaveUserInfo(userInfo);//保存用户信息
	}else//用户更改状态
	{
		m_csList.Lock();
		BOOL bFinder = FALSE;
		POSITION pos;
		for (pos = m_UserList.GetHeadPosition(); NULL != pos;)
		{
			CClientSocket *pClient = (CClientSocket*)m_UserList.GetNext(pos);
			if (pClient->m_UserInfo.m_strName == userInfo.m_strName)//修改状态
			{
				pClient->m_UserInfo.m_state = userInfo.m_state;
				bFinder = TRUE;
				break;
			}			
		}
		m_csList.Unlock();
		ASSERT(bFinder);	//必须找到该用户
	}

	UpdateListCtrl();		//更新服务器用户列表
	SendUserList();			//发送用户列表	
	return TRUE;
}

/*
 * 解析用户信息
 */
CUserInfo CEventSelectServerDlg::ParserUserInfo( const CString strUserInfo )
{
	CUserInfo userInfo;
	int nFind = strUserInfo.Find('|');
	ASSERT(-1 != nFind);
	userInfo.m_strName = strUserInfo.Left(nFind);		//名字
	CString strRight = strUserInfo.Mid(nFind + 1);		//剩余字符串
	
	nFind = strRight.Find('|');
	userInfo.m_lIP = atol(strRight.Left(nFind));		//IP
		
	strRight = strRight.Mid(nFind + 1);
	nFind = strRight.Find('|');
	userInfo.m_uPort = atoi(strRight.Left(nFind));		//端口

	userInfo.m_state = (CUserInfo::USERSTATE)(atoi(strRight.Right(1)));//状态
	
	return userInfo;	
}
/*
 * 把用户信息转换为CString对象<名称|ip|port|state>
 */
void CEventSelectServerDlg::MakeSendUserString( CString &strUserInfoList )
{
	m_csList.Lock();
	POSITION pos = NULL;
	for(pos = m_UserList.GetHeadPosition(); NULL != pos;)
	{
		CClientSocket *pClientSocket = (CClientSocket*)m_UserList.GetNext(pos);
		CUserInfo userInfo = pClientSocket->m_UserInfo;
		CString	strUserInfo;							//用户信息
		CString strUserName;							//名称
		strUserName = userInfo.m_strName;		
		CString strUserIP;								//IP
		strUserIP.Format("%ld",userInfo.m_lIP);
		CString strUserPort;							//Port
		strUserPort.Format("%d", userInfo.m_uPort);
		CString strUserState;							//状态
		strUserState.Format("%d",(int)userInfo.m_state);
		strUserInfo = "<" + strUserName + "|"
							+ strUserIP + "|"
							+ strUserPort + "|"
							+ strUserState + ">";
		
		//加入列表
		strUserInfoList += strUserInfo;
	}

	m_csList.Unlock();
}
/*
 * 对超过64个客户端套接字的处理
 */
void CEventSelectServerDlg::HandleOverClientNum( SOCKET sClient )
{
	int			nErrCode;//返回值	
	PACKETHDR	packetLen;//定义包头
	
	CString strMessage(_T("请稍候再登录服务器。"));

	packetLen.type = SERVMESSAGE;			//类型
	packetLen.len = strMessage.GetLength();	//数据长度

	//发送包头
	nErrCode = send(sClient, (char*)&packetLen, PACKETHDRLEN, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("发送消息失败！");
		return ;
	}
	//发送包体
	nErrCode = send(sClient, strMessage, packetLen.len, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("发送消息失败！");
		return ;
	}
	//关闭套接字
	closesocket(sClient);
	sClient = INVALID_SOCKET;
}

/*
 * 更新服务器界面
 */
void CEventSelectServerDlg::UpdateListCtrl( void )
{
	//删除原来数据项
	m_csList.Lock();
	m_ctlUserList.DeleteAllItems();
	
	POSITION	pos = NULL;		//位置变量
	int			nIndex = 0;		//序号
	for (pos = m_UserList.GetHeadPosition(); NULL != pos;)
	{
		//取出用户信息
		CClientSocket *pClientSocket = (CClientSocket*)m_UserList.GetNext(pos);
		CUserInfo userInfo = pClientSocket->m_UserInfo;		
		
		//第一列
		m_ctlUserList.InsertItem(nIndex, userInfo.m_strName, -1);//名称		
		//第二列
		LVITEM item;
		item.mask = LVIF_IMAGE;
		item.iItem = nIndex;
		item.iSubItem = 1;
		if (userInfo.m_state == CUserInfo::PLAY)				//游戏状态
		{
			item.iImage = 0;
		}else													//等待游戏状态
		{
			item.iImage = 1;
		}
		m_ctlUserList.SetItem(&item);
		
		//第三列
		in_addr userAddr;
		userAddr.S_un.S_addr = (long)userInfo.m_lIP;
		CString strUserAddr(inet_ntoa(userAddr));				//IP
		m_ctlUserList.SetItemText(nIndex, 2, strUserAddr);
		
		//第四列
		CString strUserPort;									//端口
		strUserPort.Format("%d",ntohs(userInfo.m_uPort));	
		m_ctlUserList.SetItemText(nIndex,3,strUserPort);	
		
		++nIndex;		
	}
	m_csList.Unlock();	
}

/*
 *	服务线程函数
 */
DWORD WINAPI CEventSelectServerDlg::ServiceThread( void *pParam )
{
	PTHREADPARAM pThreadParam = (PTHREADPARAM)pParam;
	
	while(*pThreadParam->bRunning)//服务器运行状态
	{
		DWORD	dwIndex = 0;//返回值
		
		//等待网络事件
		if ((dwIndex = WSAWaitForMultipleEvents(*pThreadParam->pTotalEvent, 
												pThreadParam->pArrEvent, 
												FALSE, SERVER_WAIT_TIMEOUT, FALSE))
												== WSA_WAIT_FAILED)
		{
			AfxMessageBox("等待事件错误！");
			break;
		}		
	
		//超时处理
		if (WSA_WAIT_TIMEOUT == dwIndex)
		{
			Sleep(SERVER_WAIT_TIMEOUT);
			continue;
		}
		
		//发生网络事件
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
				AfxMessageBox("FD_ACCEPT_BIT网络事件错误！");
				break;
			}
			//通知主线程
			::SendMessage(pThreadParam->hServHwnd, WM_USER_ACCEPT, dwIndex, NULL);
		}		
		
		//发生FD_READ网络事件	
		if (networkEvents.lNetworkEvents & FD_READ )
		{
			if (networkEvents.iErrorCode[FD_READ_BIT] != 0)//发生网络错误
			{
				AfxMessageBox("FD_READ_BIT网络事件错误！");
				break;
			}else
			{
				//通知主线程
				::SendMessage(pThreadParam->hServHwnd, WM_USER_READ,dwIndex, NULL);
			}
		}		
		//发生FD_CLOSE网络事件	
		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{	
			if (networkEvents.iErrorCode[FD_CLOSE_BIT] != 0)//发生网络错误
			{
				AfxMessageBox("FD_CLOSE_BIT网络事件错误！");
				break;
			}else
			{
				//通知主线程
				::SendMessage(pThreadParam->hServHwnd, WM_USER_CLOSE,dwIndex,NULL);	
			}			
		}
	}	

	SetEvent(pThreadParam->hEventExit);	//通知主线程
	delete pThreadParam;				//释放资源
	pThreadParam = NULL;	
	return 0;	
}

void CEventSelectServerDlg::DeleteAllClientNodes( void )
{
	POSITION		pos = NULL;
	CClientSocket	*pClientSocket = NULL;
	
	m_csList.Lock();
	//遍历整个链表找到套接字对应的节点
	for (pos = m_UserList.GetHeadPosition(); pos != NULL;)
	{
		pClientSocket = (CClientSocket*)m_UserList.GetNext(pos);
		delete pClientSocket;

	}
	m_UserList.RemoveAll();
	m_csList.Unlock();
	pClientSocket = NULL;	
}

/*
 * FD_CLOSE事件响应函数
 */
afx_msg LRESULT CEventSelectServerDlg::OnSocketClose( WPARAM wParam, LPARAM lParam )
{
	int nIndex = wParam;
	//删除该节点
	DeleteClientNode(m_arrSocket[nIndex - WSA_WAIT_EVENT_0]);
	//将该套接字和事件对象从数组中清除
	ClearSocketAndEventFromArr(nIndex - WSA_WAIT_EVENT_0);	
	return 0;
}

/*
 *	FD_READ事件响应函数
 */
afx_msg LRESULT CEventSelectServerDlg::OnSocketRead( WPARAM wParam, LPARAM lParam )
{
	//获取客户端指针
	CClientSocket *pClientSocket = FindClientNode(
									m_arrSocket[wParam - WSA_WAIT_EVENT_0]);
	if (NULL != pClientSocket)
	{
		BOOL reVal;						//返回值
		reVal = pClientSocket->Recv();	//接收数据
		if (FALSE == reVal)				//操作失败或者用户重复登录
		{
			DeleteClientNode(m_arrSocket[wParam - WSA_WAIT_EVENT_0]);//删除该节点	
			//将该套接字和事件对象从数组中清除
			ClearSocketAndEventFromArr(wParam - WSA_WAIT_EVENT_0);	
			return -1;
		}
	}
	return 0;
}

/*
 * 接受客户端连接请求
 */
afx_msg LRESULT CEventSelectServerDlg::OnSocketAccept( WPARAM wParam, LPARAM lParam )
{
	SOCKET sAccept = INVALID_SOCKET;//套接字
	ASSERT(m_sListen == m_arrSocket[wParam - WSA_WAIT_EVENT_0]);
	
	//接受连接请求
	if ((sAccept = accept(m_arrSocket[wParam - WSA_WAIT_EVENT_0],NULL, NULL)) == INVALID_SOCKET)
	{		
		AfxMessageBox("接受连接请求错误！");
		return -1;
	}
	
	//超过最大值
	if (m_nTotalEvent > WSA_MAXIMUM_WAIT_EVENTS)
	{
		HandleOverClientNum(sAccept);//对超过64个套接字的处理
		return 0;
	}			
	//为接受套接字创建事件对象
	if ((m_arrEvent[m_nTotalEvent] = WSACreateEvent()) == WSA_INVALID_EVENT)
	{
		AfxMessageBox("为新建套接字创建套接字事件对象错误！");
		return 0;
	}
	m_arrSocket[m_nTotalEvent] = sAccept;//加入套接字数组
	
	//为套接字注册FD_READ FD_CLOSE网络事件
	if (WSAEventSelect(sAccept, m_arrEvent[m_nTotalEvent], FD_READ|FD_CLOSE) == SOCKET_ERROR)
	{
		AfxMessageBox("为新建套接字注册网络事件错误！");
		return 0;
	}
	
	//创建CClientSocket对象
	CClientSocket *pClient = new CClientSocket(this, sAccept, m_arrEvent[m_nTotalEvent]);
	
	m_csList.Lock();
	m_UserList.AddTail(pClient);//加入链表
	m_csList.Unlock();
	++m_nTotalEvent;			//总数加1
	return 0;
}