#include "StdAfx.h"
#include "IPCConnection.h"
#include <time.h>
#include "IPCEnv.h"

CIPCConnection::CIPCConnection(void)
{
	Create(::GetDesktopWindow());
	m_dwStatus = ENUM_IPC_CONN;
	TCHAR ch[255];
	HANDLE hFileMapping = NULL;
	do 
	{
		srand((unsigned)time(NULL));
		m_addrSrc.dwPort = (DWORD)(((double)rand() / (double)RAND_MAX) * 65535 + 1);
		_stprintf_s(ch, _T("IPC_MEM_%d_%d"), m_addrSrc.dwPid, m_addrSrc.dwPort);
		hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, ch);
	} while(hFileMapping != NULL);

	_stprintf_s(ch, _T("IPC_MEM_%d_%d"), m_addrSrc.dwPid, m_addrSrc.dwPort);
	m_hFileMap = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, IPC_MEM_SIZE, ch);
}

CIPCConnection::~CIPCConnection(void)
{
	CloseHandle(m_hBusyEvent);
	CloseHandle(m_hDesBusyEvent);
	CloseHandle(m_hDesReadEvent);
}


DWORD CIPCConnection::IPCConnect(IPCAddress& addr)
{
	
	m_addrDes = addr;


	HANDLE hProcess = OpenProcess(STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0xFFF, FALSE, addr.dwPid); 
	if (hProcess == NULL)
	{
		assert(_T("IPCConnect:hProcess == NULL"));
	}
	CloseHandle(hProcess);

	TCHAR ch[255];
	_stprintf_s(ch, _T("IPC_BUSY_EVENT_%d_%d"), m_addrSrc.dwPid, m_addrSrc.dwPort);
	m_hBusyEvent = CreateEvent(NULL, FALSE, TRUE, ch);

	_stprintf_s(ch, _T("IPC_READ_EVENT_%d_%d"), m_addrSrc.dwPid, m_addrSrc.dwPort);
	m_hReadEvent = CreateEvent(NULL, FALSE, FALSE, ch);

	_stprintf_s(ch, _T("IPC_WRITE_EVENT_%d_%d"), m_addrSrc.dwPid, m_addrSrc.dwPort);
	m_hWriteEvent = CreateEvent(NULL, FALSE, TRUE, ch);

	_stprintf_s(ch, _T("IPC_BUSY_EVENT_%d_%d"), m_addrDes.dwPid, m_addrDes.dwPort);
	m_hDesBusyEvent = OpenEvent(EVENT_ALL_ACCESS, false, ch);
	if (m_hDesBusyEvent == NULL)
	{
		return 2;
	}

	_stprintf_s(ch, _T("IPC_READ_EVENT_%d_%d"), m_addrDes.dwPid, m_addrDes.dwPort);
	m_hDesReadEvent = OpenEvent(EVENT_ALL_ACCESS, false, ch);
	if (m_hDesReadEvent == NULL)
	{
		return 3;
	}

	CIPCEnv::Instance()->AttachSocketEvent(this);

	return 0;
}

