// test_ipc_server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "..\..\SockIPC\SockIPC\SockIPC_Interface.h"

static XAF_IPC_CONNECTION_HANDLE s_conn = NULL;
static HMODULE m_hInsLibrary;
static int s_step = 0;

DWORD ON_CONNECT(XAF_IPC_CONNECTION_HANDLE pSock)
{
	printf("Enter %s\n", __FUNCTION__);
	return 0;
}

DWORD ON_SEND(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data)
{
	printf("Enter %s\n", __FUNCTION__);

	TCHAR* ch = new TCHAR[255];
	memset(ch, 0, 255);
	memcpy(ch, _T("i am server----------\n"), 255);
	XAF_IPCSend(sockIPC, (byte*)ch, 255);
	printf("----------server_send:%S", ch);
	memcpy(ch, _T("i am server2----------\n"), 255);
	XAF_IPCSend(sockIPC, (byte*)ch, 255);
	printf("----------server_send:%S", ch);
	memcpy(ch, _T("i am server3----------\n"), 255);
	XAF_IPCSend(sockIPC, (byte*)ch, 255);
	printf("----------server_send:%S", ch);
	memcpy(ch, _T("i am server4----------\n"), 255);
	XAF_IPCSend(sockIPC, (byte*)ch, 255);
	printf("----------server_send:%S", ch);
	memcpy(ch, _T("i am server5----------\n"), 255);
	XAF_IPCSend(sockIPC, (byte*)ch, 255);
	printf("----------server_send:%S", ch);
	memcpy(ch, _T("i am server6----------\n"), 255);
	XAF_IPCSend(sockIPC, (byte*)ch, 255);
	printf("----------server_send:%S", ch);
	memcpy(ch, _T("i am server7----------\n"), 255);
	XAF_IPCSend(sockIPC, (byte*)ch, 255);
	printf("----------server_send:%S", ch);
	memcpy(ch, _T("i am server8----------\n"), 255);
	XAF_IPCSend(sockIPC, (byte*)ch, 255);
	printf("----------server_send:%S", ch);
	s_step++;
/*	memcpy(ch, _T("i am server2----------\n"), 255);
	XAF_IPCSend(sockIPC, (byte*)ch, 255);
	printf("----------server_send:%S", ch);
	memcpy(ch, _T("i am server3----------\n"), 255);
	XAF_IPCSend(sockIPC, (byte*)ch, 255);
	printf("----------server_send:%S", ch);
	memcpy(ch, _T("i am server4----------\n"), 255);
	XAF_IPCSend(sockIPC, (byte*)ch, 255);
	printf("----------server_send:%S", ch);
	memcpy(ch, _T("i am server5----------\n"), 255);
	XAF_IPCSend(sockIPC, (byte*)ch, 255);
	printf("----------server_send:%S", ch);*/
	return 0;
}

DWORD ON_RECV(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data)
{
	printf("Enter %s\n", __FUNCTION__);

	Sleep(5000);
	TCHAR* ch = new TCHAR[255];
	memset(ch, 0, 255);
	XAF_IPCRecv(sockIPC, (byte*)ch, 255);
	std::printf("----------server_recv:%S", ch);
	/*
	switch(s_step)
	{
	case 1:
		{
			TCHAR* ch = new TCHAR[255];
			memset(ch, 0, 255);
			memcpy(ch, _T("i am server2----------\n"), 255);
		//	XAF_IPCSend(sockIPC, (byte*)ch, 255);
		//	printf("----------server_send:%S", ch);
			s_step++;
		}
		break;
	case 2:
		{
			TCHAR* ch = new TCHAR[255];
			memset(ch, 0, 255);
			memcpy(ch, _T("i am server3----------\n"), 255);
		//	XAF_IPCSend(sockIPC, (byte*)ch, 255);
		//	printf("----------server_send:%S", ch);
			s_step++;
		}
		break;
	case 3:
		{
			TCHAR* ch = new TCHAR[255];
			memset(ch, 0, 255);
			memcpy(ch, _T("i am server4----------\n"), 255);
		//	XAF_IPCSend(sockIPC, (byte*)ch, 255);
		//	printf("----------server_send:%S", ch);
			s_step++;
		}
		break;
	default:
		break;
	}*/

	return 0;
}

DWORD ON_ACCPET(XAF_IPC_LISTENER_HANDLE pSock, XAF_IPC_CONNECTION_HANDLE sockConn, IPCAddress* pAddr, void* data)
{
	printf("Enter %s\n", __FUNCTION__);

	//printf("on_accp:%d", pAddr->dwPid);
	s_conn = sockConn;
	return 0;
}

DWORD ON_CLOSE(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data, long resultCode)
{
	printf("Enter %s\n", __FUNCTION__);

//	XAF_IPCCloseConnection(sockIPC);
	printf("----------conn_close------------\n");
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	system("pause");
	//m_hInsLibrary = LoadLibrary(_T("SockIPC"));
	XAF_IPCStartup(0);

	XAF_IPC_LISTENER_HANDLE listener = XAF_IPCListener();

	IPCAddress addr;
	addr.dwPid = GetCurrentProcessId();
	addr.dwPort = 8000;
	XAF_IPCBind(listener, addr);
	XAF_IPCAttachListenerEvent(listener, IPC_ACCEPT, ON_ACCPET, NULL);
	XAF_IPCAttachListenerEvent(listener, IPC_WRITE, ON_SEND, NULL);
	XAF_IPCAttachListenerEvent(listener, IPC_READ, ON_RECV, NULL);
	XAF_IPCAttachListenerEvent(listener, IPC_CLOSE, ON_CLOSE, NULL);
	XAF_IPCListen(listener, 5);

	XAF_IPC_CONNECTION_HANDLE conn = NULL;

	while (true)
	{
		MSG msg;
		if (GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message > WM_USER && msg.message < (WM_USER+10))
			{
				printf("getmessage:%d\n", msg.message-WM_USER);
			}
			TranslateMessage(&msg); 
			DispatchMessage(&msg); 
		}
	}
	return 0;
}

