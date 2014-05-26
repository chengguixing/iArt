
// WSAAsyncSelectServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WSAAsyncSelectServer.h"
#include "WSAAsyncSelectServerDlg.h"
#include "afxdialogex.h"
#include "ClientSocket.h"
#include "ListenSocket.h"
#include "ServerAddressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CWSAAsyncSelectServerDlg 对话框



CWSAAsyncSelectServerDlg::CWSAAsyncSelectServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWSAAsyncSelectServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pListenSocket = NULL;
	m_pImageList = NULL;
}


CWSAAsyncSelectServerDlg::~CWSAAsyncSelectServerDlg()
{
	NULL;
}

void CWSAAsyncSelectServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLIENT_LIST, m_ctlUserList);
}

BEGIN_MESSAGE_MAP(CWSAAsyncSelectServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_CLIENT_LIST, OnRclickClientList)
	ON_COMMAND(ID_SERVER_LIST_DELETE_MENUITEM, OnServerListDeleteMenuitem)
END_MESSAGE_MAP()


// CWSAAsyncSelectServerDlg 消息处理程序

BOOL CWSAAsyncSelectServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();



	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	Init();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWSAAsyncSelectServerDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWSAAsyncSelectServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CWSAAsyncSelectServerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

	SaveUserList(m_UserList);			//保存用户信息

	SaveOfflineMsg(m_OfflineMsgList);	//保存离线信息

	DeleteAllChatter();					//删除在线用户

	if (NULL != m_pListenSocket)		//删除CListenSocket类对象
	{
		delete m_pListenSocket;
		m_pListenSocket = NULL;
	}
	if (NULL != m_pImageList)			//删除CImageList类对象
	{
		delete m_pImageList;
		m_pImageList = NULL;
	}
}


void CWSAAsyncSelectServerDlg::ProcessPendingAccept(void)
{
	CClientSocket* pClient = new CClientSocket(this);
	SOCKADDR_IN clientAddr;
	int socketLen = sizeof(SOCKADDR_IN);

	m_pListenSocket->Accept(*pClient, (SOCKADDR*)&clientAddr, &socketLen);

	pClient->Init();
	pClient->SaveUserInfo(clientAddr);

	m_csChatterList.Lock();
	m_ChatterList.AddTail(pClient);
	m_csChatterList.Unlock();


}


BOOL CWSAAsyncSelectServerDlg::Init(void)
{
	InitListCtrlSetting();
	InitUserList();
	LoadOfflineMsg(m_OfflineMsgList);

	return TRUE;
}


BOOL CWSAAsyncSelectServerDlg::StartService(void)
{
	CServerAddressDlg servAddrDlg;
	if (IDOK != servAddrDlg.DoModal())
	{
		return FALSE;
	}


	in_addr servAdd;
	servAdd.S_un.S_addr = htonl(servAddrDlg.m_strServIP);
	CString strServIP = inet_ntoa(servAdd);

	m_pListenSocket = new CListenSocket(this);


	if (m_pListenSocket->Create(servAddrDlg.m_shServPort, SOCK_STREAM, strServIP))
	{
		if (!m_pListenSocket->Listen())
		{
			delete m_pListenSocket;
			m_pListenSocket = NULL;
			AfxMessageBox(_T("创建套接字失败!"));
			return FALSE;
		}
	}
	else
	{
		delete m_pListenSocket;
		m_pListenSocket = NULL;
		AfxMessageBox(_T("创建套接字失败!"));
		return FALSE;
	}
	return TRUE;
}

