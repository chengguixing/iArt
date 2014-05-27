#include "stdafx.h"
#include "ClientView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientView

IMPLEMENT_DYNCREATE(CClientView, CView)

BEGIN_MESSAGE_MAP(CClientView, CView)
	//{{AFX_MSG_MAP(CClientView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_CLIENT_NEWGAME, OnClientNewgame)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_NEWGAME, OnUpdateClientNewgame)
	ON_COMMAND(ID_CLIENT_SURRENDER, OnClientSurrender)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_SURRENDER, OnUpdateClientSurrender)
	ON_COMMAND(ID_CLIENT_DISCONNECT, OnClientDisconnect)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_DISCONNECT, OnUpdateClientDisconnect)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_USER_CLIENTACCEPT, OnSocketAccept)
	ON_MESSAGE(WM_USER_CLIENTREAD, OnSocketRead)
	ON_MESSAGE(WM_USER_CLIENTCLOSE, OnSocketClose)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientView construction/destruction

CClientView::CClientView()
:m_nTotalEvent(0),
m_sListen(INVALID_SOCKET),
m_sServer(INVALID_SOCKET),
m_sClient(INVALID_SOCKET),
m_hEventExit(NULL),
m_bRuning(FALSE),
m_pServerSocket(NULL),
m_pListenSocket(NULL),
m_pClientSocket(NULL),
m_bLinked(FALSE),
m_bInGame(FALSE),
m_bOpponentWin(FALSE)
{
	// TODO: add construction code here

}

CClientView::~CClientView()
{
	
}

BOOL CClientView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CClientView drawing

void CClientView::OnDraw(CDC* pDC)
{
	CClientDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CClientView diagnostics

#ifdef _DEBUG
void CClientView::AssertValid() const
{
	CView::AssertValid();
}

void CClientView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CClientDoc* CClientView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CClientDoc)));
	return (CClientDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CClientView message handlers

/*
 * WM_PAINT��Ϣ��Ӧ����
 */
void CClientView::OnPaint() 
{
	CPaintDC dc(this);				//�豸����
	
	CBitmap bitmap;
	bitmap.LoadBitmap (IDB_BKWOOD);	//��������λͼ
	
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);	//�����ڴ��豸����
	//������λͼѡ���ڴ��豸����
	CBitmap* pOldBitmap = dcMem.SelectObject(&bitmap);

	//��ȡ�ͻ�����
	CRect rect(0, 0, 0, 0);
	GetClientRect(&rect);

	//������
	dc.StretchBlt(0, 0, rect.Width(), rect.Height(), 
					&dcMem, 0, 0, 640, 640, SRCCOPY);

	//����
	for(int i = 1; i <= DIVISIONS; i++)
	{
		dc.MoveTo(CELLSIZE, i * CELLSIZE);
		dc.LineTo(DIVISIONS * CELLSIZE, i * CELLSIZE);//ˮƽ��
		dc.MoveTo(i * CELLSIZE, CELLSIZE);
		dc.LineTo(i * CELLSIZE, DIVISIONS * CELLSIZE);//��ֱ��
	}
	
	//����Ԫ����λ
	dc.SelectStockObject(BLACK_BRUSH);//��ɫˢ��	
	int nRadius = 4;//�뾶
	dc.Ellipse(CELLSIZE + CELLSIZE * 3 - nRadius,
			   CELLSIZE + CELLSIZE * 3 - nRadius,
			   CELLSIZE + CELLSIZE * 3 + nRadius,
			   CELLSIZE + CELLSIZE * 3 + nRadius);

	dc.Ellipse(CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) - nRadius,
			   CELLSIZE + CELLSIZE * 3 - nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) + nRadius,
			   CELLSIZE + CELLSIZE * 3 + nRadius);
	
	dc.Ellipse(CELLSIZE + CELLSIZE * (DIVISIONS / 2) - nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS / 2) - nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS / 2) + nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS / 2) + nRadius);
	
	dc.Ellipse(CELLSIZE + CELLSIZE * 3 - nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) - nRadius,
			   CELLSIZE + CELLSIZE * 3 + nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) + nRadius);
	
	dc.Ellipse(CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) - nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) - nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) + nRadius,
			   CELLSIZE + CELLSIZE * (DIVISIONS - 3 - 1) + nRadius);

	//�������
	dc.SelectStockObject(NULL_BRUSH);	
	CPen pen(PS_SOLID, 4, RGB(0, 0, 0));//��ɫ����
	CPen* pOldPen = dc.SelectObject(&pen);
	CRect rectFrame(CELLSIZE, CELLSIZE, DIVISIONS * CELLSIZE + 2, DIVISIONS * CELLSIZE + 2);
	rectFrame.InflateRect(5, 5);
	dc.Rectangle(&rectFrame);

	dc.SelectObject(pOldPen);	//������ѡ���豸����
	dc.SelectObject(pOldBitmap);//��λͼѡ���豸����

	//������
	DrawPieces();

}

/*
 *������
 */
