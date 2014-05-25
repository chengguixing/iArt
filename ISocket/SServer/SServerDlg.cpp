
// SServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SServer.h"
#include "SServerDlg.h"
#include "afxdialogex.h"
#include "Client.h"
#include "tslog/tslog.h"

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


// CSServerDlg 对话框



CSServerDlg::CSServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bConning = FALSE;
	m_hReqAndData = NULL;
	InitializeCriticalSection(&m_cs);
}

CSServerDlg::~CSServerDlg()
{
	NULL;
}


void CSServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS, m_ctlIP);
	DDX_Control(pDX, IDC_STATIC_TIP, m_ctlTip);
}

BEGIN_MESSAGE_MAP(CSServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_STARTUP, &CSServerDlg::OnButtonStartup)
END_MESSAGE_MAP()


// CSServerDlg 消息处理程序

BOOL CSServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
	m_ctlIP.SetAddress(127, 0, 0, 1);
	InitSocket();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSServerDlg::OnPaint()
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
HCURSOR CSServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSServerDlg::OnButtonStartup()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(TRUE);

	int reVal;
	DWORD dwServIP;
	m_ctlIP.GetAddress(dwServIP);

	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(dwServIP);
	servAddr.sin_port = htons(SERVERPORT);

	reVal = bind(m_sServer, (sockaddr*)&servAddr, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == reVal)
	{
		AfxMessageBox(_T("服务器绑定失败"), MB_OK, 0);
		closesocket(m_sServer);
		WSACleanup();
		return ;
	}
	else
	{
		m_ctlTip.SetWindowText(_T("服务器绑定成功!"));
		UpdateData(false);
	}


	reVal = listen(m_sServer, SOMAXCONN);
	TSDEBUG("WSAGetLastError() = %d", WSAGetLastError());
	if (SOCKET_ERROR == reVal)
	{
		AfxMessageBox(_T("服务器监听失败!"), MB_OK, 0);
		closesocket(m_sServer);
		WSACleanup();
		return ;
	}

	m_bConning = TRUE;

	DWORD dwThread;
	m_hReqAndData = CreateThread(NULL, 0, DirAndFileSizeServiceThread, this, 0, &dwThread);
	CloseHandle(m_hReqAndData);


	(CButton*)GetDlgItem(IDC_BUTTON_STARTUP)->EnableWindow(FALSE);
	m_ctlIP.EnableWindow(FALSE);
	m_ctlTip.SetWindowText(_T("服务器启动成功!"));
	UpdateData(FALSE);
}


void CSServerDlg::InitSocket(void)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int nErrCode;


	wVersionRequested = MAKEWORD(2, 2);
	nErrCode = WSAStartup(wVersionRequested, &wsaData);
	if (0 != nErrCode)
	{
		return ;
	}


	m_sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sServer)
	{
		return ;
	}


	unsigned int uiRcvBuf;
	int uiRcvBufLen = sizeof(uiRcvBuf);
	nErrCode = getsockopt(m_sServer, SOL_SOCKET, SO_SNDBUF, (char*)&uiRcvBuf, &uiRcvBufLen);
	if (SOCKET_ERROR == nErrCode)
	{
		return ;
	}


	uiRcvBuf *= BUF_TIMES;

	nErrCode = setsockopt(m_sServer, SOL_SOCKET, SO_SNDBUF, (char*)&uiRcvBuf, uiRcvBufLen);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox(_T("修改系统发送数据缓冲区失败!"));
	}

	unsigned int uiNewRcvBuf;
	getsockopt(m_sServer, SOL_SOCKET, SO_SNDBUF, (char*)&uiNewRcvBuf, &uiRcvBufLen);
	if (SOCKET_ERROR == nErrCode || uiNewRcvBuf != uiRcvBuf)
	{
		AfxMessageBox(_T("修改系统发送数据缓冲区失败!"));
	}
}


void CSServerDlg::UnInitSocket(void)
{
	closesocket(m_sServer);
	WSACleanup();
}


