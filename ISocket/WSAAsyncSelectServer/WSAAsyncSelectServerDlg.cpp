
// WSAAsyncSelectServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WSAAsyncSelectServer.h"
#include "WSAAsyncSelectServerDlg.h"
#include "afxdialogex.h"
#include "ClientSocket.h"
#include "ListenSocket.h"
#include "ServerAddressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CWSAAsyncSelectServerDlg �Ի���



CWSAAsyncSelectServerDlg::CWSAAsyncSelectServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWSAAsyncSelectServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pListenSocket = NULL;
	m_pImageList = NULL;
}


CWSAAsyncSelectServerDlg::~CWSAAsyncSelectServerDlg()
{
	NULL;
}

void CWSAAsyncSelectServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CLIENT_LIST, m_ctlUserList);
}

BEGIN_MESSAGE_MAP(CWSAAsyncSelectServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_CLIENT_LIST, OnRclickClientList)
	ON_COMMAND(ID_SERVER_LIST_DELETE_MENUITEM, OnServerListDeleteMenuitem)
END_MESSAGE_MAP()


// CWSAAsyncSelectServerDlg ��Ϣ�������

BOOL CWSAAsyncSelectServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();



	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	Init();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CWSAAsyncSelectServerDlg::OnPaint()
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
HCURSOR CWSAAsyncSelectServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CWSAAsyncSelectServerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������

	SaveUserList(m_UserList);			//�����û���Ϣ

	SaveOfflineMsg(m_OfflineMsgList);	//����������Ϣ

	DeleteAllChatter();					//ɾ�������û�

	if (NULL != m_pListenSocket)		//ɾ��CListenSocket�����
	{
		delete m_pListenSocket;
		m_pListenSocket = NULL;
	}
	if (NULL != m_pImageList)			//ɾ��CImageList�����
	{
		delete m_pImageList;
		m_pImageList = NULL;
	}
}


void CWSAAsyncSelectServerDlg::ProcessPendingAccept(void)
{
	CClientSocket* pClient = new CClientSocket(this);
	SOCKADDR_IN clientAddr;
	int socketLen = sizeof(SOCKADDR_IN);

	m_pListenSocket->Accept(*pClient, (SOCKADDR*)&clientAddr, &socketLen);

	pClient->Init();
	pClient->SaveUserInfo(clientAddr);

	m_csChatterList.Lock();
	m_ChatterList.AddTail(pClient);
	m_csChatterList.Unlock();


}


BOOL CWSAAsyncSelectServerDlg::Init(void)
{
	InitListCtrlSetting();
	InitUserList();
	LoadOfflineMsg(m_OfflineMsgList);

	return TRUE;
}


BOOL CWSAAsyncSelectServerDlg::StartService(void)
{
	CServerAddressDlg servAddrDlg;
	if (IDOK != servAddrDlg.DoModal())
	{
		return FALSE;
	}


	in_addr servAdd;
	servAdd.S_un.S_addr = htonl(servAddrDlg.m_strServIP);
	CString strServIP = inet_ntoa(servAdd);

	m_pListenSocket = new CListenSocket(this);


	if (m_pListenSocket->Create(servAddrDlg.m_shServPort, SOCK_STREAM, strServIP))
	{
		if (!m_pListenSocket->Listen())
		{
			delete m_pListenSocket;
			m_pListenSocket = NULL;
			AfxMessageBox(_T("�����׽���ʧ��!"));
			return FALSE;
		}
	}
	else
	{
		delete m_pListenSocket;
		m_pListenSocket = NULL;
		AfxMessageBox(_T("�����׽���ʧ��!"));
		return FALSE;
	}
	return TRUE;
}