void CClientView::DrawPieces()
{
	CClientDC dc(this);					//�豸����

	CBitmap bmWhite, bmBlack, bmMask;
	bmWhite.LoadBitmap(IDB_WHITEPIECE);	//�������λͼ
	bmBlack.LoadBitmap(IDB_BLACKPIECE);	//�������λͼ
	bmMask.LoadBitmap(IDB_MASK);		//��������λͼ

	//���������ڴ��豸����
	CDC dcMemWhite;
	dcMemWhite.CreateCompatibleDC(&dc);
	//������λͼѡ���ڴ��豸����
	CBitmap* pOld1 = dcMemWhite.SelectObject(&bmWhite);

	//���������ڴ��豸����
	CDC dcMemBlack;
	dcMemBlack.CreateCompatibleDC(&dc);
	//������ѡ���ڴ��豸����
	CBitmap* pOld2 = dcMemBlack.SelectObject(&bmBlack);

	//���������ڴ��豸����
	CDC dcMemMask;
	dcMemMask.CreateCompatibleDC(&dc);
	//������λͼѡ���ڴ��豸����
	CBitmap* pOld3 = dcMemMask.SelectObject(&bmMask);

	//���뷽ʽ������ 
	dcMemWhite.BitBlt(0, 0, 36, 36, &dcMemMask, 0, 0, SRCAND);
	//���뷽ʽ������ 
	dcMemBlack.BitBlt(0, 0, 36, 36, &dcMemMask, 0, 0, SRCAND);

	//������
	for(int i = 0; i < DIVISIONS; i++)
		for(int j = 0; j < DIVISIONS; j++)
		{
			if(m_nCoords[i][j] == -1)
			{
				
				dc.BitBlt((i + 1) * CELLSIZE - 18, (j + 1) * CELLSIZE - 18, 
						36, 36, &dcMemMask, 0, 0, 0x220326);

				//�û�ʽ������ 
				dc.BitBlt((i + 1) * CELLSIZE - 18, (j + 1) * CELLSIZE - 18,
						  36, 36, &dcMemWhite, 0, 0, SRCPAINT);
			}
			
			if(m_nCoords[i][j] == 1)
			{
				dc.BitBlt((i + 1) * CELLSIZE - 18, (j + 1) * CELLSIZE - 18,
						  36, 36, &dcMemMask, 0, 0, 0x220326);
				//�û�ʽ������ 
				dc.BitBlt((i + 1) * CELLSIZE - 18, (j + 1) * CELLSIZE - 18,
						  36, 36, &dcMemBlack, 0, 0, SRCPAINT);
			}
			//����һ�����ӵ�λ��
			if(i == m_ptLastStep.x && 
			   j == m_ptLastStep.y &&
			   m_nCoords[i][j] != 0)
			{
				CPen pen(PS_SOLID, 1, RGB(192, 0, 0));
				CPen* pOldPen = dc.SelectObject(&pen);
				dc.SelectStockObject(NULL_BRUSH);

				dc.Rectangle((i + 1) * CELLSIZE - 18,(j + 1) * CELLSIZE - 18,
							 (i + 1) * CELLSIZE + 18,(j + 1) * CELLSIZE + 18);
				dc.SelectObject(pOldPen);
			}
		}
	//��λͼѡ���ڴ��豸����
	dcMemWhite.SelectObject(pOld1);
	dcMemBlack.SelectObject(pOld2);
	dcMemMask.SelectObject(pOld3);
}

int CClientView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


/*
 * ��ʼ��
 */
void CClientView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	//���ӷ�����
	if(FALSE == ConnectServer())
	{
		GetParent()->GetParent()->PostMessage(WM_NCDESTROY);
		return;
	}

	//��ʼ����
	if(FALSE == BeginListen())
	{
		GetParent()->GetParent()->PostMessage(WM_NCDESTROY);		
		return;
	}

	//���ÿͻ�������״̬
	m_bRuning = TRUE;

	//�����߳��˳��¼�����
	m_hEventExit = CreateEvent(NULL, TRUE, FALSE, NULL);//�ֶ����ã���ʼ��δ���š�״̬

	//�����̲߳���
	PTHREADPARAM pThreadParam = new THREADPARAM;
	pThreadParam->hClientHwnd = this->GetSafeHwnd();
	pThreadParam->pRunning = &m_bRuning;
	pThreadParam->pTotalEvent = &m_nTotalEvent;
	pThreadParam->pArrEvent = m_arrEvent;
	pThreadParam->pArrSocket = m_arrSocket;
	pThreadParam->hEventExit = m_hEventExit;
	//���������߳�
	DWORD dwThread;	
	HANDLE hServiceThread = CreateThread(NULL, 0, RunningThread, pThreadParam, 0, &dwThread);
	CloseHandle(hServiceThread);

	//��ȡ�ͻ��˵�ַ�Ͷ˿�
	SOCKADDR_IN clientAddr;
	int			clientLen = sizeof(clientAddr);
	getsockname(m_sServer, (SOCKADDR*)&clientAddr, &clientLen);	//��ȡIP
	m_UserInfo.m_lIP = clientAddr.sin_addr.S_un.S_addr;			
	getsockname(m_sListen, (SOCKADDR*)&clientAddr, &clientLen);	//��ȡ�����˿�
	m_UserInfo.m_uPort = clientAddr.sin_port;
	
	//�û���Ϣ
	m_UserInfo.m_strName = m_servDlg.m_strName;	//����
	m_UserInfo.m_state = CUserInfo::LOGIN;		//״̬

	SendUserInfo(m_UserInfo);					//������������û���Ϣ
}


/*
 * �ͻ����˳�
 */
void CClientView::OnDestroy() 
{
	CView::OnDestroy();
	//����ʼ��
	UnInit();
}

/*
 * ��ʼ����
 */
BOOL CClientView::BeginListen( void )
{
							
	int		reVal;//����ֵ
	m_sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//�����׽���
	if (INVALID_SOCKET == m_sListen)
	{
		WSACleanup();
		return FALSE;
	}

	//��
	SOCKADDR_IN clientAddr;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	clientAddr.sin_port = INADDR_ANY;
	reVal = bind(m_sListen, (SOCKADDR*)&clientAddr, sizeof(clientAddr));
	if (SOCKET_ERROR == reVal)
		return FALSE;
	
	//����
	reVal = listen(m_sListen, SOMAXCONN);
	if (SOCKET_ERROR == reVal)
		return FALSE;

	m_arrSocket[m_nTotalEvent]= m_sListen;			//�������׽��ּ��뼯��
	m_arrEvent[m_nTotalEvent] = WSACreateEvent();	//�����׽����¼�����
	//Ϊ�����׽���ע��FD_ACCEPT�� FD_CLOSE�����¼�
	if(SOCKET_ERROR == WSAEventSelect(m_sListen, m_arrEvent[m_nTotalEvent], 
		FD_ACCEPT | FD_CLOSE))
		return FALSE;
	
	m_pListenSocket = new CISocket(this, m_sListen, m_arrEvent[m_nTotalEvent]);
	m_nTotalEvent++;//�׽����¼������	

	return TRUE;
}
/*
 * ���ӷ�����
 */
