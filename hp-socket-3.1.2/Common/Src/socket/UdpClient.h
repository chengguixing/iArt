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
 
#pragma once

#include "SocketHelper.h"
#include "../Event.h"
#include "../bufferptr.h"
#include "../bufferpool.h"
#include "../CriticalSection.h"

class CUdpClient : public IUdpClient
{
public:
	virtual BOOL Start	(LPCTSTR pszRemoteAddress, USHORT usPortt, BOOL bAsyncConnect = FALSE);
	virtual BOOL Stop	();
	virtual BOOL Send	(CONNID dwConnID, const BYTE* pBuffer, int iLength);
	virtual BOOL			HasStarted		()	{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState	GetState		()	{return m_enState;}
	virtual CONNID			GetConnectionID	()	{return m_dwConnID;};
	virtual EnClientError	GetLastError	()	{return m_enLastError;}
	virtual BOOL			GetLocalAddress	(LPTSTR lpszAddress, int& iAddressLen, USHORT& usPort);
	virtual LPCTSTR			GetLastErrorDesc();

public:
	virtual void SetMaxDatagramSize	(DWORD dwMaxDatagramSize)	{m_dwMaxDatagramSize = dwMaxDatagramSize;}
	virtual void SetDetectAttempts	(DWORD dwDetectAttempts)	{m_dwDetectAttempts	 = dwDetectAttempts;}
	virtual void SetDetectInterval	(DWORD dwDetectInterval)	{m_dwDetectInterval	 = dwDetectInterval;}

	virtual DWORD GetMaxDatagramSize()							{return m_dwMaxDatagramSize;}
	virtual DWORD GetDetectAttempts	()							{return m_dwDetectAttempts;}
	virtual DWORD GetDetectInterval	()							{return m_dwDetectInterval;}

	virtual void SetFreeBufferPoolSize	(DWORD dwFreeBufferPoolSize)	{m_dwFreeBufferPoolSize = dwFreeBufferPoolSize;}
	virtual void SetFreeBufferPoolHold	(DWORD dwFreeBufferPoolHold)	{m_dwFreeBufferPoolHold = dwFreeBufferPoolHold;}

	virtual DWORD GetFreeBufferPoolSize	()	{return m_dwFreeBufferPoolSize;}
	virtual DWORD GetFreeBufferPoolHold	()	{return m_dwFreeBufferPoolHold;}

protected:
	virtual ISocketListener::EnHandleResult FirePrepareConnect(CONNID dwConnID, SOCKET socket)
		{return m_psoListener->OnPrepareConnect(dwConnID, socket);}
	virtual ISocketListener::EnHandleResult FireConnect(CONNID dwConnID)
		{return m_psoListener->OnConnect(dwConnID);}
	virtual ISocketListener::EnHandleResult FireSend(CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_psoListener->OnSend(dwConnID, pData, iLength);}
	virtual ISocketListener::EnHandleResult FireReceive(CONNID dwConnID, const BYTE* pData, int iLength)
		{return m_psoListener->OnReceive(dwConnID, pData, iLength);}
	virtual ISocketListener::EnHandleResult FireReceive(CONNID dwConnID, int iLength)
		{return m_psoListener->OnReceive(dwConnID, iLength);}
	virtual ISocketListener::EnHandleResult FireClose(CONNID dwConnID)
		{return m_psoListener->OnClose(dwConnID);}
	virtual ISocketListener::EnHandleResult FireError(CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
		{return m_psoListener->OnError(dwConnID, enOperation, iErrorCode);}

	virtual void Reset();

private:
	BOOL CheckParams();
	BOOL CheckStarting();
	BOOL CheckStoping();
	BOOL CreateClientSocket();
	BOOL ConnectToServer(LPCTSTR pszRemoteAddress, USHORT usPort);
	BOOL CreateWorkerThread();
	BOOL CreateDetectorThread();
	BOOL ProcessNetworkEvent();
	BOOL ReadData();
	BOOL SendData();
	TItem* GetSendBuffer();
	void WaitForWorkerThreadEnd(DWORD dwCurrentThreadID);
	void WaitForDetectorThreadEnd(DWORD dwCurrentThreadID);

	BOOL HandleError();
	BOOL HandleRead(WSANETWORKEVENTS& events);
	BOOL HandleWrite(WSANETWORKEVENTS& events);
	BOOL HandleConnect(WSANETWORKEVENTS& events);
	BOOL HandleClosse(WSANETWORKEVENTS& events);

	int DetectConnection	();
	BOOL NeedDetectorThread	() {return m_dwDetectAttempts > 0 && m_dwDetectInterval > 0;}

	void SetLastError		(EnClientError code, LPCSTR func, int ec);

	static 
#ifndef _WIN32_WCE
	UINT
#else
	DWORD
#endif
	 WINAPI WorkerThreadProc(LPVOID pv);

	static 
#ifndef _WIN32_WCE
	UINT
#else
	DWORD
#endif
	 WINAPI DetecotrThreadProc(LPVOID pv);

private:
	static const DWORD DEFAULT_MAX_DATAGRAM_SIZE		= 1472;
	static const DWORD DEFAULT_FREE_BUFFER_POOL_SIZE	= 10;
	static const DWORD DEFAULT_FREE_BUFFER_POOL_HOLD	= 30;
	static const DWORD DEFAULT_DETECT_ATTEMPTS			= 3;
	static const DWORD DEFAULT_DETECT_INTERVAL			= 10;

	static volatile CONNID	sm_dwConnID;

public:
	CUdpClient(IUdpClientListener* psoListener)
	: m_psoListener			(psoListener)
	, m_lsSend				(m_itPool)
	, m_soClient			(INVALID_SOCKET)
	, m_evSocket			(nullptr)
	, m_dwConnID			(0)
	, m_hWorker				(nullptr)
	, m_dwWorkerID			(0)
	, m_hDetector			(nullptr)
	, m_dwDetectorID		(0)
	, m_bAsyncConnect		(FALSE)
	, m_enState				(SS_STOPED)
	, m_enLastError			(CE_OK)
	, m_dwDetectFails		(0)
	, m_dwMaxDatagramSize	(DEFAULT_MAX_DATAGRAM_SIZE)
	, m_dwFreeBufferPoolSize(DEFAULT_FREE_BUFFER_POOL_SIZE)
	, m_dwFreeBufferPoolHold(DEFAULT_FREE_BUFFER_POOL_HOLD)
	, m_dwDetectAttempts	(DEFAULT_DETECT_ATTEMPTS)
	, m_dwDetectInterval	(DEFAULT_DETECT_INTERVAL)
	{
		ASSERT(m_psoListener);
	}

	virtual ~CUdpClient()	{if(HasStarted()) Stop();}

protected:
	CItemPool		m_itPool;

private:
	CInitSocket		m_wsSocket;
	SOCKET			m_soClient;
	HANDLE			m_evSocket;
	CONNID			m_dwConnID;

	CEvt			m_evWorker;
	HANDLE			m_hWorker;

#ifndef _WIN32_WCE
	UINT
#else
	DWORD
#endif
					m_dwWorkerID;

	CEvt			m_evDetector;
	HANDLE			m_hDetector;

#ifndef _WIN32_WCE
	UINT
#else
	DWORD
#endif
					m_dwDetectorID;

	BOOL			m_bAsyncConnect;

	CEvt			m_evBuffer;
	CCriSec			m_scSend;
	TItemList		m_lsSend;
	CBufferPtr		m_rcBuffer;

	volatile EnServiceState	m_enState;
	volatile EnClientError	m_enLastError;
	volatile DWORD			m_dwDetectFails;

private:
	IUdpClientListener*	m_psoListener;
	
private:
	DWORD m_dwMaxDatagramSize;
	DWORD m_dwFreeBufferPoolSize;
	DWORD m_dwFreeBufferPoolHold;
	DWORD m_dwDetectAttempts;
	DWORD m_dwDetectInterval;
};
