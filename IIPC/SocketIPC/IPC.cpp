
#include "stdafx.h"
#include "IPC.h"
#include "CommonTool.h"


IPC::IPC()
: m_bLinkCompletely(FALSE)
, m_DataCommunicationInterface(NULL)
{

}

IPC::~IPC()
{
	if (NULL != m_DataCommunicationInterface)
	{
		delete m_DataCommunicationInterface;
		m_DataCommunicationInterface = NULL;
	}
}

LRESULT IPC::OnLinkCallback(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TSDEBUG(L"Enter");

	LPLISTEN_MSG_DATA lpLinkCallbackData = (LPLISTEN_MSG_DATA)wParam;
	if (NULL == lpLinkCallbackData)
	{
		TSDEBUG(L"wParam is NULL");
		return 0;
	}

	if (!lpLinkCallbackData->bRet)
	{
		TSDEBUG(L"Listen failed");
		m_LinkCallbackInfo.pfnOnLinkCallback(-1, m_LinkCallbackInfo.pUserData);
		return 0;
	}

	m_bLinkCompletely = TRUE;

	//初始化接收SocketIPCO数据结构
	m_SocketIpcData.ulHeaderSize = sizeof(SOCKETIPC_DATA_HEADER);
	m_SocketIpcData.ulHeaderFillCount = 0;
	m_SocketIpcData.szHeader = new char[m_SocketIpcData.ulHeaderSize];
	m_SocketIpcData.szData = NULL;
	m_SocketIpcData.ulDataSize = 0;
	m_SocketIpcData.ulDataFillCount = 0;

	m_LinkCallbackInfo.pfnOnLinkCallback(0, m_LinkCallbackInfo.pUserData);

	delete lpLinkCallbackData;
	lpLinkCallbackData = NULL;

	return 0;
}

LRESULT IPC::OnSendCallback(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TSDEBUG(L"Enter");

	LPSEND_MSG_DATA lpSendCallbackData = (LPSEND_MSG_DATA)wParam;
	DWORD dwId = lpSendCallbackData->dwId;
	DWORD dwErrCode = lpSendCallbackData->dwErrCode;
	TSDEBUG(L"dwId = %u, dwErrCode = %u", dwId, dwErrCode);

	std::map<DWORD, IPC_SEND_CALLBACK>::iterator it = m_mSendCallbackInfo.find(dwId);
	if (m_mSendCallbackInfo.end() != it)
	{
		it->second.pfnOnSendCallback(dwErrCode, it->second.pUserData);
		m_mSendCallbackInfo.erase(it);
	}

	delete lpSendCallbackData;
	lpSendCallbackData = NULL;

	return 0;
}

