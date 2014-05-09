
#include "stdafx.h"
#include "ServerDataCommunication.h"


ServerDataCommunication::ServerDataCommunication()
: m_hWnd(NULL)
, m_hIoCompletionPort(NULL)
, m_serverSocket(INVALID_SOCKET)
, m_localSocket(INVALID_SOCKET)
{

}

ServerDataCommunication::~ServerDataCommunication()
{

}


long ServerDataCommunication::Init(HWND hWnd)
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

	//创建完成端口
	m_hIoCompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (NULL == m_hIoCompletionPort)
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"CreateIoCompletionPort failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	//这里原本是要获得机器是内核，然后创建相应个线程，因这里实际使用时并发量只是1，所以这里只创建一个线程
	LPSERVERWORKER_THREADPARAM lpServerWorkerThreadParam = new SERVERWORKER_THREADPARAM;
	lpServerWorkerThreadParam->pThis = this;
	lpServerWorkerThreadParam->hIoCompletionPort = m_hIoCompletionPort;
	HANDLE hServerWorkerThread = (HANDLE)_beginthreadex(NULL, 0, &ServerWorkerThread, lpServerWorkerThreadParam, 0, NULL);
	if (NULL == hServerWorkerThread)
	{
		TSDEBUG(L"_beginthreadex ServerWorkerThread failed");
		return -1;
	}
	::CloseHandle(hServerWorkerThread);

	//创建套接字
	m_serverSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_serverSocket)
	{
		int iErrCode = ::WSAGetLastError();
		TSDEBUG(L"WSASocket failed, error code is %d", iErrCode);
		return iErrCode;
	}

	return 0;
}

long ServerDataCommunication::Link()
{
	TSDEBUG(L"Enter");

	//填充sockaddr_in结构
	sockaddr_in localSockAddr;
	localSockAddr.sin_family = AF_INET;
	localSockAddr.sin_addr.s_addr = INADDR_ANY;
	localSockAddr.sin_port = htons(SOCKETIPC_PORT);

	//绑定端口
	if (0 != bind(m_serverSocket, (sockaddr*)&localSockAddr, sizeof(localSockAddr)))
	{
		int iErrCode = ::WSAGetLastError();
		TSDEBUG(L"bind failed, error code is %d", iErrCode);
		return iErrCode;
	}

	//监听
	if (0 != listen(m_serverSocket, 5))
	{
		int iErrCode = ::WSAGetLastError();
		TSDEBUG(L"listen failed, error code is %d", iErrCode);
		return iErrCode;
	}

	//开启监听线程
	HANDLE hListenThread = (HANDLE)_beginthreadex(NULL, 0, &ListenThread, this, 0, NULL);
	if (NULL == hListenThread)
	{
		TSDEBUG(L"_beginthreadex failed");
		return -1;
	}
	::CloseHandle(hListenThread);

	return 0;
}

long ServerDataCommunication::SendData(DWORD dwId, const char* szData, const long& lDataLen)
{
	TSDEBUG(L"Enter, dwId = %u, lDataLen = %d", dwId, lDataLen);

	DWORD SendBytes = 0;

	LPPER_IO_OPERATION_DATA lpPerIoOperationData = (LPPER_IO_OPERATION_DATA)::GlobalAlloc(GPTR, sizeof(PER_IO_OPERATION_DATA));
	if (lpPerIoOperationData == NULL)
	{
		TSDEBUG(L"GlobalAlloc LPPER_IO_OPERATION_DATA failed, error code is %u", ::GetLastError());
		return -1;
	}
	::ZeroMemory(&(lpPerIoOperationData->Overlapped), sizeof(OVERLAPPED));
	lpPerIoOperationData->Buffer = new CHAR[lDataLen];
	memcpy(lpPerIoOperationData->Buffer, szData, lDataLen);
	lpPerIoOperationData->DataBuf.len = lDataLen;
	lpPerIoOperationData->DataBuf.buf = lpPerIoOperationData->Buffer;
	lpPerIoOperationData->BytesSEND = 0;
	lpPerIoOperationData->BytesRECV = 0;
	lpPerIoOperationData->IsSEND = TRUE;
	lpPerIoOperationData->Id = dwId;

	if (::WSASend(m_localSocket, &(lpPerIoOperationData->DataBuf), 1, &SendBytes, 0, &(lpPerIoOperationData->Overlapped), NULL) == SOCKET_ERROR)
	{
		int iErrCode = ::WSAGetLastError();
		TSDEBUG(L"WSASend failed, error code is %u", iErrCode);
		if (iErrCode != ERROR_IO_PENDING)
		{
			TSDEBUG(L"iErrCode is not ERROR_IO_PENDING");
			PostSendCompleteMsg(dwId, iErrCode);
			return iErrCode;
		}
	}

	return 0;
}


