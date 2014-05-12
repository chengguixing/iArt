#pragma once
#include "SockIPC_Interface.h" 
#include <queue>
#include <queue>
#include <map>
#include <vector>
#include <assert.h>
using namespace std;

#define	IPC_ONE_SIZE (1024)
#define IPC_MEM_SIZE (500 * 1024)
#define IPC_PING_TIMER	5000

#define WM_IPC_READ		(WM_USER + 1)
#define WM_IPC_WRITE	(WM_USER + 2)	
#define WM_IPC_OOB		(WM_USER + 3)	
#define WM_IPC_ACCEPT	(WM_USER + 4)	
#define WM_IPC_CONNECT	(WM_USER + 5)	
#define WM_IPC_CLOSE	(WM_USER + 6)
#define WM_IPC_BREAK	(WM_USER + 7)
#define WM_IPC_DELETE	(WM_USER + 8)

class CIPCSocket;
class CIPCConnection;
class CIPCListener;

enum IPC_STATUS
{
	ENUM_IPC_CONN = 101,
	ENUM_IPC_ACCP,
	ENUM_IPC_SEND,
	ENUM_IPC_RECV,
	ENUM_IPC_PING,
	ENUM_IPC_RECVPING,
	ENUM_IPC_CLOSE,
	ENUM_IPC_RECVCLOSE
};

struct IPCCallback
{
	IPC_EVENT ipcEvent;
	CIPCSocket* pSock;
	void* pfn;
	void* userdata;
};

struct IPCCmd
{
	DWORD	dwSrcPid;
	DWORD	dwSrcPort;
	DWORD	dwDesPid;
	DWORD	dwDesPort;
	DWORD	dwFlag;
	byte*	byteBuffer;
	size_t	tBufferSize;
};

class CIPCSocket : public CWindowImpl<CIPCSocket, CWindow, CWinTraits<WS_DISABLED> >
{
public:
	CIPCSocket(void);
	~CIPCSocket(void);

	BEGIN_MSG_MAP(CIPCSocket)
		MESSAGE_HANDLER(WM_IPC_CONNECT, Fire_OnConnect)
		MESSAGE_HANDLER(WM_IPC_READ, Fire_OnRead)
		MESSAGE_HANDLER(WM_IPC_WRITE, Fire_OnWrite)
		MESSAGE_HANDLER(WM_IPC_OOB, Fire_OnOob)
		MESSAGE_HANDLER(WM_IPC_ACCEPT, Fire_OnAccept)
		MESSAGE_HANDLER(WM_IPC_CLOSE, Fire_OnClose)
		MESSAGE_HANDLER(WM_IPC_BREAK, Fire_OnBreak)
		MESSAGE_HANDLER(WM_IPC_DELETE, OnDelete)
	END_MSG_MAP()

	DWORD	IPCSocket();
	virtual DWORD IPCSend(byte* byteBuff, size_t buffSize, DWORD dwFlag) = 0;
	virtual DWORD IPCRecv(byte* byteBuff, size_t buffSize) = 0;
	virtual DWORD IPCClose() = 0;
	virtual	DWORD OnSend() = 0;
	virtual DWORD OnRecv() = 0;
	virtual DWORD OnTimer() = 0;
	DWORD	IPCSelect(HWND hwnd, unsigned int wMsg, IPC_EVENT lEvent);
	DWORD	IPCAttachEvent(IPC_EVENT lEvent, void* pfn, void* userdata);
	unsigned int ThreadProc();

	LRESULT Fire_OnConnect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT Fire_OnRead(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT Fire_OnWrite(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT Fire_OnOob(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT Fire_OnAccept(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT Fire_OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT Fire_OnBreak(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDelete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	//向目标写入数据
	DWORD WriteBuffer(byte* byteBuff, size_t size, const IPCAddress& addrDes);
	//读本身数据
	DWORD ReadBuffer(byte* byteBuff, size_t size);

	//向buff数据写入head
	DWORD SetHeader(byte* byteHeader, size_t len, const IPCAddress& addrSrc, const IPCAddress& addrDes, const DWORD& dwFlag);
	//从buff数据读出head
	DWORD GetHeader(IPCAddress& addrSrc, IPCAddress& addrDes, DWORD& dwFlag, byte* byteHeader, size_t len);


	//获取目的内存指针
	byte* GetWriteFileMap();
	byte* GetReadFileMap();
	DWORD FireMessage(IPC_EVENT msg, WPARAM wParam = 0, LPARAM lParam = 0);
public:

	DWORD m_dwThreadID;

	IPCAddress	m_addrSrc;
	IPCAddress	m_addrDes;

	HANDLE	m_hWriteEvent;
	HANDLE	m_hReadEvent;

	vector<HANDLE*>	m_vecEvents;

	CIPCSocket* m_hParent;

	HANDLE	m_hFileMap;

	CComCriticalSection m_CriticalSend;
    CComCriticalSection m_CriticalRecv;
	std::queue<IPCCmd> m_queueSendCmd;
	std::queue<IPCCmd> m_queueRecvCmd;

	std::map<DWORD, IPCCallback> m_mapMsg;
	DWORD m_dwEventCookie;
};