void CWSAAsyncSelectServerDlg::InitListCtrlSetting(void)
{
	m_pImageList = new CImageList();
	ASSERT(m_pImageList != NULL);

	m_pImageList->Create(32, 32, ILC_COLOR8 | ILC_MASK, 2, 2);
	CWinApp* pApp = AfxGetApp();
	HICON hIconOnline = pApp->LoadIconA(IDI_CHATTER_ONLINE);
	HICON hIconOffline = pApp->LoadIconA(IDI_CHATTER_OFFLINE);

	m_pImageList->Add(hIconOnline);
	m_pImageList->Add(hIconOffline);

	m_ctlUserList.SetImageList(m_pImageList, LVSIL_SMALL);

	m_ctlUserList.InsertColumn(0, _T("名称"), LVCFMT_IMAGE | LVCFMT_LEFT);
	m_ctlUserList.InsertColumn(1, _T("密码"), LVCFMT_CENTER);
	m_ctlUserList.InsertColumn(2, _T("地址"), LVCFMT_CENTER);
	m_ctlUserList.InsertColumn(3, _T("端口"), LVCFMT_CENTER);
	m_ctlUserList.InsertColumn(4, _T("时间"), LVCFMT_CENTER);


	CRect rect;
	m_ctlUserList.GetClientRect(&rect);

	int nWidth = rect.Width();
	m_ctlUserList.SetColumnWidth(0, nWidth * 25 / 100);
	m_ctlUserList.SetColumnWidth(1, nWidth * 13 / 100);
	m_ctlUserList.SetColumnWidth(2, nWidth * 25 / 100);
	m_ctlUserList.SetColumnWidth(3, nWidth * 12 / 100);
	m_ctlUserList.SetColumnWidth(4, nWidth * 25 / 100);

}


void CWSAAsyncSelectServerDlg::InitUserList(void)
{
	LoadUserList(m_UserList);

	POSITION pos;
	int nLine = 0;
	for (pos = m_UserList.GetHeadPosition(); NULL != pos;)
	{
		CUserInfo* pUserInfo = (CUserInfo*)m_UserList.GetNext(pos);
		if (NULL != pUserInfo)
		{
			m_ctlUserList.InsertItem(nLine, pUserInfo->m_strName, 1);
			m_ctlUserList.SetItemText(nLine, 1, pUserInfo->m_strPassword);

			in_addr userAddr;
			userAddr.S_un.S_addr = (long)pUserInfo->m_lIP;
			CString strUserAddr(inet_ntoa(userAddr));
			m_ctlUserList.SetItemText(nLine, 2, strUserAddr);

			CString strUserPort;
			strUserPort.Format("%d", ntohs(pUserInfo->m_nPort));
			m_ctlUserList.SetItemText(nLine, 3, strUserPort);

			CString strUserTime = pUserInfo->m_time.Format("%c");
			m_ctlUserList.SetItemText(nLine, 4, strUserTime);
			
			
			++nLine;
		}
	}
}


void CWSAAsyncSelectServerDlg::SaveUserList(CObList& obList)
{
	char appPath[256];
	GetCurrentDirectory(256, appPath);
	CString filePath;
	filePath.Format("%s", appPath);
	filePath += "\\";
	filePath += USERLISTINI;

	DeleteFile(filePath);

	POSITION pos;
	int nIndex = 0;

	for (pos = obList.GetHeadPosition(); pos != NULL;)
	{
		CUserInfo* pUserInfo = (CUserInfo*)obList.GetNext(pos);
		if (NULL == pUserInfo)
		{
			break;
		}

		CString strSection("section");
		CString strIndex;
		strIndex.Format("%d", nIndex);
		strSection += strIndex;

		in_addr userAddr;
		userAddr.S_un.S_addr = (long)pUserInfo->m_lIP;
		CString strUserAddr(inet_ntoa(userAddr));

		CString strUserPort;
		strUserPort.Format("%d", pUserInfo->m_nPort);

		CString strUserTime;
		strUserTime.Format("%ld", (long)pUserInfo->m_time.GetTime());

		//写用户名字
		WritePrivateProfileString(strSection, _T("Name"), pUserInfo->m_strName, filePath);	
		//写用户密码
		WritePrivateProfileString(strSection, _T("Password"), pUserInfo->m_strPassword, filePath);	
		//写用户地址
		WritePrivateProfileString(strSection, _T("Address"), strUserAddr,filePath);	
		//写用户端口
		WritePrivateProfileString(strSection, _T("Port"), strUserPort,filePath);					
		//写用时间
		WritePrivateProfileString(strSection, _T("Time"), strUserTime,filePath);					


		delete pUserInfo;//删除对象
		pUserInfo = NULL;
		nIndex++;
	}
	obList.RemoveAll();
}


