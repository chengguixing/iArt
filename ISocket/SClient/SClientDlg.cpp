
// SClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SClient.h"
#include "SClientDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//下载文件线程函数参数
typedef	struct _FileDownloadParam  
{
	unsigned short	ID;			//标识
	hdr				header;		//包头
	SOCKET			s;			//套接字
	CSClientDlg*		ptr;		//主线程
	string			strFilePath;//下载文件路径
}FileDownloadParam, *pFileDownloadParam;


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


CSClientDlg::CSClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSClientDlg)
	m_strSaveFile = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_hThreadSR = NULL;
	//m_hThreadRecv = NULL;
	//memset(m_hThreadRecvF, 0, MAX_RECVFILE_THREAD);
	m_bConning = FALSE;
	m_nReqCur = REQNON;
	//memset(m_sFData, INVALID_SOCKET, MAX_RECVFILE_THREAD);

	//初始化套接字等
	m_ulFileSize = 0;

	m_ulCurTotal = 0;
}

CSClientDlg::~CSClientDlg()
{
	
}
void CSClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSClientDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_ctlProgress);
	DDX_Control(pDX, IDC_TREE, m_ctlTree);
	DDX_Control(pDX, IDC_IPADDRESS, m_ctlIP);
	DDX_Text(pDX, IDC_EDIT_FILEPATH, m_strSaveFile);
	DDX_Control(pDX, IDC_STATIC_TIP, m_ctlTip);
	DDX_Control(pDX, IDC_STATIC_PROGRESS, m_ctlFileDLTip);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSClientDlg, CDialog)
	//{{AFX_MSG_MAP(CSClientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_SETPATH, OnButtonSetFilePath)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE, OnItemexpandingTree)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnDblclkTree)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_MENUITEM_DOWNLOAD, OnFileDownload)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSClientDlg message handlers

BOOL CSClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	InitSocket();													//初始化Windows Sockets

	m_ctlIP.SetAddress(127, 0, 0, 1);								//设置服务器地址

	m_ctlTree.InsertItem(_T("我的电脑"),TVI_ROOT, TVI_LAST );		//插入根节点
	m_hTreeCur = m_ctlTree.GetRootItem();

	SetTimer(TIMER, 10, NULL);										//设置定时器 	



	return TRUE;  
}

void CSClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSClientDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/*
 *	初始化Windows Sockets
 */
void CSClientDlg::InitSocket(void)
{
	
	WORD	wVersionRequested;			//请求socket版本
	WSADATA	wsaData;					//wsaData结构
	int		nErrCode;					//返回值
	
	wVersionRequested = MAKEWORD( 2, 2 );//请求windows Sockets 2.2版本	
	nErrCode = WSAStartup( wVersionRequested, &wsaData );
	if ( 0 != nErrCode )
	{
		return;
	}
	
	//创建套接字
	m_sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sHost)
	{
		return;
	}
	
	//设置系统接收数据为默认的BUF_TIMES倍
	unsigned int uiRcvBuf;
	int uiRcvBufLen = sizeof(uiRcvBuf);
	nErrCode= getsockopt(m_sHost, SOL_SOCKET, SO_RCVBUF,(char*)&uiRcvBuf, &uiRcvBufLen);
	if (SOCKET_ERROR == nErrCode)
	{
		return;
	}
	uiRcvBuf *= BUF_TIMES;
	nErrCode = setsockopt(m_sHost, SOL_SOCKET, SO_RCVBUF,(char*)&uiRcvBuf, uiRcvBufLen);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox(_T("修改系统发送数据缓冲区失败！"));
	}

	//检查设置系统接收数据缓冲区是否成功
	unsigned int uiNewRcvBuf;
	getsockopt(m_sHost, SOL_SOCKET, SO_RCVBUF,(char*)&uiNewRcvBuf, &uiRcvBufLen);
	if (SOCKET_ERROR == nErrCode || uiNewRcvBuf != uiRcvBuf)
	{
		AfxMessageBox(_T("修改系统发送数据缓冲区失败！"));;
	}
}

/*
 *	释放套接字占用的资源
 */
void CSClientDlg::UnInitSocket(void)
{
	closesocket(m_sHost);
	WSACleanup();
	
}

/*
 *	连接服务器
 */