unsigned __stdcall ServerDataCommunication::ServerWorkerThread(void* pParam)
{
	TSDEBUG(L"Enter");
	LPSERVERWORKER_THREADPARAM lpServerWorkerThreadParam = (LPSERVERWORKER_THREADPARAM)pParam;
	HANDLE hIoCompletionPort = lpServerWorkerThreadParam->hIoCompletionPort;
	DWORD dwBytesTransferred;
	LPPER_HANDLE_DATA lpPerHandleData;
	LPPER_IO_OPERATION_DATA lpPerIoOperationData;
	DWORD dwSendBytes, dwRecvBytes;
	DWORD dwFlags;

	while(TRUE)
	{
		TSDEBUG(L"Call GetQueuedCompletionStatus to wait");
		if (!::GetQueuedCompletionStatus(hIoCompletionPort, &dwBytesTransferred, (LPDWORD)&lpPerHandleData, (LPOVERLAPPED *) &lpPerIoOperationData, INFINITE))
		{
			TSDEBUG(L"GetQueuedCompletionStatus failed, error code is %u", ::GetLastError());

			::GlobalFree(lpPerHandleData);
			delete [] lpPerIoOperationData->Buffer;
			::GlobalFree(lpPerIoOperationData);

			break;
		}
		TSDEBUG(L"dwBytesTransferred = %u, lpPerIoOperationData->IsSEND = %d, lpPerIoOperationData->DataBuf.len = %u, lpPerIoOperationData->BytesRECV = %u, lpPerIoOperationData->BytesSEND = %u", dwBytesTransferred, lpPerIoOperationData->IsSEND, lpPerIoOperationData->DataBuf.len, lpPerIoOperationData->BytesRECV, lpPerIoOperationData->BytesSEND);

		// First check to see if an error has occured on the socket and if so
		// then close the socket and cleanup the SOCKET_INFORMATION structure
		// associated with the socket.
		if (dwBytesTransferred == 0)
		{
			TSDEBUG(L"BytesTransferred = 0, close socket %d", lpPerHandleData->Socket);

			if (::closesocket(lpPerHandleData->Socket) == SOCKET_ERROR)
			{
				TSDEBUG(L"closesocket failed, error code is %u", ::WSAGetLastError());

				::GlobalFree(lpPerHandleData);
				delete [] lpPerIoOperationData->Buffer;
				::GlobalFree(lpPerIoOperationData);

				break;
			}
			continue;
		}

		TSDEBUG(L"PerIoData->IsSEND = %d", lpPerIoOperationData->IsSEND);
		if (lpPerIoOperationData->IsSEND)
		{
			TSDEBUG(L"Send data completelly");

			//发送完数据后通知外界
			lpServerWorkerThreadParam->pThis->PostSendCompleteMsg(lpPerIoOperationData->Id, 0);

			//清理数据
			delete [] lpPerIoOperationData->Buffer;
			lpPerIoOperationData->Buffer = NULL;
			::GlobalFree(lpPerIoOperationData);
		}
		else
		{
			TSDEBUG(L"Recieve data completelly");

			//对接收到的数据进行处理
			lpServerWorkerThreadParam->pThis->PostRecvCompleteMsg(lpPerIoOperationData->Buffer, dwBytesTransferred);

			//继续投递接收请求
			dwFlags = 0;
			ZeroMemory(&(lpPerIoOperationData->Overlapped), sizeof(OVERLAPPED));
			memset(lpPerIoOperationData->Buffer, 0, DATA_BUFSIZE);

			lpPerIoOperationData->DataBuf.len = DATA_BUFSIZE;
			lpPerIoOperationData->DataBuf.buf = lpPerIoOperationData->Buffer;

			TSDEBUG(L"Call WSARecv to recieve data");
			if (::WSARecv(lpPerHandleData->Socket, &(lpPerIoOperationData->DataBuf), 1, &dwRecvBytes, &dwFlags, &(lpPerIoOperationData->Overlapped), NULL) == SOCKET_ERROR)
			{
				DWORD dwErrCode = ::WSAGetLastError();
				TSDEBUG(L"WSARecv failed, error code is %u", dwErrCode);
				if (dwErrCode != ERROR_IO_PENDING)
				{
					TSDEBUG(L"dwErrCode is not ERROR_IO_PENDING");
					break;
				}
			}
		}
	}

	delete lpServerWorkerThreadParam;
	lpServerWorkerThreadParam = NULL;

	lpServerWorkerThreadParam->pThis->PostDisconnectMsg();

	return 0;
}