void CWSAAsyncSelectServerDlg::LoadUserList(CObList& obList)
{
#define  TEMP_BUF_SIZE 32//缓冲区长度

	char appPath[256];
	GetCurrentDirectory(256, appPath);	//取得应用程序当前路径
	CString  filePath;					//保存Ini文件名
	filePath.Format("%s", appPath);
	filePath += "\\";
	filePath += USERLISTINI;

	int nIndex = 0;
	while (TRUE)
	{
		CString strSection("section");
		CString strIndex;
		strIndex.Format("%d", nIndex);
		strSection += strIndex;

		//用户名称
		CString strSectionKey = "Name";					
		CString strValue = _T("");
		char cBuf[TEMP_BUF_SIZE];
		memset(cBuf, 0, TEMP_BUF_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}

		CUserInfo *pUserInfo = new CUserInfo();//新建CUserInfo对象
		pUserInfo->m_strName = cBuf;

		//读取用户密码
		strSectionKey = "Password";
		memset(cBuf, 0, TEMP_BUF_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}
		pUserInfo->m_strPassword = cBuf;

		//读取用户地址
		strSectionKey = "Address";
		memset(cBuf, 0, TEMP_BUF_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}
		u_long clinetAddr = inet_addr(cBuf);
		pUserInfo->m_lIP = (DWORD)clinetAddr;

		//读取用户端口
		strSectionKey = "Port";
		memset(cBuf, 0, TEMP_BUF_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}
		pUserInfo->m_nPort = atoi(cBuf);

		//读取用户时间
		strSectionKey = "Time";
		memset(cBuf, 0, TEMP_BUF_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}
		time_t time = atol(cBuf);
		pUserInfo->m_time = time;

		obList.AddTail(pUserInfo);		//加入链表
		++nIndex;
	}
}

void CWSAAsyncSelectServerDlg::SaveOfflineMsg(CObList& obList)
{
	char appPath[256];
	GetCurrentDirectory(256, appPath);	//取得应用程序当前路径
	CString  filePath;					//保存Ini文件名
	filePath.Format("%s", appPath);
	filePath +="\\";
	filePath += OFFLINEMSGLIST;	

	//删除原来的文件
	DeleteFile(filePath);

	POSITION pos;
	int nIndex = 0;
	for(pos = obList.GetHeadPosition(); pos != NULL; )
	{
		CChatPacket *pPacket = (CChatPacket*)obList.GetNext(pos);//用户信息
		if (NULL == pPacket)
		{
			break;
		}

		CString strSection("section");							//section
		CString strIndex;
		strIndex.Format("%d", nIndex);
		strSection += strIndex;

		CString strTime;
		strTime.Format("%ld",(long)pPacket->m_time.GetTime());

		pPacket->m_strMsg.Replace("\r\n", "$");//取代回车符为$
		//写发送消息的用户名称 
		WritePrivateProfileString(strSection, _T("SendMsgUser"), pPacket->m_UserInfo.m_strName, filePath);		//发送消息用户
		//写接收消息的用户名称
		WritePrivateProfileString(strSection, _T("RecvMsgUser"), pPacket->m_OfflineUserInfo.m_strName, filePath);	//接收消息用户
		//写发送消息 的时间
		WritePrivateProfileString(strSection, _T("Time"), strTime,filePath);									//发送消息时间
		//写发送的消息
		WritePrivateProfileString(strSection, _T("Message"), pPacket->m_strMsg, filePath);						//发送的消息
		delete pPacket;
		pPacket = NULL;
		++nIndex;
	}
	obList.RemoveAll();

}

