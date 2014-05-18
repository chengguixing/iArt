/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.1.2
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 75375912
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "stdafx.h"
#include "TcpClient.h"
#include "../WaitFor.h"

#ifndef _WIN32_WCE
	#include <process.h>
#else
	#define _beginthreadex	::CreateThread
#endif

volatile CONNID CTcpClient::sm_dwConnID = 0;

BOOL CTcpClient::Start(LPCTSTR pszRemoteAddress, USHORT usPort, BOOL bAsyncConnect)
{
	if(!CheckParams() || !CheckStarting())
		return FALSE;

	BOOL isOK		= FALSE;
	m_bAsyncConnect	= bAsyncConnect;

	if(CreateClientSocket())
	{
		if(FirePrepareConnect(m_dwConnID, m_soClient) != ISocketListener::HR_ERROR)
		{
			if(ConnectToServer(pszRemoteAddress, usPort))
			{
				if(CreateWorkerThread())
					isOK = TRUE;
				else
					SetLastError(CE_WORKER_CREATE_FAIL, __FUNCTION__, ERROR_CREATE_FAILED);
			}
			else
				SetLastError(CE_CONNECT_SERVER_FAIL, __FUNCTION__, ::WSAGetLastError());
		}
		else
			SetLastError(CE_SOCKET_PREPARE_FAIL, __FUNCTION__, ERROR_FUNCTION_FAILED);
	}
	else
		SetLastError(CE_SOCKET_CREATE_FAIL, __FUNCTION__, ::WSAGetLastError());

	if(!isOK) Stop();

	return isOK;
}

BOOL CTcpClient::CheckParams()
{
	m_itPool.SetItemCapacity((int)m_dwSocketBufferSize);
	m_itPool.SetPoolSize((int)m_dwFreeBufferPoolSize);
	m_itPool.SetPoolHold((int)m_dwFreeBufferPoolHold);

	if((int)m_dwSocketBufferSize > 0)
		if((int)m_dwFreeBufferPoolSize >= 0)
			if((int)m_dwFreeBufferPoolHold >= 0)
				if((int)m_dwKeepAliveTime >= 0)
					if((int)m_dwKeepAliveInterval >= 0)
						return TRUE;

	SetLastError(CE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
	return FALSE;
}

BOOL CTcpClient::CheckStarting()
{
	if(m_enState == SS_STOPED)
		m_enState = SS_STARTING;
	else
	{
		SetLastError(CE_ILLEGAL_STATE, __FUNCTION__, ERROR_INVALID_OPERATION);
		return FALSE;
	}

	return TRUE;
}

BOOL CTcpClient::CheckStoping()
{
	if(m_enState == SS_STARTED || m_enState == SS_STARTING)
		m_enState = SS_STOPING;
	else
	{
		SetLastError(CE_ILLEGAL_STATE, __FUNCTION__, ERROR_INVALID_OPERATION);
		return FALSE;
	}

	return TRUE;
}

BOOL CTcpClient::CreateClientSocket()
{
	m_soClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_soClient != INVALID_SOCKET)
	{
#ifndef _WIN32_WCE
		BOOL bOnOff	= (m_dwKeepAliveTime > 0 && m_dwKeepAliveInterval > 0);
		VERIFY(::SSO_KeepAliveVals(m_soClient, bOnOff, m_dwKeepAliveTime, m_dwKeepAliveInterval) == NO_ERROR);
#endif

		m_evSocket	= ::WSACreateEvent();
		ASSERT(m_evSocket != WSA_INVALID_EVENT);

		m_dwConnID	= ::GenerateConnectionID(sm_dwConnID);

		return TRUE;
	}

	return FALSE;
}

