#include "stdafx.h"
#include "ClientSocket.h"
#include "Item.h"

CClientSocket::CClientSocket(CPClientDlg* pClientDlg)
{
	int nRet;
	WSADATA wsaData;
	if ((nRet = WSAStartup(0x0202, &wsaData)) != 0)
	{
		return ;
	}

	if ((m_s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		AfxMessageBox(TEXT("创建套接字错误"));
		WSACleanup();
	}
	m_pClientDlg = pClientDlg;
}
CClientSocket::~CClientSocket(void)
{
	closesocket(m_s);
	WSACleanup();
}


BOOL CClientSocket::ConnectServer(DWORD& dwServIP, short& sServPort)
{
	int nRet;
	SOCKADDR_IN servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(dwServIP);
	servAddr.sin_port = htons(sServPort);
	int nServLen = sizeof(servAddr);


	nRet = connect(m_s, (SOCKADDR*)&servAddr, nServLen);
	if (SOCKET_ERROR == nRet)
	{
		return FALSE;
	}

	const char chOpt = 1;
	nRet = setsockopt(m_s, IPPROTO_TCP, TCP_NODELAY, &chOpt, sizeof(char));
	if (SOCKET_ERROR == nRet)
	{
		return FALSE;
	}
	return TRUE;
}
BOOL CClientSocket::SendPacket(PHDER pHdr, CPClientDlg::state state, char* pSend)
{
	int nRet;
	ZeroMemory(m_buf, BUFFER_SIZE);
	memcpy(m_buf, (char*)pHdr, HEADELEN);

	u_short usState = (u_short)state;
	memcpy(m_buf + HEADELEN, (char*)&usState, sizeof(u_short));

	if (NULL != pSend)
	{
		int nSendLen = strlen(pSend);
		memcpy(m_buf + HEADELEN + sizeof(u_short), pSend, nSendLen);
	}


	nRet = send(m_s, m_buf, HEADELEN + pHdr->usLen, 0);
	if (SOCKET_ERROR == nRet)
	{
		AfxMessageBox(TEXT("SendPacket数据失败"));
		return FALSE;
	}
	return TRUE;
}
BOOL CClientSocket::SendPulsePacket(void)
{
	int nRet;
	ZeroMemory(m_buf, BUFFER_SIZE);
	HDR hdr;
	hdr.ustype = PULSE;
	hdr.usLen = 0;
	nRet = send(m_s, (char*)&hdr, HEADELEN, 0);
	if (SOCKET_ERROR == nRet)
	{
		AfxMessageBox(TEXT("SendPulsePacket数据失败"));
		return FALSE;
	}
	return TRUE;
}
BOOL CClientSocket::RecvPaper(CString& strName, CObList* pItemList)
{
	int nRet;

	HDR hdr;
	ZeroMemory(&hdr, HEADELEN);
	nRet = recv(m_s, (char*)&hdr, HEADELEN, 0);
	ASSERT(STNAME == hdr.ustype);
	if (0 == hdr.usLen)
	{
		AfxMessageBox(TEXT("用户重复登录或者学号错误!"));
		return FALSE;
	}


	char recvBuf[BUFFER_SIZE];
	ZeroMemory(recvBuf, BUFFER_SIZE);
	nRet = recv(m_s, recvBuf, hdr.usLen, 0);
	strName = recvBuf;


	ZeroMemory(&hdr, HEADELEN);
	nRet = recv(m_s, (char*)&hdr, HEADELEN, 0);
	if (SOCKET_ERROR == nRet)
	{
		return FALSE;
	}


	int nCount = 0;
	int nTotal = hdr.usLen;
	CItem* pItem = NULL;
	while (nCount < nTotal)
	{
		int nRead = 0;
		char c;
		nRead = recv(m_s, &c, 1, 0);
		if (SOCKET_ERROR == nRead)
		{
			AfxMessageBox(TEXT("接收试卷失败"));
			break;
		}
		else
		{
			if (c == '<')
			{
				pItem = new CItem(m_pClientDlg);
			}
			else if (c == '>')
			{
				pItemList->AddTail(pItem);
			}
			else
			{
				pItem->m_strItem += c;
			}
			++nCount;
		}
	}
	return TRUE;
}