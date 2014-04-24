
// ServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "SocketIPC\SockIPC_Interface.h"

static XAF_IPC_CONNECTION_HANDLE s_conn = NULL;


VOID InitIPC();


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CServerDlg �Ի���




CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDITSEND, m_Send);
	DDX_Control(pDX, IDC_EDITRECV, m_Recv);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTONSEND, &CServerDlg::OnSend)
	ON_BN_CLICKED(IDC_BUTTONRECV, &CServerDlg::OnRecv)
	ON_BN_CLICKED(IDC_BUTTONINIT, &CServerDlg::OnInit)
END_MESSAGE_MAP()


// CServerDlg ��Ϣ�������

BOOL CServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CServerDlg::OnSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	m_Send.GetWindowText(str);

	unsigned char ch[256];
	strcpy((char*)ch, (LPSTR)(LPCTSTR)str);

	XAF_IPCSend(s_conn, (byte*)ch, 255);

}

void CServerDlg::OnRecv()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TCHAR* ch = new TCHAR[255];
	memset(ch, 0, 255);
	XAF_IPCRecv(s_conn, (byte*)ch, 255);

	CString str;
	str.Format(_T("%S"),ch);  

	m_Recv.SetWindowText(str);
}


DWORD ON_CONNECT(XAF_IPC_CONNECTION_HANDLE pSock)
{
	printf("Enter %s\n", __FUNCTION__);

	return 0;
}

DWORD ON_SEND(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data)
{
	printf("Enter %s\n", __FUNCTION__);

	TCHAR* ch = new TCHAR[255];
	memset(ch, 0, 255);
	memcpy(ch, _T("i am server----------\n"), 255);
	XAF_IPCSend(sockIPC, (byte*)ch, 255);

	wprintf(L"%s\n", ch);
	return 0;
}

DWORD ON_RECV(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data)
{
	printf("Enter %s\n", __FUNCTION__);

	TCHAR* ch = new TCHAR[255];
	memset(ch, 0, 255);
	XAF_IPCRecv(sockIPC, (byte*)ch, 255);
	printf("%S\n", ch);

	return 0;
}

DWORD ON_ACCPET(XAF_IPC_LISTENER_HANDLE pSock, XAF_IPC_CONNECTION_HANDLE sockConn, IPCAddress* pAddr, void* data)
{
	printf("Enter %s\n", __FUNCTION__);
	s_conn = sockConn;
	return 0;
}

DWORD ON_CLOSE(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data, long resultCode)
{
	printf("Enter %s\n", __FUNCTION__);

	return 0;
}


VOID InitIPC()
{
	printf("start server\n");

	XAF_IPCStartup(0);

	XAF_IPC_LISTENER_HANDLE listener = XAF_IPCListener();

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

	XAF_IPC_CONNECTION_HANDLE conn = NULL;
}
void CServerDlg::OnInit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InitIPC();
}