void CWSAAsyncSelectServerDlg::LoadOfflineMsg(CObList& obList)
{
	m_csOfflineList.Lock();	
	char appPath[256];
	GetCurrentDirectory(256,appPath);	//取得应用程序当前路径
	CString  filePath;					//保存Ini文件名
	filePath.Format("%s",appPath);
	filePath += "\\";
	filePath += OFFLINEMSGLIST;

	int nIndex = 0;
	while (TRUE)
	{
		CString strSection("section");						//section
		CString strIndex;
		strIndex.Format("%d",nIndex);
		strSection += strIndex;

		//读取发送消息的用户名称
		CString strSectionKey = "SendMsgUser";
		CString strValue = _T("");
		char cBuf[MAX_MSG_SIZE];
		memset(cBuf, 0, MAX_MSG_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}

		CChatPacket *pPacket = new CChatPacket();	//新建CChatPacket对象
		pPacket->m_type = CChatPacket::MESSAGE;
		pPacket->m_UserInfo.m_strName = cBuf;		//发送消息的用户名


		//读取接收消息的用户名称
		strSectionKey = "RecvMsgUser";
		memset(cBuf, 0, MAX_MSG_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}
		pPacket->m_OfflineUserInfo.m_strName = cBuf;


		//读取发送消息时间
		strSectionKey = "Time";
		memset(cBuf, 0, MAX_MSG_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}
		time_t time = atol(cBuf);
		pPacket->m_time = time;


		//读取发送消息
		strSectionKey = "Message";
		memset(cBuf, 0, MAX_MSG_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}

		////取代$为回车符
		CString strMsg(cBuf);
		strMsg.Replace( "$", "\r\n");
		pPacket->m_strMsg = strMsg;


		obList.AddTail(pPacket);//加入链表
		++nIndex;
	}

	m_csOfflineList.Unlock();
}


BOOL CWSAAsyncSelectServerDlg::ProcessPendingRead(CArchive* pArchiveIn, CArchive* pArchiveOut, CClientSocket* pClientSocket)
{
	do 
	{		
		CChatPacket chatPact;//接收数据包
		chatPact.Serialize(*pArchiveIn);

		if (CChatPacket::MESSAGE == chatPact.m_type)//发给离线用户的消息
		{
			m_csOfflineList.Lock();

			//保存离线用户消息
			CChatPacket *pChatPacket = new CChatPacket();
			//包类型
			pChatPacket->m_type = CChatPacket::MESSAGE;	
			//发送消息用户
			pChatPacket->m_UserInfo.m_strName = chatPact.m_UserInfo.m_strName;				
			//接收消息用户
			pChatPacket->m_OfflineUserInfo.m_strName = chatPact.m_OfflineUserInfo.m_strName;
			//发送的消息
			pChatPacket->m_strMsg = chatPact.m_strMsg;	
			//发包时间
			pChatPacket->m_time = chatPact.m_time;											

			//加入离线消息链表
			m_OfflineMsgList.AddTail(pChatPacket);											

			m_csOfflineList.Unlock();			

		}else if (CChatPacket::USERLIST == chatPact.m_type)//请求用户列表--用户刚登录
		{
			//保存用户信息
			pClientSocket->SaveUserInfo(chatPact.m_UserInfo);

			//户更新信息
			if (!UpdateUserList(chatPact.m_UserInfo, /*pArchiveOut,*/ pClientSocket))
				return FALSE;

			//向所有的在线用户发送用户列表
			SendUserList();

			//转发离线消息
			TransmitMsg(chatPact, /*pArchiveOut*/pClientSocket);

			//更新服务器界面
			UpdateServerListCtl(m_UserList);

		}

	} while(!pArchiveIn->IsBufferEmpty());

	return TRUE;
}


