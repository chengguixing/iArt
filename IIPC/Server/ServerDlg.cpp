
// ServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "SocketIPC\SockIPC_Interface.h"
#include "tslog/tslog.h"



static XAF_IPC_CONNECTION_HANDLE s_conn = NULL;


VOID InitIPC();


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CServerDlg 对话框




CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDITSEND, m_Send);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTONSEND, &CServerDlg::OnSend)
	ON_BN_CLICKED(IDC_BUTTONINIT, &CServerDlg::OnInit)
END_MESSAGE_MAP()


// CServerDlg 消息处理程序

BOOL CServerDlg::OnInitDialog()
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CServerDlg::OnPaint()
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
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CServerDlg::OnSend()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	m_Send.GetWindowText(str);


	char text[255] = "Server OnSend";
	DWORD ret = XAF_IPCSend(s_conn, (byte*)text, 255);


}

DWORD ON_CONNECT(XAF_IPC_CONNECTION_HANDLE pSock)
{
	return 0;
}

DWORD ON_SEND(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data)
{
	TSDEBUG(L"Server ON_SEND %s", (byte*)data);
	return 0;
}

DWORD ON_RECV(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data)
{
	TSDEBUG(L"Server ON_RECV %s", (byte*)data);
	return 0;
}

DWORD ON_ACCPET(XAF_IPC_LISTENER_HANDLE pSock, XAF_IPC_CONNECTION_HANDLE sockConn, IPCAddress* pAddr, void* data)
{
	s_conn = sockConn;
	return 0;
}

DWORD ON_CLOSE(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data, long resultCode)
{
	return 0;
}


VOID InitIPC()
{
	XAF_IPCStartup(0);

	XAF_IPC_LISTENER_HANDLE listener = XAF_IPCListener();

	/*进程PID和端口号*/
	IPCAddress addr;
	addr.dwPid = GetCurrentProcessId();
	addr.dwPort = 8000;
	XAF_IPCBind(listener, addr);
	XAF_IPCAttachListenerEvent(listener, IPC_CONNECT, ON_CONNECT, NULL);
	XAF_IPCAttachListenerEvent(listener, IPC_ACCEPT, ON_ACCPET, NULL);
	XAF_IPCAttachListenerEvent(listener, IPC_WRITE, ON_SEND, NULL);
	XAF_IPCAttachListenerEvent(listener, IPC_READ, ON_RECV, NULL);
	XAF_IPCAttachListenerEvent(listener, IPC_CLOSE, ON_CLOSE, NULL);
	XAF_IPCListen(listener, 5);
}
void CServerDlg::OnInit()
{
	// TODO: 在此添加控件通知处理程序代码
	InitIPC();
}