unsigned __stdcall ServerDataCommunication::ListenThread(void* pParam)
{
	TSDEBUG(L"Enter");

	ServerDataCommunication* pThis = (ServerDataCommunication*)pParam;

	SOCKET acceptSocket;
	DWORD dwRecvBytes = 0;
	DWORD dwFlags = 0;

	while(TRUE)
	{
		TSDEBUG(L"Enter while");
		acceptSocket = ::WSAAccept(pThis->m_serverSocket, NULL, NULL, NULL, 0);
		if (INVALID_SOCKET == acceptSocket)
		{
			int iErrCode = ::WSAGetLastError();
			TSDEBUG(L"WSAAccept failed, error code is %d", iErrCode);
			//发送连接完成消息
			pThis->PostListenCompleteMsg(FALSE);
			break;
		}
		TSDEBUG(L"acceptSocket = %d", acceptSocket);
		pThis->m_localSocket = acceptSocket;

		//发送连接完成消息
		pThis->PostListenCompleteMsg(TRUE);

		LPPER_HANDLE_DATA lpPerHandleData = (LPPER_HANDLE_DATA)::GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));
		if (lpPerHandleData == NULL)
		{
			TSDEBUG(L"GlobalAlloc LPPER_HANDLE_DATA failed, error code is %u", ::GetLastError());
			break;
		}
		lpPerHandleData->Socket = acceptSocket;

		::CreateIoCompletionPort((HANDLE)acceptSocket, pThis->m_hIoCompletionPort, (DWORD)lpPerHandleData, 0);

		//接收数据
		LPPER_IO_OPERATION_DATA lpPerIoOperationData = (LPPER_IO_OPERATION_DATA)::GlobalAlloc(GPTR, sizeof(PER_IO_OPERATION_DATA));
		if (lpPerIoOperationData == NULL)
		{
			TSDEBUG(L"GlobalAlloc LPPER_IO_OPERATION_DATA failed, error code is %u", ::GetLastError());
			break;
		}
		::ZeroMemory(&(lpPerIoOperationData->Overlapped), sizeof(OVERLAPPED));
		lpPerIoOperationData->Buffer = new CHAR[DATA_BUFSIZE];
		lpPerIoOperationData->DataBuf.len = DATA_BUFSIZE;
		lpPerIoOperationData->DataBuf.buf = lpPerIoOperationData->Buffer;
		lpPerIoOperationData->BytesSEND = 0;
		lpPerIoOperationData->BytesRECV = 0;
		lpPerIoOperationData->IsSEND = FALSE;

		if (::WSARecv(lpPerHandleData->Socket, &(lpPerIoOperationData->DataBuf), 1, &dwRecvBytes, &dwFlags, &(lpPerIoOperationData->Overlapped), NULL) == SOCKET_ERROR)
		{
			DWORD dwErrCode = ::WSAGetLastError();
			TSDEBUG(L"WSARecv failed, error code is %u", dwErrCode);
			if (dwErrCode != ERROR_IO_PENDING)
			{
				TSDEBUG(L"dwErrCode is not ERROR_IO_PENDING");
				return 0;
			}
		}
	}

	return 0;
}

long ServerDataCommunication::PostListenCompleteMsg(BOOL bRet)
{
	TSDEBUG(L"Enter, bRet = %d", bRet);

	LPLISTEN_MSG_DATA lpListenCallbackData = new LISTEN_MSG_DATA;
	lpListenCallbackData->bRet = bRet;

	if (!::PostMessage(m_hWnd, WM_DATACOMMUNICATION_LINKCALLBACK, (WPARAM)lpListenCallbackData, NULL))
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"PostMessage WM_DATACOMMUNICATION_LINKCALLBACK failed, error code is %u", dwErrCode);
		return dwErrCode;
	}

	return 0;
}

long ServerDataCommunication::PostSendCompleteMsg(DWORD dwId, DWORD dwErrCode)
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

long ServerDataCommunication::PostRecvCompleteMsg(const char* szData, DWORD dwNumberOfBytesRecv)
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

long ServerDataCommunication::PostDisconnectMsg()
{
	TSDEBUG(L"Enter");

	if (!::PostMessage(m_hWnd, WM_DATACOMMUNICATION_DISCONNECTCALLBACK, NULL, NULL))
	{
		DWORD dwErrCode = ::GetLastError();
		TSDEBUG(L"PostMessage WM_DATACOMMUNICATION_DISLINKCALLBACK failed, error code is %u", dwErrCode);
		return dwErrCode;
	}
}