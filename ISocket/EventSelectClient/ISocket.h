#pragma once
class CClientView;

class CISocket : public CObject  
{
public:

	CISocket(CClientView* pClientView, SOCKET s, WSAEVENT event);
	virtual ~CISocket();

public:
	BOOL	Send(const PCLIENTPACKET pClientPacket);//��������
	BOOL	Recv(void);								//��������

private:
	CISocket();

private:
	CClientView	*m_pClientView;//ָ��
	SOCKET		m_s;			//�׽���
	WSAEVENT	m_event;		//�¼�����
};