void CSClientDlg::OnButtonConnect() 
{
	//获得服务器的IP地址
	UpdateData(TRUE);
	DWORD dwServIP;
	m_ctlIP.GetAddress(dwServIP);

	//服务器套结字地址
	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(dwServIP);
	servAddr.sin_port = htons(SERVERPORT);

	//连接服务器
	int nErrCode;
	nErrCode = connect(m_sHost,(sockaddr*)&servAddr, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("连接服务器失败！",MB_OK, 0);	
		return;
	}
	
	//显示连接服务器成功信息
	m_ctlTip.SetWindowText(_T("连接服务器成功!"));
	m_bConning = TRUE;

	//请求服务器目录
	m_nReqCur = REQROOT;

	//创建发送和接收目录线程
	DWORD dwThread;
	m_hThreadSR = CreateThread(NULL, 0, SendAndRecvDirInforThread, this, 0, &dwThread);
 	CloseHandle(m_hThreadSR);

	//设置连接服务器按钮为无效状态
	CButton *pBt = (CButton*)this->GetDlgItem(IDC_BUTTON_CONNECT);
	pBt->EnableWindow(FALSE);
}

/*
 * 设置文件保存位置
 */
void CSClientDlg::OnButtonSetFilePath() 
{
	
	TCHAR tcSaveFilePath[MAX_PATH];	//临时字符数组
	BROWSEINFO browInfo = {0};		//清零
	browInfo.hwndOwner = GetSafeHwnd();
	browInfo.ulFlags = BIF_RETURNFSANCESTORS   ;
	browInfo.lpszTitle = "Setting file path";//标题
	LPITEMIDLIST browseFolder = SHBrowseForFolder(&browInfo);//打开浏览文件夹对话框
	if( browseFolder != NULL )
	{
		SHGetPathFromIDList(browseFolder, tcSaveFilePath);	//得到文件夹路径    
		SetCurrentDirectory(tcSaveFilePath);				//设置该目录为当前的工作目录
		m_strSaveFile.Empty();
		m_strSaveFile = tcSaveFilePath;						//保存文件

		m_strSaveFilePath = tcSaveFilePath;					//保存文件路径
		m_strSaveFilePath += "\\";
		UpdateData(FALSE);									//更新对话框
	}
}

/*
 * 发送请求
 */
BOOL CSClientDlg::SendReq(void)
{
	int reVal;			//返回值
	switch(m_nReqCur)	//请求类型
	{
	case REQROOT://根目录
		{
			//发送数据包，只有包头
			hdr header;
			memset(&header, 0, sizeof(header));
			header.type = ROOT;
			header.len = HEADLEN;
			
			reVal = send(m_sHost, (char*)&header, HEADLEN, 0);
			if (SOCKET_ERROR == reVal)
			{
				AfxMessageBox(_T("发送数据失败！"));
				return FALSE;
			}
			m_nReqCur = REQNON;
			break;
		}
	case REQDIRC://子目录
		{
			hdr header;
			memset(&header, 0, sizeof(header));
			header.type = DIRC;
			header.len = HEADLEN + m_strReqDir.size();

			//先发送包头
			reVal = send(m_sHost,(char*)&header, HEADLEN, 0);
			if (SOCKET_ERROR == reVal)
			{
				AfxMessageBox(_T("发送数据失败！"));
				return FALSE;
			}

			//再发送包体
			reVal = send(m_sHost,m_strReqDir.c_str(), m_strReqDir.size(), 0);
			if (SOCKET_ERROR == reVal)
			{
				AfxMessageBox(_T("发送数据失败！"));
				return FALSE;
			}
			m_nReqCur = REQNON;
			m_strReqDir.erase(m_strReqDir.begin(), m_strReqDir.end());//清空
			break;
		}
	case REQFSIZ:
		{
			hdr header;
			memset(&header, 0, sizeof(header));
			header.type = FSIZ;
			header.len = HEADLEN + m_strReqFile.size();
			
			//先发送包头
			reVal = send(m_sHost,(char*)&header, HEADLEN, 0);
			if (SOCKET_ERROR == reVal)
			{
				AfxMessageBox(_T("发送数据失败！"));
				return FALSE;
			}
			
			//再发送包体
			reVal = send(m_sHost,m_strReqFile.c_str(), m_strReqFile.size(), 0);
			if (SOCKET_ERROR == reVal)
			{
				AfxMessageBox(_T("发送数据失败！"));
				return FALSE;
			}
			m_nReqCur = REQNON;

			break;
		}
	case REQFDAT:
		{
			break;
		}
	default:
		break;
	}
	return TRUE;

}