BOOL CTcpClient::ConnectToServer(LPCTSTR pszRemoteAddress, USHORT usPort)
{
	TCHAR szAddress[40];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);

	if(!::GetIPAddress(pszRemoteAddress, szAddress, iAddressLen))
		return FALSE;

	SOCKADDR_IN addr;
	if(!::sockaddr_A_2_IN(AF_INET, szAddress, usPort, addr))
		return FALSE;

	BOOL isOK = FALSE;

	if(m_bAsyncConnect)
	{
		if(::WSAEventSelect(m_soClient, m_evSocket, FD_CONNECT | FD_CLOSE) != SOCKET_ERROR)
		{
			int rc = connect(m_soClient, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
			isOK = (rc == NO_ERROR || (rc == SOCKET_ERROR && ::WSAGetLastError() == WSAEWOULDBLOCK));
		}
	}
	else
	{
		if(connect(m_soClient, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) != SOCKET_ERROR)
		{
			if(::WSAEventSelect(m_soClient, m_evSocket, FD_READ | FD_WRITE | FD_CLOSE) != SOCKET_ERROR)
			{
				if(FireConnect(m_dwConnID) != ISocketListener::HR_ERROR)
				{
					m_enState	= SS_STARTED;
					isOK		= TRUE;
				}
			}
		}
	}

	return isOK;
}

BOOL CTcpClient::CreateWorkerThread()
{
	m_hWorker = (HANDLE)_beginthreadex(nullptr, 0, WorkerThreadProc, (LPVOID)this, 0, &m_dwWorkerID);

	return m_hWorker != nullptr;
}

#ifndef _WIN32_WCE
	UINT
#else
	DWORD
#endif
	WINAPI CTcpClient::WorkerThreadProc(LPVOID pv)
{
	TRACE("---------------> Client Worker Thread 0x%08X started <---------------\n", ::GetCurrentThreadId());

	CTcpClient* pClient	= (CTcpClient*)pv;
	HANDLE hEvents[]	= {pClient->m_evSocket, pClient->m_evBuffer, pClient->m_evWorker};

	pClient->m_rcBuffer.Malloc(pClient->m_dwSocketBufferSize);

	while(pClient->HasStarted())
	{
		DWORD retval = ::WSAWaitForMultipleEvents(3, hEvents, FALSE, WSA_INFINITE, FALSE);

		if(retval == WSA_WAIT_EVENT_0)
		{
			if(!pClient->ProcessNetworkEvent())
			{
				if(pClient->HasStarted())
					pClient->Stop();

				break;
			}
		}
		else if(retval == WSA_WAIT_EVENT_0 + 1)
		{
			if(!pClient->SendData())
			{
				if(pClient->HasStarted())
					pClient->Stop();

				break;
			}
		}
		else if(retval == WSA_WAIT_EVENT_0 + 2)
			break;
		else
			ASSERT(FALSE);
	}

	TRACE("---------------> Client Worker Thread 0x%08X stoped <---------------\n", ::GetCurrentThreadId());

	return 0;
}

BOOL CTcpClient::ProcessNetworkEvent()
{
	BOOL bContinue = TRUE;
	WSANETWORKEVENTS events;
	
	int rc = ::WSAEnumNetworkEvents(m_soClient, m_evSocket, &events);

	if(rc == SOCKET_ERROR)
	{
		int code = ::WSAGetLastError();
		SetLastError(CE_NETWORK_ERROR, __FUNCTION__, code);

		VERIFY(::WSAResetEvent(m_evSocket));
		FireError(m_dwConnID, SO_UNKNOWN, code);

		bContinue = FALSE;
	}

	if(bContinue && events.lNetworkEvents & FD_READ)
	{
		int iCode = events.iErrorCode[FD_READ_BIT];

		if(iCode == 0)
			bContinue = ReadData();
		else
		{
			SetLastError(CE_NETWORK_ERROR, __FUNCTION__, iCode);
			FireError(m_dwConnID, SO_RECEIVE, iCode);
			bContinue = FALSE;
		}
	}

	if(bContinue && events.lNetworkEvents & FD_WRITE)
	{
		int iCode = events.iErrorCode[FD_WRITE_BIT];

		if(iCode == 0)
			bContinue = SendData();
		else
		{
			SetLastError(CE_NETWORK_ERROR, __FUNCTION__, iCode);
			FireError(m_dwConnID, SO_SEND, iCode);
			bContinue = FALSE;
		}
	}

	if(m_bAsyncConnect && bContinue && events.lNetworkEvents & FD_CONNECT)
	{
		int iCode = events.iErrorCode[FD_CONNECT_BIT];

		if(iCode == 0)
		{
			if(::WSAEventSelect(m_soClient, m_evSocket, FD_READ | FD_WRITE | FD_CLOSE) != SOCKET_ERROR)
			{
				if(FireConnect(m_dwConnID) != ISocketListener::HR_ERROR)
					m_enState = SS_STARTED;
				else
					iCode = ERROR_FUNCTION_FAILED;
			}
			else
				iCode = ::WSAGetLastError();
		}

		if(iCode != 0)
		{
			SetLastError(CE_NETWORK_ERROR, __FUNCTION__, iCode);
			FireError(m_dwConnID, SO_CONNECT, iCode);
			bContinue = FALSE;
		}
	}

	if(bContinue && events.lNetworkEvents & FD_CLOSE)
	{
		int iCode = events.iErrorCode[FD_CLOSE_BIT];

		if(iCode == 0)
			FireClose(m_dwConnID);
		else
		{
			SetLastError(CE_NETWORK_ERROR, __FUNCTION__, iCode);
			FireError(m_dwConnID, SO_UNKNOWN, iCode);
		}

		bContinue = FALSE;
	}

	return bContinue;
}

