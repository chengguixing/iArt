
// OverLapClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "OverLapClient.h"
#include "OverLapClientDlg.h"
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


// COverLapClientDlg 对话框



COverLapClientDlg::COverLapClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COverLapClientDlg::IDD, pParent),
	m_bRunning(FALSE),
	m_pLookupSocket(NULL),
	m_nEventTotal(0)
{
	m_sServPort = SERVERPORT;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COverLapClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLIENT_IPADDRESS, m_ctlServIP);
	DDX_Control(pDX, IDC_CLIENT_WORD_EDIT, m_ctlWord);
	DDX_Control(pDX, IDC_CLIENT_MEANING_EDIT, m_ctlMeaning);
	DDX_Text(pDX, IDC_CLIENT_PORT_EDIT, m_sServPort);
	DDV_MinMaxInt(pDX, m_sServPort, 5000, 6000);

}

BEGIN_MESSAGE_MAP(COverLapClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT_BUTTON, &COverLapClientDlg::OnConnectButton)
	ON_BN_CLICKED(IDC_LOOKUP_BUTTON, &COverLapClientDlg::OnLookupButton)
END_MESSAGE_MAP()


// COverLapClientDlg 消息处理程序

BOOL COverLapClientDlg::OnInitDialog()
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

	SetWindowText(TEXT("英汉电子词典---Client"));
	m_ctlServIP.SetAddress(127, 0, 0, 1);
	InitSocket();

	m_ctlWord.EnableWindow(FALSE);

	(CButton*)GetDlgItem(IDC_LOOKUP_BUTTON)->EnableWindow(FALSE);
	(CButton*)GetDlgItem(IDOK)->EnableWindow(FALSE);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void COverLapClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void COverLapClientDlg::OnPaint()
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
HCURSOR COverLapClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void COverLapClientDlg::OnConnectButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int nRet;
	UpdateData(TRUE);

	DWORD dwServIP;
	m_ctlServIP.GetAddress(dwServIP);

	if (0 == dwServIP)
	{
		AfxMessageBox(TEXT("请填写服务器地址"));
		return ;
	}

	if (0 == m_sServPort)
	{
		AfxMessageBox(TEXT("请填写服务器端口"));
		return ;
	}

	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(dwServIP);
	servAddr.sin_port = htons(m_sServPort);
	int nServLen = sizeof(servAddr);


	nRet = connect(m_sHost, (SOCKADDR*)&servAddr, nServLen);
	if (SOCKET_ERROR == nRet)
	{
		AfxMessageBox(TEXT("连接服务器失败"));
		return ;
	}

	m_bRunning = TRUE;

	m_arrEvent[m_nEventTotal] = WSACreateEvent();
	++m_nEventTotal;

	m_arrEvent[m_nEventTotal] = WSACreateEvent();
	++m_nEventTotal;
	m_pLookupSocket = new CLookupSocket(this, m_sHost, m_arrEvent[m_nEventTotal - 2], m_arrEvent[m_nEventTotal - 1]);

	DWORD dwThreadId;
	m_hWorkerThread = CreateThread(NULL, 0, WorkerThread, this, 0, &dwThreadId);


	m_ctlWord.EnableWindow(TRUE);
	(CButton*)GetDlgItem(IDC_LOOKUP_BUTTON)->EnableWindow(TRUE);
	(CButton*)GetDlgItem(IDOK)->EnableWindow(TRUE);

	m_ctlServIP.EnableWindow(FALSE);
	(CEdit*)GetDlgItem(IDC_CLIENT_PORT_EDIT)->EnableWindow(FALSE);
	(CButton*)GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(FALSE);
}


void COverLapClientDlg::OnLookupButton()
{
	// TODO: 在此添加控件通知处理程序代码
	if (NULL != m_pLookupSocket)
	{
		CString strWord;
		m_ctlWord.GetWindowTextLengthW(strWord);

		if (strWord.IsEmpty())
		{
			m_ctlMeaning.SetWindowTextW(TEXT("请输入单词"));
			return ;
		}

		CString strWordLookup;
		if (IsWord((char*)(LPCTSTR)strWord, strWordLookup))
		{
			if (FALSE == m_pLookupSocket->Lookup(strWordLookup))
			{
				AfxMessageBox(TEXT("查询单词失败"));
			}
			else
			{
				m_ctlMeaning.SetWindowTextW(TEXT("请输入单词, 不要包含汉子和其他符号"));
			}
		}
	}
}