void CWSAAsyncSelectServerDlg::SendUserList(void)
{
	//复制用户链表
	CObList tempChatterlist;
	CopyUserList(tempChatterlist);

	CChatPacket packet;
	packet.m_type = CChatPacket::USERLIST;	//包的类型
	packet.m_pUserList = &tempChatterlist;	//用户链表

	m_csChatterList.Lock();//上锁

	//向所有在线用户发送列表
	POSITION pos = NULL;
	for (pos = m_ChatterList.GetHeadPosition(); NULL!=pos;)
	{
		CClientSocket *pClientSocket = (CClientSocket*)m_ChatterList.GetNext(pos);
		pClientSocket->SendUserList(&packet);//发送数据包
	}
	m_csChatterList.Unlock();//解锁

	DeleteTempUserList(tempChatterlist);//删除临时链表
}
void CWSAAsyncSelectServerDlg::CopyUserList(CObList& obList)
{
	if (m_UserList.IsEmpty())
	{
		return ;
	}

	POSITION pos = NULL;
	CUserInfo *pUserInfo = NULL;
	CUserInfo *pTempUserInfo = NULL;
	for (pos = m_UserList.GetHeadPosition(); pos != NULL;)
	{
		pUserInfo = (CUserInfo*)m_UserList.GetNext(pos);

		//不要复制用户的密码
		pTempUserInfo = new CUserInfo();
		pTempUserInfo->m_strName = pUserInfo->m_strName;	//名称
		pTempUserInfo->m_eStatus = pUserInfo->m_eStatus;	//状态
		pTempUserInfo->m_lIP = pUserInfo->m_lIP;			//IP
		pTempUserInfo->m_nPort = pUserInfo->m_nPort;		//端口

		obList.AddTail(pTempUserInfo);
	}
}
void CWSAAsyncSelectServerDlg::DeleteTempUserList(CObList& obList)
{
	if (obList.IsEmpty())
	{
		return;
	}

	POSITION pos = NULL;
	CUserInfo *pUserInfo = NULL;

	for (pos = obList.GetHeadPosition(); NULL != pos;)
	{
		pUserInfo = (CUserInfo*)obList.GetNext(pos);
		delete pUserInfo;
		pUserInfo = NULL;
	}

	obList.RemoveAll();
}

BOOL CWSAAsyncSelectServerDlg::UpdateUserList(const CUserInfo& userInfo, CClientSocket* pClientSocket)
{
	BOOL retVal = TRUE;				//返回值
	m_csUserList.Lock();			//上锁

	POSITION	pos;				//位置变量		
	CUserInfo	*pUserInfo = NULL;	//用户对象指针
	BOOL bFind = FALSE;				//是否找到该用户

	//该用户是否存在于链表中
	for (pos = m_UserList.GetHeadPosition(); NULL != pos;)
	{
		pUserInfo = (CUserInfo*)m_UserList.GetNext(pos);//节点下移		

		if (pUserInfo->m_strName == userInfo.m_strName )//名字相同
		{
			if (pUserInfo->m_strPassword == userInfo.m_strPassword)	//在册用户
			{
				if(CUserInfo::ONLINE == pUserInfo->m_eStatus)		//用户已经登录
				{
					//从在线链表中删除该节点
					DeleteChatter(pClientSocket);

					//发送“用户已经登录”消息
					CChatPacket packet;
					packet.m_type = CChatPacket::SERVERMSG;		//数据包类型
					packet.m_strMsg = _T("    用户已经登录!   ");//消息
					packet.m_time = CTime::GetCurrentTime();	//时间

					pClientSocket->SendUserMsg(&packet);		//发送数据

					m_csUserList.Unlock();						//解锁	
					return FALSE;

				}else//用户登录
				{
					pUserInfo->m_eStatus = CUserInfo::ONLINE;	//修改用户状态
					pUserInfo->m_lIP = userInfo.m_lIP;			//IP地址
					pUserInfo->m_nPort = userInfo.m_nPort;		//端口
					bFind = TRUE;
					break;						
				}

			}else//密码错误
			{
				//从在线链表中删除该节点
				DeleteChatter(pClientSocket);

				//发送“密码错误”消息
				CChatPacket packet;
				packet.m_type = CChatPacket::SERVERMSG;						//数据包类型
				packet.m_strMsg = _T("    密码错误\t\n    请重新登录!    "); //消息	
				packet.m_time = CTime::GetCurrentTime();					//时间

				pClientSocket->SendUserMsg(&packet);//发送数据

				m_csUserList.Unlock();//解锁
				return FALSE;				
			}			
		}
	}

	if (FALSE == bFind)//注册新用户
	{
		CUserInfo *pUserInfo = new CUserInfo();
		pUserInfo->m_strName = userInfo.m_strName;			//姓名
		pUserInfo->m_strPassword = userInfo.m_strPassword;	//密码
		pUserInfo->m_eStatus = CUserInfo::ONLINE;			//状态
		pUserInfo->m_lIP = userInfo.m_lIP;					//地址
		pUserInfo->m_nPort = userInfo.m_nPort;				//端口
		pUserInfo->m_time = userInfo.m_time;				//时间

		m_UserList.AddTail(pUserInfo);						//加入链表
	}

	m_csUserList.Unlock();//解锁
	return retVal;	
}


