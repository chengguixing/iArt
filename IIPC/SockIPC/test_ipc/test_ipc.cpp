// test_ipc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\..\SockIPC\SockIPC\SockIPC_Interface.h"

static XAF_IPC_CONNECTION_HANDLE s_conn = NULL;
static HMODULE m_hInsLibrary;
static int s_step = 0;

DWORD ON_CONNECT(XAF_IPC_CONNECTION_HANDLE* pSock)
{
	printf("Enter %s\n", __FUNCTION__);

	return 0;
}

DWORD ON_SEND(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data)
{
	printf("Enter %s\n", __FUNCTION__);

	TCHAR* ch = new TCHAR[255];
	memset(ch, 0, 255);
	memcpy(ch, _T("i am connection----------\n"), 255);
	XAF_IPCSend(s_conn, (byte*)ch, 255);
	printf("----------conn_send:%S", ch);
/*	memcpy(ch, _T("i am connection2----------\n"), 255);
	XAF_IPCSend(s_conn, (byte*)ch, 255);
	printf("----------conn_send:%S", ch);
	memcpy(ch, _T("i am connection3----------\n"), 255);
	XAF_IPCSend(s_conn, (byte*)ch, 255);
	printf("----------conn_send:%S", ch);
	memcpy(ch, _T("i am connection4----------\n"), 255);
	XAF_IPCSend(s_conn, (byte*)ch, 255);
	printf("----------conn_send:%S", ch);

	s_step++;
/*	memcpy(ch, _T("i am connection2----------\n"), 255);
	XAF_IPCSend(s_conn, (byte*)ch, 255);
	printf("----------conn_send:%S", ch);
	memcpy(ch, _T("i am connection3----------\n"), 255);
	XAF_IPCSend(s_conn, (byte*)ch, 255);
	printf("----------conn_send:%S", ch);
	memcpy(ch, _T("i am connection4----------\n"), 255);
	XAF_IPCSend(s_conn, (byte*)ch, 255);
	printf("----------conn_send:%S", ch);
	memcpy(ch, _T("i am connection5----------\n"), 255);
	XAF_IPCSend(s_conn, (byte*)ch, 255);
	printf("----------conn_send:%S", ch);
	memcpy(ch, _T("i am connection6----------\n"), 255);
	XAF_IPCSend(s_conn, (byte*)ch, 255);
	printf("----------conn_send:%S", ch);
	memcpy(ch, _T("i am connection7----------\n"), 255);
	XAF_IPCSend(s_conn, (byte*)ch, 255);
	printf("----------conn_send:%S", ch);*/
	return 0;
}

DWORD ON_RECV(XAF_IPC_CONNECTION_HANDLE* pSock)
{
	printf("Enter %s\n", __FUNCTION__);

	//Sleep(5000);
	TCHAR* ch = new TCHAR[255];
	memset(ch, 0, 255);
	XAF_IPCRecv(s_conn, (byte*)ch, 255);
	printf("----------conn_recv:%S", ch);/*
	switch(s_step)
	{
	case 1:
		{
			TCHAR* ch = new TCHAR[255];
			memset(ch, 0, 255);
			memcpy(ch, _T("i am connection2----------\n"), 255);
			XAF_IPCSend(s_conn, (byte*)ch, 255);
			printf("----------conn_send:%S", ch);
			s_step++;
		}
		break;
	case 2:
		{
			TCHAR* ch = new TCHAR[255];
			memset(ch, 0, 255);
			memcpy(ch, _T("i am connection3----------\n"), 255);
			XAF_IPCSend(s_conn, (byte*)ch, 255);
			printf("----------conn_send:%S", ch);
			s_step++;
		}
		break;
	case 3:
		{
			TCHAR* ch = new TCHAR[255];
			memset(ch, 0, 255);
			memcpy(ch, _T("i am connection4----------\n"), 255);
			XAF_IPCSend(s_conn, (byte*)ch, 255);
			printf("----------conn_send:%S", ch);
			s_step++;
		}
		break;
	default:
		break;
	}*/
//	XAF_IPCCloseConnection(pSock);
//	printf("----------conn_close------------\n");
	return 0;
}

DWORD ON_CLOSE(XAF_IPC_CONNECTION_HANDLE sockIPC, void* data, long resultCode)
{
	printf("Enter %s\n", __FUNCTION__);

	//XAF_IPCCloseConnection(sockIPC);
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	XAF_IPCStartup(0);

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;
	LPTSTR szCmdline = _tcsdup(TEXT("D:\\work\\code_svn\\thunder\\src\\SockIPC\\Debug\\test_ipc_server.exe"));

	CreateProcess(szCmdline, NULL, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	system("pause");

	IPCAddress addr;
	addr.dwPid = pi.dwProcessId;
	addr.dwPort = 8000;

	XAF_IPC_CONNECTION_HANDLE conn = XAF_IPCConnection();
	s_conn = conn;

	XAF_IPCAttachConnectionEvent(s_conn, IPC_WRITE, ON_SEND, NULL);
	XAF_IPCAttachConnectionEvent(s_conn, IPC_READ, ON_RECV, NULL);
	XAF_IPCAttachConnectionEvent(s_conn, IPC_CLOSE, ON_CLOSE, NULL);
	XAF_IPCConnect(conn, addr);


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

