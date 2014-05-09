
#include "stdafx.h"
#include "ClientDataCommunication.h"


ClientDataCommunication::ClientDataCommunication()
: m_hWnd(NULL)
, m_localSocket(INVALID_SOCKET)
, m_dwEventTotal(0)
{

}

ClientDataCommunication::~ClientDataCommunication()
{

}


long ClientDataCommunication::Init(HWND hWnd)
{
	TSDEBUG(L"Enter");

	if (NULL == hWnd)
	{
		TSDEBUG(L"Invalided param, hWnd = NULL");
		return -1;
	}
	m_hWnd = hWnd;

	//初始化WinSock
	WSADATA wsaData;
	::WSAStartup( MAKEWORD( 2, 2), &wsaData );

	//创建套接字
	m_localSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (INVALID_SOCKET == m_localSocket)
	{
		int iErrCode = ::WSAGetLastError();
		TSDEBUG(L"WSASocket failed, error code is %d", iErrCode);
		return iErrCode;
	}
	TSDEBUG(L"m_localSocket = %d", m_localSocket);

	long lRet = CreateSocketInformation(m_localSocket);
	if (0 != lRet)
	{
		TSDEBUG(L"CreateSocketInformation failed, error code is %d", lRet);
		return lRet;
	}

	return 0;
}

long ClientDataCommunication::Link()
{
	TSDEBUG(L"Enter");

	HANDLE hClientWorkerThread = (HANDLE)_beginthreadex(NULL, 0, &ClientWorkerThread, this, 0, NULL);
	if (NULL == hClientWorkerThread)
	{
		TSDEBUG(L"_beginthreadex failed");
		return -1;
	}
	::CloseHandle(hClientWorkerThread);

	if (SOCKET_ERROR == ::WSAEventSelect(m_localSocket, m_aEventArray[m_dwEventTotal - 1], FD_CONNECT | FD_CLOSE))
	{
		int iErrCode = ::WSAGetLastError();
		TSDEBUG(L"WSAEventSelect failed, error code is %d", iErrCode);
		return iErrCode;
	}

	//获取主机信息
	struct hostent *hp;
	unsigned int addr;
	if (inet_addr(SOCKETIPC_SERVERIP) == INADDR_NONE)
	{
		TSDEBUG(L"inet_addr return INADDR_NONE");
		hp = ::gethostbyname(SOCKETIPC_SERVERIP);
	}
	else
	{
		TSDEBUG(L"inet_addr does not return INADDR_NONE");
		addr = inet_addr(SOCKETIPC_SERVERIP);
		hp = ::gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
	}
	if (hp == NULL)
	{
		TSDEBUG(L"hp = NULL");
		return -1;
	}

	SOCKADDR_IN InternetAddr;
	InternetAddr.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_port = htons(SOCKETIPC_PORT);

	TSDEBUG(L"Call WSAConnect to connect");
	if (SOCKET_ERROR == ::WSAConnect(m_localSocket, (SOCKADDR*)&InternetAddr, sizeof(InternetAddr), NULL, NULL, NULL, NULL))
	{
		int iErrCode = ::WSAGetLastError();
		TSDEBUG(L"WSAConnect failed, error code is %d", iErrCode);
		if (WSAEWOULDBLOCK != iErrCode)
		{
			TSDEBUG(L"iErrCode is not WSAEWOULDBLOCK");
			return iErrCode;
		}
	}

	return 0;
}

