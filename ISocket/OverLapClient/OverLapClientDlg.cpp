
// OverLapClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "OverLapClient.h"
#include "OverLapClientDlg.h"
#include "afxdialogex.h"

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


// COverLapClientDlg �Ի���



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


// COverLapClientDlg ��Ϣ�������

BOOL COverLapClientDlg::OnInitDialog()
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

	SetWindowText(TEXT("Ӣ�����Ӵʵ�---Client"));
	m_ctlServIP.SetAddress(127, 0, 0, 1);
	InitSocket();

	m_ctlWord.EnableWindow(FALSE);

	(CButton*)GetDlgItem(IDC_LOOKUP_BUTTON)->EnableWindow(FALSE);
	(CButton*)GetDlgItem(IDOK)->EnableWindow(FALSE);


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void COverLapClientDlg::OnPaint()
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
HCURSOR COverLapClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void COverLapClientDlg::OnConnectButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nRet;
	UpdateData(TRUE);

	DWORD dwServIP;
	m_ctlServIP.GetAddress(dwServIP);

	if (0 == dwServIP)
	{
		AfxMessageBox(TEXT("����д��������ַ"));
		return ;
	}

	if (0 == m_sServPort)
	{
		AfxMessageBox(TEXT("����д�������˿�"));
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
		AfxMessageBox(TEXT("���ӷ�����ʧ��"));
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (NULL != m_pLookupSocket)
	{
		CString strWord;
		m_ctlWord.GetWindowTextLengthW(strWord);

		if (strWord.IsEmpty())
		{
			m_ctlMeaning.SetWindowTextW(TEXT("�����뵥��"));
			return ;
		}

		CString strWordLookup;
		if (IsWord((char*)(LPCTSTR)strWord, strWordLookup))
		{
			if (FALSE == m_pLookupSocket->Lookup(strWordLookup))
			{
				AfxMessageBox(TEXT("��ѯ����ʧ��"));
			}
			else
			{
				m_ctlMeaning.SetWindowTextW(TEXT("�����뵥��, ��Ҫ�������Ӻ���������"));
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
		printf("��ʼ���׽��ֶ�̬��ʧ�� %d\n", WSAGetLastError());
		return FALSE;
	}


	if ((m_sHost = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		printf("�����׽��ִ��� %d\n", WSAGetLastError());
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
			printf("WSAWaitForMultipleEventsʧ�� %d\n", WSAGetLastError());
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
	if (NULL == pMeaning )//�ַ���Ϊ��
	{
		return;
	}
	CString strMeaning(pMeaning);		//���ʺ���
	m_ctlMeaning.SetWindowText(_T(""));	//���ԭ���ַ���

	//�����ַ���
	CString meaning1;					//��һ������
	CString meaning2;					//�ڶ�������
	int nIndex = strMeaning.Find(">");
	if (-1 == nIndex)
	{
		return;
	}
	meaning1 = strMeaning.Mid(1,nIndex -1);
	int nLen = strMeaning.GetLength();
	meaning2 = strMeaning.Mid(nIndex + 2, nLen - meaning1.GetLength() - 4);

	CString meaning;			//��ʾ�����ַ���
	if (!meaning2.IsEmpty())	//�ڶ������岻Ϊ��
	{
		meaning ="�� " +
			meaning1 +
			"\r\n"	+		//����
			"�� " +
			meaning2;
	}else						//�ڶ�������Ϊ��
	{
		meaning ="�� " +
			meaning1;
	}

	m_ctlMeaning.SetWindowText(meaning);//��ʾ����
}


BOOL COverLapClientDlg::IsWord( char * pExp, CString &strWord )
{
	int nCount = strlen(pExp);	//�ַ�����
	int nLen = 0;
	char *pTemp = pExp;
	while (*pTemp == ' ')		//�޳�ǰ��Ŀո�
	{
		nLen++;
		pTemp++;
	}	
	char *pStartWord = pTemp;	//�ַ���ʼλ��

	//����ѭ��������:1.�����ո� 2.�ַ�������
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
	int nNum = pTemp - pStartWord;		//��Ч�ַ�����
	CString strTemp(pStartWord, nNum);	//��ʱ����
	strWord = strTemp;
	return TRUE;
}


/*
 * ��ʼ��ѯ����
 */
void COverLapClientDlg::OnOK() 
{
	OnLookupButton();
}

/*
 * �˳�
 */
void COverLapClientDlg::OnCancel() 
{
	m_bRunning = FALSE;								//���ÿͻ�������״̬
	WaitForSingleObject(m_hWorkerThread, INFINITE);	//�ȴ������߳��˳�
	delete m_pLookupSocket;							//ɾ��CLookupSocketָ��
	m_pLookupSocket = NULL;
	WSACleanup();									//�ͷ��׽�����Դ
	CDialog::OnCancel();							//���ٶԻ���
}
