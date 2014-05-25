
// SServerDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
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

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSServerDlg �Ի���



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


// CSServerDlg ��Ϣ�������

BOOL CSServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
	m_ctlIP.SetAddress(127, 0, 0, 1);
	InitSocket();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSServerDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSServerDlg::OnButtonStartup()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

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
		AfxMessageBox(_T("��������ʧ��"), MB_OK, 0);
		closesocket(m_sServer);
		WSACleanup();
		return ;
	}
	else
	{
		m_ctlTip.SetWindowText(_T("�������󶨳ɹ�!"));
		UpdateData(false);
	}


	reVal = listen(m_sServer, SOMAXCONN);
	TSDEBUG("WSAGetLastError() = %d", WSAGetLastError());
	if (SOCKET_ERROR == reVal)
	{
		AfxMessageBox(_T("����������ʧ��!"), MB_OK, 0);
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
	m_ctlTip.SetWindowText(_T("�����������ɹ�!"));
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
		AfxMessageBox(_T("�޸�ϵͳ�������ݻ�����ʧ��!"));
	}

	unsigned int uiNewRcvBuf;
	getsockopt(m_sServer, SOL_SOCKET, SO_SNDBUF, (char*)&uiNewRcvBuf, &uiRcvBufLen);
	if (SOCKET_ERROR == nErrCode || uiNewRcvBuf != uiRcvBuf)
	{
		AfxMessageBox(_T("�޸�ϵͳ�������ݻ�����ʧ��!"));
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
	//����������
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
	m_bConning = FALSE;								//���÷�����״̬
	m_ctlTip.SetWindowText(_T("�������˳���"));		 //���½���
	Sleep(THREAD_SLEEP * 100);						//�����߳��˳�ʱ��
	DeleteCriticalSection(&m_cs);					//ɾ���ٽ�������
	UnInitSocket();									//�ͷ��׽�����Դ
	CDialog::OnClose();
}

void CSServerDlg::ShowClientNumberInfo(int nNum)
{
	CString strTip;
	strTip.Format(_T("Ŀǰ����%d�ͻ����ӡ�"), nNum -1);	
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