LRESULT IPC::OnRecvCallback(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TSDEBUG(L"Enter");

	LPRECV_MSG_DATA lpRecvCallbackData = (LPRECV_MSG_DATA)wParam;
	if (NULL == lpRecvCallbackData || lpRecvCallbackData->dwNumberOfBytesRecv <= 0)
	{
		TSDEBUG(L"wParam is NULL");
		return 0;
	}
	DWORD dwNumberOfBytesRead = 0;
	TSDEBUG(L"dwNumberOfBytesRecv = %u", lpRecvCallbackData->dwNumberOfBytesRecv);

	while (dwNumberOfBytesRead < lpRecvCallbackData->dwNumberOfBytesRecv)
	{
		//判断包头是否已经接收完成
		TSDEBUG(L"dwNumberOfBytesRead = %u, ulHeaderSize = %u, ulHeaderFillCount = %u, ulDataSize = %u, ulDataFillCount = %u", dwNumberOfBytesRead, m_SocketIpcData.ulHeaderSize, m_SocketIpcData.ulHeaderFillCount, m_SocketIpcData.ulDataSize, m_SocketIpcData.ulDataFillCount);
		if (m_SocketIpcData.ulHeaderFillCount != m_SocketIpcData.ulHeaderSize)
		{
			//继续读取剩余包头
			unsigned long ulHeaderReadSize = m_SocketIpcData.ulHeaderSize - m_SocketIpcData.ulHeaderFillCount;
			if (ulHeaderReadSize > lpRecvCallbackData->dwNumberOfBytesRecv - dwNumberOfBytesRead)
			{
				ulHeaderReadSize = lpRecvCallbackData->dwNumberOfBytesRecv - dwNumberOfBytesRead;
			}
			TSDEBUG(L"ulHeaderReadSize = %u", ulHeaderReadSize);

			memcpy((m_SocketIpcData.szHeader + m_SocketIpcData.ulHeaderFillCount), (lpRecvCallbackData->szData + dwNumberOfBytesRead), ulHeaderReadSize);
			m_SocketIpcData.ulHeaderFillCount += ulHeaderReadSize;
			dwNumberOfBytesRead += ulHeaderReadSize;

			TSDEBUG(L"ulHeaderFillCount = %u, ulHeaderSize = %u", m_SocketIpcData.ulHeaderFillCount, m_SocketIpcData.ulHeaderSize);
			if (m_SocketIpcData.ulHeaderFillCount == m_SocketIpcData.ulHeaderSize)
			{
				//读取包头信息
				LPSOCKETIPC_DATA_HEADER lpSocketIpcDataHeader = (LPSOCKETIPC_DATA_HEADER)m_SocketIpcData.szHeader;
				TSDEBUG(L"ucFlags = %u, ulType = %u, ulDataLen = %u", lpSocketIpcDataHeader->ucFlags, lpSocketIpcDataHeader->ulType, lpSocketIpcDataHeader->ulDataLen);
				m_SocketIpcData.szData = new char[lpSocketIpcDataHeader->ulDataLen];
				m_SocketIpcData.ulDataSize = lpSocketIpcDataHeader->ulDataLen;
			}
		}
		else if (m_SocketIpcData.ulDataFillCount != m_SocketIpcData.ulDataSize)
		{
			//继续读取剩余数据
			unsigned long ulDataReadSize = m_SocketIpcData.ulDataSize - m_SocketIpcData.ulDataFillCount;
			if (ulDataReadSize > lpRecvCallbackData->dwNumberOfBytesRecv - dwNumberOfBytesRead)
			{
				ulDataReadSize = lpRecvCallbackData->dwNumberOfBytesRecv - dwNumberOfBytesRead;
			}
			TSDEBUG(L"ulDataReadSize = %u", ulDataReadSize);

			memcpy((m_SocketIpcData.szData + m_SocketIpcData.ulDataFillCount), (lpRecvCallbackData->szData + dwNumberOfBytesRead), ulDataReadSize);
			m_SocketIpcData.ulDataFillCount += ulDataReadSize;
			dwNumberOfBytesRead += ulDataReadSize;

			TSDEBUG(L"ulDataFillCount = %u, ulDataSize = %u", m_SocketIpcData.ulDataFillCount, m_SocketIpcData.ulDataSize);
			if (m_SocketIpcData.ulDataFillCount == m_SocketIpcData.ulDataSize)
			{
				//回调上层函数
				LPSOCKETIPC_STREAM lpSocketIpcStream = CommonTool::CreateSocketIpcStream();

				long lRet = CommonTool::StreamWriteString(lpSocketIpcStream, m_SocketIpcData.szData);
				lpSocketIpcStream->uiCurPos = 0;
				if (m_RecvCallbackInfo.pfnRecvCallback)
				{
					LPSOCKETIPC_DATA_HEADER lpSocketIpcDataHeader = (LPSOCKETIPC_DATA_HEADER)m_SocketIpcData.szHeader;
					m_RecvCallbackInfo.pfnRecvCallback(0, lpSocketIpcDataHeader->ulType, lpSocketIpcStream, m_RecvCallbackInfo.pUserData);
				}

				delete [] m_SocketIpcData.szData;
				m_SocketIpcData.szData = NULL;
				memset(m_SocketIpcData.szHeader, 0, m_SocketIpcData.ulHeaderSize);
				m_SocketIpcData.ulDataFillCount = 0;
				m_SocketIpcData.ulDataSize = 0;
				m_SocketIpcData.ulHeaderFillCount = 0;

				delete [] lpSocketIpcStream->szBuffer;
				delete lpSocketIpcStream;
				lpSocketIpcStream = NULL;
			}
		}
		TSDEBUG(L"dwNumberOfBytesRead = %u", dwNumberOfBytesRead);
	}

	delete lpRecvCallbackData;
	lpRecvCallbackData = NULL;

	return 0;
}