BOOL CClientView::ConnectServer( void )
{
	int	reVal;					//����ֵ
	WORD	wVersionRequested;	//�����Windows Sockets ʵ�ְ汾
	WSADATA	wsaData;			//����Э�̽��

	wVersionRequested = MAKEWORD(2, 2);	
	reVal = WSAStartup( wVersionRequested, &wsaData );//��ʼ���׽��ֶ�̬��
	if ( 0 != reVal ) 
	{
		return FALSE;
	}

	//�����׽���
	m_sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//�����׽���
	if (INVALID_SOCKET == m_sServer)
	{
		WSACleanup();
		return FALSE;
	}
	
		//��¼�Ի���
	if (IDOK != m_servDlg.DoModal())
	{	
		closesocket(m_sServer);
		WSACleanup();
		return FALSE;
	}

	//�������׽��ֵ�ַ
	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(m_servDlg.m_lservIP);
	servAddr.sin_port = htons(m_servDlg.m_servPort);
	int servAddrLen = sizeof(servAddr);

	reVal = connect(m_sServer, (SOCKADDR*)&servAddr, servAddrLen);
	if (SOCKET_ERROR == reVal)
	{
		AfxMessageBox("���ӷ���ʧ��");
		closesocket(m_sServer);
		WSACleanup();
		return FALSE;
	}

	m_arrSocket[m_nTotalEvent]= m_sServer;			//�������׽��ּ��뼯��
	m_arrEvent[m_nTotalEvent] = WSACreateEvent();	//�����׽����¼�����
	//Ϊ�����׽���ע��FD_READ�� FD_CLOSE�����¼�
	if(SOCKET_ERROR == WSAEventSelect(m_sServer, m_arrEvent[m_nTotalEvent], 
		FD_READ | FD_CLOSE))
		return FALSE;

	m_pServerSocket = new CServerSocket(this, m_sServer, m_arrEvent[m_nTotalEvent]);
	m_nTotalEvent++;//�׽����¼������1
	return TRUE;
}

/*
 * �����߳�
 */
DWORD WINAPI CClientView::RunningThread( void *pParam )
{
	PTHREADPARAM pThreadParam = (PTHREADPARAM)pParam;
	while (*pThreadParam->pRunning)
	{
		
		DWORD	dwIndex = 0;				//����ֵ
		SOCKET	sAccept = INVALID_SOCKET;	//���������ͻ��������׽���
		
		//�ȴ������¼�
		if ((dwIndex = WSAWaitForMultipleEvents(*pThreadParam->pTotalEvent, 
											pThreadParam->pArrEvent, FALSE, 
											CLIENT_WAIT_TIMEOUT, FALSE)) 
											== WSA_WAIT_FAILED)
		{
			AfxMessageBox("�ȴ��¼�����");
			break;
		}		
		
		if (WSA_WAIT_TIMEOUT == dwIndex)//��ʱ
		{
			Sleep(CLIENT_WAIT_TIMEOUT);
			continue;
		}
		//�鿴���׽����Ϸ����������¼�
		WSANETWORKEVENTS networkEvents;//�����¼��ṹ
		if (WSAEnumNetworkEvents(pThreadParam->pArrSocket[dwIndex - WSA_WAIT_EVENT_0], 
								pThreadParam->pArrEvent[dwIndex - WSA_WAIT_EVENT_0], 
								&networkEvents) == SOCKET_ERROR)
		{
			AfxMessageBox("�����¼��ṹ����");
			break;
		}
		
		//FD_ACCEPT�����¼�
		if (networkEvents.lNetworkEvents & FD_ACCEPT)
		{
			if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)//�����������
			{
				AfxMessageBox("FD_ACCEPT_BIT �����¼�����");
				break;
			}
			//�������ڷ���WM_USER_CLIENTACCEPT��Ϣ
			::SendMessage(pThreadParam->hClientHwnd, 
						WM_USER_CLIENTACCEPT, dwIndex, NULL);
		}		
		
		//����FD_READ�����¼�	
		if (networkEvents.lNetworkEvents & FD_READ )
		{
			if (networkEvents.iErrorCode[FD_READ_BIT] != 0)//�����������
			{
				AfxMessageBox("FD_READ_BIT �����¼�����");
				break;
			}else
			{
				//�������ڷ���WM_USER_CLIENTREAD��Ϣ
				::SendMessage(pThreadParam->hClientHwnd, 
							WM_USER_CLIENTREAD, dwIndex, NULL);		
			}
		}
		
		//����FD_CLOSE�����¼�	
		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			//�������ڷ���WM_USER_CLIENTCLOSE��Ϣ
			::SendMessage(pThreadParam->hClientHwnd, 
						WM_USER_CLIENTCLOSE, dwIndex, NULL);	
		}		
	}

	//�����˳��¼�
	SetEvent(pThreadParam->hEventExit);	
	delete pThreadParam;
	pThreadParam = NULL;
	return 0;
}

/*
 * ����׽��ֺ��¼�����
 */
