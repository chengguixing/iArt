// ClientSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "WSAAsyncSelectServerDlg.h"
#include "ClientSocket.h"


// CClientSocket

CClientSocket::CClientSocket(CWSAAsyncSelectServerDlg* pServDlg)
{
	m_pServDlg = pServDlg;
}

CClientSocket::~CClientSocket()
{
	m_pServDlg = NULL;

	if (NULL != m_pFile)
	{
		delete m_pFile;
	}
}


void CClientSocket::Init(void)
{
	m_pFile = new CSocketFile(this);
}



void CClientSocket::OnReceive(int nErrorCode)
{
	CArchive* pArchiveIn = new CArchive(m_pFile, CArchive::load);
	CArchive* pArchiveOut = new CArchive(m_pFile, CArchive::store);
	BOOL reVal = m_pServDlg->ProcessPendingRead(pArchiveIn, pArchiveOut, this);
	delete pArchiveIn;
	pArchiveIn = NULL;
	delete pArchiveOut;
	pArchiveOut = NULL;

	if (FALSE == reVal)
	{
		delete this;
	}

}


void CClientSocket::OnClose(int nErrorCode)
{
	ShutDown(sends);

	m_pServDlg->ReleaseChatter(this);

	delete this;
}


void CClientSocket::SendUserList(CChatPacket*  pPacket)
{
	CArchive* pArchiveOut = new CArchive(m_pFile, CArchive::store);
	pPacket->Serialize(*pArchiveOut);
	pArchiveOut->Flush();
	delete pArchiveOut;
	pArchiveOut = NULL;
}
void CClientSocket::SendUserMsg(CChatPacket* pPacket)
{
	CArchive* pArchiveOut = new CArchive(m_pFile, CArchive::store);
	pPacket->Serialize(*pArchiveOut);
	pArchiveOut->Flush();

	delete pArchiveOut;
	pArchiveOut = NULL;
}
void CClientSocket::SaveUserInfo(SOCKADDR_IN clientAddr)
{
	m_userInfo.m_lIP = clientAddr.sin_addr.S_un.S_addr;
	m_userInfo.m_nPort = clientAddr.sin_port;
}
void CClientSocket::SaveUserInfo(const CUserInfo& userInfo)
{
	m_userInfo.m_eStatus = userInfo.m_eStatus;
	m_userInfo.m_strName = userInfo.m_strName;
	m_userInfo.m_strPassword = userInfo.m_strPassword;
	m_userInfo.m_time = userInfo.m_time;
}
CUserInfo CClientSocket::GetUserInfo(void)
{
	return m_userInfo;
}

// CClientSocket 成员函数
