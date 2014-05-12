#include "StdAfx.h"
#include "IPCEnv.h"

static unsigned int __stdcall thread_proc(void* param)
{
	::ImmDisableIME(::GetCurrentThreadId());
	CIPCEnv* ptr = (CIPCEnv*)param;
	return ptr->ThreadProc();
}

CIPCEnv::CIPCEnv(void)
{
	m_dwPid = GetCurrentThreadId();
	TCHAR ch[255];
	_stprintf_s(ch, _T("UPDATE_EVENT_%d"), m_dwPid);
	m_hUpdateEvent = CreateEvent(NULL, FALSE, FALSE, ch);
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &thread_proc, (void*)this, CREATE_SUSPENDED, &m_dwThreadID);
	::ResumeThread(m_hThread);
}

CIPCEnv::~CIPCEnv(void)
{
	;
}

unsigned int CIPCEnv::ThreadProc()
{
	while(true)
	{
		DWORD dwSocketNum = m_vecSocket.size();
		if (dwSocketNum > (MAX_EVENT_NUM - 1) / 2)
		{
			dwSocketNum = (MAX_EVENT_NUM - 1) / 2;
		}
 		HANDLE hEvents[MAX_EVENT_NUM];
		int nSendVec = m_vecSendEvents.size();
		int nRecvVec = m_vecRecvEvents.size();
 		for (int i = 0; i < (int)dwSocketNum; ++i)
 		{
			if (nRecvVec > i)
			{
				hEvents[i] = *m_vecRecvEvents[i];
			}
 			
			if (nSendVec > i)
			{
				hEvents[i + dwSocketNum] = *m_vecSendEvents[i];
			}
		}
		//update消息，有新对象加入，通知其break，然后更新
		hEvents[2 * dwSocketNum] = m_hUpdateEvent;

		//printf("/********WaitForMultipleObjects********/\n");
		DWORD dwEvent;
		dwEvent = WaitForMultipleObjects(2 * dwSocketNum + 1, hEvents, FALSE, INFINITE);
 		dwEvent -= WAIT_OBJECT_0;
		TSDEBUG(L"Enter %s : %s dwEvent = %d", __FILEW__, __FUNCTIONW__, dwEvent);
		if (dwEvent == WAIT_TIMEOUT)
		{
			//printf("/********OnTimer********/\n");
			for (int i = 0; i < (int)dwSocketNum; ++i)
			{
				m_vecSocket[i]->OnTimer();
			}
		}
		if (dwEvent == 2 * dwSocketNum)
		{
			//printf("/********AddNew********/\n");
			continue;
		}
		if (dwEvent < dwSocketNum)
		{
			//printf("/********OnRecv********/\n");
			m_vecSocket[dwEvent]->OnRecv();
		}
		else if (dwEvent < 2 * dwSocketNum)
		{
			//printf("/********OnSend********/\n");
			m_vecSocket[dwEvent - dwSocketNum]->OnSend();
		}
	}
	//printf("/********ThreadProc return********/\n");
	return 0;
}


DWORD CIPCEnv::IPCStartup(DWORD dwParam)
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	return 0;
}
DWORD CIPCEnv::IPCClearup()
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	PostQuitMessage(0);
	int size = m_vecSocket.size();
	for (int i=0; i<size; i++)
	{
		m_vecSocket[i]->SendMessage(WM_IPC_DELETE);
	}
	delete this;
	return 0;
}

DWORD  CIPCEnv::AttachSocketEvent(CIPCSocket* pSock)
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	m_vecSocket.push_back(pSock);
	m_vecRecvEvents.push_back(&(pSock->m_hReadEvent));
	m_vecSendEvents.push_back(&(pSock->m_hWriteEvent));
	SetEvent(m_hUpdateEvent);
	return 0;
}

DWORD  CIPCEnv::DetachSocketEvent(CIPCSocket* pSock)
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	DWORD dwSize = m_vecSocket.size();
	for (vector<HANDLE*>::iterator iter=m_vecRecvEvents.begin(); iter!=m_vecRecvEvents.end(); iter++)
	{
		if (*iter == &(pSock->m_hReadEvent))
		{
			m_vecRecvEvents.erase(iter);
			break;
		}
	}
	for (vector<HANDLE*>::iterator iter=m_vecSendEvents.begin(); iter!=m_vecSendEvents.end(); iter++)
	{
		if (*iter == &(pSock->m_hWriteEvent))
		{
			m_vecSendEvents.erase(iter);
			break;
		}
	}
	for (vector<CIPCSocket*>::iterator iter=m_vecSocket.begin(); iter!=m_vecSocket.end(); iter++)
	{
		if (*iter == pSock)
		{
			m_vecSocket.erase(iter);
			break;
		}
	}
	SetEvent(m_hUpdateEvent);
	return 0;
}