void CClientView::ClearSocketAndEventFromArr( const int nIndex )
{
	//�����׽��ֺ��¼���������������
	for (int i = nIndex - WSA_WAIT_EVENT_0; i < m_nTotalEvent; i++)
	{
		m_arrEvent[i] = m_arrEvent[i + 1];	
		m_arrSocket[i] = m_arrSocket[i + 1];
	}
	//������1		
	m_nTotalEvent--;	
}
/*
 * ����׽��ֺ��¼�����
 */
void CClientView::ClearSocketAndEventFromArr( SOCKET s )
{
	int nIndex;
	for (nIndex = 0; nIndex < m_nTotalEvent; ++nIndex)
	{
		if (m_arrSocket[nIndex] == s)
		{
			ClearSocketAndEventFromArr(nIndex);	//�����׽��ֺ��¼���������������
			break;
		}
	}
}

/*
 * ������������û���Ϣ
 */
void CClientView::SendUserInfo(const CUserInfo &userInfo)
{
	CString strUserInfo;//�û���Ϣ
	CString strUserName;//����
	strUserName= userInfo.m_strName;					//����
	CString	strUserIP;
	strUserIP.Format("%ld",userInfo.m_lIP);				//ip
	CString strUserPort;
	strUserPort.Format("%d",userInfo.m_uPort);			//�˿�
	CString StrUserState;
	StrUserState.Format("%d",(int)userInfo.m_state);	//״̬
	strUserInfo = "<" +
			strUserName + 
			"|"	+
			strUserIP + 
			"|"	+ 
			strUserPort + 
			"|"	+
			StrUserState+
			">";

	m_pServerSocket->Send(USERINFOR, strUserInfo);//�����û���Ϣ
}

/*
 * �����û��б�
 */
void CClientView::UpdateUserList( CString strUserList )
{
	//��ȡ���ָ��
	CMainFrame *pMF = (CMainFrame*)AfxGetApp()->GetMainWnd();
	//��ȡ�û��б���ָ��
	CUserView *pUserView = pMF->GetUserView();
	//�����û��б�
	pUserView->UpdateUserList(strUserList);
}

/*
 * FD_ACCEPT�����¼���Ӧ����
 */
afx_msg LRESULT CClientView::OnSocketAccept( WPARAM wParam, LPARAM lParam )
{
	SOCKET sAccept = INVALID_SOCKET;//�����׽���
	ASSERT(m_sListen == m_arrSocket[wParam - WSA_WAIT_EVENT_0]);	

	//������������
	if ((sAccept = accept(m_arrSocket[wParam - WSA_WAIT_EVENT_0],
							NULL, NULL)) == INVALID_SOCKET)
	{		
		AfxMessageBox("���������������");
		return -1;
	}
	
	if(!m_bLinked)//û���������û�����
	{	
		int result = AfxMessageBox(_T("���Ժ���������?"), MB_YESNO);		
		if(result == IDYES)
		{
			ASSERT(m_sClient == INVALID_SOCKET);			
			//���ͽ�����Ϣ
			CLIENTPACKET clientPacket;
			clientPacket.type = ACCEPT;
			clientPacket.xPos = 0;
			clientPacket.yPos = 0;
			int reVal = send(sAccept, (char*)&clientPacket, CLIENTPACKETLEN, 0);
			if (reVal == SOCKET_ERROR)//ʧ�ܴ���
			{
				
				AfxMessageBox(_T("���ӱ��Ͽ�\n���ܴ���Ϸ��"));
				m_bLinked = FALSE;							//δ����
				m_bInGame = FALSE;							//û����Ϸ
				closesocket(sAccept);						//�ر��׽���
				GetDocument()->SetTitle("������---׼��");	//����
				return -1;
			}		
			//�����׽����¼�����
			if ((m_arrEvent[m_nTotalEvent] = WSACreateEvent())
										== WSA_INVALID_EVENT)
			{
				AfxMessageBox("Ϊ�½��׽��ִ����׽����¼��������");
				return -1;
			}
			m_arrSocket[m_nTotalEvent] = sAccept;//�����׽�������
			m_sClient = sAccept;
			
			//ע��FD_READ FD_CLOSE�����¼�
			if (WSAEventSelect(sAccept, m_arrEvent[m_nTotalEvent],
								FD_READ|FD_CLOSE) == SOCKET_ERROR)
			{
				AfxMessageBox("Ϊ�½��׽���ע�������¼�����");
				return -1;
			}
			
			//����CClientSocket����
			m_pClientSocket = new CISocket(this, sAccept, m_arrEvent[m_nTotalEvent]);
			m_nTotalEvent++;//������1			
			
			//��������������Ϣ
			m_UserInfo.m_state = CUserInfo::PLAY;				
			SendUserInfo(m_UserInfo);		
			
			//�ı��Լ���״̬
			m_bLinked = TRUE;		//����״̬
			m_bMyTurn = TRUE;		//��ʼ����
			m_bOpponentWin = FALSE;	//�Է�δʤ
			m_bInGame = TRUE;		//��Ϸ��
			ResetCoords();			//��������
			m_nColor = 1;			//��������
			GetDocument()->SetTitle("������---��������");		
		}
		else
		{
			//���;ܾ���Ϣ
			CLIENTPACKET  clientPacket;
			clientPacket.type = DECLINE;
			clientPacket.xPos = 0;
			clientPacket.yPos = 0;
			send(sAccept,(char*)&clientPacket, CLIENTPACKETLEN, 0);
			GetDocument()->SetTitle("������---�ȴ�");	
			closesocket(sAccept);
			
		}
	}
	else// �Ѿ��������û�����
	{
		//���;ܾ���Ϣ
		CLIENTPACKET  clientPacket;
		clientPacket.type = DECLINE;
		clientPacket.xPos = 0;
		clientPacket.yPos = 0;
		send(sAccept,(char*)&clientPacket, CLIENTPACKETLEN, 0);
		closesocket(sAccept);
	}	
	return 0;
}



