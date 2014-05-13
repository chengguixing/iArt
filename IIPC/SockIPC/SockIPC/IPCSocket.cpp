#include "StdAfx.h"
#include "IPCSocket.h"

extern HINSTANCE g_hInstance;

static unsigned int thread_entry(void* param)
{
	CIPCSocket* ptr = (CIPCSocket*)param;
	return ptr->ThreadProc();
}

CIPCSocket::CIPCSocket(void)
{
	m_addrSrc.dwPid = GetCurrentProcessId();
	m_dwThreadID = GetCurrentThreadId();
	m_vecEvents.push_back(&m_hReadEvent);
	m_vecEvents.push_back(&m_hWriteEvent);
   
	m_CriticalSend.Init();
	m_CriticalRecv.Init();
	m_dwEventCookie = 0;
}


CIPCSocket::~CIPCSocket(void)
{
    m_CriticalSend.Term();
	m_CriticalRecv.Term();
//	CloseHandle(m_hWriteEvent);
//	CloseHandle(m_hReadEvent);
//	CloseHandle(m_hFileMap);
}

DWORD CIPCSocket::IPCSocket()
{
	return 0;
}

DWORD CIPCSocket::IPCSelect(HWND hwnd, unsigned int wMsg, IPC_EVENT lEvent)
{
	return 0;
}

DWORD CIPCSocket::IPCSend(byte* byteBuff, size_t buffSize, DWORD dwFlag)
{
	return 0;
}
DWORD CIPCSocket::IPCRecv(byte* byteBuff, size_t buffSize)
{
	return 0;
}
DWORD CIPCSocket::IPCClose()
{
	return 0;
}

DWORD CIPCSocket::SetHeader(byte* byteHeader, size_t len, const IPCAddress& addrSrc, const IPCAddress& addrDes, const DWORD& dwFlag)
{
	
	if (len < 5 * sizeof(UINT32))
	{
		return 0;
	}
	long offset = 0;
	UINT32 uTemp = 0;
	//SourcePid
	uTemp = (UINT32)addrSrc.dwPid;
	memcpy(byteHeader + offset, &uTemp, sizeof(uTemp));
	offset += sizeof(uTemp);

	//DestPid
	uTemp = (UINT32)addrDes.dwPid;
	memcpy(byteHeader + offset, &uTemp, sizeof(uTemp));
	offset += sizeof(uTemp);

	//sourcePort
	uTemp = (UINT32)addrSrc.dwPort;
	memcpy(byteHeader + offset, &uTemp, sizeof(uTemp));
	offset += sizeof(uTemp);

	//DestPort
	uTemp = (UINT32)addrDes.dwPort;
	memcpy(byteHeader + offset, &uTemp, sizeof(uTemp));
	offset += sizeof(uTemp);

	//FLAG
	uTemp = (UINT32)dwFlag;
	memcpy(byteHeader + offset, &uTemp, sizeof(uTemp));
	offset += sizeof(uTemp);
	
	return offset;
}

DWORD CIPCSocket::GetHeader(IPCAddress& addrSrc, IPCAddress& addrDes, DWORD& dwFlag, byte* byteHeader, size_t len)
{
	if (len < 5 * sizeof(UINT32))
	{
		return 0;
	}
	long offset = 0;
	UINT32 uTemp = 0;
	//SourcePid
	memcpy(&uTemp, byteHeader + offset, sizeof(uTemp));
	addrSrc.dwPid = (UINT32)uTemp;
	offset += sizeof(uTemp);

	//DestPid
	memcpy(&uTemp, byteHeader + offset, sizeof(uTemp));
	addrDes.dwPid = (UINT32)uTemp;
	offset += sizeof(uTemp);

	//sourcePort
	memcpy(&uTemp, byteHeader + offset, sizeof(uTemp));
	addrSrc.dwPort = (UINT32)uTemp;
	offset += sizeof(uTemp);

	//DestPort
	memcpy(&uTemp, byteHeader + offset, sizeof(uTemp));
	addrDes.dwPort = (UINT32)uTemp;
	offset += sizeof(uTemp);

	//flag
	memcpy(&uTemp, byteHeader + offset, sizeof(uTemp));
	dwFlag = (UINT32)uTemp;
	offset += sizeof(uTemp);

	return offset;
}

DWORD CIPCSocket::IPCAttachEvent(IPC_EVENT lEvent, void* pfn, void* userdata)
{
	HWND hwnd = NULL;
	++m_dwEventCookie;
	DWORD dwThreadID = GetCurrentThreadId();
	if (hwnd == NULL)
	{

	}
	IPCCallback callback;
	callback.ipcEvent = lEvent;
	callback.pfn = pfn;
	callback.pSock = this;
	callback.userdata = userdata;
	m_mapMsg.insert(pair<DWORD, IPCCallback>(m_dwEventCookie, callback));
	return m_dwEventCookie;
	// 
	// 	map<IPC_EVENT, unsigned int>::iterator iter = m_mapMsg.begin();
	// 	for (;iter != m_mapMsg.end(); iter++)
	// 	{
	// 		if (lEvent == iter->first)
	// 		{
	// 			iter->second = wMsg;
	// 			break;
	// 		}
	// 	}
	// 	if (iter == m_mapMsg.end())
	// 	{
	// 		m_mapMsg.insert(pair<IPC_EVENT, unsigned int>(lEvent, wMsg));
	// 	}
	return 0;
}


