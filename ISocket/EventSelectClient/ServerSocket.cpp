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
	closesocket(m_s);			//关闭套结字
	m_s = INVALID_SOCKET;
	WSACloseEvent(m_event);		//释放事件对象
}

/*
 * 发送数据
 */
BOOL CServerSocket::Send(const u_short type, const CString strData )
{
	int			nErrCode;	//返回值	
	PACKETHDR	packetHdr;	//定义包头

	packetHdr.type = type;				//包类型
	packetHdr.len = strData.GetLength();//数据长度

	//发送包头
	nErrCode = send(m_s, (char*)&packetHdr, PACKETHDRLEN, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("发送用户列表错误！");
		return FALSE;
	}
	//发送包体
	nErrCode = send(m_s, strData, packetHdr.len, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("发送用户列表错误！");
		return FALSE;
	}
	return TRUE;	
}

/*
 * 接收数据
 */
BOOL CServerSocket::Recv( void )
{
	int reVal;							//返回值
	WSAEventSelect(m_s, m_event, 0);	//取消网络事件
	
	//获取数据包体的长度
	PACKETHDR packetHdr;			
	reVal = recv(m_s, (char*)&packetHdr, PACKETHDRLEN, 0);
	
	if ( 0 == reVal)
	{
		return FALSE;
	}else if (SOCKET_ERROR == reVal)//网络错误
	{
		int nErrCode = WSAGetLastError();
		if (WSAEWOULDBLOCK == nErrCode)
		{
			WSAEventSelect(m_s, m_event, FD_READ|FD_CLOSE);
			return TRUE;
		}
		WSAEventSelect(m_s, m_event, FD_READ|FD_CLOSE);
		AfxMessageBox("客户端接收用户列表错误！");
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
 * 接收服务器消息
 */
BOOL CServerSocket::RecvMessage( int nMessageLen )
{
	CString	strMsg;						//用户信息
	int		nTotalLen = 0;				//已经读取字符数量
	int		nDataLen = nMessageLen;		//数据长度
	int		nReadLen = 0;				//每次读取字符数量 
	while ( nTotalLen != nDataLen )
	{		
		char cRecv;								//接收字符
		nReadLen = recv(m_s, &cRecv, 1,0);		//每次接收一个字符
		if (SOCKET_ERROR == nReadLen)			//网络错误
		{
			if (WSAEWOULDBLOCK == WSAGetLastError())
			{
				continue;						//继续接收数据
			}
			AfxMessageBox(_T("读取客户端数据失败！"));
			return FALSE;
		}else if (0 == nReadLen)
		{
			AfxMessageBox(_T("服务器关闭了连接！"));
			return	FALSE;
		}else if (nReadLen > 0)
		{
			strMsg += cRecv;		
			nTotalLen += nReadLen;
		}		
	}		

	AfxMessageBox(strMsg);//显示消息
	return TRUE;
}
/*
 * 接收用户列表
 */
BOOL CServerSocket::RecvUserList( int nMessageLen )
{
	CString	strUserList;				//用户列表信息
	int		nTotalLen = 0;				//已经读取字符数量
	int		nDataLen = nMessageLen;		//数据长度
	int		nReadLen = 0;				//每次读取字符数量 
	while ( nTotalLen != nDataLen )
	{		
		char cRecv;								//接收字符
		nReadLen = recv(m_s, &cRecv, 1,0);		//每次接收一个字符
		if (SOCKET_ERROR == nReadLen)			//网络错误
		{
			if (WSAEWOULDBLOCK == WSAGetLastError())
			{			
				continue;						//继续接收数据
			}
			AfxMessageBox(_T("读取客户端数据失败！"));
			return FALSE;
		}else if (0 == nReadLen)
		{	
			AfxMessageBox(_T("客户端关闭了连接！"));
			return  FALSE;
		}else if (nReadLen > 0)
		{
			strUserList += cRecv;
			nTotalLen += nReadLen;
		}		
	}
	m_pClientView->UpdateUserList(strUserList);//更新用户列表
	return TRUE;

}