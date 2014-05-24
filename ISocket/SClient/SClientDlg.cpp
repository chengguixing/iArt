
// SClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SClient.h"
#include "SClientDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//�����ļ��̺߳�������
typedef	struct _FileDownloadParam  
{
	unsigned short	ID;			//��ʶ
	hdr				header;		//��ͷ
	SOCKET			s;			//�׽���
	CSClientDlg*		ptr;		//���߳�
	string			strFilePath;//�����ļ�·��
}FileDownloadParam, *pFileDownloadParam;


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

	//��ʼ���׽��ֵ�
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

	InitSocket();													//��ʼ��Windows Sockets

	m_ctlIP.SetAddress(127, 0, 0, 1);								//���÷�������ַ

	m_ctlTree.InsertItem(_T("�ҵĵ���"),TVI_ROOT, TVI_LAST );		//������ڵ�
	m_hTreeCur = m_ctlTree.GetRootItem();

	SetTimer(TIMER, 10, NULL);										//���ö�ʱ�� 	



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
 *	��ʼ��Windows Sockets
 */
void CSClientDlg::InitSocket(void)
{
	
	WORD	wVersionRequested;			//����socket�汾
	WSADATA	wsaData;					//wsaData�ṹ
	int		nErrCode;					//����ֵ
	
	wVersionRequested = MAKEWORD( 2, 2 );//����windows Sockets 2.2�汾	
	nErrCode = WSAStartup( wVersionRequested, &wsaData );
	if ( 0 != nErrCode )
	{
		return;
	}
	
	//�����׽���
	m_sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sHost)
	{
		return;
	}
	
	//����ϵͳ��������ΪĬ�ϵ�BUF_TIMES��
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
		AfxMessageBox(_T("�޸�ϵͳ�������ݻ�����ʧ�ܣ�"));
	}

	//�������ϵͳ�������ݻ������Ƿ�ɹ�
	unsigned int uiNewRcvBuf;
	getsockopt(m_sHost, SOL_SOCKET, SO_RCVBUF,(char*)&uiNewRcvBuf, &uiRcvBufLen);
	if (SOCKET_ERROR == nErrCode || uiNewRcvBuf != uiRcvBuf)
	{
		AfxMessageBox(_T("�޸�ϵͳ�������ݻ�����ʧ�ܣ�"));;
	}
}

/*
 *	�ͷ��׽���ռ�õ���Դ
 */
void CSClientDlg::UnInitSocket(void)
{
	closesocket(m_sHost);
	WSACleanup();
	
}

/*
 *	���ӷ�����
 */
void CSClientDlg::OnButtonConnect() 
{
	//��÷�������IP��ַ
	UpdateData(TRUE);
	DWORD dwServIP;
	m_ctlIP.GetAddress(dwServIP);

	//�������׽��ֵ�ַ
	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(dwServIP);
	servAddr.sin_port = htons(SERVERPORT);

	//���ӷ�����
	int nErrCode;
	nErrCode = connect(m_sHost,(sockaddr*)&servAddr, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("���ӷ�����ʧ�ܣ�",MB_OK, 0);	
		return;
	}
	
	//��ʾ���ӷ������ɹ���Ϣ
	m_ctlTip.SetWindowText(_T("���ӷ������ɹ�!"));
	m_bConning = TRUE;

	//���������Ŀ¼
	m_nReqCur = REQROOT;

	//�������ͺͽ���Ŀ¼�߳�
	DWORD dwThread;
	m_hThreadSR = CreateThread(NULL, 0, SendAndRecvDirInforThread, this, 0, &dwThread);
 	CloseHandle(m_hThreadSR);

	//�������ӷ�������ťΪ��Ч״̬
	CButton *pBt = (CButton*)this->GetDlgItem(IDC_BUTTON_CONNECT);
	pBt->EnableWindow(FALSE);
}

/*
 * �����ļ�����λ��
 */