DWORD CIPCSocket::FireMessage(IPC_EVENT event, WPARAM wParam, LPARAM lParam)
{
	
	BOOL b = false;
	map<DWORD, IPCCallback>::iterator iter = m_mapMsg.begin();
	for (;iter != m_mapMsg.end(); iter++)
	{
		if (event == iter->second.ipcEvent)
		{
			SendMessage((event + WM_USER), wParam, lParam);
		}
	}
	return 0;
}

LRESULT CIPCSocket::Fire_OnConnect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	CIPCSocket *pSock = (CIPCSocket*)wParam;
	for (map<DWORD, IPCCallback>::iterator iter = m_mapMsg.begin(); iter != m_mapMsg.end(); iter++)
	{
		if (iter->second.ipcEvent == (WM_IPC_CONNECT - WM_USER))
		{
			ON_IPC_CONNECT pConfn = (ON_IPC_CONNECT)iter->second.pfn;
			pConfn(pSock);
		}
	}
	return 0;
}

LRESULT CIPCSocket::Fire_OnRead(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	for (map<DWORD, IPCCallback>::iterator iter = m_mapMsg.begin(); iter != m_mapMsg.end(); iter++)
	{
		if (iter->second.ipcEvent == (WM_IPC_READ - WM_USER))
		{
			ON_IPC_RECV pConfn = (ON_IPC_RECV)iter->second.pfn;
			void* userdata = iter->second.userdata;
			pConfn(this, userdata);
		}
	}
	return 0;
}

LRESULT CIPCSocket::Fire_OnWrite(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	for (map<DWORD, IPCCallback>::iterator iter = m_mapMsg.begin(); iter != m_mapMsg.end(); iter++)
	{
		if (iter->second.ipcEvent == (WM_IPC_WRITE - WM_USER))
		{
			ON_IPC_SEND pConfn = (ON_IPC_SEND)iter->second.pfn;
			void* userdata = iter->second.userdata;
			pConfn(this, userdata);
		}
	}
	return 0;
}

LRESULT CIPCSocket::Fire_OnOob(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	return 0;
}

LRESULT CIPCSocket::Fire_OnAccept(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	CIPCSocket *pSock = (CIPCSocket*)wParam;
	for (map<DWORD, IPCCallback>::iterator iter = m_mapMsg.begin(); iter != m_mapMsg.end(); iter++)
	{
		if (iter->second.ipcEvent == (WM_IPC_ACCEPT - WM_USER))
		{
			ON_IPC_ACCPET pConfn = (ON_IPC_ACCPET)iter->second.pfn;
			void* userdata = iter->second.userdata;
			pConfn(this, pSock, (IPCAddress*)lParam, userdata);
		}
	}
	return 0;
}

LRESULT CIPCSocket::Fire_OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	for (map<DWORD, IPCCallback>::iterator iter = m_mapMsg.begin(); iter != m_mapMsg.end(); iter++)
	{
		if (iter->second.ipcEvent == (WM_IPC_CLOSE - WM_USER))
		{
			ON_IPC_CLOSE pConfn = (ON_IPC_CLOSE)iter->second.pfn;
			void* userdata = iter->second.userdata;
			pConfn(this, userdata, wParam);
		}
	}
	return 0;
}

LRESULT CIPCSocket::Fire_OnBreak(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	for (map<DWORD, IPCCallback>::iterator iter = m_mapMsg.begin(); iter != m_mapMsg.end(); iter++)
	{
		if (iter->second.ipcEvent == (WM_IPC_BREAK - WM_USER))
		{
			ON_IPC_BREAK pConfn = (ON_IPC_BREAK)iter->second.pfn;
			void* userdata = iter->second.userdata;
			pConfn(this, userdata, wParam);
		}
	}
	return 0;
}

LRESULT CIPCSocket::OnDelete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	DestroyWindow();
	return 0;
}

byte* CIPCSocket::GetWriteFileMap()
{
	
	TCHAR ch[255];
	_stprintf_s(ch, _T("IPC_MEM_%d_%d"), m_addrDes.dwPid, m_addrDes.dwPort);

	HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, ch);
	if (NULL == hFileMapping)
	{
		assert("");
		return NULL;
	}
	byte* pBuf = (byte*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (NULL == pBuf)
	{
		CloseHandle(hFileMapping);
		hFileMapping = NULL;
		return NULL;
	}
	return pBuf;
}
byte* CIPCSocket::GetReadFileMap()
{
	
	byte* pBuf = (byte*)MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (NULL == pBuf)
	{
		CloseHandle(m_hFileMap);
		m_hFileMap = NULL;
		return NULL;
	}
	return pBuf;
}

DWORD CIPCSocket::WriteBuffer(byte* byteBuff, size_t size, const IPCAddress& addrDes)
{
	//Ð´Êý¾Ý
	TCHAR ch[255];
	_stprintf_s(ch, _T("IPC_MEM_%d_%d"), addrDes.dwPid, addrDes.dwPort);

	HANDLE hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, ch);
	if (NULL == hFileMapping)
	{
		assert("");
		return 2;
	}
	byte* pBuf = (byte*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (NULL == pBuf)
	{
		CloseHandle(hFileMapping);
		hFileMapping = NULL;
		return 3;
	}
	memcpy(pBuf, byteBuff, size);
	return 0;
}

DWORD CIPCSocket::ReadBuffer(byte* byteBuff, size_t size)
{
	if (size < IPC_MEM_SIZE)
	{
		return 1;
	}
	byte* pBuf = (byte*)MapViewOfFile(m_hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (NULL == pBuf)
	{
		CloseHandle(m_hFileMap);
		m_hFileMap = NULL;
		return 1;
	}
	memcpy(byteBuff, pBuf, IPC_MEM_SIZE);
	return 0;
}

unsigned int CIPCSocket::ThreadProc()
{
	return 0;
}