void CWSAAsyncSelectServerDlg::InitListCtrlSetting(void)
{
	m_pImageList = new CImageList();
	ASSERT(m_pImageList != NULL);

	m_pImageList->Create(32, 32, ILC_COLOR8 | ILC_MASK, 2, 2);
	CWinApp* pApp = AfxGetApp();
	HICON hIconOnline = pApp->LoadIconA(IDI_CHATTER_ONLINE);
	HICON hIconOffline = pApp->LoadIconA(IDI_CHATTER_OFFLINE);

	m_pImageList->Add(hIconOnline);
	m_pImageList->Add(hIconOffline);

	m_ctlUserList.SetImageList(m_pImageList, LVSIL_SMALL);

	m_ctlUserList.InsertColumn(0, _T("����"), LVCFMT_IMAGE | LVCFMT_LEFT);
	m_ctlUserList.InsertColumn(1, _T("����"), LVCFMT_CENTER);
	m_ctlUserList.InsertColumn(2, _T("��ַ"), LVCFMT_CENTER);
	m_ctlUserList.InsertColumn(3, _T("�˿�"), LVCFMT_CENTER);
	m_ctlUserList.InsertColumn(4, _T("ʱ��"), LVCFMT_CENTER);


	CRect rect;
	m_ctlUserList.GetClientRect(&rect);

	int nWidth = rect.Width();
	m_ctlUserList.SetColumnWidth(0, nWidth * 25 / 100);
	m_ctlUserList.SetColumnWidth(1, nWidth * 13 / 100);
	m_ctlUserList.SetColumnWidth(2, nWidth * 25 / 100);
	m_ctlUserList.SetColumnWidth(3, nWidth * 12 / 100);
	m_ctlUserList.SetColumnWidth(4, nWidth * 25 / 100);

}


void CWSAAsyncSelectServerDlg::InitUserList(void)
{
	LoadUserList(m_UserList);

	POSITION pos;
	int nLine = 0;
	for (pos = m_UserList.GetHeadPosition(); NULL != pos;)
	{
		CUserInfo* pUserInfo = (CUserInfo*)m_UserList.GetNext(pos);
		if (NULL != pUserInfo)
		{
			m_ctlUserList.InsertItem(nLine, pUserInfo->m_strName, 1);
			m_ctlUserList.SetItemText(nLine, 1, pUserInfo->m_strPassword);

			in_addr userAddr;
			userAddr.S_un.S_addr = (long)pUserInfo->m_lIP;
			CString strUserAddr(inet_ntoa(userAddr));
			m_ctlUserList.SetItemText(nLine, 2, strUserAddr);

			CString strUserPort;
			strUserPort.Format("%d", ntohs(pUserInfo->m_nPort));
			m_ctlUserList.SetItemText(nLine, 3, strUserPort);

			CString strUserTime = pUserInfo->m_time.Format("%c");
			m_ctlUserList.SetItemText(nLine, 4, strUserTime);
			
			
			++nLine;
		}
	}
}


void CWSAAsyncSelectServerDlg::SaveUserList(CObList& obList)
{
	char appPath[256];
	GetCurrentDirectory(256, appPath);
	CString filePath;
	filePath.Format("%s", appPath);
	filePath += "\\";
	filePath += USERLISTINI;

	DeleteFile(filePath);

	POSITION pos;
	int nIndex = 0;

	for (pos = obList.GetHeadPosition(); pos != NULL;)
	{
		CUserInfo* pUserInfo = (CUserInfo*)obList.GetNext(pos);
		if (NULL == pUserInfo)
		{
			break;
		}

		CString strSection("section");
		CString strIndex;
		strIndex.Format("%d", nIndex);
		strSection += strIndex;

		in_addr userAddr;
		userAddr.S_un.S_addr = (long)pUserInfo->m_lIP;
		CString strUserAddr(inet_ntoa(userAddr));

		CString strUserPort;
		strUserPort.Format("%d", pUserInfo->m_nPort);

		CString strUserTime;
		strUserTime.Format("%ld", (long)pUserInfo->m_time.GetTime());

		//д�û�����
		WritePrivateProfileString(strSection, _T("Name"), pUserInfo->m_strName, filePath);	
		//д�û�����
		WritePrivateProfileString(strSection, _T("Password"), pUserInfo->m_strPassword, filePath);	
		//д�û���ַ
		WritePrivateProfileString(strSection, _T("Address"), strUserAddr,filePath);	
		//д�û��˿�
		WritePrivateProfileString(strSection, _T("Port"), strUserPort,filePath);					
		//д��ʱ��
		WritePrivateProfileString(strSection, _T("Time"), strUserTime,filePath);					


		delete pUserInfo;//ɾ������
		pUserInfo = NULL;
		nIndex++;
	}
	obList.RemoveAll();
}