/*
 *	发送请求接收目录信息线程
 */
DWORD CSClientDlg::SendAndRecvDirInforThread(void* pParam)
{
	CSClientDlg* pClient = (CSClientDlg*)pParam;
	SOCKET	sHost = pClient->GetHostSocket();//客户端套接字	
	
	FD_SET writefd;	//可写集合
	FD_SET readfd;	//可读集合

	while (pClient->IsConning())
	{
		FD_ZERO(&writefd);		//清零
		FD_ZERO(&readfd);		//清零
		FD_SET(sHost, &writefd);//添加到可写集合
		FD_SET(sHost, &readfd);	//添加到可读集合
		
		int reVal = 0;
		reVal = select(0, &readfd, &writefd, NULL, NULL);//等待套接字满足条件
		if (SOCKET_ERROR == reVal)
		{
			AfxMessageBox(_T("select错误"));
			return 0;
		}else if ( reVal > 0)
		{
			if (FD_ISSET(sHost, &writefd))			//满足可写的条件
			{
				if (FALSE == pClient->SendReq())	//发送数据
				{
					AfxMessageBox(_T("select错误"));
					return 0;
				}			
			}			
			
			if (FD_ISSET(sHost, &readfd))			//满足可读的条件
			{
				if(FALSE == pClient->RecvDirInfor())//接收数据
				{
					AfxMessageBox("接收目录信息失败！");
					return 0;
				}
			}			
		}

		Sleep(THREAD_SLEEP);//线程睡眠		
	}
	
	return 0;
}

/*
 * 接收服务器数据
 */
BOOL CSClientDlg::RecvDirInfor(void)
{
	BOOL	reVal = TRUE;	//返回值	
	int		nErrCode;		//错误值

	//读取包头
	hdr header;
	nErrCode = recv(m_sHost,(char*)&header, HEADLEN,0);		
	if (SOCKET_ERROR == nErrCode || 0 == nErrCode)//服务器关闭了
	{
		AfxMessageBox(_T("服务器关闭！"));
		reVal = FALSE;			
	}	
	
	//读取包体
	int nDataLen = header.len - HEADLEN;//包体的长度	
	switch(header.type)					//根据数据包的类型分类 再读取包体
	{
	case ROOT://根目录
	case DIRC://文件目录
		{
			if(FALSE == RecvDirString(nDataLen))
			{
				AfxMessageBox("接收目录信息失败！");
				reVal = FALSE;
			}			
			break;
		}
	case FSIZ://文件大小
		{
			RecvFileSize(header.flen);
			break;
		}	
	default:
		break;						
	}	
	return reVal;
}

/*
 * 接收目录包括根目录
 */
BOOL CSClientDlg::RecvDirString(int nDataLen)
{
	BOOL reVal = TRUE;		//返回值
	int	nReadLen = 0;		//接收数据长度
	int	nTotalLen = 0;		//已读数据长度
	
	list<string> listDir;	//二级目录链表
	listDir.clear();		//清空
	string strDir;			//二级目录

	while (m_bConning && nTotalLen != nDataLen )
	{		
		char cRecv;								//接收字符
		nReadLen = recv(m_sHost, &cRecv, 1,0);	//每次接收一个字符
		if (SOCKET_ERROR == nReadLen)
		{
			AfxMessageBox(_T("读取目录失败！"));
			reVal = FALSE;
		}else if (0 == nReadLen)
		{
			AfxMessageBox(_T("服务器关闭了连接！"));
			reVal = FALSE;
		}else if (nReadLen > 0)
		{
			if ('<' == cRecv)								//二级目录的开始分隔符，
			{				
				strDir.erase(strDir.begin(), strDir.end());	//清除数据
				
			}else if ('>' == cRecv)							//二级目录的结束分隔符
			{
				listDir.push_back(strDir);					//加入二级目录链表中
				
			}else
			{
				strDir += cRecv;							//添加字符
			}			
			nTotalLen += nReadLen;
		}		
	}
		

	PaintTree(m_hTreeCur,listDir);		//填充树视
	listDir.clear();					//清空链表
	m_ctlTree.EnableWindow(TRUE);		//使树视有效

	return reVal;	
}