long ClientDataCommunication::SendData(DWORD dwId, const char* szData, const long& lDataLen)
{
	TSDEBUG(L"Enter, dwId = %u, lDataLen", dwId, lDataLen);

	DWORD dwSendBytes = 0;

	//发送连接数据
	LPSOCKET_INFORMATION lpSocketInfo = m_aSocketInfo[m_dwEventTotal - 1];
	if (lpSocketInfo->BufferSize < lDataLen)
	{
		delete [] lpSocketInfo->Buffer;
		lpSocketInfo->Buffer = new CHAR[lDataLen];
		lpSocketInfo->BufferSize = lDataLen;
	}
	memcpy(lpSocketInfo->Buffer, szData, lDataLen);
	lpSocketInfo->BytesRECV = lDataLen;
	lpSocketInfo->BytesSEND = 0;
	lpSocketInfo->DataBuf.buf = lpSocketInfo->Buffer;
	lpSocketInfo->DataBuf.len = lpSocketInfo->BytesRECV;
	lpSocketInfo->Id = dwId;
	TSDEBUG(L"Call WSASend to send data");
	if (::WSASend(lpSocketInfo->Socket, &(lpSocketInfo->DataBuf), 1, &dwSendBytes, 0, NULL, NULL) == SOCKET_ERROR)
	{
		int iErrCode = ::WSAGetLastError();
		TSDEBUG(L"WSASend failed, error code is %d", iErrCode);
		if (iErrCode != WSAEWOULDBLOCK)
		{
			TSDEBUG(L"iErrCode is not WSAEWOULDBLOCK");
			PostSendCompleteMsg(dwId, iErrCode);
			return iErrCode;
		}
	}
	else
	{
		PostSendCompleteMsg(dwId, 0);
	}

	return 0;
}


long ClientDataCommunication::CreateSocketInformation(SOCKET localSocket)
{
	TSDEBUG(L"Enter");
	LPSOCKET_INFORMATION lpSocketInfo;

	m_aEventArray[m_dwEventTotal] = ::WSACreateEvent();
	if (WSA_INVALID_EVENT == m_aEventArray[m_dwEventTotal])
	{
		int iErrCode = ::WSAGetLastError();
		TSDEBUG(L"WSACreateEvent failed, error code is %d", iErrCode);
		return iErrCode;
	}

	lpSocketInfo = (LPSOCKET_INFORMATION)::GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION));
	if (NULL == lpSocketInfo)
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"GlobalAlloc failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	// Prepare SocketInfo structure for use.

	lpSocketInfo->Socket = localSocket;
	lpSocketInfo->Buffer = new CHAR[DATA_BUFSIZE];
	lpSocketInfo->BufferSize = DATA_BUFSIZE;
	lpSocketInfo->BytesSEND = 0;
	lpSocketInfo->BytesRECV = 0;
	lpSocketInfo->Id = 0;

	m_aSocketInfo[m_dwEventTotal] = lpSocketInfo;

	m_dwEventTotal++;

	return 0;
}

long ClientDataCommunication::FreeSocketInformation(DWORD dwEvent)
{
	LPSOCKET_INFORMATION lpSocketInfo = m_aSocketInfo[dwEvent];

	::closesocket(lpSocketInfo->Socket);

	delete [] lpSocketInfo->Buffer;
	lpSocketInfo->Buffer = NULL;

	::GlobalFree(lpSocketInfo);

	::WSACloseEvent(m_aEventArray[dwEvent]);

	// Squash the socket and event arrays

	for (DWORD dwIndex = dwEvent; dwIndex < m_dwEventTotal; dwIndex++)
	{
		m_aEventArray[dwIndex] = m_aEventArray[dwIndex + 1];
		m_aSocketInfo[dwIndex] = m_aSocketInfo[dwIndex + 1];
	}

	m_dwEventTotal--;

	return 0;
}

