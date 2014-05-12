#pragma once
#include "IPCConnection.h"
#include "IPCListener.h"

#define MAX_EVENT_NUM	32

class CIPCEnv
{
private:
	CIPCEnv(void);
	~CIPCEnv(void);

public:
	unsigned int ThreadProc();

	HANDLE	        m_hThread;
	unsigned int	m_dwThreadID;

	static CIPCEnv* Instance()
	{
		static CIPCEnv* s_pEnv;
		if (s_pEnv == NULL)
		{
			s_pEnv = new CIPCEnv;
		}
		return s_pEnv;
	}

	DWORD IPCStartup(DWORD dwParam);
	DWORD IPCClearup();
	DWORD AttachSocketEvent(CIPCSocket* pLis);
	DWORD DetachSocketEvent(CIPCSocket* pLis);


	vector<CIPCSocket*> m_vecSocket;

	vector<HANDLE*>	m_vecRecvEvents;
	vector<HANDLE*>	m_vecSendEvents;

	HANDLE	m_hUpdateEvent;
	DWORD	m_dwPid;
};