/*
 * 接收文件的数据0:正常1:异常2:发送数据结束
 */
int CSClientDlg::RecvFileData(SOCKET s, unsigned short ID)
{
	int nErrCode;	//返回错误
	hdr header;		//包头
	
	//接收包头
	nErrCode = recv(s, (char*)&header, HEADLEN, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("接收文件失败！");
		return EXCEPT;
	}	
	if (FDAT != header.type)
	{
		AfxMessageBox("服务器发送文件错误！");
		return EXCEPT;
	}
	
	//发送文件结束
	if (0 == header.fpos && 0 == header.flen )
	{
		return FINISH;
	}


	unsigned long ulFileSeg = header.flen;	//文件长度
	unsigned long ulFilePos = header.fpos;	//文件位置(相对位置)
	
	m_ulCurTotal += ulFileSeg;				//下载文件总长度

	unsigned long nlTotal = 0;				//每次接收下载文件总长度
	unsigned long nRead = 0;				//接收长度
	char cBuf[BUF_SIZE];
	while (m_bConning && nlTotal != ulFileSeg)
	{
		memset(cBuf, 0, BUF_SIZE);
		nRead = recv(s, cBuf, ulFileSeg, 0);//准备接收1024*60个字节长度
		if (SOCKET_ERROR == nRead)
		{
			AfxMessageBox("接收文件失败！");
			return EXCEPT;
		}else if (0 == nRead)
		{
			int err = WSAGetLastError();
			return EXCEPT;
		}

		SaveFile(ID, cBuf, nRead, ulFilePos + nlTotal);//保存临时文件
		nlTotal += nRead;//加入接收字节数
	}
	return NORMAL;
}

/*
 * 保存临时文件
 */
 void CSClientDlg::SaveFile(u_short nIndexThread, const char* pBuf, u_long ulLen, u_long ulPos)
{
	FILE *stream = NULL;
	string strTemp = m_strSaveFilePath;	//保存文件路径
	switch(nIndexThread)
	{
	case 0:
		{
			strTemp += "F";//第一个临时文件名
			break;
		}
	case 1:
		{
			strTemp += "S";//第二个临时文件名
			break;
		}
	case 2:
		{
			strTemp += "T";//第三个临时文件名
			break;
		}
	default:
		break;
	}
	
	stream = fopen(strTemp.c_str(), "ab+");		//打开文件
	ASSERT(stream);
	
	int nErrCode = fseek(stream,ulPos,SEEK_SET);//移动文件指针
	ASSERT(nErrCode == 0);
	
	nErrCode = fwrite(pBuf, 1, ulLen, stream);	//写文件
	ASSERT(nErrCode == ulLen);

	fflush(stream);								//清除文件缓冲区
	fclose(stream);								//关闭文件
}

/*
 * 合并文件
 */
void CSClientDlg::AssemblyFile(void)
{
	//文件指针	
	FILE *streamIn = NULL;//写
	FILE *streamF = NULL; //读
	FILE *streamS = NULL; //读
	FILE *streamT = NULL; //读
	
	//得到文件名
	int nIndex = m_strReqFile.find_last_of('\\');//找到末尾的'\'	
	string strFileName = m_strReqFile.substr(nIndex + 1, m_strReqFile.size());	
	
	string strFile = m_strSaveFilePath;//文件
	strFile += strFileName;
	
	string strTempF = m_strSaveFilePath;//第一个临时文件
	strTempF += "F";
	string strTempS = m_strSaveFilePath;//第二个临时文件
	strTempS += "S";					
	string strTempT = m_strSaveFilePath;//第三个临时文件
	strTempT += "T";
	

	streamIn =  fopen(strFile.c_str(), "wb+");//打开文件
	ASSERT(streamIn);
	
	streamF =  fopen(strTempF.c_str(), "rb");//打开文件
	ASSERT(streamF);
	
	streamS =  fopen(strTempS.c_str(), "rb");//打开文件
	ASSERT(streamS);
	
	streamT =  fopen(strTempT.c_str(), "rb");//打开文件
	ASSERT(streamT);
	
	
	//合并第一个文件
	u_long	ulFirstFileLen = AssembleLine(streamIn, 0, streamF);
	//合并第二个文件
	u_long	ulSecondFileLen = AssembleLine(streamIn, ulFirstFileLen, streamS);
	//合并第三个文件
	u_long	ulThirdFileLen = AssembleLine(streamIn, ulFirstFileLen + ulSecondFileLen, streamT);
	
	//关闭文件
	fclose(streamIn);
	fclose(streamF);
	fclose(streamS);
	fclose(streamT);
	
	//删除临时文件	
	DeleteFile(strTempF.c_str());
	DeleteFile(strTempS.c_str());
	DeleteFile(strTempT.c_str());	
}