BOOL CTcpClient::ReadData()
{
	while(TRUE)
	{
		int rc = recv(m_soClient, (char*)(BYTE*)m_rcBuffer, m_dwSocketBufferSize, 0);

		if(rc > 0)
		{
			if(FireReceive(m_dwConnID, m_rcBuffer, rc) == ISocketListener::HR_ERROR)
			{
				TRACE("<C-CNNID: %Iu> OnReceive() event return 'HR_ERROR', connection will be closed !\n", m_dwConnID);

				SetLastError(CE_DATA_PROC_ERROR, __FUNCTION__, ERROR_FUNCTION_FAILED);
				FireError(m_dwConnID, SO_RECEIVE, ERROR_FUNCTION_FAILED);

				return FALSE;
			}
		}
		else if(rc == SOCKET_ERROR)
		{
			int code = ::WSAGetLastError();

			if(code == WSAEWOULDBLOCK)
				break;
			else
			{
				SetLastError(CE_NETWORK_ERROR, __FUNCTION__, code);
				FireError(m_dwConnID, SO_RECEIVE, code);

				return FALSE;
			}
		}
		else if(rc == 0)
		{
			FireClose(m_dwConnID);
			return FALSE;
		}
		else
			ASSERT(FALSE);
	}

	return TRUE;
}

BOOL CTcpClient::SendData()
{
	BOOL isOK = TRUE;

	while(TRUE)
	{
		TItemPtr itPtr(m_itPool, GetSendBuffer());

		if(itPtr.IsValid())
		{
			ASSERT(!itPtr->IsEmpty());

			isOK = DoSendData(itPtr);

			if(isOK)
			{
				if(!itPtr->IsEmpty())
				{
					CCriSecLock locallock(m_scSend);
					m_lsSend.PushFront(itPtr.Detach());
					
					break;;
				}
			}
			else
				break;
		}
		else
			break;
	}

	return isOK;
}

TItem* CTcpClient::GetSendBuffer()
{
	TItem* pItem = nullptr;

	if(m_lsSend.Size() > 0)
	{
		CCriSecLock locallock(m_scSend);

		if(m_lsSend.Size() > 0)
			pItem = m_lsSend.PopFront();
	}

	return pItem;
}

BOOL CTcpClient::DoSendData(TItem* pItem)
{
	while(!pItem->IsEmpty())
	{
		int rc = send(m_soClient, (char*)pItem->Ptr(), min(pItem->Size(), (int)m_dwSocketBufferSize), 0);

		if(rc > 0)
		{
			if(FireSend(m_dwConnID, pItem->Ptr(), rc) == ISocketListener::HR_ERROR)
			{
				TRACE("<C-CNNID: %Iu> OnSend() event should not return 'HR_ERROR' !!\n", m_dwConnID);
				ASSERT(FALSE);
			}

			pItem->Reduce(rc);
		}
		else if(rc == SOCKET_ERROR)
		{
			int code = ::WSAGetLastError();

			if(code == WSAEWOULDBLOCK)
				break;
			else
			{
				SetLastError(CE_NETWORK_ERROR, __FUNCTION__, code);
				FireError(m_dwConnID, SO_SEND, code);

				return FALSE;
			}
		}
		else
			ASSERT(FALSE);
	}

	return TRUE;
}