BOOL COverLapClientDlg::InitSocket(void)
{
	int reVal;
	WSADATA wsaData;
	if ((reVal = WSAStartup(0x0202, &wsaData)) != 0)
	{
		printf("初始化套接字动态库失败 %d\n", WSAGetLastError());
		return FALSE;
	}


	if ((m_sHost = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		printf("创建套接字错误 %d\n", WSAGetLastError());
		WSACleanup();
		return FALSE;
	}

	return FALSE;
}
DWORD WINAPI COverLapClientDlg::WorkerThread(void* pParam)
{
	COverLapClientDlg* pClientDlg = (COverLapClientDlg*)pParam;
	while (pClientDlg->m_bRunning)
	{
		DWORD dwIndex;
		DWORD dwFlags;
		DWORD dwBytesTraned;
		LPWSAOVERLAPPED pIO = NULL;
		if ((dwIndex = WSAWaitForMultipleEvents(pClientDlg->m_nEventTotal, pClientDlg->m_arrEvent, FALSE, THREAD_SLEEP_TIME, TRUE)) == WSA_WAIT_FAILED)
		{
			printf("WSAWaitForMultipleEvents失败 %d\n", WSAGetLastError());
			return 0;
		}
		if (WSA_WAIT_TIMEOUT == dwIndex)
		{
			continue;
		}

		WSAResetEvent(pClientDlg->m_arrEvent[dwIndex - WSA_WAIT_EVENT_0]);

		pIO = pClientDlg->MapEventToOverlapped(dwIndex - WSA_WAIT_EVENT_0);
		ASSERT(NULL != pIO);


		BOOL bRet = WSAGetOverlappedResult(pClientDlg->m_sHost, pIO, &dwBytesTraned, TRUE, &dwFlags)
		if (bRet == FALSE || dwBytesTraned == 0)
		{
			return -1;
		}
		else
		{
			if (pIO == &pClientDlg->m_pLookupSocket->m_iIO)
			{
				ASSERT(1 == dwIndex);
				pClientDlg->m_pLookupSocket->HandleData();
			}
			else
			{
				;
			}
		}
	}
	return 0;
}


WSAOVERLAPPED* COverLapClientDlg::MapEventToOverlapped(int nIndex)
{
	if (0 == nIndex)
	{
		return &m_pLookupSocket->m_oIO;
	}
	else if (1 == nIndex)
	{
		return &m_pLookupSocket->m_iIO;
	}
	else
	{
		return NULL;
	}
}


void COverLapClientDlg::ShowMeaning( TCHAR *pMeaning )
{
	if (NULL == pMeaning )//字符串为空
	{
		return;
	}
	CString strMeaning(pMeaning);		//单词含义
	m_ctlMeaning.SetWindowText(_T(""));	//清空原来字符串

	//解析字符串
	CString meaning1;					//第一个含义
	CString meaning2;					//第二个含义
	int nIndex = strMeaning.Find(">");
	if (-1 == nIndex)
	{
		return;
	}
	meaning1 = strMeaning.Mid(1,nIndex -1);
	int nLen = strMeaning.GetLength();
	meaning2 = strMeaning.Mid(nIndex + 2, nLen - meaning1.GetLength() - 4);

	CString meaning;			//显示含义字符串
	if (!meaning2.IsEmpty())	//第二个含义不为空
	{
		meaning ="◇ " +
			meaning1 +
			"\r\n"	+		//换行
			"◇ " +
			meaning2;
	}else						//第二个含义为空
	{
		meaning ="◇ " +
			meaning1;
	}

	m_ctlMeaning.SetWindowText(meaning);//显示含义
}


BOOL COverLapClientDlg::IsWord( char * pExp, CString &strWord )
{
	int nCount = strlen(pExp);	//字符长度
	int nLen = 0;
	char *pTemp = pExp;
	while (*pTemp == ' ')		//剔除前面的空格
	{
		nLen++;
		pTemp++;
	}	
	char *pStartWord = pTemp;	//字符开始位置

	//结束循环的条件:1.遇到空格 2.字符串结束
	while (*pTemp != ' ' && nLen != nCount)
	{
		if (*pTemp < 65)
		{
			return FALSE;
		}else if ( *pTemp > 90 && *pTemp < 97)
		{
			return FALSE;
		}else if ( *pTemp > 122)
		{
			return FALSE;
		}else
		{
			pTemp++;
			nLen++;
		}		
	}
	int nNum = pTemp - pStartWord;		//有效字符数量
	CString strTemp(pStartWord, nNum);	//临时变量
	strWord = strTemp;
	return TRUE;
}


/*
 * 开始查询单词
 */
void COverLapClientDlg::OnOK() 
{
	OnLookupButton();
}

/*
 * 退出
 */
void COverLapClientDlg::OnCancel() 
{
	m_bRunning = FALSE;								//设置客户端运行状态
	WaitForSingleObject(m_hWorkerThread, INFINITE);	//等待工作线程退出
	delete m_pLookupSocket;							//删除CLookupSocket指针
	m_pLookupSocket = NULL;
	WSACleanup();									//释放套接字资源
	CDialog::OnCancel();							//销毁对话框
}