void CWSAAsyncSelectServerDlg::LoadUserList(CObList& obList)
{
#define  TEMP_BUF_SIZE 32//����������

	char appPath[256];
	GetCurrentDirectory(256, appPath);	//ȡ��Ӧ�ó���ǰ·��
	CString  filePath;					//����Ini�ļ���
	filePath.Format("%s", appPath);
	filePath += "\\";
	filePath += USERLISTINI;

	int nIndex = 0;
	while (TRUE)
	{
		CString strSection("section");
		CString strIndex;
		strIndex.Format("%d", nIndex);
		strSection += strIndex;

		//�û�����
		CString strSectionKey = "Name";					
		CString strValue = _T("");
		char cBuf[TEMP_BUF_SIZE];
		memset(cBuf, 0, TEMP_BUF_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}

		CUserInfo *pUserInfo = new CUserInfo();//�½�CUserInfo����
		pUserInfo->m_strName = cBuf;

		//��ȡ�û�����
		strSectionKey = "Password";
		memset(cBuf, 0, TEMP_BUF_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}
		pUserInfo->m_strPassword = cBuf;

		//��ȡ�û���ַ
		strSectionKey = "Address";
		memset(cBuf, 0, TEMP_BUF_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}
		u_long clinetAddr = inet_addr(cBuf);
		pUserInfo->m_lIP = (DWORD)clinetAddr;

		//��ȡ�û��˿�
		strSectionKey = "Port";
		memset(cBuf, 0, TEMP_BUF_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}
		pUserInfo->m_nPort = atoi(cBuf);

		//��ȡ�û�ʱ��
		strSectionKey = "Time";
		memset(cBuf, 0, TEMP_BUF_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}
		time_t time = atol(cBuf);
		pUserInfo->m_time = time;

		obList.AddTail(pUserInfo);		//��������
		++nIndex;
	}
}

void CWSAAsyncSelectServerDlg::SaveOfflineMsg(CObList& obList)
{
	char appPath[256];
	GetCurrentDirectory(256, appPath);	//ȡ��Ӧ�ó���ǰ·��
	CString  filePath;					//����Ini�ļ���
	filePath.Format("%s", appPath);
	filePath +="\\";
	filePath += OFFLINEMSGLIST;	

	//ɾ��ԭ�����ļ�
	DeleteFile(filePath);

	POSITION pos;
	int nIndex = 0;
	for(pos = obList.GetHeadPosition(); pos != NULL; )
	{
		CChatPacket *pPacket = (CChatPacket*)obList.GetNext(pos);//�û���Ϣ
		if (NULL == pPacket)
		{
			break;
		}

		CString strSection("section");							//section
		CString strIndex;
		strIndex.Format("%d", nIndex);
		strSection += strIndex;

		CString strTime;
		strTime.Format("%ld",(long)pPacket->m_time.GetTime());

		pPacket->m_strMsg.Replace("\r\n", "$");//ȡ���س���Ϊ$
		//д������Ϣ���û����� 
		WritePrivateProfileString(strSection, _T("SendMsgUser"), pPacket->m_UserInfo.m_strName, filePath);		//������Ϣ�û�
		//д������Ϣ���û�����
		WritePrivateProfileString(strSection, _T("RecvMsgUser"), pPacket->m_OfflineUserInfo.m_strName, filePath);	//������Ϣ�û�
		//д������Ϣ ��ʱ��
		WritePrivateProfileString(strSection, _T("Time"), strTime,filePath);									//������Ϣʱ��
		//д���͵���Ϣ
		WritePrivateProfileString(strSection, _T("Message"), pPacket->m_strMsg, filePath);						//���͵���Ϣ
		delete pPacket;
		pPacket = NULL;
		++nIndex;
	}
	obList.RemoveAll();

}