/*
 * 合并临时文件
 */
u_long  CSClientDlg::AssembleLine(FILE* destFile, int startPos, FILE* resoFile)
{
	//获取读文件的长度
	fseek( resoFile, 0, SEEK_END);
	unsigned long resoFileLen = ftell( resoFile );
	
	unsigned long  nOffsetLen = 0;	//已经读取字节数 
	unsigned long  nReadLen = 0;	//fread读取字节数

	char buf[BUF_SIZE];				//临时数组
	memset(buf, 0, BUF_SIZE);		//清空
	while (resoFileLen != nOffsetLen && !feof(resoFile))
	{
		fseek(resoFile,0 + nOffsetLen,SEEK_SET);		//移动读文件指针
		nReadLen = fread(buf,1,BUF_SIZE, resoFile);		//读入数据
		fseek(destFile,startPos + nOffsetLen,SEEK_SET);	//移动写文件指针
		fwrite(buf,1,nReadLen,destFile);				//写入数据
		nOffsetLen += nReadLen;							//增加已读数据
		memset(buf, 0, BUF_SIZE);						//清空
	}

	return nOffsetLen;									//返回实际文件长度
}
/*
 * 接收文件大小
 */
void CSClientDlg::RecvFileSize(unsigned long ulFileLen)
{
	m_ulFileSize = ulFileLen;	//接收文件大小

	PrepareFileDownload();		//准备创建文件下载线程

	m_nReqCur = REQNON;			//设置发送标志
}

/*
 * 准备创建文件下载线程
 */
void CSClientDlg::PrepareFileDownload(void)
{
	DWORD dwThreadID;
	m_hThreadFileDL = CreateThread(NULL, 0, CreateDownloadFileThread, this, 0, &dwThreadID);
	CloseHandle(m_hThreadFileDL);
}

/*
 * 创建套接字
 */
BOOL CSClientDlg::CreateSocket(SOCKET &s)
{
		//创建套接字
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sHost)
	{
		return FALSE;
	}

	//设置系统接收数据为默认的BUF_TIMES倍
	int nErrCode;
	unsigned int uiRcvBuf;
	int uiRcvBufLen = sizeof(uiRcvBuf);
	nErrCode= getsockopt(s, SOL_SOCKET, SO_RCVBUF,(char*)&uiRcvBuf, &uiRcvBufLen);
	if (SOCKET_ERROR == nErrCode)
	{
		return FALSE;
	}
	uiRcvBuf *= BUF_TIMES;
	nErrCode = setsockopt(s, SOL_SOCKET, SO_RCVBUF,(char*)&uiRcvBuf, uiRcvBufLen);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox(_T("修改系统发送数据缓冲区失败！"));
	}
	//检查设置系统发送数据缓冲区是否成功
	getsockopt(s, SOL_SOCKET, SO_RCVBUF,(char*)&uiRcvBuf, &uiRcvBufLen);	

	
	//创建套接字
//	UpdateData(TRUE);
	DWORD dwServIP;
	m_ctlIP.GetAddress(dwServIP);

    //服务器套结字地址
	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(dwServIP);
	servAddr.sin_port = htons(SERVERPORT);
	//连接服务器

	nErrCode = connect(s,(sockaddr*)&servAddr, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("连接服务器失败！",MB_OK, 0);	
		return FALSE;
	}
	
	//显示连接服务器成功信息
	m_ctlTip.SetWindowText(_T("开始下载文件!"));

	return TRUE;

}

/*
 * 创建文件下载线程
 */
