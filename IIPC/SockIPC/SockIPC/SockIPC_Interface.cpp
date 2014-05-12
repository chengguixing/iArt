#include "stdafx.h"
#include "SockIPC_Interface.h"
#include "IPCConnection.h"
#include "IPCListener.h"
#include "IPCEnv.h"

//³õÊ¼»¯
DWORD __stdcall XAF_IPCStartup(DWORD dwParam)
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	return CIPCEnv::Instance()->IPCStartup(dwParam);
}

XAF_IPC_CONNECTION_HANDLE __stdcall XAF_IPCConnection()
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	CIPCConnection* pConn = new CIPCConnection;
	return (XAF_IPC_CONNECTION_HANDLE)pConn;
}

DWORD __stdcall XAF_IPCConnect(XAF_IPC_CONNECTION_HANDLE conn, IPCAddress& addr)
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	CIPCConnection* pConn = (CIPCConnection*)conn;
	return pConn->IPCConnect(addr);
}

XAF_IPC_LISTENER_HANDLE __stdcall XAF_IPCListener()
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	CIPCListener* pListener = new CIPCListener;
	return (XAF_IPC_LISTENER_HANDLE)pListener;
}

DWORD __stdcall XAF_IPCBind(XAF_IPC_LISTENER_HANDLE serverIPC, IPCAddress& IPCadd)
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	CIPCListener* pIPC = (CIPCListener*)serverIPC;
	return pIPC->IPCBind(IPCadd);
}

DWORD __stdcall XAF_IPCListen(XAF_IPC_LISTENER_HANDLE serverIPC, DWORD dwQueueNum)
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	CIPCListener* pIPC = (CIPCListener*)serverIPC;
	return pIPC->IPCListen(dwQueueNum);
}

DWORD __stdcall XAF_IPCAccept(XAF_IPC_LISTENER_HANDLE serverIPC, XAF_IPC_CONNECTION_HANDLE* pConn, IPCAddress& addr)
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	CIPCListener* pListener = (CIPCListener*)serverIPC;
	return pListener->IPCAccept((CIPCConnection**)pConn, addr);
}

DWORD __stdcall XAF_IPCSend(XAF_IPC_CONNECTION_HANDLE sockIPC, byte* byteBuff, size_t buffSize)
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	CIPCSocket* pIPC = (CIPCSocket*)sockIPC;
	return pIPC->IPCSend(byteBuff, buffSize, ENUM_IPC_SEND);
}

DWORD __stdcall XAF_IPCRecv(XAF_IPC_CONNECTION_HANDLE sockIPC, byte* byteBuff, size_t buffSize)
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	CIPCSocket* pIPC = (CIPCSocket*)sockIPC;
	return pIPC->IPCRecv(byteBuff, buffSize);
}


DWORD __stdcall XAF_IPCCloseListener(XAF_IPC_LISTENER_HANDLE sockIPC)
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	CIPCSocket* pIPC = (CIPCSocket*)sockIPC;
	return pIPC->IPCClose();
}

DWORD __stdcall XAF_IPCCloseConnection(XAF_IPC_LISTENER_HANDLE sockIPC)
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	CIPCSocket* pIPC = (CIPCSocket*)sockIPC;
	return pIPC->IPCClose();
}
DWORD __stdcall XAF_IPCClearup()
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	return CIPCEnv::Instance()->IPCClearup();
}

DWORD __stdcall XAF_IPCSelect(XAF_IPC_CONNECTION_HANDLE sockIPC, HWND hwnd, unsigned int wMsg, IPC_EVENT lEvent)
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	CIPCSocket* pIPC = (CIPCSocket*)sockIPC;
	return pIPC->IPCSelect(hwnd, wMsg, lEvent);
}

DWORD __stdcall XAF_IPCAttachListenerEvent(XAF_IPC_LISTENER_HANDLE sockIPC, IPC_EVENT lEvent, void* pfn, void* userdata)
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	CIPCSocket* pIPC = (CIPCSocket*)sockIPC;
	return pIPC->IPCAttachEvent(lEvent, pfn, userdata);
}

DWORD __stdcall XAF_IPCAttachConnectionEvent(XAF_IPC_CONNECTION_HANDLE sockIPC, IPC_EVENT lEvent, void* pfn, void* userdata)
{
	TSDEBUG(L"Enter %s : %s", __FILEW__, __FUNCTIONW__);
	CIPCSocket* pIPC = (CIPCSocket*)sockIPC;
	return pIPC->IPCAttachEvent(lEvent, pfn, userdata);
}