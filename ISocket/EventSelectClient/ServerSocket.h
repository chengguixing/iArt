#pragma once
#include "afx.h"

class CClientView;
class CServerSocket : public CObject  
{
public:

	CServerSocket(CClientView* pClientView, SOCKET s, WSAEVENT event);
	virtual ~CServerSocket();


public:
	BOOL	Send(const u_short type, const CString strData);//向服务器发送数据
	BOOL	Recv(void);										//接收数据
	BOOL	RecvMessage(int nMessageLen);					//接收服务器消息
	BOOL	RecvUserList(int nMessageLen);					//接收用户列表

private:
	CServerSocket();

private:
	CClientView	*m_pClientView;	//指针
	SOCKET		m_s;			//套接字
	WSAEVENT	m_event;		//事件对象
};