/*
 * FD_READ�����¼���Ӧ����
 */
afx_msg LRESULT CClientView::OnSocketRead( WPARAM wParam, LPARAM lParam )
{
	//�������׽���
	if (m_sServer == m_arrSocket[wParam - WSA_WAIT_EVENT_0])
	{
		if(!m_pServerSocket->Recv())//���շ���������
			return -1;
	}else//�ͻ����׽���
	{	
		ASSERT(m_sClient == m_arrSocket[wParam - WSA_WAIT_EVENT_0]);
		if(FALSE == m_pClientSocket->Recv())		//���տͻ�������
		{
			ClearSocketAndEventFromArr(m_sClient);	//����ͻ����׽���
			delete m_pClientSocket;					//ɾ��ָ��
			m_pClientSocket = NULL;
			m_sClient = INVALID_SOCKET;
			return -1;
		}
	}
	return 0;	
}

/*
 * FD_CLOSE�����¼���Ӧ����
 */
afx_msg LRESULT CClientView::OnSocketClose( WPARAM wParam, LPARAM lParam )
{
	//�ͻ��˷������ӱ��ܾ����߶Է��˳���Ϸ
	if (m_sClient == m_arrSocket[wParam - WSA_WAIT_EVENT_0])
	{
		ClearSocketAndEventFromArr(m_sClient);//����׽���
		delete m_pClientSocket;
		m_pClientSocket = NULL;	
		m_sClient = INVALID_SOCKET;

		m_UserInfo.m_state = CUserInfo::WAIT;//�ı�״̬
		SendUserInfo(m_UserInfo);

	}else if (m_sServer == m_arrSocket[wParam - WSA_WAIT_EVENT_0])//�������׽���
	{
		AfxMessageBox("�ͻ����˳�");
		CUserView *pUserView = ((CMainFrame*)AfxGetApp()->GetMainWnd())->GetUserView();
		pUserView->UnInit();
		PostMessage(WM_DESTROY);
	}else{}
	return 0;
}


/*
 * ��Է���������
 */
void CClientView::RequestPlay( const CUserInfo &userInfo )
{
	if (m_sClient != INVALID_SOCKET || TRUE == m_bLinked)
	{
		AfxMessageBox("���Ѿ��ںͱ��������ˣ�");
		return;
	}
	ASSERT(NULL == m_pClientSocket);	
	//�����׽���
	m_sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sClient)
		return ;	
	
	//�ͻ��˵�ַ
	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = userInfo.m_lIP;
	servAddr.sin_port =userInfo.m_uPort;
	int servAddrLen = sizeof(servAddr);
	
	//����
	int reVal = connect(m_sClient, (SOCKADDR*)&servAddr, servAddrLen);
	if (SOCKET_ERROR == reVal)
	{
		AfxMessageBox("���ӿͻ���ʧ��");
		closesocket(m_sClient);
		return ;
	}
	
	m_arrSocket[m_nTotalEvent]= m_sClient;			//���׽��ּ��뼯��
	m_arrEvent[m_nTotalEvent] = WSACreateEvent();	//�����׽����¼�����
	//Ϊ�����׽���ע��FD_READ�� FD_CLOSE�����¼�
	if(SOCKET_ERROR == WSAEventSelect(m_sClient, m_arrEvent[m_nTotalEvent],
						FD_READ | FD_CLOSE))
		return ;
	
	//����CRenjuSocket����
	m_pClientSocket = new CISocket(this, m_sClient, m_arrEvent[m_nTotalEvent]);
	m_nTotalEvent++;//�׽����¼������1
	
	//��m_sClient�ϵȴ���Ϣ,Ȼ���������״̬
	//�������������Ϣ����״̬
	return ;	
}


/*
 * ˢ������
 */
void CClientView::ResetCoords()
{
	//��������Ϊλ��ֵΪ0
	for(int i = 0; i < DIVISIONS; i++)
	{
		for(int j = 0; j < DIVISIONS; j++)
		{
			m_nCoords[i][j] = 0;
		}
	}
	//���½���
 	Invalidate(TRUE);
 	UpdateWindow();
}

/*
 * ����ͻ�������
 */
BOOL CClientView::HandleClientData( PCLIENTPACKET pClientPacket )
{	
	switch(pClientPacket->type)
	{
	case NEWGAME://�¿�������
		{
			InviteNewGame();
			break;
		}
	case NONEWGAME://�ܾ��¿���
		{
			GetDocument()->SetTitle(_T("������---��Ϸ����"));
			AfxMessageBox(_T("�Է����ڲ��������¿����ˡ�"));
			break;
		}
	case DISCONNECT://�˳���Ϸ
		{		
			m_bLinked = FALSE;
			m_bInGame = FALSE;
			AfxMessageBox(_T("�Է��˳���Ϸ"));
			ResetCoords();
			GetDocument()->SetTitle(_T("������---�ȴ�"));			
			break;
		}
	case ACCEPT://�Է�������������¿���
		{		
			m_bMyTurn = FALSE;
			m_bLinked = TRUE;
			m_bOpponentWin = FALSE;
			m_bInGame = TRUE;
			ResetCoords();
			m_nColor = -1;//����
			GetDocument()->SetTitle(_T("������---�Է�����"));

			//���ͷ�������Ϣ
			m_UserInfo.m_state = CUserInfo::PLAY;
			SendUserInfo( m_UserInfo);
			break;
		}
	case DECLINE://�Է��ܾ����� 
		{
			AfxMessageBox(_T("�Է��ܾ��������"));
			GetDocument()->SetTitle(_T("������---�ȴ�"));
			break;
		}
	case SURRENDER://�Է�Ͷ��
		{
			m_bMyTurn = FALSE;
			m_bInGame = FALSE;
			GetDocument()->SetTitle(_T("������---��Ϸ����"));
			AfxMessageBox(_T("�Է�Ͷ��\n��Ӯ��!"));				
			break;
		}
	default:AnalyseOpponent(pClientPacket->type, pClientPacket->xPos, pClientPacket->yPos);			
	}	
	return TRUE;
}