DWORD CSClientDlg::CreateDownloadFileThread(void *pParam)
{
	CSClientDlg* pClient = (CSClientDlg*)pParam;
	HANDLE  hThreadFileDL[MAX_RECVFILE_THREAD];			//下载文件线程句柄数组
	SOCKET  sFileDL[MAX_RECVFILE_THREAD];				//下载文件套接字数组

	u_long ulFileSize = pClient->GetFileDLSize();		//文件长度
    string strFile = pClient->GetFileDL();				//下载文件

	u_long ulSegment = ulFileSize / MAX_RECVFILE_THREAD;//每个线程下载文件平均长度

	hdr header[MAX_RECVFILE_THREAD];					//定义包头
	int i;
	for (i = 0; i < MAX_RECVFILE_THREAD; ++i)
	{
		memset(&header[i], 0, sizeof(hdr));
		header[i].type = FDAT;							//请求类型
		header[i].len = HEADLEN + strFile.size();		//数据包长度
		header[i].fpos = i * ulSegment;					//下载文件开始位置
		if (i != MAX_RECVFILE_THREAD -1)				//下载文件长度
		{
			header[i].flen = ulSegment;
		}else
		{
			header[i].flen = ulFileSize - ulSegment * i;//第三线程下载剩下文件部分
		}
		
		SOCKET sFileData = INVALID_SOCKET;
		if (FALSE == pClient->CreateSocket(sFileData))	//创建套接字
		{
			AfxMessageBox("创建套接字失败！");
			return FALSE;
		}
		sFileDL[i] = sFileData;

		//定义线程函数传递参数
		pFileDownloadParam pThreadPara= new FileDownloadParam();
		pThreadPara->ID = i;
		pThreadPara->header = header[i];
		pThreadPara->s = sFileDL[i];
		pThreadPara->ptr = pClient;
		pThreadPara->strFilePath = strFile;

		//创建下载文件线程
		DWORD dwThread;		
		hThreadFileDL[i] = CreateThread(NULL, 0, DownloadFileThread, pThreadPara, 0, &dwThread);

	}

	//等待文件下载线程的退出
	WaitForMultipleObjects(MAX_RECVFILE_THREAD, hThreadFileDL, TRUE, INFINITE);	

	pClient->AssemblyFile();							//合并文件

	pClient->m_ctlTip.SetWindowText(_T("下载文件结束!"));//显示信息

	pClient->EnableSomeControl(TRUE);					//使控件有效
	
	pClient->ResetCurFileDL();							//隐藏进度条
	
	for (i = 0; i < MAX_RECVFILE_THREAD; ++i)			//清理资源
	{
		CloseHandle(hThreadFileDL[i]);
		closesocket(sFileDL[i]);
	}
	return 0;
}

/*
 *	下载文件线程
 */
DWORD CSClientDlg::DownloadFileThread(void* pParam)
{
	pFileDownloadParam pFileDownload = (pFileDownloadParam)pParam;
	unsigned short usID = pFileDownload->ID;		//线程序号
	SOCKET sFileDownload = pFileDownload->s;		//套接字
	hdr header = pFileDownload->header;				//包头
	string strFile = pFileDownload->strFilePath;	//下载文件
	CSClientDlg *pClient = pFileDownload->ptr;		//指针

	delete pFileDownload;//释放内存
	pFileDownload = NULL;

	int nErrCode;//返回值

	//发送包头
	nErrCode = send(sFileDownload, (char*)&header, HEADLEN, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("发送文件下载请求数据包失败！");
		return 0;
	}
	
	//发送包体
	nErrCode = send(sFileDownload, strFile.c_str(), strFile.size(), 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("发送文件下载请求数据包失败！");
		return 0;
	}

	FD_SET readfd;//可读集合
	//接收文件
	while (pClient->IsConning())
	{
		FD_ZERO(&readfd);								//清空集合
		FD_SET(sFileDownload, &readfd);					//加入集合
		nErrCode = select(0, &readfd, NULL, NULL, NULL);//等待
		if (SOCKET_ERROR == nErrCode)					//错误
		{
			AfxMessageBox("接收文件失败selcet");
			return 0;
		}

		if (nErrCode > 0)
		{
			if (FD_ISSET(sFileDownload, &readfd))						//可以读文件
			{
				int nErr = pClient->RecvFileData(sFileDownload, usID);	//接收文件
				if (EXCEPT == nErr)
				{
					AfxMessageBox("接收文件失败selcet");
					return 0;
				}else if (FINISH == nErr)								//下载结束
				{
					return 0;
				}
			}
		}
		Sleep(THREAD_SLEEP);
	}
	return 0;
}

/*
 * 显示目录
 */
