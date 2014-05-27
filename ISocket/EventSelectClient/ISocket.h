#pragma once
class CClientView;

class CISocket : public CObject  
{
public:

	CISocket(CClientView* pClientView, SOCKET s, WSAEVENT event);
	virtual ~CISocket();

public:
	BOOL	Send(const PCLIENTPACKET pClientPacket);//发送数据
	BOOL	Recv(void);								//接收数据

private:
	CISocket();

private:
	CClientView	*m_pClientView;//指针
	SOCKET		m_s;			//套接字
	WSAEVENT	m_event;		//事件对象
};