LRESULT IPC::OnDisconnectCallback(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TSDEBUG(L"Enter");

	m_DisconnectCallbackInfo.pfnDisconnectCallback(0, m_DisconnectCallbackInfo.pUserData);

	return 0;
}


long IPC::Init(IpcType ipcType)
{
	TSDEBUG(L"Enter, ipcType = %d", ipcType);

	if (!IsWindow())
	{
		m_hWnd = Create(HWND_MESSAGE, NULL, L"IPCServer_WndName");
		if (NULL == m_hWnd)
		{
			DWORD dwErrCode = ::GetLastError();
			TSDEBUG(L"Create failed, error code is %u", dwErrCode);
			return dwErrCode;
		}
	}

	//初始化m_DataCommunicationInterface
	if (IpcType_Client == ipcType)
	{
		m_DataCommunicationInterface = new ClientDataCommunication();
	}
	else
	{
		m_DataCommunicationInterface = new ServerDataCommunication();
	}
	if (NULL == m_DataCommunicationInterface)
	{
		TSDEBUG(L"Init failed, m_DataCommunicationInterface = NULL");
		return -1;
	}

	//初始化
	return m_DataCommunicationInterface->Init(m_hWnd);
}

long IPC::Link(ONLINKCALLBACK pfnOnListenCallback, void* pUserData)
{
	TSDEBUG(L"Enter");

	m_LinkCallbackInfo.pfnOnLinkCallback = pfnOnListenCallback;
	m_LinkCallbackInfo.pUserData = pUserData;

	return m_DataCommunicationInterface->Link();
}

long IPC::Send(DWORD dwId, LPSOCKETIPC_STREAM lpSocketIpcStream, ONSENDCALLBACK pfnOnSendCallback, void* pUserData)
{
	TSDEBUG(L"Enter");

	if (NULL == lpSocketIpcStream)
	{
		TSDEBUG(L"Invalided param, lpSocketIpcStream is NULL");
		return 0;
	}

	//存储回调信息
	IPC_SEND_CALLBACK ipcserverSendCallback;
	ipcserverSendCallback.dwId = dwId;
	ipcserverSendCallback.pfnOnSendCallback = pfnOnSendCallback;
	ipcserverSendCallback.pUserData = pUserData;
	m_mSendCallbackInfo[dwId] = ipcserverSendCallback;

	long lLen = CommonTool::CombineSocketIpcData(0, 0, lpSocketIpcStream, NULL);
	TSDEBUG(L"lLen = %d", lLen);

	char* szData = new char[lLen];
	memset(szData, 0, lLen);
	long lRet = CommonTool::CombineSocketIpcData(0, 0, lpSocketIpcStream, szData);
	if (0 != lRet)
	{
		TSDEBUG(L"CombineSocketIpcData failed, error code is %d", lRet);
		delete [] szData;
		szData = NULL;
		return lRet;
	}

	lRet = m_DataCommunicationInterface->SendData(0, szData, lLen);

	delete lpSocketIpcStream;
	lpSocketIpcStream = NULL;
	delete [] szData;
	szData = NULL;
	return lRet;
}

long IPC::RegisterRecvCallback(ONRECVCALLBACK pfnOnRecvCallback, void* pUserData)
{
	TSDEBUG(L"Enter");

	m_RecvCallbackInfo.pfnRecvCallback = pfnOnRecvCallback;
	m_RecvCallbackInfo.pUserData = pUserData;

	return 0;
}

long IPC::RegisterDislinkCallback(ONDISCONNECTCALLBACK pfnDisconnectCallback, void* pUserData)
{
	TSDEBUG(L"Enter");

	m_DisconnectCallbackInfo.pfnDisconnectCallback = pfnDisconnectCallback;
	m_DisconnectCallbackInfo.pUserData = pUserData;

	return 0;
}