/*
 * ���¿��ֵ�����
 */
void CClientView::InviteNewGame(void)
{
	//��Ϣ��
	int result = AfxMessageBox(_T("��������һ����?"),	MB_YESNO);

	if(result == IDYES)//��������
	{
		//�������ݰ�
		CLIENTPACKET	clientPacket;
		clientPacket.type = ACCEPT;
		clientPacket.xPos = 0;
		clientPacket.yPos = 0;	
		//�������ݰ�
		if (FALSE == m_pClientSocket->Send(&clientPacket))
		{
			//ʧ�ܴ���
			AfxMessageBox(_T("��������\n ���ܿ���."));
			m_bLinked = FALSE;
			m_bInGame = FALSE;
			DestroyClientSocket();
			GetDocument()->SetTitle(_T("������---�ȴ���"));
			return;;
		}
		
		//������Ϸ״̬
		m_bMyTurn = TRUE;			//������
		m_bLinked = TRUE;			//����״̬
		m_bOpponentWin = FALSE;		//�Է�δ��ʤ
		m_bInGame = TRUE;			//��Ϸ��

		ResetCoords();				//ˢ������
		m_nColor = 1;				//����
		GetDocument()->SetTitle(_T("������---��������"));		
	}
	else//�ܾ��¿���,�����Ͽ�����
	{
		//�������ݰ�
		CLIENTPACKET	clientPacket;
		clientPacket.type = NONEWGAME;
		clientPacket.xPos = 0;
		clientPacket.yPos = 0;
		//�������ݰ�
		if (FALSE == m_pClientSocket->Send(&clientPacket))
		{
			//ʧ�ܴ���
			AfxMessageBox(_T("��������\n ���ܿ���."));
			m_bLinked = FALSE;
			m_bInGame = FALSE;
			DestroyClientSocket();
			GetDocument()->SetTitle(_T("������---�ȴ���"));
			return;
		}
		GetDocument()->SetTitle(_T("������---��Ϸ����"));		
	}
	return ;
}

/*
 * �����Է�
 */
void	CClientView::AnalyseOpponent(int state, int x, int y)
{
	DrawOneStep(x, y);//���Է�����
	
	switch(state)
	{
	case EVEN://ƽ��
		{
			m_bMyTurn = FALSE;
			m_bInGame = FALSE;
			GetDocument()->SetTitle(_T("������---��Ϸ����"));
			AfxMessageBox(_T("ƽ��!"));
			break;
		}
	case ULOSE://�Է���ʤ
		{
			m_bMyTurn = FALSE;
			m_bInGame = FALSE;
			GetDocument()->SetTitle(_T("������---��Ϸ����"));		
			AfxMessageBox(_T("�����ˡ�"));
			break;
		}
	case UWIN://�Է����
		{
			m_bMyTurn = FALSE;
			m_bInGame = FALSE;
			GetDocument()->SetTitle(_T("������---��Ϸ����"));
			AfxMessageBox(_T("��Ӯ�ˡ�"));
			break;
		}			
	case IWIN://�Է���ʱ��ʤ
		{
			m_bMyTurn = TRUE;
			m_bOpponentWin = TRUE;
			GetDocument()->SetTitle(_T("������---��������"));
			break;
		}
	
	case COORDS://����ʤ��
		{
			m_bMyTurn = TRUE;
			GetDocument()->SetTitle(_T("������---��������"));
			break;
		}
	default: break;
	}	
}

/*
 * ���Է�����
 */
void CClientView::DrawOneStep(int x, int y)
{
	CRect invalidOldRect((m_ptLastStep.x + 1) * CELLSIZE - CELLSIZE / 2,
						 (m_ptLastStep.y + 1) * CELLSIZE - CELLSIZE / 2,
						 (m_ptLastStep.x + 1) * CELLSIZE + CELLSIZE / 2,
						 (m_ptLastStep.y + 1) * CELLSIZE + CELLSIZE / 2);

	InvalidateRect(&invalidOldRect, FALSE);

	m_ptLastStep.x = x;
	m_ptLastStep.y = y;
	m_nCoords[x][y] = -m_nColor;
				
	PlaySound(_T("down.wav"), NULL, SND_FILENAME | SND_ASYNC);
	CRect invalidRect((x + 1) * CELLSIZE - CELLSIZE / 2,
					  (y + 1) * CELLSIZE - CELLSIZE / 2,
					  (x + 1) * CELLSIZE + CELLSIZE / 2,
					  (y + 1) * CELLSIZE + CELLSIZE / 2);
	InvalidateRect(&invalidRect, FALSE);
}

//�����������ɾ��ָ�룬ɾ������
void CClientView::DestroyClientSocket()
{
	ClearSocketAndEventFromArr(m_sClient);
	
	delete m_pClientSocket;
	m_pClientSocket = NULL;
	m_sClient = INVALID_SOCKET;
}

/*
 * ���������״
 */
BOOL CClientView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(nHitTest == HTCLIENT && m_bLinked && m_bInGame)
	{
		if((m_bMyTurn && (m_nColor == 1)) ||//�ұ���������--�ҵ����Ӻ�ɫ
			(!m_bMyTurn && (m_nColor == -1)))//������Է����壬�Է����Ӻ�ɫ
			::SetCursor(AfxGetApp()->LoadCursor(IDC_BLACK));
		else
			::SetCursor(AfxGetApp()->LoadCursor(IDC_WHITE));
		
		return TRUE;
	}


	return CView::OnSetCursor(pWnd, nHitTest, message);
}