//由listener创建的connection，对源connection进行首次连接
DWORD CIPCConnection::IPCAccept(IPCAddress& addr)
{
	m_addrDes = addr;

	HANDLE hProcess = OpenProcess(STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0xFFF, FALSE, addr.dwPid); 
	if (hProcess == NULL)
	{
		assert(_T("IPCAccept:hProcess == NULL"));
	}
	CloseHandle(hProcess);

	TCHAR ch[255];

	_stprintf_s(ch, _T("IPC_BUSY_EVENT_%d_%d"), m_addrSrc.dwPid, m_addrSrc.dwPort);
	m_hBusyEvent = CreateEvent(NULL, FALSE, TRUE, ch);

	_stprintf_s(ch, _T("IPC_READ_EVENT_%d_%d"), m_addrSrc.dwPid, m_addrSrc.dwPort);
	m_hReadEvent = CreateEvent(NULL, FALSE, FALSE, ch);

	_stprintf_s(ch, _T("IPC_WRITE_EVENT_%d_%d"), m_addrSrc.dwPid, m_addrSrc.dwPort);
	m_hWriteEvent = CreateEvent(NULL, FALSE, FALSE, ch);

	_stprintf_s(ch, _T("IPC_BUSY_EVENT_%d_%d"), m_addrDes.dwPid, m_addrDes.dwPort);
	m_hDesBusyEvent = OpenEvent(EVENT_ALL_ACCESS, false, ch);
	if (m_hDesBusyEvent == NULL)
	{
		return 1;
	}

	_stprintf_s(ch, _T("IPC_READ_EVENT_%d_%d"), m_addrDes.dwPid, m_addrDes.dwPort);
	m_hDesReadEvent = OpenEvent(EVENT_ALL_ACCESS, false, ch);
	if (m_hDesReadEvent == NULL)
	{
		return 1;
	}

	//直接写数据
	byte* buf = GetWriteFileMap();
	if (NULL == buf)
	{
		return 0;
	}
	size_t offset = SetHeader(buf, IPC_MEM_SIZE, m_addrSrc, m_addrDes, ENUM_IPC_ACCP);

	//WriteBuffer(buf, IPC_MEM_SIZE, addr);

	//delete []buf;
	//buf = NULL;

	m_dwStatus = ENUM_IPC_SEND;
	CIPCEnv::Instance()->AttachSocketEvent(this);

	//通知对方读
	SetEvent(m_hDesReadEvent);
	ResetEvent(m_hDesBusyEvent);
	FireMessage(IPC_WRITE);

	return 0;
}
DWORD CIPCConnection::IPCSend(byte* byteBuff, size_t buffSize, DWORD dwFlag)
{
	if (m_queueSendCmd.size() > 10)
	{
		return 1;
	}
	if (byteBuff == NULL || buffSize <= 0 || buffSize > IPC_MEM_SIZE)
	{
		return 2;
	}
	byte* buf = new byte[IPC_MEM_SIZE];
	memcpy(buf, byteBuff, buffSize);

	IPCCmd cmd;
	cmd.dwSrcPid = m_addrSrc.dwPid;
	cmd.dwSrcPort = m_addrSrc.dwPort;
	cmd.dwDesPid = m_addrDes.dwPid;
	cmd.dwDesPort = m_addrDes.dwPort;		
	cmd.dwFlag = dwFlag;
	cmd.byteBuffer = buf;
	cmd.tBufferSize = buffSize;

	m_CriticalSend.Lock();
	m_queueSendCmd.push(cmd);
    m_CriticalSend.Unlock();
	//等待目的内存锁
	//	TCHAR ch[255];
	//	_stprintf_s(ch, _T("IPC_WRITE_EVENT_%d_%d"), m_addrDes.dwPid, m_addrDes.dwPort);
	//	m_hWriteEvent = OpenEvent(EVENT_ALL_ACCESS, false, ch);
	SetEvent(m_hWriteEvent);

	return 0;
}
DWORD CIPCConnection::IPCRecv(byte* byteBuff, size_t buffSize)
{
	if (m_queueRecvCmd.empty())
	{
		return 1;
	}

	m_CriticalRecv.Lock();
	IPCCmd cmd = m_queueRecvCmd.front();
	m_queueRecvCmd.pop();
	m_CriticalRecv.Unlock();

	if (buffSize < cmd.tBufferSize)
	{
		return 1;
	}
	m_addrDes.dwPid = cmd.dwDesPid;
	m_addrDes.dwPort = cmd.dwDesPort;
	memcpy(byteBuff, cmd.byteBuffer + 6 * sizeof(UINT32), cmd.tBufferSize);
	delete []cmd.byteBuffer;
	ResetEvent(m_hReadEvent);
	SetEvent(m_hBusyEvent);
	return 0;
}

