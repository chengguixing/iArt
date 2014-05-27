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
// 	int	nErrCode;		//����ֵ	
// 	PACKETHDR packetLen;//�����ͷ
// 
// 	packetLen.type = type;				//����
// 	packetLen.len = strData.GetLength();//���ݳ���
// 
// 	//���Ͱ�ͷ
// 	nErrCode = send(m_s, (char*)&packetLen, PACKETHDRLEN, 0);
// 	if (SOCKET_ERROR == nErrCode)
// 	{
// 		AfxMessageBox("�����û��б����");
// 		return FALSE;
// 	}
// 	//���Ͱ���
// 	nErrCode = send(m_s, strData, packetLen.len, 0);
// 	if (SOCKET_ERROR == nErrCode)
// 	{
// 		AfxMessageBox("�����û��б����");
// 		return FALSE;
// 	}
// 	return TRUE;	
// }

BOOL CISocket::Send( const PCLIENTPACKET pClientPacket )
{
	int	nErrCode;		//����ֵ

	nErrCode = send(m_s, (char*)pClientPacket, CLIENTPACKETLEN, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("�ͻ��˷������ݴ���");
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
		AfxMessageBox("�ͻ��˽������ݴ���");
		return FALSE;
	}
	m_pClientView->HandleClientData(&clientPacket);
	return TRUE;
}