unsigned __stdcall ClientDataCommunication::ClientWorkerThread(void* pParam)
{
	TSDEBUG(L"Enter");

	ClientDataCommunication* pThis = (ClientDataCommunication*)pParam;
	DWORD dwEvent;
	WSANETWORKEVENTS networkEvents;
	DWORD dwFlags = 0;
	DWORD dwRecvBytes;
	DWORD dwSendBytes;

	while(TRUE)
	{
		// Wait for one of the sockets to receive I/O notification
		dwEvent = ::WSAWaitForMultipleEvents(pThis->m_dwEventTotal, pThis->m_aEventArray, FALSE, WSA_INFINITE, FALSE);
		TSDEBUG(L"WSAWaitForMultipleEvents return dwEvent = %u", dwEvent);
		if (WSA_WAIT_FAILED == dwEvent)
		{
			TSDEBUG(L"WSAWaitForMultipleEvents failed, error code is %d", ::WSAGetLastError());
			return 0;
		}

		TSDEBUG(L"Ready to call WSAEnumNetworkEvents");
		// Discovers occurrences of network events for the indicated socket,clear interal network event records,and reset event objects(optional).
		if (SOCKET_ERROR == ::WSAEnumNetworkEvents(pThis->m_aSocketInfo[dwEvent - WSA_WAIT_EVENT_0]->Socket, pThis->m_aEventArray[dwEvent - WSA_WAIT_EVENT_0], &networkEvents))
		{
			TSDEBUG(L"WSAEnumNetworkEvents failed, error code is %d", ::WSAGetLastError());
			return 0;
		}

		//Connect
		if (networkEvents.lNetworkEvents & FD_CONNECT)
		{
			TSDEBUG(L"networkEvents.lNetworkEvents & FD_CONNECT return true");
			if (networkEvents.iErrorCode[FD_CONNECT_BIT] != 0)
			{
				TSDEBUG(L"FD_CONNECT failed, networkEvents.INetworkEvents[FD_CONNECT_BIT] = %d", networkEvents.iErrorCode[FD_CONNECT_BIT]);
				pThis->PostConnectCompleteMsg(FALSE);
				break;
			}

			pThis->PostConnectCompleteMsg(TRUE);

			if (::WSAEventSelect(pThis->m_aSocketInfo[pThis->m_dwEventTotal - 1]->Socket, pThis->m_aEventArray[pThis->m_dwEventTotal - 1], FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
			{
				int iErrCode = ::WSAGetLastError();
				TSDEBUG(L"WSAEventSelect failed, error code is %d", iErrCode);
				return iErrCode;
			}
		}

		//Read
		if (networkEvents.lNetworkEvents & FD_READ)
		{
			TSDEBUG(L"networkEvents.lNetworkEvents & FD_READ return true");
			if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
			{
				TSDEBUG(L"FD_READ failed, networkEvents.INetworkEvents[FD_READ_BIT] = %d", networkEvents.iErrorCode[FD_READ_BIT]);
				break;
			}

			LPSOCKET_INFORMATION lpSocketInfo = pThis->m_aSocketInfo[dwEvent - WSA_WAIT_EVENT_0];

			lpSocketInfo->DataBuf.buf = lpSocketInfo->Buffer;
			lpSocketInfo->DataBuf.len = lpSocketInfo->BufferSize;

			dwFlags = 0;
			if (::WSARecv(lpSocketInfo->Socket, &(lpSocketInfo->DataBuf), 1, &dwRecvBytes, &dwFlags, NULL, NULL) == SOCKET_ERROR)
			{
				int iErrCode = ::WSAGetLastError();
				TSDEBUG(L"WSARecv failed, error code is %d", iErrCode);
				if (iErrCode != WSAEWOULDBLOCK)
				{
					TSDEBUG(L"iErrCode is not WSAEWOULDBLOCK");
					pThis->FreeSocketInformation(dwFlags - WSA_WAIT_EVENT_0);
					return 0;
				}
			} 
			else
			{
				TSDEBUG(L"dwRecvBytes = %u", dwRecvBytes);
				pThis->PostRecvCompleteMsg(lpSocketInfo->DataBuf.buf, dwRecvBytes);
			}
		}

		//Write
		if (networkEvents.lNetworkEvents & FD_WRITE)
		{
			TSDEBUG(L"networkEvents.lNetworkEvents & FD_WRITE return true");
			if (networkEvents.iErrorCode[FD_WRITE_BIT] != 0)
			{
				TSDEBUG(L"FD_WRITE failed, networkEvents.INetworkEvents[FD_READ_BIT] = %d", networkEvents.iErrorCode[FD_WRITE_BIT]);
				break;
			}

			LPSOCKET_INFORMATION lpSocketInfo = pThis->m_aSocketInfo[dwEvent - WSA_WAIT_EVENT_0];
			if (lpSocketInfo->BufferSize != DATA_BUFSIZE)
			{
				delete [] lpSocketInfo->Buffer;
				lpSocketInfo->Buffer = new CHAR[DATA_BUFSIZE];
				lpSocketInfo->BufferSize = DATA_BUFSIZE;
			}
			pThis->PostSendCompleteMsg(lpSocketInfo->Id, 0);
			TSDEBUG(L"lpSocketInfo->DataBuf.len = %u, lpSocketInfo->BytesRECV = %u, lpSocketInfo->BytesSEND = %u", lpSocketInfo->DataBuf.len, lpSocketInfo->BytesRECV, lpSocketInfo->BytesSEND);
		}

		//Close
		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			TSDEBUG(L"networkEvents.lNetworkEvents & FD_CLOSE return true");
			if (networkEvents.iErrorCode[FD_WRITE_BIT] != 0)
			{
				TSDEBUG(L"FD_CLOSE failed, networkEvents.INetworkEvents[FD_CLOSE_BIT] = %d", networkEvents.iErrorCode[FD_CLOSE_BIT]);
				break;
			}

			TSDEBUG(L"Closing socket information %d\n", pThis->m_aSocketInfo[dwEvent - WSA_WAIT_EVENT_0]->Socket);

			pThis->FreeSocketInformation(dwEvent - WSA_WAIT_EVENT_0);
			pThis->PostDisconnectMsg();
		}
	}

	return 0;
}

long ClientDataCommunication::PostConnectCompleteMsg(BOOL bRet)
{
	TSDEBUG(L"Enter, bRet = %d", bRet);

	LPCONNECT_MSG_DATA lpConnectCallbackData = new CONNECT_MSG_DATA;
	lpConnectCallbackData->bRet = bRet;

	if (!::PostMessage(m_hWnd, WM_DATACOMMUNICATION_LINKCALLBACK, (WPARAM)lpConnectCallbackData, NULL))
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"PostMessage WM_DATACOMMUNICATION_LINKCALLBACK failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	return 0;
}

long ClientDataCommunication::PostSendCompleteMsg(DWORD dwId, DWORD dwErrCode)
{
	TSDEBUG(L"Enter, dwId = %u, dwErrCode = %u", dwId, dwErrCode);

	LPSEND_MSG_DATA lpSendCallbackData = new SEND_MSG_DATA;
	lpSendCallbackData->dwId = dwId;
	lpSendCallbackData->dwErrCode = dwErrCode;

	if (!::PostMessage(m_hWnd, WM_DATACOMMUNICATION_SENDCALLBACK, (WPARAM)lpSendCallbackData, NULL))
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"PostMessage WM_DATACOMMUNICATION_SENDCALLBACK failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	return 0;
}

long ClientDataCommunication::PostRecvCompleteMsg(const char* szData, DWORD dwNumberOfBytesRecv)
{
	TSDEBUG(L"Enter, dwNumberOfBytesRecv = %u", dwNumberOfBytesRecv);

	LPRECV_MSG_DATA lpRecvCallbackData = new RECV_MSG_DATA;
	memcpy(lpRecvCallbackData->szData, szData, DATA_BUFSIZE);
	lpRecvCallbackData->dwNumberOfBytesRecv = dwNumberOfBytesRecv;

	if (!::PostMessage(m_hWnd, WM_DATACOMMUNICATION_RECVCALLBACK, (WPARAM)lpRecvCallbackData, NULL))
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"PostMessage WM_DATACOMMUNICATION_RECVCALLBACK failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	return 0;
}

long ClientDataCommunication::PostDisconnectMsg()
{
	TSDEBUG(L"Enter");

	if (!::PostMessage(m_hWnd, WM_DATACOMMUNICATION_DISCONNECTCALLBACK, NULL, NULL))
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"PostMessage WM_DATACOMMUNICATION_DISLINKCALLBACK failed, error code is %u", dwErrCode);
		return dwErrCode;
	}
}