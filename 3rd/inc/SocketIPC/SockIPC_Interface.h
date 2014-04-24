#pragma once




typedef void* XAF_IPC_CONNECTION_HANDLE;
typedef void* XAF_IPC_LISTENER_HANDLE;

enum IPC_EVENT
{
	IPC_READ = 1,
	IPC_WRITE,
	IPC_OOB,
	IPC_ACCEPT,
	IPC_CONNECT,
	IPC_CLOSE,
	IPC_BREAK
};

struct IPCAddress
{
	DWORD dwPid;
	DWORD dwPort;
};


typedef DWORD (*ON_IPC_ACCPET)(XAF_IPC_LISTENER_HANDLE pLis, XAF_IPC_CONNECTION_HANDLE sockConn, IPCAddress* pAddr, void* data);
typedef DWORD (*ON_IPC_SEND)(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data);
typedef DWORD (*ON_IPC_RECV)(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data);
typedef DWORD (*ON_IPC_CLOSE)(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data, long resultCode);
typedef DWORD (*ON_IPC_BREAK)(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data, long resultCode);










//��ʼ��
DWORD __stdcall XAF_IPCStartup(DWORD dwParam);

//����һ��connect
XAF_IPC_CONNECTION_HANDLE __stdcall XAF_IPCConnection();
long __stdcall XAF_IPCConnectionAddRef(XAF_IPC_CONNECTION_HANDLE hConn);
long __stdcall XAF_IPCConnectionRelease(XAF_IPC_CONNECTION_HANDLE hConn);

//����һ��connect�������ӵ�һ����ַ
DWORD __stdcall XAF_IPCConnect(XAF_IPC_CONNECTION_HANDLE conn, IPCAddress& addr);

//����һ��listener
XAF_IPC_LISTENER_HANDLE __stdcall XAF_IPCListener();

//listener���Լ��˿�
DWORD __stdcall XAF_IPCBind(XAF_IPC_LISTENER_HANDLE serverIPC, IPCAddress& addr);

//listener��ʼ����
DWORD __stdcall XAF_IPCListen(XAF_IPC_LISTENER_HANDLE serverIPC, DWORD dwMaxNum);
long XAF_IPCListenerAddRef(XAF_IPC_LISTENER_HANDLE hListener);
long XAF_IPCListenerRelease(XAF_IPC_LISTENER_HANDLE hListener);

//����һ��conn���󲢴���һ����conn��������ͨ��
DWORD __stdcall XAF_IPCAccept(XAF_IPC_LISTENER_HANDLE serverIPC, XAF_IPC_CONNECTION_HANDLE* pConn, IPCAddress &addr);

//��������
DWORD __stdcall XAF_IPCSend(XAF_IPC_CONNECTION_HANDLE sockIPC, byte* byteBuff, size_t buffSize);

//��������
DWORD __stdcall XAF_IPCRecv(XAF_IPC_CONNECTION_HANDLE sockIPC, byte* byteBuff, size_t buffSize);

//�ر�
DWORD __stdcall XAF_IPCCloseListener(XAF_IPC_LISTENER_HANDLE sockIPC);
DWORD __stdcall XAF_IPCCloseConnection(XAF_IPC_CONNECTION_HANDLE sockIPC);

DWORD __stdcall XAF_IPCAttachListenerEvent(XAF_IPC_LISTENER_HANDLE sockIPC, IPC_EVENT lEvent, void* pfn, void* userdata);
DWORD __stdcall XAF_IPCAttachConnectionEvent(XAF_IPC_CONNECTION_HANDLE sockIPC, IPC_EVENT lEvent, void* pfn, void* userdata);

//�������
DWORD __stdcall XAF_IPCClearup();
