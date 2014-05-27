#include "stdafx.h"
#include "ServerSocket.h"
#include "ClientView.h"


CServerSocket::CServerSocket(CClientView* pClientView, SOCKET s, WSAEVENT event)
{
	m_pClientView = pClientView;
	m_s = s;
	m_event = event;
}

CServerSocket::~CServerSocket()
{
	m_pClientView = NULL;
	closesocket(m_s);			//�ر��׽���
	m_s = INVALID_SOCKET;
	WSACloseEvent(m_event);		//�ͷ��¼�����
}

/*
 * ��������
 */
BOOL CServerSocket::Send(const u_short type, const CString strData )
{
	int			nErrCode;	//����ֵ	
	PACKETHDR	packetHdr;	//�����ͷ

	packetHdr.type = type;				//������
	packetHdr.len = strData.GetLength();//���ݳ���

	//���Ͱ�ͷ
	nErrCode = send(m_s, (char*)&packetHdr, PACKETHDRLEN, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("�����û��б����");
		return FALSE;
	}
	//���Ͱ���
	nErrCode = send(m_s, strData, packetHdr.len, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("�����û��б����");
		return FALSE;
	}
	return TRUE;	
}

/*
 * ��������
 */
BOOL CServerSocket::Recv( void )
{
	int reVal;							//����ֵ
	WSAEventSelect(m_s, m_event, 0);	//ȡ�������¼�
	
	//��ȡ���ݰ���ĳ���
	PACKETHDR packetHdr;			
	reVal = recv(m_s, (char*)&packetHdr, PACKETHDRLEN, 0);
	
	if ( 0 == reVal)
	{
		return FALSE;
	}else if (SOCKET_ERROR == reVal)//�������
	{
		int nErrCode = WSAGetLastError();
		if (WSAEWOULDBLOCK == nErrCode)
		{
			WSAEventSelect(m_s, m_event, FD_READ|FD_CLOSE);
			return TRUE;
		}
		WSAEventSelect(m_s, m_event, FD_READ|FD_CLOSE);
		AfxMessageBox("�ͻ��˽����û��б����");
		return FALSE;
	}else
	{
		if (packetHdr.type == SERVMESSAGE)
		{
			 BOOL bRet=  RecvMessage(packetHdr.len);
			 WSAEventSelect(m_s, m_event, FD_READ|FD_CLOSE);
			 return bRet;

		}else
		{
			BOOL bRet = RecvUserList(packetHdr.len);
			WSAEventSelect(m_s, m_event, FD_READ|FD_CLOSE);
			return bRet;
		}
	}
	return TRUE;
}
/*
 * ���շ�������Ϣ
 */
BOOL CServerSocket::RecvMessage( int nMessageLen )
{
	CString	strMsg;						//�û���Ϣ
	int		nTotalLen = 0;				//�Ѿ���ȡ�ַ�����
	int		nDataLen = nMessageLen;		//���ݳ���
	int		nReadLen = 0;				//ÿ�ζ�ȡ�ַ����� 
	while ( nTotalLen != nDataLen )
	{		
		char cRecv;								//�����ַ�
		nReadLen = recv(m_s, &cRecv, 1,0);		//ÿ�ν���һ���ַ�
		if (SOCKET_ERROR == nReadLen)			//�������
		{
			if (WSAEWOULDBLOCK == WSAGetLastError())
			{
				continue;						//������������
			}
			AfxMessageBox(_T("��ȡ�ͻ�������ʧ�ܣ�"));
			return FALSE;
		}else if (0 == nReadLen)
		{
			AfxMessageBox(_T("�������ر������ӣ�"));
			return	FALSE;
		}else if (nReadLen > 0)
		{
			strMsg += cRecv;		
			nTotalLen += nReadLen;
		}		
	}		

	AfxMessageBox(strMsg);//��ʾ��Ϣ
	return TRUE;
}
/*
 * �����û��б�
 */
BOOL CServerSocket::RecvUserList( int nMessageLen )
{
	CString	strUserList;				//�û��б���Ϣ
	int		nTotalLen = 0;				//�Ѿ���ȡ�ַ�����
	int		nDataLen = nMessageLen;		//���ݳ���
	int		nReadLen = 0;				//ÿ�ζ�ȡ�ַ����� 
	while ( nTotalLen != nDataLen )
	{		
		char cRecv;								//�����ַ�
		nReadLen = recv(m_s, &cRecv, 1,0);		//ÿ�ν���һ���ַ�
		if (SOCKET_ERROR == nReadLen)			//�������
		{
			if (WSAEWOULDBLOCK == WSAGetLastError())
			{			
				continue;						//������������
			}
			AfxMessageBox(_T("��ȡ�ͻ�������ʧ�ܣ�"));
			return FALSE;
		}else if (0 == nReadLen)
		{	
			AfxMessageBox(_T("�ͻ��˹ر������ӣ�"));
			return  FALSE;
		}else if (nReadLen > 0)
		{
			strUserList += cRecv;
			nTotalLen += nReadLen;
		}		
	}
	m_pClientView->UpdateUserList(strUserList);//�����û��б�
	return TRUE;

}