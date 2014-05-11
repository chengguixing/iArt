
// ClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "SocketIPC\SockIPC_Interface.h"
#include "tslog/tslog.h"



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


// CClientDlg �Ի���




CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDITSEND, m_Send);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTONSEND, &CClientDlg::OnSend)
	ON_BN_CLICKED(IDC_BUTTONINIT, &CClientDlg::OnInit)
END_MESSAGE_MAP()


// CClientDlg ��Ϣ�������

BOOL CClientDlg::OnInitDialog()
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

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CClientDlg::OnPaint()
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
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CClientDlg::OnSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TSDEBUG(L"Enter %s", __FUNCTIONW__);
	CString cstr;
	m_Send.GetWindowText(cstr);


	char text[255] = "Client Send";
	XAF_IPCSend(s_conn, (byte*)text, 255);

	
	TSDEBUG(L"Leave %s", __FUNCTIONW__);
}


DWORD ON_CONNECT(XAF_IPC_CONNECTION_HANDLE* pSock)
{
	TSDEBUG(L"Enter %s", __FUNCTIONW__);
	TSDEBUG(L"Client ON_CONNECT");
	TSDEBUG(L"Leave %s", __FUNCTIONW__);
	return 0;
}

DWORD ON_SEND(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data)
{
	TSDEBUG(L"Enter %s", __FUNCTIONW__);
	TSDEBUG(L"Client ON_SEND");
	TSDEBUG(L"Leave %s", __FUNCTIONW__);

	return 0;
}

DWORD ON_RECV(XAF_IPC_CONNECTION_HANDLE* pSock)
{
	TSDEBUG(L"Enter %s", __FUNCTIONW__);
	TSDEBUG(L"Client ON_RECV");
	TSDEBUG(L"Leave %s", __FUNCTIONW__);
	return 0;
}

DWORD ON_CLOSE(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data, long resultCode)
{
	TSDEBUG(L"Enter %s", __FUNCTIONW__);
	TSDEBUG(L"Client ON_CLOSE");
	TSDEBUG(L"Leave %s", __FUNCTIONW__);
	return 0;
}

VOID InitIPC()
{
	TSDEBUG(L"Enter %s", __FUNCTIONW__);

	XAF_IPCStartup(0);




	IPCAddress addr;
	addr.dwPid = 7268;
	addr.dwPort = 8000;

	XAF_IPC_CONNECTION_HANDLE conn = XAF_IPCConnection();
	TSDEBUG(L"Client XAF_IPCConnection");
	s_conn = conn;

	XAF_IPCAttachConnectionEvent(s_conn, IPC_CONNECT, ON_CONNECT, NULL);
	XAF_IPCAttachConnectionEvent(s_conn, IPC_WRITE, ON_SEND, NULL);
	XAF_IPCAttachConnectionEvent(s_conn, IPC_READ, ON_RECV, NULL);
	XAF_IPCAttachConnectionEvent(s_conn, IPC_CLOSE, ON_CLOSE, NULL);
	XAF_IPCConnect(conn, addr);

	TSDEBUG(L"Leave %s", __FUNCTIONW__);
}



void CClientDlg::OnInit()
{
	TSDEBUG(L"Enter %s", __FUNCTIONW__);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	InitIPC();
	TSDEBUG(L"Leave %s", __FUNCTIONW__);
}
