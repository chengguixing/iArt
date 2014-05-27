#include "stdafx.h"
#include "ISocket.h"
#include "ClientView.h"


CISocket::CISocket(CClientView* pClientView, SOCKET s, WSAEVENT event)
{
	m_pClientView = pClientView;
	m_s = s;
	m_event = event;
}

CISocket::~CISocket()
{
	m_pClientView = NULL;
	closesocket(m_s);
	m_s = INVALID_SOCKET;
	WSACloseEvent(m_event);
}

// BOOL CISocket::Send(const byte type, const CString &strData )
// {
// 	int	nErrCode;		//返回值	
// 	PACKETHDR packetLen;//定义包头
// 
// 	packetLen.type = type;				//类型
// 	packetLen.len = strData.GetLength();//数据长度
// 
// 	//发送包头
// 	nErrCode = send(m_s, (char*)&packetLen, PACKETHDRLEN, 0);
// 	if (SOCKET_ERROR == nErrCode)
// 	{
// 		AfxMessageBox("发送用户列表错误！");
// 		return FALSE;
// 	}
// 	//发送包体
// 	nErrCode = send(m_s, strData, packetLen.len, 0);
// 	if (SOCKET_ERROR == nErrCode)
// 	{
// 		AfxMessageBox("发送用户列表错误！");
// 		return FALSE;
// 	}
// 	return TRUE;	
// }

BOOL CISocket::Send( const PCLIENTPACKET pClientPacket )
{
	int	nErrCode;		//返回值

	nErrCode = send(m_s, (char*)pClientPacket, CLIENTPACKETLEN, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("客户端发送数据错误！");
		return FALSE;
	}
	return TRUE;	
}

BOOL CISocket::Recv( void )
{
	int nErrCode;
	CLIENTPACKET clientPacket;
	nErrCode = recv(m_s, (char*)&clientPacket, CLIENTPACKETLEN, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		if (WSAEWOULDBLOCK == WSAGetLastError())
		{
			return TRUE;
		}
		AfxMessageBox("客户端接收数据错误");
		return FALSE;
	}
	m_pClientView->HandleClientData(&clientPacket);
	return TRUE;
}