DWORD CIPCConnection::IPCClose()
{
	SetEvent(m_hDesBusyEvent);
	if (m_dwStatus != ENUM_IPC_RECVCLOSE)
	{
		m_dwStatus = ENUM_IPC_CLOSE;
	}
	//直接写数据
	byte* buf = GetWriteFileMap();
	if (NULL == buf)
	{
		return 0;
	}
	size_t offset = SetHeader(buf, IPC_MEM_SIZE, m_addrSrc, m_addrDes, m_dwStatus);

//	WriteBuffer(buf, IPC_MEM_SIZE, m_addrDes);

//	delete []buf;
//	buf = NULL;
	//通知对方读
	SetEvent(m_hDesReadEvent);
	ResetEvent(m_hDesBusyEvent);
	return 0;
}

DWORD CIPCConnection:: OnConnection()
{
	return 0;
}
DWORD CIPCConnection::OnSend()
{
	if (m_dwStatus == ENUM_IPC_CONN || m_dwStatus == ENUM_IPC_PING)
	{
		//先等对方空闲
		WaitForSingleObject(m_hDesBusyEvent, INFINITE);
		//锁目的可写数据，让别人不进来写,write已有锁
		//ResetEvent(m_hDesBusyEvent);

		byte *byteConn = GetWriteFileMap();
		if (NULL == byteConn)
		{
			return 0;
		}
		size_t headlen = SetHeader(byteConn, IPC_MEM_SIZE, m_addrSrc, m_addrDes, m_dwStatus);

		//WriteBuffer(byteConn, headlen, m_addrDes);

	//	delete []byteConn;
	//	byteConn = NULL;

		ResetEvent(m_hWriteEvent);
		SetEvent(m_hDesReadEvent);
		if (m_dwStatus == ENUM_IPC_PING)
		{
			m_dwStatus = ENUM_IPC_SEND;
		}
	}
	else if (m_dwStatus == ENUM_IPC_SEND)
	{
		//等待目的可写
		WaitForSingleObject(m_hDesBusyEvent, INFINITE);
		if (m_queueSendCmd.empty())
		{
			ResetEvent(m_hWriteEvent);
			SetEvent(m_hDesBusyEvent);
			return 0;
		}

		//锁目的可写数据，让别人不进来写,write已有锁
		//ResetEvent(m_hDesBusyEvent);

        m_CriticalSend.Lock();
		IPCCmd cmd = m_queueSendCmd.front();
		m_queueSendCmd.pop();
		m_CriticalSend.Unlock();

		byte *byteConn = GetWriteFileMap();
		if (NULL == byteConn)
		{
			delete []cmd.byteBuffer;
			cmd.byteBuffer = NULL;
			return 0;
		}
		IPCAddress addr;
		addr.dwPid = cmd.dwDesPid;
		addr.dwPort = cmd.dwDesPort;

		size_t offset = SetHeader(byteConn, IPC_MEM_SIZE, m_addrSrc, addr, cmd.dwFlag);

		memcpy(byteConn + offset, &cmd.tBufferSize, sizeof(UINT32));
		offset += sizeof(UINT32);

		memcpy(byteConn + offset, cmd.byteBuffer, cmd.tBufferSize);

		//WriteBuffer(byteConn, offset+cmd.tBufferSize, addr);


		delete []cmd.byteBuffer;
		cmd.byteBuffer = NULL;

	//	delete byteConn;

		SetEvent(m_hWriteEvent);
		SetEvent(m_hDesReadEvent);
	}
	return 0;
}
DWORD CIPCConnection::OnRecv()
{
	byte *byteConn = new byte[IPC_MEM_SIZE];
	IPCAddress addrSrc;
	IPCAddress addrDes;
	DWORD dwFlag = 0;
	ReadBuffer(byteConn, IPC_MEM_SIZE);
	size_t offset = GetHeader(addrSrc, addrDes, dwFlag, byteConn, IPC_MEM_SIZE);
	m_dwTickCount = GetTickCount();
	//102为accp消息，可以开始发送数据了
	if (dwFlag == ENUM_IPC_ACCP)
	{
		m_addrDes = addrSrc;

		TCHAR ch[255];
		_stprintf_s(ch, _T("IPC_BUSY_EVENT_%d_%d"), addrSrc.dwPid, addrSrc.dwPort);
		CloseHandle(m_hDesBusyEvent);
		m_hDesBusyEvent = OpenEvent(EVENT_ALL_ACCESS, false, ch);

		_stprintf_s(ch, _T("IPC_READ_EVENT_%d_%d"), addrSrc.dwPid, addrSrc.dwPort);
		CloseHandle(m_hDesReadEvent);
		m_hDesReadEvent = OpenEvent(EVENT_ALL_ACCESS, false, ch);

		_stprintf_s(ch, _T("IPC_CLOSE_EVENT_%d_%d"), addrSrc.dwPid, addrSrc.dwPort);
		//	m_hCloseTimer = CreateWaitableTimer(NULL, TRUE, ch);
		////	LARGE_INTEGER liDueTime;
		//	liDueTime.QuadPart = -100000000LL;
		//	SetWaitableTimer(m_hCloseTimer, &liDueTime, 0, NULL, NULL, 0);

		//printlog("SetEvent(m_hWriteEvent);", "d:\\log\\log.txt");
		//SetEvent(m_hWriteEvent);

		m_dwStatus = ENUM_IPC_SEND;
		FireMessage(IPC_WRITE);
	}
	//103为用户自定义消息，接收用户信息
	else if (dwFlag == ENUM_IPC_SEND)
	{
		IPCCmd cmd;
		UINT32 leng = 0;
		memcpy(&leng, byteConn+offset, sizeof(UINT32));
		offset += sizeof(UINT32);

		cmd.dwSrcPid = m_addrSrc.dwPid;
		cmd.dwSrcPort = m_addrSrc.dwPort;
		cmd.dwDesPid = m_addrDes.dwPid;
		cmd.dwDesPort = m_addrDes.dwPort;
		cmd.dwFlag = dwFlag;
		cmd.byteBuffer = byteConn;
		cmd.tBufferSize = leng;

		m_CriticalRecv.Lock();
		m_queueRecvCmd.push(cmd);
		m_CriticalRecv.Unlock();

		FireMessage(IPC_READ);
		//ResetEvent(m_hReadEvent);
		//SetEvent(m_hBusyEvent);
		return 0;
	}
	else if (dwFlag == ENUM_IPC_PING)
	{
		m_dwStatus = ENUM_IPC_RECVPING;
		SetEvent(m_hWriteEvent);
	}
	else if (dwFlag == ENUM_IPC_CLOSE)
	{
		delete []byteConn;
		byteConn = NULL;
		m_dwStatus = ENUM_IPC_RECVCLOSE;
		CIPCEnv::Instance()->DetachSocketEvent(this);
		IPCClose();
		FireMessage(IPC_CLOSE);
		SendMessage(WM_IPC_DELETE);
		return 0;
	}
	else if (dwFlag == ENUM_IPC_RECVCLOSE)
	{
		delete []byteConn;
		byteConn = NULL;
		m_dwStatus = ENUM_IPC_RECVCLOSE;
		CIPCEnv::Instance()->DetachSocketEvent(this);
		FireMessage(IPC_CLOSE);
		SendMessage(WM_IPC_DELETE);
		return 0;
	}
	delete []byteConn;
	byteConn = NULL;
	ResetEvent(m_hReadEvent);
	SetEvent(m_hBusyEvent);
	return 0;
}
DWORD CIPCConnection::OnTimer()
{
	DWORD dwTickCount = GetTickCount();
	if (m_dwStatus == ENUM_IPC_SEND)
	{
		if (dwTickCount - m_dwTickCount <= IPC_PING_TIMER)
		{

		}
		else if (dwTickCount - m_dwTickCount <= 2 * IPC_PING_TIMER)
		{
			m_dwStatus = ENUM_IPC_PING;
			SetEvent(m_hWriteEvent);
		}
		else if (dwTickCount - m_dwTickCount >= 2 * IPC_PING_TIMER)
		{
			m_dwStatus = ENUM_IPC_RECVCLOSE;
			CIPCEnv::Instance()->DetachSocketEvent(this);
			FireMessage(IPC_BREAK);
			SendMessage(WM_IPC_DELETE);
	//		SendMessage(WM_NCDESTROY);
		}
	}
	return 0;
}