void CWSAAsyncSelectServerDlg::TransmitMsg(const CChatPacket& packet, CClientSocket* pClientSocket)
{
	m_csOfflineList.Lock();
	POSITION pos1 = NULL;		//位置变量
	POSITION pos2 = NULL;		//位置变量
	CChatPacket *pPacket = NULL;//数据包

	//遍历整个离线消息链表
	for (pos1 = m_OfflineMsgList.GetHeadPosition(); (pos2 = pos1) != NULL;)
	{
		//取出数据包
		pPacket = (CChatPacket*)m_OfflineMsgList.GetNext(pos1);

		//登录用户的名字和接收离线消息的用户名字相同
		if (pPacket->m_OfflineUserInfo.m_strName == packet.m_UserInfo.m_strName)
		{
			pClientSocket->SendUserMsg(pPacket);//发送离线消息
			m_OfflineMsgList.RemoveAt(pos2);	//删除链表节点
			delete pPacket;						//删除对象
			pPacket = NULL;					
		}
	}
	m_csOfflineList.Unlock();
}

void CWSAAsyncSelectServerDlg::UpdateServerListCtl(const CObList& obList)
{
	m_csUserList.Lock();//上锁

	//删除原来数据项
	m_ctlUserList.DeleteAllItems();

	POSITION	pos = NULL;		//位置变量
	int			nIndex = 0;		//序号
	for (pos = m_UserList.GetHeadPosition(); NULL != pos;)
	{
		//取出用户信息
		CUserInfo *pUserInfo = (CUserInfo*)m_UserList.GetNext(pos);
		if (CUserInfo::ONLINE == pUserInfo->m_eStatus)//在线状态
		{
			//插入数据项 
			m_ctlUserList.InsertItem(nIndex, pUserInfo->m_strName,CUserInfo::ONLINE);

		}else//离线状态 		
		{
			//插入数据项 
			m_ctlUserList.InsertItem(nIndex, pUserInfo->m_strName,CUserInfo::OFFLINE);
		}

		m_ctlUserList.SetItemText(nIndex,1,pUserInfo->m_strPassword);	//用户密码

		in_addr userAddr;
		userAddr.S_un.S_addr = (long)pUserInfo->m_lIP;
		CString strUserAddr(inet_ntoa(userAddr));						//用户IP
		m_ctlUserList.SetItemText(nIndex,2,strUserAddr);

		CString strUserPort;											//用户端口
		strUserPort.Format("%d",ntohs(pUserInfo->m_nPort));	
		m_ctlUserList.SetItemText(nIndex,3,strUserPort);

		CString strUserTime = pUserInfo->m_time.Format("%c");			//登录时间
		m_ctlUserList.SetItemText(nIndex,4,strUserTime);
		nIndex++;		
	}
	m_csUserList.Unlock();//解锁
}