/*
 * ��������Ӧ����
 */
void CClientView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_bLinked && m_bMyTurn)
	{
		int x = (point.x - CELLSIZE / 2) / CELLSIZE;			//ת������Ϊ��������
		int y = (point.y - CELLSIZE / 2) / CELLSIZE;
		if(x >= 0 && x < DIVISIONS && y >= 0 && y < DIVISIONS)	//�����̷�Χ��
		{
			if(m_nCoords[x][y] == 0)//��û������
			{	
				//�����һ�����ӱ��
				CRect invalidOldRect((m_ptLastStep.x + 1) * CELLSIZE - CELLSIZE / 2,
									 (m_ptLastStep.y + 1) * CELLSIZE - CELLSIZE / 2,
									 (m_ptLastStep.x + 1) * CELLSIZE + CELLSIZE / 2,
									 (m_ptLastStep.y + 1) * CELLSIZE + CELLSIZE / 2);

				InvalidateRect(&invalidOldRect, FALSE);
				//��������
				PlaySound(_T("down.wav"), NULL, SND_FILENAME | SND_ASYNC);
				m_ptLastStep.x = x;
				m_ptLastStep.y = y;
				m_nCoords[x][y] = m_nColor;
				m_bMyTurn = FALSE;
				
				//������ǰ���ӱ��
				CRect invalidRect((x + 1) * CELLSIZE - CELLSIZE / 2,
								  (y + 1) * CELLSIZE - CELLSIZE / 2,
								  (x + 1) * CELLSIZE + CELLSIZE / 2,
								  (y + 1) * CELLSIZE + CELLSIZE / 2);

				InvalidateRect(&invalidRect, FALSE);

				CLIENTPACKET clientPacket;
				clientPacket.xPos = x;
				clientPacket.yPos = y;
				if(IsWin(x, y))//�ж�ʤ�����
				{
					if(m_nColor == -1)//����,
					{
						if(m_bOpponentWin)//�Է�Ӧ����ʱ��ʤ
						{
							clientPacket.type = EVEN;//ƽ��
							m_bInGame = FALSE;
							GetDocument()->SetTitle(_T("������---��Ϸ����"));
							AfxMessageBox("��Ϸ����");
						}else//����ս��
						{
							clientPacket.type = ULOSE;// white win and game over
							m_bInGame = FALSE;
							GetDocument()->SetTitle(_T("������---��Ϸ����"));
							AfxMessageBox("ף�أ���Ӯ�ˣ�");
						}
					}
					else//������ʱ��ʤ
					{
						clientPacket.type = IWIN;
						GetDocument()->SetTitle(_T("������---�Է�����"));
					}
				}
				else//FALSE
				{
					if(m_bOpponentWin)//�Է���ʤ
					{
						clientPacket.type = UWIN;
						m_bInGame = FALSE;
						GetDocument()->SetTitle(_T("������---��Ϸ����"));
						AfxMessageBox("������!");
					}
					else//Ϊ��ʤ��
					{
						clientPacket.type = COORDS;					
						GetDocument()->SetTitle(_T("������---�Է�����"));
					}
				}

				//��Է���������
				BOOL bRet = m_pClientSocket->Send(&clientPacket);
				if (FALSE == bRet)
				{
					AfxMessageBox(_T("����Ͽ�\n������Ϸ"));
					m_bLinked = FALSE;
 					m_bInGame = FALSE;
 					DestroyClientSocket();
 					GetDocument()->SetTitle(_T("������---�ȴ�"));				
					//�������������Ϣ
					m_UserInfo.m_state = CUserInfo::WAIT;
					SendUserInfo(m_UserInfo);
 					return;
				}
			}
		}
		else
		{
			::MessageBeep(MB_ICONEXCLAMATION);//����λ�ô�������
		}
	}
	CView::OnLButtonDown(nFlags, point);
}
/*
 * �ж�ʤ��
 */
BOOL CClientView::IsWin(int x, int y)
{
	int xMin=max(0, x - 4);				//ˮƽλ����Сֵ
	int xMax=min(DIVISIONS - 1, x + 4);	//ˮƽλ�����ֵ
	int yMin=max(0, y - 4);				//��ֱλ����Сֵ
	int yMax=min(DIVISIONS - 1, y + 4);	//��ֱλ�����ֵ

	int i, j, p, q, sum1, sum2, sum3, sum4;

	//�ӵ�ǰ���������Զλ�ã�x-4����ʼѭ��������5��ѭ����
	//ÿѭ��һ�������ƶ�һ��λ�ã���
	for(j=xMin; j<=xMax-4; j++)
	{
		for(i=j, sum1=0; i<j+5; i++)//�������ˮƽλ���Ƿ������ͬ��ɫ����
			if(m_nCoords[i][y]==m_nCoords[x][y])
				sum1++;
			else
				break;

		if(sum1==5)
			return TRUE;
	}
	
	//�ӵ�ǰ����������Զλ�ã�y-4����ʼѭ��������5��ѭ����
	for(i=yMin; i<=yMax-4; i++)
	{
		for(j=i, sum2=0; j<i+5; j++)
			if(m_nCoords[x][j]==m_nCoords[x][y])
				sum2++;
			else
				break;
					

		if(sum2==5)
			return TRUE;
	}
	
	//���Ͻ�λ��
	for(i=x, j=y; i>xMin && j>yMin; i--, j--);

	//�����Ͻǵ����½�
	for(; i<=xMax-4 && j<=yMax-4; i++, j++)
	{
		for(sum3=0, p=i, q=j; p<i+5; p++, q++)
			if(m_nCoords[p][q]==m_nCoords[x][y])
				sum3++;
			else
				break;

		if(sum3==5)
			return TRUE;
	}

	//���½�λ��
	for(i=x, j=y; i>xMin && j<yMax; i--, j++);

	//�����½ǵ����Ͻ�
	for(; i<=xMax-4 && j>=yMin+4; i++, j--)
	{
		for(sum4=0, p=i, q=j; p<i+5; p++, q--)
			if(m_nCoords[p][q]==m_nCoords[x][y])
				sum4++;
			else
				break;

		if(sum4==5)
			return TRUE;
	}
	return FALSE;
}

