
#include "stdafx.h"
#include "SocketIPC.h"
#include "IPC.h"
#include "CommonTool.h"


SIPC_INSTANCE SIPC_CreateIpcServer()
{
	TSDEBUG(L"Enter");

	IPCInterface* lpIPCServer = new IPC;

	return lpIPCServer;
}

long SIPC_InitIpcServer(SIPC_INSTANCE hSIpcInstance)
{
	TSDEBUG(L"Enter");

	IPCInterface* lpIPCInterface = (IPCInterface*)hSIpcInstance;
	return lpIPCInterface->Init(IPCInterface::IpcType_Server);
}

SIPC_INSTANCE SIPC_CreateIpcClient()
{
	TSDEBUG(L"Enter");

	IPCInterface* lpIPCClient = new IPC;

	return lpIPCClient;
}

long SIPC_InitIpcClient(SIPC_INSTANCE hSIpcInstance)
{
	TSDEBUG(L"Enter");

	IPCInterface* lpIPCInterface = (IPCInterface*)hSIpcInstance;
	return lpIPCInterface->Init(IPCInterface::IpcType_Client);
}

long SIPC_ListenClient(SIPC_INSTANCE hSIpcInstance, LISTENCALLBACK pfnListenCallback, void* pUserData)
{
	TSDEBUG(L"Enter");

	IPCInterface* lpIPCInterface = (IPCInterface*)hSIpcInstance;
	return lpIPCInterface->Link(pfnListenCallback, pUserData);
}

long SIPC_ConnectServer(SIPC_INSTANCE hSIpcInstance, CONNECTCALLBACK pfnConnectCallback, void* pUserData)
{
	TSDEBUG(L"Enter");

	IPCInterface* lpIPCInterface = (IPCInterface*)hSIpcInstance;
	return lpIPCInterface->Link(pfnConnectCallback, pUserData);
}

long SIPC_Send(SIPC_INSTANCE hSIpcInstance, DWORD dwId, SIPC_STREAM hSIpcStream, SENDCALLBACK pfnSendCallback, void* pUserData)
{
	TSDEBUG(L"Enter");

	IPCInterface* lpIPCInterface = (IPCInterface*)hSIpcInstance;
	LPSOCKETIPC_STREAM lpSocketIpcStream = (LPSOCKETIPC_STREAM)hSIpcStream;
	return lpIPCInterface->Send(dwId, lpSocketIpcStream, pfnSendCallback, pUserData);
}

long SIPC_RegisterRecvCallback(SIPC_INSTANCE hSIpcInstance, RECVCALLBACK pfnRecvCallback, void* pUserData)
{
	TSDEBUG(L"Enter");

	IPCInterface* lpIPCInterface = (IPCInterface*)hSIpcInstance;
	ONRECVCALLBACK pfnOnRecvCallback = (ONRECVCALLBACK)pfnRecvCallback;
	return lpIPCInterface->RegisterRecvCallback(pfnOnRecvCallback, pUserData);
}

long SIPC_RegisterDislinkCallback(SIPC_INSTANCE hSIpcInstance, DISLINKCALLBACK pfnDisconnectCallback, void* pUserData)
{
	TSDEBUG(L"Enter");

	IPCInterface* lpIPCInterface = (IPCInterface*)hSIpcInstance;
	ONDISCONNECTCALLBACK pfnOnDisconnectCallback = (ONDISCONNECTCALLBACK)pfnDisconnectCallback;
	return lpIPCInterface->RegisterDislinkCallback(pfnOnDisconnectCallback, pUserData);
}

SIPC_STREAM SIPC_CreateSocketIpcStream()
{
	TSDEBUG(L"Enter");

	LPSOCKETIPC_STREAM lpSocketIpcStream = CommonTool::CreateSocketIpcStream();
	return (SIPC_STREAM)lpSocketIpcStream;
}

long SIPC_StreamWriteString(SIPC_STREAM hSIpcStream, const char* szData)
{
	TSDEBUG(L"Enter");

	LPSOCKETIPC_STREAM lpSocketIpcStream = (LPSOCKETIPC_STREAM)hSIpcStream;
	return CommonTool::StreamWriteString(lpSocketIpcStream, szData);
}

long SIPC_StreamReadString(SIPC_STREAM hSIpcStream, char* szData, int iLen)
{
	TSDEBUG(L"Enter");

	LPSOCKETIPC_STREAM lpSocketIpcStream = (LPSOCKETIPC_STREAM)hSIpcStream;
	return CommonTool::StreamReadString(lpSocketIpcStream, szData, iLen);
}