BOOL CTcpClient::Stop()
{
	BOOL bNeedFireClose			= FALSE;
	EnServiceState enCurState	= m_enState;
	DWORD dwCurrentThreadID		= ::GetCurrentThreadId();

	if(!CheckStoping())
		return FALSE;

	if(enCurState == SS_STARTED && dwCurrentThreadID != m_dwWorkerID)
		bNeedFireClose = TRUE;

	WaitForWorkerThreadEnd(dwCurrentThreadID);

	if(bNeedFireClose)
		FireClose(m_dwConnID);

	if(m_evSocket != nullptr)
	{
		::WSACloseEvent(m_evSocket);
		m_evSocket	= nullptr;
	}

	if(m_soClient != INVALID_SOCKET)
	{
		shutdown(m_soClient, SD_SEND);
		closesocket(m_soClient);
		m_soClient	= INVALID_SOCKET;
	}

	Reset();

	return TRUE;
}

void CTcpClient::Reset()
{
	m_rcBuffer.Free();
	m_evBuffer.Reset();
	m_evWorker.Reset();
	m_lsSend.Clear();
	m_itPool.Clear();

	m_enState = SS_STOPED;
}

void CTcpClient::WaitForWorkerThreadEnd(DWORD dwCurrentThreadID)
{
	if(m_hWorker != nullptr)
	{
		if(dwCurrentThreadID != m_dwWorkerID)
		{
			m_evWorker.Set();
			VERIFY(::WaitForSingleObject(m_hWorker, INFINITE) == WAIT_OBJECT_0);
		}

		::CloseHandle(m_hWorker);

		m_hWorker		= nullptr;
		m_dwWorkerID	= 0;
	}
}

BOOL CTcpClient::Send(CONNID dwConnID, const BYTE* pBuffer, int iLength)
{
	ASSERT(pBuffer && iLength > 0);

	if(!HasStarted())
	{
		SetLastError(CE_ILLEGAL_STATE, __FUNCTION__, ERROR_INVALID_STATE);
		return FALSE;
	}

	if(!pBuffer || iLength <= 0)
	{
		SetLastError(CE_INVALID_PARAM, __FUNCTION__, ERROR_INCORRECT_SIZE);
		return FALSE;
	}

	{
		CCriSecLock locallock(m_scSend);

		if(!HasStarted())
		{
			SetLastError(CE_ILLEGAL_STATE, __FUNCTION__, ERROR_INVALID_STATE);
			return FALSE;
		}

		m_lsSend.Cat(pBuffer, iLength);
		m_evBuffer.Set();
	}

	return TRUE;
}

void CTcpClient::SetLastError(EnClientError code, LPCSTR func, int ec)
{
	m_enLastError = code;

	TRACE("%s --> Error: %d, EC: %d\n", func, code, ec);
}

BOOL CTcpClient::GetLocalAddress(LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	return ::GetSocketLocalAddress(m_soClient, lpszAddress, iAddressLen, usPort);
}

LPCTSTR CTcpClient::GetLastErrorDesc()
{
	switch(m_enLastError)
	{
	case CE_OK:						return _T("�ɹ�");
	case CE_ILLEGAL_STATE:			return _T("��ǰ״̬���������");
	case CE_INVALID_PARAM:			return _T("�Ƿ�����");
	case CE_SOCKET_CREATE_FAIL:		return _T("���� Client Socket ʧ��");
	case CE_SOCKET_PREPARE_FAIL:	return _T("���� Client Socket ʧ��");
	case CE_CONNECT_SERVER_FAIL:	return _T("���ӷ�����ʧ��");
	case CE_WORKER_CREATE_FAIL:		return _T("���������߳�ʧ��");
	case CE_NETWORK_ERROR:			return _T("�������");
	case CE_DATA_PROC_ERROR:		return _T("���ݴ������");
	default: ASSERT(FALSE);			return _T("");
	}
}