void CWSAAsyncSelectServerDlg::LoadOfflineMsg(CObList& obList)
{
	m_csOfflineList.Lock();	
	char appPath[256];
	GetCurrentDirectory(256,appPath);	//ȡ��Ӧ�ó���ǰ·��
	CString  filePath;					//����Ini�ļ���
	filePath.Format("%s",appPath);
	filePath += "\\";
	filePath += OFFLINEMSGLIST;

	int nIndex = 0;
	while (TRUE)
	{
		CString strSection("section");						//section
		CString strIndex;
		strIndex.Format("%d",nIndex);
		strSection += strIndex;

		//��ȡ������Ϣ���û�����
		CString strSectionKey = "SendMsgUser";
		CString strValue = _T("");
		char cBuf[MAX_MSG_SIZE];
		memset(cBuf, 0, MAX_MSG_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}

		CChatPacket *pPacket = new CChatPacket();	//�½�CChatPacket����
		pPacket->m_type = CChatPacket::MESSAGE;
		pPacket->m_UserInfo.m_strName = cBuf;		//������Ϣ���û���


		//��ȡ������Ϣ���û�����
		strSectionKey = "RecvMsgUser";
		memset(cBuf, 0, MAX_MSG_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}
		pPacket->m_OfflineUserInfo.m_strName = cBuf;


		//��ȡ������Ϣʱ��
		strSectionKey = "Time";
		memset(cBuf, 0, MAX_MSG_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}
		time_t time = atol(cBuf);
		pPacket->m_time = time;


		//��ȡ������Ϣ
		strSectionKey = "Message";
		memset(cBuf, 0, MAX_MSG_SIZE);
		if(0 == GetPrivateProfileString (strSection, strSectionKey, NULL, cBuf, TEMP_BUF_SIZE, filePath))
		{
			break;
		}

		////ȡ��$Ϊ�س���
		CString strMsg(cBuf);
		strMsg.Replace( "$", "\r\n");
		pPacket->m_strMsg = strMsg;


		obList.AddTail(pPacket);//��������
		++nIndex;
	}

	m_csOfflineList.Unlock();
}


BOOL CWSAAsyncSelectServerDlg::ProcessPendingRead(CArchive* pArchiveIn, CArchive* pArchiveOut, CClientSocket* pClientSocket)
{
	do 
	{		
		CChatPacket chatPact;//�������ݰ�
		chatPact.Serialize(*pArchiveIn);

		if (CChatPacket::MESSAGE == chatPact.m_type)//���������û�����Ϣ
		{
			m_csOfflineList.Lock();

			//���������û���Ϣ
			CChatPacket *pChatPacket = new CChatPacket();
			//������
			pChatPacket->m_type = CChatPacket::MESSAGE;	
			//������Ϣ�û�
			pChatPacket->m_UserInfo.m_strName = chatPact.m_UserInfo.m_strName;				
			//������Ϣ�û�
			pChatPacket->m_OfflineUserInfo.m_strName = chatPact.m_OfflineUserInfo.m_strName;
			//���͵���Ϣ
			pChatPacket->m_strMsg = chatPact.m_strMsg;	
			//����ʱ��
			pChatPacket->m_time = chatPact.m_time;											

			//����������Ϣ����
			m_OfflineMsgList.AddTail(pChatPacket);											

			m_csOfflineList.Unlock();			

		}else if (CChatPacket::USERLIST == chatPact.m_type)//�����û��б�--�û��յ�¼
		{
			//�����û���Ϣ
			pClientSocket->SaveUserInfo(chatPact.m_UserInfo);

			//��������Ϣ
			if (!UpdateUserList(chatPact.m_UserInfo, /*pArchiveOut,*/ pClientSocket))
				return FALSE;

			//�����е������û������û��б�
			SendUserList();

			//ת��������Ϣ
			TransmitMsg(chatPact, /*pArchiveOut*/pClientSocket);

			//���·���������
			UpdateServerListCtl(m_UserList);

		}

	} while(!pArchiveIn->IsBufferEmpty());

	return TRUE;
}


