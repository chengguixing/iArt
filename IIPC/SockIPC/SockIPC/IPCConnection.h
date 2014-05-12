#pragma once
#include "ipcsocket.h"

class CIPCConnection : public CIPCSocket
{
public:
	CIPCConnection(void);
	~CIPCConnection(void);

	DWORD IPCConnect(IPCAddress& addr);
	DWORD IPCAccept(IPCAddress& addr);
	DWORD WorkByWriteEvent();
	DWORD WorkByReadEvent();

	DWORD IPCSend(byte* byteBuff, size_t buffSize, DWORD dwFlag = ENUM_IPC_SEND);
	DWORD IPCRecv(byte* byteBuff, size_t buffSize);
	DWORD IPCClose();
	void OnFinalMessage(HWND hwnd)
	{
		delete this;
	}

	DWORD SetDesAddr(IPCAddress& addr)
	{
		m_addrDes = addr;
		return 0;
	}
	DWORD GetSrcAddr(IPCAddress& addr)
	{
		addr = m_addrSrc;
		return 0;
	}
	DWORD SetParent(CIPCSocket* pParent)
	{
		m_hParent = pParent;
		return 0;
	}

	DWORD SetMsgMap(map<DWORD, IPCCallback> mapMsg)
	{
		m_mapMsg = mapMsg;
		return 0;
	}


	DWORD OnConnection();
	DWORD OnSend();
	DWORD OnRecv();
	DWORD OnTimer();

	HANDLE m_hBusyEvent;
	//目的write锁
	HANDLE m_hDesBusyEvent;
	//目的read消息锁
	HANDLE m_hDesReadEvent;

	DWORD m_dwTickCount;
	DWORD m_dwStatus;
};