void CSClientDlg::PaintTree(HTREEITEM hTreeItem, list<string> &listSubDir)
{
	
	list<string>::iterator itSubDir = listSubDir.begin();
	for (itSubDir; itSubDir != listSubDir.end(); ++itSubDir)
	{	
		//从二级目录链表中取出一个目录(包含了若干三级目录)
		string strSubDir = *itSubDir;		
		
		string strFind = "\\";					//二级目录与三级目录的分隔符
		int rootPos = strSubDir.find(strFind);	//找到分隔符位置
		if (rootPos != string::npos)
		{
			//二级目录名称
			string strSubDirName = strSubDir.substr(0, rootPos);
			//加入树视的二级节点
			HTREEITEM hChild = m_ctlTree.InsertItem(strSubDirName.c_str(), m_hTreeCur,TVI_LAST);		
			
			//移动二级目录之后的位置，就是三级目录开始的位置
			string::iterator itstr = strSubDir.begin();
			int nOffet = 0;
			while (nOffet < rootPos +1)
			{
				itstr++;
				nOffet++;
			}
			
			//将三级目录加入树视二级节点中
			string item;//临时变量，保存三级目录
			for (; itstr != strSubDir.end();++itstr)
			{			
				if (SEPERATOR == *itstr)//三级目录分隔符'|'
				{	
					//加入三级目录
					m_ctlTree.InsertItem(item.c_str(), hChild, TVI_LAST);
					item.erase(item.begin(),item.end());				
				}else
				{
					item += *itstr;
				}
			}
			
		}
		//nChild++;//增加二级目录的数量
	}
	
	m_ctlTree.Expand(m_hTreeCur,TVE_EXPAND);
}

/*
 * 得到文件路径
 */
void CSClientDlg::GetFilePath(HTREEITEM hTreeItem, string &strPath)
{
	string		strTemp;//临时变量
	HTREEITEM	hParent = hTreeItem;
	
	while (hParent )
	{		
		strTemp  = m_ctlTree.GetItemText( hParent );	//得到文本
		if (!strTemp.compare("我的电脑"))
		{
			break;
		}
		strTemp += "\\";
		string::size_type pos = 0;
		strPath.insert(pos, strTemp);					//在前面插入			;					
		hParent = m_ctlTree.GetParentItem( hParent );	//父目录
	}
}

/*
 * 浏览目录
 */
void CSClientDlg::OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hTreeItem = pNMTreeView->itemNew.hItem;		//选中的树项句柄

	if (m_hTreeCur == hTreeItem)							//展开项为当前项
	{
		return;												//折叠与展开之间切换
	}else
	{
		m_hTreeCur = hTreeItem;								//修改当前树项为展开项
		m_ctlTree.SelectItem(m_hTreeCur);					//设置该项为选中状态
	}

	//遍历第二级目录，判断是否存在三级目录
	HTREEITEM hChild = m_ctlTree.GetChildItem(hTreeItem);	//第一个第二级目录;
	BOOL bHasThirdDir = FALSE;								//初始值设置为没有三级目录
	while (hChild != NULL)
	{
		 bHasThirdDir = m_ctlTree.ItemHasChildren(hChild);	//三级节点
		if (TRUE == bHasThirdDir)							//存在三级节点
		{
			break;
		}		
 		hChild = m_ctlTree.GetNextSiblingItem(hChild);		//下一个二级目录
	}
	
	//存在第三级节点
	if (TRUE == bHasThirdDir)
	{
		return;//折叠与展开之间切换
	}


	//不存在第三级节点
	m_ctlTree.EnableWindow(FALSE);				//在请求目录期间禁止其他操作
	GetFilePath(hTreeItem, m_strReqDir);		//获取一级目录的路径
	m_nReqCur = REQDIRC;						//设置发送目录请求
	
	//删除第二级目录
	hChild = m_ctlTree.GetChildItem(hTreeItem);	//第一个二级节点					
	while (hChild != NULL)
	{
		HTREEITEM hTreeItemTemp	= m_ctlTree.GetNextSiblingItem(hChild);//下一个二级节点
		m_ctlTree.DeleteItem(hChild);
		hChild = hTreeItemTemp;
	}

	*pResult = 0;
}


/*
 * 显示菜单
 */