void CSClientDlg::OnButtonSetFilePath() 
{
	
	TCHAR tcSaveFilePath[MAX_PATH];	//��ʱ�ַ�����
	BROWSEINFO browInfo = {0};		//����
	browInfo.hwndOwner = GetSafeHwnd();
	browInfo.ulFlags = BIF_RETURNFSANCESTORS   ;
	browInfo.lpszTitle = "Setting file path";//����
	LPITEMIDLIST browseFolder = SHBrowseForFolder(&browInfo);//������ļ��жԻ���
	if( browseFolder != NULL )
	{
		SHGetPathFromIDList(browseFolder, tcSaveFilePath);	//�õ��ļ���·��    
		SetCurrentDirectory(tcSaveFilePath);				//���ø�Ŀ¼Ϊ��ǰ�Ĺ���Ŀ¼
		m_strSaveFile.Empty();
		m_strSaveFile = tcSaveFilePath;						//�����ļ�

		m_strSaveFilePath = tcSaveFilePath;					//�����ļ�·��
		m_strSaveFilePath += "\\";
		UpdateData(FALSE);									//���¶Ի���
	}
}

/*
 * ��������
 */
BOOL CSClientDlg::SendReq(void)
{
	int reVal;			//����ֵ
	switch(m_nReqCur)	//��������
	{
	case REQROOT://��Ŀ¼
		{
			//�������ݰ���ֻ�а�ͷ
			hdr header;
			memset(&header, 0, sizeof(header));
			header.type = ROOT;
			header.len = HEADLEN;
			
			reVal = send(m_sHost, (char*)&header, HEADLEN, 0);
			if (SOCKET_ERROR == reVal)
			{
				AfxMessageBox(_T("��������ʧ�ܣ�"));
				return FALSE;
			}
			m_nReqCur = REQNON;
			break;
		}
	case REQDIRC://��Ŀ¼
		{
			hdr header;
			memset(&header, 0, sizeof(header));
			header.type = DIRC;
			header.len = HEADLEN + m_strReqDir.size();

			//�ȷ��Ͱ�ͷ
			reVal = send(m_sHost,(char*)&header, HEADLEN, 0);
			if (SOCKET_ERROR == reVal)
			{
				AfxMessageBox(_T("��������ʧ�ܣ�"));
				return FALSE;
			}

			//�ٷ��Ͱ���
			reVal = send(m_sHost,m_strReqDir.c_str(), m_strReqDir.size(), 0);
			if (SOCKET_ERROR == reVal)
			{
				AfxMessageBox(_T("��������ʧ�ܣ�"));
				return FALSE;
			}
			m_nReqCur = REQNON;
			m_strReqDir.erase(m_strReqDir.begin(), m_strReqDir.end());//���
			break;
		}
	case REQFSIZ:
		{
			hdr header;
			memset(&header, 0, sizeof(header));
			header.type = FSIZ;
			header.len = HEADLEN + m_strReqFile.size();
			
			//�ȷ��Ͱ�ͷ
			reVal = send(m_sHost,(char*)&header, HEADLEN, 0);
			if (SOCKET_ERROR == reVal)
			{
				AfxMessageBox(_T("��������ʧ�ܣ�"));
				return FALSE;
			}
			
			//�ٷ��Ͱ���
			reVal = send(m_sHost,m_strReqFile.c_str(), m_strReqFile.size(), 0);
			if (SOCKET_ERROR == reVal)
			{
				AfxMessageBox(_T("��������ʧ�ܣ�"));
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
 *	�����������Ŀ¼��Ϣ�߳�
 */
DWORD CSClientDlg::SendAndRecvDirInforThread(void* pParam)
{
	CSClientDlg* pClient = (CSClientDlg*)pParam;
	SOCKET	sHost = pClient->GetHostSocket();//�ͻ����׽���	
	
	FD_SET writefd;	//��д����
	FD_SET readfd;	//�ɶ�����

	while (pClient->IsConning())
	{
		FD_ZERO(&writefd);		//����
		FD_ZERO(&readfd);		//����
		FD_SET(sHost, &writefd);//��ӵ���д����
		FD_SET(sHost, &readfd);	//��ӵ��ɶ�����
		
		int reVal = 0;
		reVal = select(0, &readfd, &writefd, NULL, NULL);//�ȴ��׽�����������
		if (SOCKET_ERROR == reVal)
		{
			AfxMessageBox(_T("select����"));
			return 0;
		}else if ( reVal > 0)
		{
			if (FD_ISSET(sHost, &writefd))			//�����д������
			{
				if (FALSE == pClient->SendReq())	//��������
				{
					AfxMessageBox(_T("select����"));
					return 0;
				}			
			}			
			
			if (FD_ISSET(sHost, &readfd))			//����ɶ�������
			{
				if(FALSE == pClient->RecvDirInfor())//��������
				{
					AfxMessageBox("����Ŀ¼��Ϣʧ�ܣ�");
					return 0;
				}
			}			
		}

		Sleep(THREAD_SLEEP);//�߳�˯��		
	}
	
	return 0;
}

/*
 * ���շ���������
 */
BOOL CSClientDlg::RecvDirInfor(void)
{
	BOOL	reVal = TRUE;	//����ֵ	
	int		nErrCode;		//����ֵ

	//��ȡ��ͷ
	hdr header;
	nErrCode = recv(m_sHost,(char*)&header, HEADLEN,0);		
	if (SOCKET_ERROR == nErrCode || 0 == nErrCode)//�������ر���
	{
		AfxMessageBox(_T("�������رգ�"));
		reVal = FALSE;			
	}	
	
	//��ȡ����
	int nDataLen = header.len - HEADLEN;//����ĳ���	
	switch(header.type)					//�������ݰ������ͷ��� �ٶ�ȡ����
	{
	case ROOT://��Ŀ¼
	case DIRC://�ļ�Ŀ¼
		{
			if(FALSE == RecvDirString(nDataLen))
			{
				AfxMessageBox("����Ŀ¼��Ϣʧ�ܣ�");
				reVal = FALSE;
			}			
			break;
		}
	case FSIZ://�ļ���С
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
 * ����Ŀ¼������Ŀ¼
 */
BOOL CSClientDlg::RecvDirString(int nDataLen)
{
	BOOL reVal = TRUE;		//����ֵ
	int	nReadLen = 0;		//�������ݳ���
	int	nTotalLen = 0;		//�Ѷ����ݳ���
	
	list<string> listDir;	//����Ŀ¼����
	listDir.clear();		//���
	string strDir;			//����Ŀ¼

	while (m_bConning && nTotalLen != nDataLen )
	{		
		char cRecv;								//�����ַ�
		nReadLen = recv(m_sHost, &cRecv, 1,0);	//ÿ�ν���һ���ַ�
		if (SOCKET_ERROR == nReadLen)
		{
			AfxMessageBox(_T("��ȡĿ¼ʧ�ܣ�"));
			reVal = FALSE;
		}else if (0 == nReadLen)
		{
			AfxMessageBox(_T("�������ر������ӣ�"));
			reVal = FALSE;
		}else if (nReadLen > 0)
		{
			if ('<' == cRecv)								//����Ŀ¼�Ŀ�ʼ�ָ�����
			{				
				strDir.erase(strDir.begin(), strDir.end());	//�������
				
			}else if ('>' == cRecv)							//����Ŀ¼�Ľ����ָ���
			{
				listDir.push_back(strDir);					//�������Ŀ¼������
				
			}else
			{
				strDir += cRecv;							//����ַ�
			}			
			nTotalLen += nReadLen;
		}		
	}
		

	PaintTree(m_hTreeCur,listDir);		//�������
	listDir.clear();					//�������
	m_ctlTree.EnableWindow(TRUE);		//ʹ������Ч

	return reVal;	
}

/*
 * �����ļ�������0:����1:�쳣2:�������ݽ���
 */
int CSClientDlg::RecvFileData(SOCKET s, unsigned short ID)
{
	int nErrCode;	//���ش���
	hdr header;		//��ͷ
	
	//���հ�ͷ
	nErrCode = recv(s, (char*)&header, HEADLEN, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("�����ļ�ʧ�ܣ�");
		return EXCEPT;
	}	
	if (FDAT != header.type)
	{
		AfxMessageBox("�����������ļ�����");
		return EXCEPT;
	}
	
	//�����ļ�����
	if (0 == header.fpos && 0 == header.flen )
	{
		return FINISH;
	}


	unsigned long ulFileSeg = header.flen;	//�ļ�����
	unsigned long ulFilePos = header.fpos;	//�ļ�λ��(���λ��)
	
	m_ulCurTotal += ulFileSeg;				//�����ļ��ܳ���

	unsigned long nlTotal = 0;				//ÿ�ν��������ļ��ܳ���
	unsigned long nRead = 0;				//���ճ���
	char cBuf[BUF_SIZE];
	while (m_bConning && nlTotal != ulFileSeg)
	{
		memset(cBuf, 0, BUF_SIZE);
		nRead = recv(s, cBuf, ulFileSeg, 0);//׼������1024*60���ֽڳ���
		if (SOCKET_ERROR == nRead)
		{
			AfxMessageBox("�����ļ�ʧ�ܣ�");
			return EXCEPT;
		}else if (0 == nRead)
		{
			int err = WSAGetLastError();
			return EXCEPT;
		}

		SaveFile(ID, cBuf, nRead, ulFilePos + nlTotal);//������ʱ�ļ�
		nlTotal += nRead;//��������ֽ���
	}
	return NORMAL;
}

/*
 * ������ʱ�ļ�
 */
 void CSClientDlg::SaveFile(u_short nIndexThread, const char* pBuf, u_long ulLen, u_long ulPos)
{
	FILE *stream = NULL;
	string strTemp = m_strSaveFilePath;	//�����ļ�·��
	switch(nIndexThread)
	{
	case 0:
		{
			strTemp += "F";//��һ����ʱ�ļ���
			break;
		}
	case 1:
		{
			strTemp += "S";//�ڶ�����ʱ�ļ���
			break;
		}
	case 2:
		{
			strTemp += "T";//��������ʱ�ļ���
			break;
		}
	default:
		break;
	}
	
	stream = fopen(strTemp.c_str(), "ab+");		//���ļ�
	ASSERT(stream);
	
	int nErrCode = fseek(stream,ulPos,SEEK_SET);//�ƶ��ļ�ָ��
	ASSERT(nErrCode == 0);
	
	nErrCode = fwrite(pBuf, 1, ulLen, stream);	//д�ļ�
	ASSERT(nErrCode == ulLen);

	fflush(stream);								//����ļ�������
	fclose(stream);								//�ر��ļ�
}

/*
 * �ϲ��ļ�
 */
void CSClientDlg::AssemblyFile(void)
{
	//�ļ�ָ��	
	FILE *streamIn = NULL;//д
	FILE *streamF = NULL; //��
	FILE *streamS = NULL; //��
	FILE *streamT = NULL; //��
	
	//�õ��ļ���
	int nIndex = m_strReqFile.find_last_of('\\');//�ҵ�ĩβ��'\'	
	string strFileName = m_strReqFile.substr(nIndex + 1, m_strReqFile.size());	
	
	string strFile = m_strSaveFilePath;//�ļ�
	strFile += strFileName;
	
	string strTempF = m_strSaveFilePath;//��һ����ʱ�ļ�
	strTempF += "F";
	string strTempS = m_strSaveFilePath;//�ڶ�����ʱ�ļ�
	strTempS += "S";					
	string strTempT = m_strSaveFilePath;//��������ʱ�ļ�
	strTempT += "T";
	

	streamIn =  fopen(strFile.c_str(), "wb+");//���ļ�
	ASSERT(streamIn);
	
	streamF =  fopen(strTempF.c_str(), "rb");//���ļ�
	ASSERT(streamF);
	
	streamS =  fopen(strTempS.c_str(), "rb");//���ļ�
	ASSERT(streamS);
	
	streamT =  fopen(strTempT.c_str(), "rb");//���ļ�
	ASSERT(streamT);
	
	
	//�ϲ���һ���ļ�
	u_long	ulFirstFileLen = AssembleLine(streamIn, 0, streamF);
	//�ϲ��ڶ����ļ�
	u_long	ulSecondFileLen = AssembleLine(streamIn, ulFirstFileLen, streamS);
	//�ϲ��������ļ�
	u_long	ulThirdFileLen = AssembleLine(streamIn, ulFirstFileLen + ulSecondFileLen, streamT);
	
	//�ر��ļ�
	fclose(streamIn);
	fclose(streamF);
	fclose(streamS);
	fclose(streamT);
	
	//ɾ����ʱ�ļ�	
	DeleteFile(strTempF.c_str());
	DeleteFile(strTempS.c_str());
	DeleteFile(strTempT.c_str());	
}

/*
 * �ϲ���ʱ�ļ�
 */
u_long  CSClientDlg::AssembleLine(FILE* destFile, int startPos, FILE* resoFile)
{
	//��ȡ���ļ��ĳ���
	fseek( resoFile, 0, SEEK_END);
	unsigned long resoFileLen = ftell( resoFile );
	
	unsigned long  nOffsetLen = 0;	//�Ѿ���ȡ�ֽ��� 
	unsigned long  nReadLen = 0;	//fread��ȡ�ֽ���

	char buf[BUF_SIZE];				//��ʱ����
	memset(buf, 0, BUF_SIZE);		//���
	while (resoFileLen != nOffsetLen && !feof(resoFile))
	{
		fseek(resoFile,0 + nOffsetLen,SEEK_SET);		//�ƶ����ļ�ָ��
		nReadLen = fread(buf,1,BUF_SIZE, resoFile);		//��������
		fseek(destFile,startPos + nOffsetLen,SEEK_SET);	//�ƶ�д�ļ�ָ��
		fwrite(buf,1,nReadLen,destFile);				//д������
		nOffsetLen += nReadLen;							//�����Ѷ�����
		memset(buf, 0, BUF_SIZE);						//���
	}

	return nOffsetLen;									//����ʵ���ļ�����
}
/*
 * �����ļ���С
 */
void CSClientDlg::RecvFileSize(unsigned long ulFileLen)
{
	m_ulFileSize = ulFileLen;	//�����ļ���С

	PrepareFileDownload();		//׼�������ļ������߳�

	m_nReqCur = REQNON;			//���÷��ͱ�־
}

/*
 * ׼�������ļ������߳�
 */
void CSClientDlg::PrepareFileDownload(void)
{
	DWORD dwThreadID;
	m_hThreadFileDL = CreateThread(NULL, 0, CreateDownloadFileThread, this, 0, &dwThreadID);
	CloseHandle(m_hThreadFileDL);
}

/*
 * �����׽���
 */
BOOL CSClientDlg::CreateSocket(SOCKET &s)
{
		//�����׽���
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sHost)
	{
		return FALSE;
	}

	//����ϵͳ��������ΪĬ�ϵ�BUF_TIMES��
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
		AfxMessageBox(_T("�޸�ϵͳ�������ݻ�����ʧ�ܣ�"));
	}
	//�������ϵͳ�������ݻ������Ƿ�ɹ�
	getsockopt(s, SOL_SOCKET, SO_RCVBUF,(char*)&uiRcvBuf, &uiRcvBufLen);	

	
	//�����׽���
//	UpdateData(TRUE);
	DWORD dwServIP;
	m_ctlIP.GetAddress(dwServIP);

    //�������׽��ֵ�ַ
	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(dwServIP);
	servAddr.sin_port = htons(SERVERPORT);
	//���ӷ�����

	nErrCode = connect(s,(sockaddr*)&servAddr, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("���ӷ�����ʧ�ܣ�",MB_OK, 0);	
		return FALSE;
	}
	
	//��ʾ���ӷ������ɹ���Ϣ
	m_ctlTip.SetWindowText(_T("��ʼ�����ļ�!"));

	return TRUE;

}

/*
 * �����ļ������߳�
 */
DWORD CSClientDlg::CreateDownloadFileThread(void *pParam)
{
	CSClientDlg* pClient = (CSClientDlg*)pParam;
	HANDLE  hThreadFileDL[MAX_RECVFILE_THREAD];			//�����ļ��߳̾������
	SOCKET  sFileDL[MAX_RECVFILE_THREAD];				//�����ļ��׽�������

	u_long ulFileSize = pClient->GetFileDLSize();		//�ļ�����
    string strFile = pClient->GetFileDL();				//�����ļ�

	u_long ulSegment = ulFileSize / MAX_RECVFILE_THREAD;//ÿ���߳������ļ�ƽ������

	hdr header[MAX_RECVFILE_THREAD];					//�����ͷ
	int i;
	for (i = 0; i < MAX_RECVFILE_THREAD; ++i)
	{
		memset(&header[i], 0, sizeof(hdr));
		header[i].type = FDAT;							//��������
		header[i].len = HEADLEN + strFile.size();		//���ݰ�����
		header[i].fpos = i * ulSegment;					//�����ļ���ʼλ��
		if (i != MAX_RECVFILE_THREAD -1)				//�����ļ�����
		{
			header[i].flen = ulSegment;
		}else
		{
			header[i].flen = ulFileSize - ulSegment * i;//�����߳�����ʣ���ļ�����
		}
		
		SOCKET sFileData = INVALID_SOCKET;
		if (FALSE == pClient->CreateSocket(sFileData))	//�����׽���
		{
			AfxMessageBox("�����׽���ʧ�ܣ�");
			return FALSE;
		}
		sFileDL[i] = sFileData;

		//�����̺߳������ݲ���
		pFileDownloadParam pThreadPara= new FileDownloadParam();
		pThreadPara->ID = i;
		pThreadPara->header = header[i];
		pThreadPara->s = sFileDL[i];
		pThreadPara->ptr = pClient;
		pThreadPara->strFilePath = strFile;

		//���������ļ��߳�
		DWORD dwThread;		
		hThreadFileDL[i] = CreateThread(NULL, 0, DownloadFileThread, pThreadPara, 0, &dwThread);

	}

	//�ȴ��ļ������̵߳��˳�
	WaitForMultipleObjects(MAX_RECVFILE_THREAD, hThreadFileDL, TRUE, INFINITE);	

	pClient->AssemblyFile();							//�ϲ��ļ�

	pClient->m_ctlTip.SetWindowText(_T("�����ļ�����!"));//��ʾ��Ϣ

	pClient->EnableSomeControl(TRUE);					//ʹ�ؼ���Ч
	
	pClient->ResetCurFileDL();							//���ؽ�����
	
	for (i = 0; i < MAX_RECVFILE_THREAD; ++i)			//������Դ
	{
		CloseHandle(hThreadFileDL[i]);
		closesocket(sFileDL[i]);
	}
	return 0;
}

/*
 *	�����ļ��߳�
 */
DWORD CSClientDlg::DownloadFileThread(void* pParam)
{
	pFileDownloadParam pFileDownload = (pFileDownloadParam)pParam;
	unsigned short usID = pFileDownload->ID;		//�߳����
	SOCKET sFileDownload = pFileDownload->s;		//�׽���
	hdr header = pFileDownload->header;				//��ͷ
	string strFile = pFileDownload->strFilePath;	//�����ļ�
	CSClientDlg *pClient = pFileDownload->ptr;		//ָ��

	delete pFileDownload;//�ͷ��ڴ�
	pFileDownload = NULL;

	int nErrCode;//����ֵ

	//���Ͱ�ͷ
	nErrCode = send(sFileDownload, (char*)&header, HEADLEN, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("�����ļ������������ݰ�ʧ�ܣ�");
		return 0;
	}
	
	//���Ͱ���
	nErrCode = send(sFileDownload, strFile.c_str(), strFile.size(), 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("�����ļ������������ݰ�ʧ�ܣ�");
		return 0;
	}

	FD_SET readfd;//�ɶ�����
	//�����ļ�
	while (pClient->IsConning())
	{
		FD_ZERO(&readfd);								//��ռ���
		FD_SET(sFileDownload, &readfd);					//���뼯��
		nErrCode = select(0, &readfd, NULL, NULL, NULL);//�ȴ�
		if (SOCKET_ERROR == nErrCode)					//����
		{
			AfxMessageBox("�����ļ�ʧ��selcet");
			return 0;
		}

		if (nErrCode > 0)
		{
			if (FD_ISSET(sFileDownload, &readfd))						//���Զ��ļ�
			{
				int nErr = pClient->RecvFileData(sFileDownload, usID);	//�����ļ�
				if (EXCEPT == nErr)
				{
					AfxMessageBox("�����ļ�ʧ��selcet");
					return 0;
				}else if (FINISH == nErr)								//���ؽ���
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
 * ��ʾĿ¼
 */
void CSClientDlg::PaintTree(HTREEITEM hTreeItem, list<string> &listSubDir)
{
	
	list<string>::iterator itSubDir = listSubDir.begin();
	for (itSubDir; itSubDir != listSubDir.end(); ++itSubDir)
	{	
		//�Ӷ���Ŀ¼������ȡ��һ��Ŀ¼(��������������Ŀ¼)
		string strSubDir = *itSubDir;		
		
		string strFind = "\\";					//����Ŀ¼������Ŀ¼�ķָ���
		int rootPos = strSubDir.find(strFind);	//�ҵ��ָ���λ��
		if (rootPos != string::npos)
		{
			//����Ŀ¼����
			string strSubDirName = strSubDir.substr(0, rootPos);
			//�������ӵĶ����ڵ�
			HTREEITEM hChild = m_ctlTree.InsertItem(strSubDirName.c_str(), m_hTreeCur,TVI_LAST);		
			
			//�ƶ�����Ŀ¼֮���λ�ã���������Ŀ¼��ʼ��λ��
			string::iterator itstr = strSubDir.begin();
			int nOffet = 0;
			while (nOffet < rootPos +1)
			{
				itstr++;
				nOffet++;
			}
			
			//������Ŀ¼�������Ӷ����ڵ���
			string item;//��ʱ��������������Ŀ¼
			for (; itstr != strSubDir.end();++itstr)
			{			
				if (SEPERATOR == *itstr)//����Ŀ¼�ָ���'|'
				{	
					//��������Ŀ¼
					m_ctlTree.InsertItem(item.c_str(), hChild, TVI_LAST);
					item.erase(item.begin(),item.end());				
				}else
				{
					item += *itstr;
				}
			}
			
		}
		//nChild++;//���Ӷ���Ŀ¼������
	}
	
	m_ctlTree.Expand(m_hTreeCur,TVE_EXPAND);
}

/*
 * �õ��ļ�·��
 */
void CSClientDlg::GetFilePath(HTREEITEM hTreeItem, string &strPath)
{
	string		strTemp;//��ʱ����
	HTREEITEM	hParent = hTreeItem;
	
	while (hParent )
	{		
		strTemp  = m_ctlTree.GetItemText( hParent );	//�õ��ı�
		if (!strTemp.compare("�ҵĵ���"))
		{
			break;
		}
		strTemp += "\\";
		string::size_type pos = 0;
		strPath.insert(pos, strTemp);					//��ǰ�����			;					
		hParent = m_ctlTree.GetParentItem( hParent );	//��Ŀ¼
	}
}

/*
 * ���Ŀ¼
 */
void CSClientDlg::OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hTreeItem = pNMTreeView->itemNew.hItem;		//ѡ�е�������

	if (m_hTreeCur == hTreeItem)							//չ����Ϊ��ǰ��
	{
		return;												//�۵���չ��֮���л�
	}else
	{
		m_hTreeCur = hTreeItem;								//�޸ĵ�ǰ����Ϊչ����
		m_ctlTree.SelectItem(m_hTreeCur);					//���ø���Ϊѡ��״̬
	}

	//�����ڶ���Ŀ¼���ж��Ƿ��������Ŀ¼
	HTREEITEM hChild = m_ctlTree.GetChildItem(hTreeItem);	//��һ���ڶ���Ŀ¼;
	BOOL bHasThirdDir = FALSE;								//��ʼֵ����Ϊû������Ŀ¼
	while (hChild != NULL)
	{
		 bHasThirdDir = m_ctlTree.ItemHasChildren(hChild);	//�����ڵ�
		if (TRUE == bHasThirdDir)							//���������ڵ�
		{
			break;
		}		
 		hChild = m_ctlTree.GetNextSiblingItem(hChild);		//��һ������Ŀ¼
	}
	
	//���ڵ������ڵ�
	if (TRUE == bHasThirdDir)
	{
		return;//�۵���չ��֮���л�
	}


	//�����ڵ������ڵ�
	m_ctlTree.EnableWindow(FALSE);				//������Ŀ¼�ڼ��ֹ��������
	GetFilePath(hTreeItem, m_strReqDir);		//��ȡһ��Ŀ¼��·��
	m_nReqCur = REQDIRC;						//���÷���Ŀ¼����
	
	//ɾ���ڶ���Ŀ¼
	hChild = m_ctlTree.GetChildItem(hTreeItem);	//��һ�������ڵ�					
	while (hChild != NULL)
	{
		HTREEITEM hTreeItemTemp	= m_ctlTree.GetNextSiblingItem(hChild);//��һ�������ڵ�
		m_ctlTree.DeleteItem(hChild);
		hChild = hTreeItemTemp;
	}

	*pResult = 0;
}


/*
 * ��ʾ�˵�
 */
void CSClientDlg::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM hTreeItem = m_ctlTree.GetSelectedItem();	//��ȡ������

	//�ж��Ƿ�Ϊ�ļ�
	BOOL bHasChild = m_ctlTree.ItemHasChildren(hTreeItem);
	if (bHasChild)//Ŀ¼��������
	{
		return;
	}	
							
	string strFilePath;
	GetFilePath(hTreeItem, strFilePath);						//�õ��ļ���·��	
	m_strReqFile = strFilePath.substr(0,strFilePath.size()-1);	//ɾ���ַ���β����'\';
	
	//�����˵�
	CPoint point;
	GetCursorPos(&point);							//��ȡ���λ��
	CMenu menu;
	menu.LoadMenu(IDR_TREE_FILEDOWNLOAD);			//װ�ز˵���Դ	
	CMenu* pPopup = menu.GetSubMenu(0);				//��ȡ�Ӳ˵�

	//��ȡ������ָ��
	CWnd* pWndPopupOwner = this;
	while (pWndPopupOwner->GetStyle() & WS_CHILD)
	{
		pWndPopupOwner = pWndPopupOwner->GetParent();
	}	

	//��ʾ�˵�
	pPopup->TrackPopupMenu(TPM_CENTERALIGN |TPM_LEFTBUTTON, point.x, point.y, pWndPopupOwner);

	*pResult = 0;
}

/*
 * ���ÿؼ�״̬
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
 * ���½�����
 */
void CSClientDlg::OnTimer(UINT nIDEvent) 
{
	if (m_ulCurTotal > 0)//��ǰ�����ļ����ֽ���
	{
		int nPercent = (float)m_ulCurTotal / m_ulFileSize * 100;//�ٷֱ�
		m_ctlProgress.SetPos(nPercent);							//���ý�����λ��
		
		m_ctlProgress.ShowWindow(SW_SHOW );						//��ʾ������
		
		//��ʾ���ؽ�����Ϣ
		CString strProgInfor;
		strProgInfor.Format("�����ļ� %d%s", nPercent,"%");
		m_ctlFileDLTip.SetWindowText(strProgInfor);
	}
	
	CDialog::OnTimer(nIDEvent);
}

/*
 * ���ؽ������ؼ�
 */
void CSClientDlg::ResetCurFileDL(void)
{
	m_ulCurTotal = 0;
	m_ctlProgress.SetPos(0);
	m_ctlFileDLTip.SetWindowText("");
	m_ctlProgress.ShowWindow(SW_HIDE);
}

/*
 * �˳�
 */
void CSClientDlg::OnClose() 
{
	m_bConning = FALSE;		//�Ͽ��������������
	Sleep(THREAD_SLEEP * 2);//�������߳��˳��Ļ���
	UnInitSocket();			//�ͷ��׽�����Դ��
	CDialog::OnClose();
}

void CSClientDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	KillTimer(TIMER);	
}

/*
 * ��Ӧ�˵�����
 */
void CSClientDlg::OnFileDownload() 
{
	//�ж��Ƿ������˱����ļ���·��
	if (0 == m_strSaveFilePath.size())
	{
		AfxMessageBox("��ѡ�񱣴��ļ�·����");
		return;
	}
	
	m_nReqCur = REQFSIZ;//�����ļ���������

	//�õ��ļ�����
	int nIndex = m_strReqFile.find_last_of('\\');								//�ҵ�ĩβ��'\'	
	string strFileName = m_strReqFile.substr(nIndex + 1, m_strReqFile.size());	//�ļ���
	
	//���½��汣���ļ���λ��
	string strUpdateFile = m_strSaveFilePath ;	//�ļ�·��
	strUpdateFile += strFileName;				//·�� + �ļ���
	m_strSaveFile.Empty();						
	m_strSaveFile = strUpdateFile.c_str();
	UpdateData(FALSE);							//���±༭��
	
	//��ֹ�����ñ����ļ���λ��
	EnableSomeControl(FALSE);	
}
