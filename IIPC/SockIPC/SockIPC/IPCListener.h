#pragma once
#include "ipcsocket.h"

class CIPCListener : public CIPCSocket
{
public:
	CIPCListener(void);
	~CIPCListener(void);

	DWORD	IPCBind(IPCAddress& addr);
	DWORD	IPCListen(DWORD dwMaxConn);
	DWORD	IPCAccept(CIPCConnection** pConn, IPCAddress &addr);

	DWORD	IPCSend(byte* byteBuff, size_t buffSize, DWORD dwFlag);
	DWORD	IPCRecv(byte* byteBuff, size_t buffSize);
	DWORD	IPCClose();

	DWORD	OnSend();
	DWORD	OnRecv();
	DWORD	OnTimer();
	void OnFinalMessage(HWND hwnd)
	{
		delete this;
	}

	DWORD	m_dwMaxConnNum;
	DWORD	m_dwConnNum;

	HANDLE	m_hBusyEvent;

	//����conn�ĵ�ַ
	vector<IPCAddress>	m_vecAddrClient;

	//Ϊ���󴴽���conn��ַ
	vector<IPCAddress>	m_vecAddrConn;
};