DWORD WINAPI CSServerDlg::DirAndFileSizeServiceThread(void* pParam)
{
	CSServerDlg* pServer = (CSServerDlg*)pParam;
	SOCKET sListen = pServer->GetSocket();

	FD_SET allSockfd;
	FD_ZERO(&allSockfd);
	FD_SET(sListen, &allSockfd);

	FD_SET readfd;
	FD_SET writefd;
	while (pServer->IsConnecting())
	{
		FD_ZERO(&readfd);
		FD_ZERO(&writefd);
		readfd = allSockfd;
		writefd = allSockfd;


		pServer->ShowClientNumberInfo(allSockfd.fd_count);

		int nRet = select(0, &readfd, &writefd, NULL, NULL);

		if (nRet > 0)
		{
			for (int i = 0; i < allSockfd.fd_count; ++i)
			{
				if (FD_ISSET(allSockfd.fd_array[i], &readfd))
				{
					if (allSockfd.fd_array[i] == sListen)
					{
						SOCKADDR_IN addrClient;
						int nAddrLen = sizeof(addrClient);
						SOCKET sClient = accept(sListen, (sockaddr*)&addrClient, &nAddrLen);
						CClient* pClient = new CClient(sClient, pServer);
						pServer->AddClient(pClient);
						FD_SET(sClient, &allSockfd);
						pServer->ShowClientNumberInfo(allSockfd.fd_count);
					}
					else
					{
						CClient* pClient = pServer->GetClient(allSockfd.fd_array[i]);
						if (NULL != pClient)
						{
							BOOL bRet = pClient->RecvData();
							if (FALSE == bRet)
							{
								SOCKET sTemp = allSockfd.fd_array[i];
								FD_CLR(allSockfd.fd_array[i], &allSockfd);
								pServer->ShowClientNumberInfo(allSockfd.fd_count);
							}
						}
					}
				}
				if (FD_ISSET(allSockfd.fd_array[i], &writefd))
				{
					CClient* pClient = pServer->GetClient(allSockfd.fd_array[i]);
					if (NULL != pClient)
					{
						BOOL bRet = pClient->SendData();

						if (FALSE == bRet)
						{
							SOCKET sDelete = allSockfd.fd_array[i];
							FD_CLR(allSockfd.fd_array[i], &allSockfd);
							pServer->DeleteClient(sDelete);
							pServer->ShowClientNumberInfo(allSockfd.fd_count);
						}
					}
				}
			}
		}
		Sleep(THREAD_SLEEP);
	}
	pServer->DeleteAllClient();
	return 0;
}


CClient* CSServerDlg::GetClient(SOCKET s)
{
	CClient* pClient = NULL;
	EnterCriticalSection(&m_cs);
	list<CClient*>::iterator it = m_listClient.begin();
	for (it; it != m_listClient.end(); ++it)
	{
		pClient = *it;
		if (s == pClient->GetSocket())
		{
			break;
		}
	}
	LeaveCriticalSection(&m_cs);
	return pClient;
}


void CSServerDlg::DeleteClient(SOCKET s)
{
	CClient* pClient = NULL;
	EnterCriticalSection(&m_cs);
	list<CClient*>::iterator it = m_listClient.begin();
	for (it; it != m_listClient.end(); ++it)
	{
		pClient = *it;
		if (s == pClient->GetSocket())
		{
			m_listClient.erase(it++);
			delete pClient;
			pClient = NULL;
			break;
		}
		else
		{
			++it;
		}
	}
	LeaveCriticalSection(&m_cs);
}

void CSServerDlg::DeleteAllClient(void)
{
	EnterCriticalSection(&m_cs);
	//遍历该链表
	list<CClient*>::iterator it = m_listClient.begin();
	for (it; it != m_listClient.end();)
	{
		CClient *pClient = (CClient*)*it;
		m_listClient.erase(it++);

		delete pClient;
		pClient = NULL;		
	}

	m_listClient.clear();
	LeaveCriticalSection(&m_cs); 
}


void CSServerDlg::AddClient(CClient *pClient)
{
	EnterCriticalSection(&m_cs);
	m_listClient.push_back(pClient);
	LeaveCriticalSection(&m_cs);
} 


void CSServerDlg::OnClose() 
{
	m_bConning = FALSE;								//设置服务器状态
	m_ctlTip.SetWindowText(_T("服务器退出！"));		 //更新界面
	Sleep(THREAD_SLEEP * 100);						//给子线程退出时间
	DeleteCriticalSection(&m_cs);					//删除临界区对象
	UnInitSocket();									//释放套接字资源
	CDialog::OnClose();
}

void CSServerDlg::ShowClientNumberInfo(int nNum)
{
	CString strTip;
	strTip.Format(_T("目前共有%d客户连接。"), nNum -1);	
	m_ctlTip.SetWindowText(strTip);
}


void CSServerDlg::ShowReqDirInfor(CString strDir)
{
	m_ctlTip.SetWindowText(strDir);
}


void CSServerDlg::ShowFileULInfor(CString strFileName)
{
	m_ctlTip.SetWindowText(strFileName);
}