#pragma once
#include "afx.h"

class CClientView;
class CServerSocket : public CObject  
{
public:

	CServerSocket(CClientView* pClientView, SOCKET s, WSAEVENT event);
	virtual ~CServerSocket();


public:
	BOOL	Send(const u_short type, const CString strData);//���������������
	BOOL	Recv(void);										//��������
	BOOL	RecvMessage(int nMessageLen);					//���շ�������Ϣ
	BOOL	RecvUserList(int nMessageLen);					//�����û��б�

private:
	CServerSocket();

private:
	CClientView	*m_pClientView;	//ָ��
	SOCKET		m_s;			//�׽���
	WSAEVENT	m_event;		//�¼�����
};