void CWSAAsyncSelectServerDlg::ReleaseChatter(CClientSocket* pClientSocket)
{
	m_csChatterList.Lock();
	POSITION pos = NULL;

	//找到该用户
	if ((pos = m_ChatterList.Find(pClientSocket)) != NULL)
	{
		ASSERT((CClientSocket*)m_ChatterList.GetAt(pos) == pClientSocket);

		//获取该用户信息
		CUserInfo userInfo = pClientSocket->GetUserInfo();

		//更新该用户信息
		POSITION posUser = NULL;
		for (posUser = m_UserList.GetHeadPosition(); posUser != NULL;)
		{
			CUserInfo *pUserInfo = (CUserInfo*)m_UserList.GetNext(posUser);
			if (pUserInfo->m_strName == userInfo.m_strName)
			{
				pUserInfo->m_eStatus = CUserInfo::OFFLINE;	//修改状态为离线
				pUserInfo->m_lIP = userInfo.m_lIP;			//修改用户地址			
				pUserInfo->m_nPort = userInfo.m_nPort;		//修改用户端口
				pUserInfo->m_time = CTime::GetCurrentTime();//设置离线时间
				break;
			}
		}

		//删除该在线用户
		m_ChatterList.RemoveAt(pos);

		//通知所有在线用户更新用户列表
		SendUserList();

		//更新服务器界面
		UpdateServerListCtl(m_UserList);	
	}

	m_csChatterList.Unlock();
}
void CWSAAsyncSelectServerDlg::DeleteChatter(CClientSocket* pClientSocket)
{
	m_csChatterList.Lock();
	POSITION pos = NULL;
	if ((pos = m_ChatterList.Find(pClientSocket)) != NULL)
	{
		ASSERT((CClientSocket*)m_ChatterList.GetAt(pos) == pClientSocket);		
		//删除该在线用户
		m_ChatterList.RemoveAt(pos);	
	}

	m_csChatterList.Unlock();
}
void CWSAAsyncSelectServerDlg::DeleteAllChatter(void)
{
	POSITION pos;
	CClientSocket *pClient = NULL;
	for (pos = m_ChatterList.GetHeadPosition(); NULL != pos; )
	{
		pClient = (CClientSocket*)m_ChatterList.GetNext(pos);
		delete pClient;
		pClient = NULL;
	}
	m_ChatterList.RemoveAll();
}

void CWSAAsyncSelectServerDlg::OnRclickClientList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();					//鼠标获取位置
		CPoint point( LOWORD(dwPos), HIWORD(dwPos) );

		CMenu menu;
		VERIFY( menu.LoadMenu(IDR_SERVER_LIST_MENU));	//装载菜单资源
		CMenu* popup = menu.GetSubMenu(0);				//获取菜单项
		ASSERT( popup != NULL );

		//弹出快捷菜单
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
	}
	*pResult = 0;
}
void CWSAAsyncSelectServerDlg::OnServerListDeleteMenuitem()
{
	//获得被选中数据项的位置
	POSITION pos = m_ctlUserList.GetFirstSelectedItemPosition();
	int nSelectedItem = m_ctlUserList.GetNextSelectedItem(pos);
	//获取用户名称
	CString strPeerName = m_ctlUserList.GetItemText(nSelectedItem, 0);

	POSITION pos1 = NULL;		//位置变量 
	POSITION pos2 = NULL;		//位置变量
	BOOL bDeleteUser = FALSE;	//是否找到该用户

	//在用户链表中查找
	for (pos1 = m_UserList.GetHeadPosition();(pos2=pos1)!= NULL;)
	{
		//取出数据
		CUserInfo* pUserInfo = (CUserInfo*)m_UserList.GetNext(pos1);
		if (pUserInfo->m_strName == strPeerName)			//名称相同
		{
			if (pUserInfo->m_eStatus == CUserInfo::ONLINE)	//不能删除在线用户
			{
				AfxMessageBox("不能删除在线用户！",MB_OK, -1);
				return ;
			}else//删除不在线用户
			{
				CUserInfo *pUserInfo = (CUserInfo*)m_UserList.GetAt(pos2);
				m_UserList.RemoveAt(pos2);	//删除指针
				delete pUserInfo;			//删除对象
				pUserInfo = NULL;			

				bDeleteUser = TRUE;
				//更新服务器界面
				UpdateServerListCtl(m_UserList);

			}
		}
	}

	if (TRUE == bDeleteUser)
	{
		//通知客户端更新成员列表
		SendUserList();
	}	
}