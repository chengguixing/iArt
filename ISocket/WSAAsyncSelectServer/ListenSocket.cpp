// ListenSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "WSAAsyncSelectServer.h"
#include "ListenSocket.h"
#include "WSAAsyncSelectServerDlg.h"


// CListenSocket

CListenSocket::CListenSocket(CWSAAsyncSelectServerDlg* pServerDlg)
{
	m_pServerDlg = pServerDlg;
}

CListenSocket::~CListenSocket()
{
	m_pServerDlg = NULL;
}


// CListenSocket 成员函数


void CListenSocket::OnAccept(int nErrorCode)
{
	m_pServerDlg->ProcessPendingAccept();
}