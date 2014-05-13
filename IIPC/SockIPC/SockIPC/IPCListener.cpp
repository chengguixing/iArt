#include "StdAfx.h"
#include "IPCListener.h"
#include "IPCConnection.h"
#include "IPCEnv.h"

CIPCListener::CIPCListener(void)
{
	Create(NULL);
	m_dwMaxConnNum = 0;
	m_dwConnNum = 0;
}

CIPCListener::~CIPCListener(void)
{
	CloseHandle(m_hBusyEvent);
}

DWORD CIPCListener::IPCBind(IPCAddress& addr)
{
	
	m_addrSrc.dwPort = addr.dwPort;
	TSDEBUG(L"%s %ld %ld", __FUNCTIONW__, m_addrSrc.dwPid, m_addrSrc.dwPort);
	TCHAR ch[255];
	_stprintf_s(ch, _T("IPC_MEM_%d_%d"), m_addrSrc.dwPid, m_addrSrc.dwPort);
	HANDLE hHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, ch);
	if (hHandle != NULL)
	{
		return 1;
	}
	TSDEBUG(L"%s %s", __FUNCTIONW__, ch);
	m_hFileMap = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, IPC_MEM_SIZE, ch);

	return 0;
}
DWORD CIPCListener::IPCListen(DWORD dwMaxConn)
{
	
	m_dwMaxConnNum = dwMaxConn;
	TCHAR ch[255];
	_stprintf_s(ch, _T("IPC_WRITE_EVENT_%d_%d"), m_addrSrc.dwPid, m_addrSrc.dwPort);
	m_hWriteEvent = CreateEvent(NULL, FALSE, FALSE, ch);
	TSDEBUG(L"%s %s", __FUNCTIONW__, ch);

	_stprintf_s(ch, _T("IPC_READ_EVENT_%d_%d"), m_addrSrc.dwPid, m_addrSrc.dwPort);
	m_hReadEvent = CreateEvent(NULL, FALSE, FALSE, ch);
	TSDEBUG(L"%s %s", __FUNCTIONW__, ch);

	_stprintf_s(ch, _T("IPC_BUSY_EVENT_%d_%d"), m_addrSrc.dwPid, m_addrSrc.dwPort);
	m_hBusyEvent = CreateEvent(NULL, FALSE, TRUE, ch);
	TSDEBUG(L"%s %s", __FUNCTIONW__, ch);
	
	CIPCEnv::Instance()->AttachSocketEvent(this);

	return 0;
}
DWORD CIPCListener::IPCAccept(CIPCConnection** pConn, IPCAddress &addr)
{
	
	if (m_dwConnNum >= m_dwMaxConnNum)
	{
		*pConn = NULL;
		return 1;
	}
	++m_dwConnNum;
	CIPCConnection* pNewConn = new CIPCConnection;
	*pConn = pNewConn;
	IPCAddress addrClient = m_vecAddrClient.front();
	m_vecAddrClient.erase(m_vecAddrClient.begin());
	addr = addrClient;
	(*pConn)->SetDesAddr(addr);
	IPCAddress addrSrc;
	(*pConn)->GetSrcAddr(addrSrc);
	m_vecAddrConn.push_back(addrSrc);
	(*pConn)->SetMsgMap(m_mapMsg);
	(*pConn)->SetParent(this);
	(*pConn)->IPCAccept(addr);
	//printf_s("new conn\n");

	
	/*监听目的是否可写，把两个addr放入命令队列，通知客户端更改port

	byte* buf = new byte[IPC_MEM_SIZE];
	size_t offset = SetHeader(buf, IPC_MEM_SIZE, addrSrc, addr, 102);

	WriteBuffer(buf, IPC_MEM_SIZE, addr);

	delete []buf;
	buf = NULL;
*/
	return 0;
}

DWORD CIPCListener::IPCSend(byte* byteBuff, size_t buffSize, DWORD dwFlag)
{
	
	return 0;
}
DWORD CIPCListener::IPCRecv(byte* byteBuff, size_t buffSize)
{
	
	return 0;
}

DWORD CIPCListener::IPCClose()
{
	
	CIPCEnv::Instance()->DetachSocketEvent(this);
	SendMessage(WM_IPC_DELETE);
	return 0;
}

DWORD CIPCListener::OnSend()
{
	
	return 0;
}

DWORD CIPCListener::OnRecv()
{
	
	//读取消息，放入队列
	byte* buf = new byte[IPC_MEM_SIZE];

	ReadBuffer(buf, IPC_MEM_SIZE);

	SetEvent(m_hBusyEvent);
	ResetEvent(m_hReadEvent);

	IPCAddress addrSrc;
	IPCAddress addrDes;
	DWORD dwFlag = 0;
	size_t offset = GetHeader(addrSrc, addrDes, dwFlag, buf, IPC_MEM_SIZE);
	m_vecAddrClient.push_back(addrSrc);

	delete []buf;
	buf = NULL;

	CIPCConnection *pConn;
	IPCAddress addr;
	IPCAccept(&pConn, addr);
	TSDEBUG(L"%s %ld %ld", __FUNCTIONW__, addr.dwPid, addr.dwPort);
	if (pConn == NULL)
	{
		return 1;
	}

	//fire_accept事件addrDes
	FireMessage(IPC_ACCEPT, (WPARAM)pConn, (LPARAM)(&pConn->m_addrDes));
	return 0;
}

DWORD CIPCListener::OnTimer()
{
	
	return 0;
}