/*
 * �����¿���
 */
void CClientView::OnClientNewgame() 
{
	CLIENTPACKET clientPacket;
	clientPacket.type = NEWGAME;
	clientPacket.xPos = 0;
	clientPacket.yPos = 0;
	BOOL bRet = m_pClientSocket->Send(&clientPacket);
	if (FALSE == bRet)
	{
		AfxMessageBox(_T("�����Ѿ��Ͽ�"));
		m_bLinked = FALSE;
		m_bInGame = FALSE;
		m_bMyTurn = FALSE;
		m_bOpponentWin = FALSE;
	
		ClearSocketAndEventFromArr(m_sClient);

		delete m_pClientSocket;
		m_pClientSocket = NULL;
		m_sClient = INVALID_SOCKET;
		GetDocument()->SetTitle(_T("������---�ȴ�"));

		m_UserInfo.m_state = CUserInfo::WAIT;
		SendUserInfo( m_UserInfo);
		return;
	}
	GetDocument()->SetTitle(_T("������---�Է��ȴ���"));
}

/*
 * ���²˵���
 */
void CClientView::OnUpdateClientNewgame(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bLinked && !m_bInGame);	
}

/*
 * ����
 */
void CClientView::OnClientSurrender() 
{
	//��Ϣ��
	int result = AfxMessageBox(_T("�������䣿"),MB_YESNO);
	//����
	if(result == IDYES)
	{
		//����״̬
		m_bMyTurn = FALSE;
		m_bInGame = FALSE;
		
		//�������ݰ�
		CLIENTPACKET clientPacket;
		clientPacket.type = SURRENDER;
		clientPacket.xPos = 0;
		clientPacket.yPos = 0;;
	
		//��������
		BOOL bRet = m_pClientSocket->Send(&clientPacket);
		if(FALSE == bRet)//������
		{
			AfxMessageBox(_T("�����Ѿ��Ͽ�"));
			//����״̬
			m_bLinked = FALSE;
			m_bInGame = FALSE;
			m_bMyTurn = FALSE;
			m_bOpponentWin = FALSE;	
			
			//ɾ���׽���
			ClearSocketAndEventFromArr(m_sClient);
		
			//ɾ������
			delete m_pClientSocket;
			m_pClientSocket = NULL;
			m_sClient = INVALID_SOCKET;
			GetDocument()->SetTitle(_T("������---�ȴ�"));
			
			//������Ϣ
			m_UserInfo.m_state = CUserInfo::WAIT;
			SendUserInfo( m_UserInfo);
			return;
		}
		GetParent()->SetWindowText(_T("������---��Ϸ����"));
	}
}

/*
 * ���²˵���
 */
void CClientView::OnUpdateClientSurrender(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bLinked && m_bInGame);		
}

/*
 * �˳���Ϸ
 */
afx_msg void CClientView::OnClientDisconnect()
{
	//��Ϣ��
	int reVal = AfxMessageBox(_T("�㲻��ͶԷ���������"), MB_YESNO);

	if(reVal == IDYES)
	{
		//����DISCONNECT���ݰ�
		CLIENTPACKET clientPacket;
		clientPacket.type = DISCONNECT;
		clientPacket.xPos = 0;
		clientPacket.yPos = 0;
		m_pClientSocket->Send(&clientPacket);

		//����׽���
		ASSERT(m_sClient != INVALID_SOCKET);
		ClearSocketAndEventFromArr(m_sClient);

		//ɾ��ָ��
		delete m_pClientSocket;
		m_pClientSocket = NULL;
		m_sClient = INVALID_SOCKET;

		//�޸�״̬
		m_bLinked = FALSE;
		m_bInGame = FALSE;	
		GetDocument()->SetTitle(_T("������---�ȴ�"));
		//ˢ������
		ResetCoords();
		//������Ϣ
		m_UserInfo.m_state = CUserInfo::WAIT;
		SendUserInfo(m_UserInfo);
	}
}

/*
 * ���²˵���
 */
afx_msg void CClientView::OnUpdateClientDisconnect( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(m_bLinked);
}

/*
 * ����ʼ��
 */
void CClientView::UnInit( void )
{
	m_bRuning = FALSE;//���ÿͻ�������״̬

	WaitForSingleObject(m_hEventExit, INFINITE);//�ȴ��¼�
	//���������Ϸ��֪ͨ�Է�
	if (m_bLinked && m_pClientSocket)
	{
		//����DISCONNECT���ݰ�
		CLIENTPACKET clientPacket;
		clientPacket.type = DISCONNECT;
		clientPacket.xPos = 0;
		clientPacket.yPos = 0;
		m_pClientSocket->Send(&clientPacket);
	
		//ɾ���ͻ����׽���
		ASSERT(m_sClient != INVALID_SOCKET);
		ClearSocketAndEventFromArr(m_sClient);
		delete m_pClientSocket;
		m_pClientSocket = NULL;
		m_sClient = INVALID_SOCKET;
	
	}	
	delete m_pServerSocket;	//�رշ������׽���
	m_pServerSocket = NULL;	
	delete m_pListenSocket;	//�رռ����׽���
	m_pListenSocket = NULL;
	WSACleanup();			//�ͷ��׽��ֶ�̬��	
}