void CWSAAsyncSelectServerDlg::SendUserList(void)
{
	//�����û�����
	CObList tempChatterlist;
	CopyUserList(tempChatterlist);

	CChatPacket packet;
	packet.m_type = CChatPacket::USERLIST;	//��������
	packet.m_pUserList = &tempChatterlist;	//�û�����

	m_csChatterList.Lock();//����

	//�����������û������б�
	POSITION pos = NULL;
	for (pos = m_ChatterList.GetHeadPosition(); NULL!=pos;)
	{
		CClientSocket *pClientSocket = (CClientSocket*)m_ChatterList.GetNext(pos);
		pClientSocket->SendUserList(&packet);//�������ݰ�
	}
	m_csChatterList.Unlock();//����

	DeleteTempUserList(tempChatterlist);//ɾ����ʱ����
}
void CWSAAsyncSelectServerDlg::CopyUserList(CObList& obList)
{
	if (m_UserList.IsEmpty())
	{
		return ;
	}

	POSITION pos = NULL;
	CUserInfo *pUserInfo = NULL;
	CUserInfo *pTempUserInfo = NULL;
	for (pos = m_UserList.GetHeadPosition(); pos != NULL;)
	{
		pUserInfo = (CUserInfo*)m_UserList.GetNext(pos);

		//��Ҫ�����û�������
		pTempUserInfo = new CUserInfo();
		pTempUserInfo->m_strName = pUserInfo->m_strName;	//����
		pTempUserInfo->m_eStatus = pUserInfo->m_eStatus;	//״̬
		pTempUserInfo->m_lIP = pUserInfo->m_lIP;			//IP
		pTempUserInfo->m_nPort = pUserInfo->m_nPort;		//�˿�

		obList.AddTail(pTempUserInfo);
	}
}
void CWSAAsyncSelectServerDlg::DeleteTempUserList(CObList& obList)
{
	if (obList.IsEmpty())
	{
		return;
	}

	POSITION pos = NULL;
	CUserInfo *pUserInfo = NULL;

	for (pos = obList.GetHeadPosition(); NULL != pos;)
	{
		pUserInfo = (CUserInfo*)obList.GetNext(pos);
		delete pUserInfo;
		pUserInfo = NULL;
	}

	obList.RemoveAll();
}

BOOL CWSAAsyncSelectServerDlg::UpdateUserList(const CUserInfo& userInfo, CClientSocket* pClientSocket)
{
	BOOL retVal = TRUE;				//����ֵ
	m_csUserList.Lock();			//����

	POSITION	pos;				//λ�ñ���		
	CUserInfo	*pUserInfo = NULL;	//�û�����ָ��
	BOOL bFind = FALSE;				//�Ƿ��ҵ����û�

	//���û��Ƿ������������
	for (pos = m_UserList.GetHeadPosition(); NULL != pos;)
	{
		pUserInfo = (CUserInfo*)m_UserList.GetNext(pos);//�ڵ�����		

		if (pUserInfo->m_strName == userInfo.m_strName )//������ͬ
		{
			if (pUserInfo->m_strPassword == userInfo.m_strPassword)	//�ڲ��û�
			{
				if(CUserInfo::ONLINE == pUserInfo->m_eStatus)		//�û��Ѿ���¼
				{
					//������������ɾ���ýڵ�
					DeleteChatter(pClientSocket);

					//���͡��û��Ѿ���¼����Ϣ
					CChatPacket packet;
					packet.m_type = CChatPacket::SERVERMSG;		//���ݰ�����
					packet.m_strMsg = _T("    �û��Ѿ���¼!   ");//��Ϣ
					packet.m_time = CTime::GetCurrentTime();	//ʱ��

					pClientSocket->SendUserMsg(&packet);		//��������

					m_csUserList.Unlock();						//����	
					return FALSE;

				}else//�û���¼
				{
					pUserInfo->m_eStatus = CUserInfo::ONLINE;	//�޸��û�״̬
					pUserInfo->m_lIP = userInfo.m_lIP;			//IP��ַ
					pUserInfo->m_nPort = userInfo.m_nPort;		//�˿�
					bFind = TRUE;
					break;						
				}

			}else//�������
			{
				//������������ɾ���ýڵ�
				DeleteChatter(pClientSocket);

				//���͡����������Ϣ
				CChatPacket packet;
				packet.m_type = CChatPacket::SERVERMSG;						//���ݰ�����
				packet.m_strMsg = _T("    �������\t\n    �����µ�¼!    "); //��Ϣ	
				packet.m_time = CTime::GetCurrentTime();					//ʱ��

				pClientSocket->SendUserMsg(&packet);//��������

				m_csUserList.Unlock();//����
				return FALSE;				
			}			
		}
	}

	if (FALSE == bFind)//ע�����û�
	{
		CUserInfo *pUserInfo = new CUserInfo();
		pUserInfo->m_strName = userInfo.m_strName;			//����
		pUserInfo->m_strPassword = userInfo.m_strPassword;	//����
		pUserInfo->m_eStatus = CUserInfo::ONLINE;			//״̬
		pUserInfo->m_lIP = userInfo.m_lIP;					//��ַ
		pUserInfo->m_nPort = userInfo.m_nPort;				//�˿�
		pUserInfo->m_time = userInfo.m_time;				//ʱ��

		m_UserList.AddTail(pUserInfo);						//��������
	}

	m_csUserList.Unlock();//����
	return retVal;	
}