void CSClientDlg::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM hTreeItem = m_ctlTree.GetSelectedItem();	//获取该树项

	//判断是否为文件
	BOOL bHasChild = m_ctlTree.ItemHasChildren(hTreeItem);
	if (bHasChild)//目录，不下载
	{
		return;
	}	
							
	string strFilePath;
	GetFilePath(hTreeItem, strFilePath);						//得到文件的路径	
	m_strReqFile = strFilePath.substr(0,strFilePath.size()-1);	//删除字符串尾部的'\';
	
	//弹出菜单
	CPoint point;
	GetCursorPos(&point);							//获取光标位置
	CMenu menu;
	menu.LoadMenu(IDR_TREE_FILEDOWNLOAD);			//装载菜单资源	
	CMenu* pPopup = menu.GetSubMenu(0);				//获取子菜单

	//获取父窗口指针
	CWnd* pWndPopupOwner = this;
	while (pWndPopupOwner->GetStyle() & WS_CHILD)
	{
		pWndPopupOwner = pWndPopupOwner->GetParent();
	}	

	//显示菜单
	pPopup->TrackPopupMenu(TPM_CENTERALIGN |TPM_LEFTBUTTON, point.x, point.y, pWndPopupOwner);

	*pResult = 0;
}

/*
 * 设置控件状态
 */
void CSClientDlg::EnableSomeControl(BOOL bEnable)
{
	if (!bEnable)
	{
		CButton *pBt = (CButton*)this->GetDlgItem(IDC_BUTTON_SETPATH);
		pBt->EnableWindow(FALSE);
		CEdit *pEt = (CEdit*)this->GetDlgItem(IDC_EDIT_FILEPATH);
		pEt->EnableWindow(FALSE);		
	}else{
		CButton *pBt = (CButton*)this->GetDlgItem(IDC_BUTTON_SETPATH);
		pBt->EnableWindow(TRUE);
		CEdit *pEt = (CEdit*)this->GetDlgItem(IDC_EDIT_FILEPATH);
		pEt->EnableWindow(TRUE);		
	}
}

/*
 * 更新进度条
 */
void CSClientDlg::OnTimer(UINT nIDEvent) 
{
	if (m_ulCurTotal > 0)//当前下载文件的字节数
	{
		int nPercent = (float)m_ulCurTotal / m_ulFileSize * 100;//百分比
		m_ctlProgress.SetPos(nPercent);							//设置进度条位置
		
		m_ctlProgress.ShowWindow(SW_SHOW );						//显示进度条
		
		//显示下载进度信息
		CString strProgInfor;
		strProgInfor.Format("下载文件 %d%s", nPercent,"%");
		m_ctlFileDLTip.SetWindowText(strProgInfor);
	}
	
	CDialog::OnTimer(nIDEvent);
}

/*
 * 隐藏进度条控件
 */
void CSClientDlg::ResetCurFileDL(void)
{
	m_ulCurTotal = 0;
	m_ctlProgress.SetPos(0);
	m_ctlFileDLTip.SetWindowText("");
	m_ctlProgress.ShowWindow(SW_HIDE);
}

/*
 * 退出
 */
void CSClientDlg::OnClose() 
{
	m_bConning = FALSE;		//断开与服务器的连接
	Sleep(THREAD_SLEEP * 2);//给其它线程退出的机会
	UnInitSocket();			//释放套接字资源。
	CDialog::OnClose();
}

void CSClientDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	KillTimer(TIMER);	
}

/*
 * 响应菜单操作
 */
void CSClientDlg::OnFileDownload() 
{
	//判断是否设置了保存文件的路径
	if (0 == m_strSaveFilePath.size())
	{
		AfxMessageBox("请选择保存文件路径！");
		return;
	}
	
	m_nReqCur = REQFSIZ;//设置文件长度请求

	//得到文件名字
	int nIndex = m_strReqFile.find_last_of('\\');								//找到末尾的'\'	
	string strFileName = m_strReqFile.substr(nIndex + 1, m_strReqFile.size());	//文件名
	
	//更新界面保存文件的位置
	string strUpdateFile = m_strSaveFilePath ;	//文件路径
	strUpdateFile += strFileName;				//路径 + 文件名
	m_strSaveFile.Empty();						
	m_strSaveFile = strUpdateFile.c_str();
	UpdateData(FALSE);							//更新编辑框
	
	//禁止再设置保存文件的位置
	EnableSomeControl(FALSE);	
}
