#include "stdafx.h"
#include "ClientSocket.h"


CClientSocket::CClientSocket(CEventSelectServerDlg* pServDlg, SOCKET s, WSAEVENT hEvent)
{
	m_pServDlg = pServDlg;
	m_s = s;
	m_hEvent = hEvent;
}
CClientSocket::~CClientSocket(void)
{
	closesocket(m_s);
	WSAEventSelect(m_s, m_hEvent, 0);
	WSACloseEvent(m_hEvent);
}

BOOL CClientSocket::Recv(void)
{
	int reVal;
	WSAEventSelect(m_s, m_hEvent, 0);

	PACKETHDR packetHdr;
	reVal = recv(m_s, (char*)&packetHdr, PACKETHDRLEN, 0);

	if (0 == reVal)
	{
		return FALSE;
	}
	else if (SOCKET_ERROR == reVal)
	{
		int nErrCode = WSAGetLastError();
		if (WSAEWOULDBLOCK == nErrCode)
		{
			return TRUE;
		}
		AfxMessageBox(TEXT("接收用户列表错误!"));
	}
	else
	{
		CString strUserInfo;
		int nTotalLen = 0;
		int nDataLen = packetHdr.len;
		int nReadLen = 0;
		while (nTotalLen != nDataLen)
		{
			char cRecv;
			nReadLen = recv(m_s, &cRecv, 1, 0);
			if (SOCKET_ERROR == nReadLen)
			{
				if (WSAEWOULDBLOCK == WSAGetLastError())
				{
					continue;
				}
				AfxMessageBox(TEXT("读取客户端数据失败!"));
				reVal = FALSE;
			}
			else if (0 == nReadLen)
			{
				AfxMessageBox(TEXT("客户端关闭了连接!"));
				reVal = FALSE;
			}
			else if (nReadLen > 0)
			{
				if ('<' == cRecv)
				{
					strUserInfo.Empty();
				}
				else if ('>' == cRecv)
				{
					break;
				}
				else
				{
					strUserInfo += cRecv;
				}
			}
		}
		WSAEventSelect(m_s, m_hEvent, FD_READ | FD_CLOSE);
		return (m_pServDlg->UpdateUserList(strUserInfo, this));
	}
	return TRUE;
}

BOOL CClientSocket::Send(const u_short type, const CString& strData)
{
	ASSERT(!strData.IsEmpty());

	int nErrCode;
	PACKETHDR packetHdr;


	packetHdr.type = type;
	packetHdr.len = strData.GetLength();

	nErrCode = send(m_s, (char*)&packetHdr, PACKETHDRLEN, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox(TEXT("发送用户列表错误!"));
		return FALSE;
	}

	nErrCode = send(m_s, strData, packetHdr.len, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox(TEXT("发送用户列表错误!"));
		return FALSE;
	}
	return TRUE;
}

void CClientSocket::SaveUserInfo(const CUserInfo& userInfo)
{
	m_UserInfo = userInfo;
}