void CWSAAsyncSelectServerDlg::TransmitMsg(const CChatPacket& packet, CClientSocket* pClientSocket)
{
	m_csOfflineList.Lock();
	POSITION pos1 = NULL;		//λ�ñ���
	POSITION pos2 = NULL;		//λ�ñ���
	CChatPacket *pPacket = NULL;//���ݰ�

	//��������������Ϣ����
	for (pos1 = m_OfflineMsgList.GetHeadPosition(); (pos2 = pos1) != NULL;)
	{
		//ȡ�����ݰ�
		pPacket = (CChatPacket*)m_OfflineMsgList.GetNext(pos1);

		//��¼�û������ֺͽ���������Ϣ���û�������ͬ
		if (pPacket->m_OfflineUserInfo.m_strName == packet.m_UserInfo.m_strName)
		{
			pClientSocket->SendUserMsg(pPacket);//����������Ϣ
			m_OfflineMsgList.RemoveAt(pos2);	//ɾ������ڵ�
			delete pPacket;						//ɾ������
			pPacket = NULL;					
		}
	}
	m_csOfflineList.Unlock();
}

void CWSAAsyncSelectServerDlg::UpdateServerListCtl(const CObList& obList)
{
	m_csUserList.Lock();//����

	//ɾ��ԭ��������
	m_ctlUserList.DeleteAllItems();

	POSITION	pos = NULL;		//λ�ñ���
	int			nIndex = 0;		//���
	for (pos = m_UserList.GetHeadPosition(); NULL != pos;)
	{
		//ȡ���û���Ϣ
		CUserInfo *pUserInfo = (CUserInfo*)m_UserList.GetNext(pos);
		if (CUserInfo::ONLINE == pUserInfo->m_eStatus)//����״̬
		{
			//���������� 
			m_ctlUserList.InsertItem(nIndex, pUserInfo->m_strName,CUserInfo::ONLINE);

		}else//����״̬ 		
		{
			//���������� 
			m_ctlUserList.InsertItem(nIndex, pUserInfo->m_strName,CUserInfo::OFFLINE);
		}

		m_ctlUserList.SetItemText(nIndex,1,pUserInfo->m_strPassword);	//�û�����

		in_addr userAddr;
		userAddr.S_un.S_addr = (long)pUserInfo->m_lIP;
		CString strUserAddr(inet_ntoa(userAddr));						//�û�IP
		m_ctlUserList.SetItemText(nIndex,2,strUserAddr);

		CString strUserPort;											//�û��˿�
		strUserPort.Format("%d",ntohs(pUserInfo->m_nPort));	
		m_ctlUserList.SetItemText(nIndex,3,strUserPort);

		CString strUserTime = pUserInfo->m_time.Format("%c");			//��¼ʱ��
		m_ctlUserList.SetItemText(nIndex,4,strUserTime);
		nIndex++;		
	}
	m_csUserList.Unlock();//����
}

void CWSAAsyncSelectServerDlg::ReleaseChatter(CClientSocket* pClientSocket)
{
	m_csChatterList.Lock();
	POSITION pos = NULL;

	//�ҵ����û�
	if ((pos = m_ChatterList.Find(pClientSocket)) != NULL)
	{
		ASSERT((CClientSocket*)m_ChatterList.GetAt(pos) == pClientSocket);

		//��ȡ���û���Ϣ
		CUserInfo userInfo = pClientSocket->GetUserInfo();

		//���¸��û���Ϣ
		POSITION posUser = NULL;
		for (posUser = m_UserList.GetHeadPosition(); posUser != NULL;)
		{
			CUserInfo *pUserInfo = (CUserInfo*)m_UserList.GetNext(posUser);
			if (pUserInfo->m_strName == userInfo.m_strName)
			{
				pUserInfo->m_eStatus = CUserInfo::OFFLINE;	//�޸�״̬Ϊ����
				pUserInfo->m_lIP = userInfo.m_lIP;			//�޸��û���ַ			
				pUserInfo->m_nPort = userInfo.m_nPort;		//�޸��û��˿�
				pUserInfo->m_time = CTime::GetCurrentTime();//��������ʱ��
				break;
			}
		}

		//ɾ���������û�
		m_ChatterList.RemoveAt(pos);

		//֪ͨ���������û������û��б�
		SendUserList();

		//���·���������
		UpdateServerListCtl(m_UserList);	
	}

	m_csChatterList.Unlock();
}
void CWSAAsyncSelectServerDlg::DeleteChatter(CClientSocket* pClientSocket)
{
	m_csChatterList.Lock();
	POSITION pos = NULL;
	if ((pos = m_ChatterList.Find(pClientSocket)) != NULL)
	{
		ASSERT((CClientSocket*)m_ChatterList.GetAt(pos) == pClientSocket);		
		//ɾ���������û�
		m_ChatterList.RemoveAt(pos);	
	}

	m_csChatterList.Unlock();
}
void CWSAAsyncSelectServerDlg::DeleteAllChatter(void)
{
	POSITION pos;
	CClientSocket *pClient = NULL;
	for (pos = m_ChatterList.GetHeadPosition(); NULL != pos; )
	{
		pClient = (CClientSocket*)m_ChatterList.GetNext(pos);
		delete pClient;
		pClient = NULL;
	}
	m_ChatterList.RemoveAll();
}

void CWSAAsyncSelectServerDlg::OnRclickClientList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();					//����ȡλ��
		CPoint point( LOWORD(dwPos), HIWORD(dwPos) );

		CMenu menu;
		VERIFY( menu.LoadMenu(IDR_SERVER_LIST_MENU));	//װ�ز˵���Դ
		CMenu* popup = menu.GetSubMenu(0);				//��ȡ�˵���
		ASSERT( popup != NULL );

		//������ݲ˵�
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
	}
	*pResult = 0;
}
void CWSAAsyncSelectServerDlg::OnServerListDeleteMenuitem()
{
	//��ñ�ѡ���������λ��
	POSITION pos = m_ctlUserList.GetFirstSelectedItemPosition();
	int nSelectedItem = m_ctlUserList.GetNextSelectedItem(pos);
	//��ȡ�û�����
	CString strPeerName = m_ctlUserList.GetItemText(nSelectedItem, 0);

	POSITION pos1 = NULL;		//λ�ñ��� 
	POSITION pos2 = NULL;		//λ�ñ���
	BOOL bDeleteUser = FALSE;	//�Ƿ��ҵ����û�

	//���û������в���
	for (pos1 = m_UserList.GetHeadPosition();(pos2=pos1)!= NULL;)
	{
		//ȡ������
		CUserInfo* pUserInfo = (CUserInfo*)m_UserList.GetNext(pos1);
		if (pUserInfo->m_strName == strPeerName)			//������ͬ
		{
			if (pUserInfo->m_eStatus == CUserInfo::ONLINE)	//����ɾ�������û�
			{
				AfxMessageBox("����ɾ�������û���",MB_OK, -1);
				return ;
			}else//ɾ���������û�
			{
				CUserInfo *pUserInfo = (CUserInfo*)m_UserList.GetAt(pos2);
				m_UserList.RemoveAt(pos2);	//ɾ��ָ��
				delete pUserInfo;			//ɾ������
				pUserInfo = NULL;			

				bDeleteUser = TRUE;
				//���·���������
				UpdateServerListCtl(m_UserList);

			}
		}
	}

	if (TRUE == bDeleteUser)
	{
		//֪ͨ�ͻ��˸��³�Ա�б�
		SendUserList();
	}	
}