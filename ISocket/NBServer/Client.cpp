#include "Client.h"
#include<stdio.h>


CClient::CClient(const SOCKET sClient, const sockaddr_in& addrClient)
{
	m_hThreadRecv = NULL;
	m_hThreadSend = NULL;
	m_socket = sClient;
	m_addr = addrClient;
	m_bConning = FALSE;
	m_bExit = FALSE;
	memset(m_data.buf, 0, MAX_NUM_BUF);

	m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	InitializeCriticalSection(&m_cs);
}

CClient::~CClient()
{
	closesocket(m_socket);
	m_socket = INVALID_SOCKET;
	DeleteCriticalSection(&m_cs);
	CloseHandle(m_hEvent);
}

BOOL CClient::StartRunning(void)
{
	m_bConning = TRUE;
	unsigned long ulThreadId;
	m_hThreadRecv = CreateThread(NULL, 0, RecvDataThread, this, 0, &ulThreadId);
	if (NULL == m_hThreadRecv)
	{
		return FALSE;
	}
	else
	{
		CloseHandle(m_hThreadRecv);
	}

	m_hThreadSend = CreateThread(NULL, 0, SendDataThread, this, 0, &ulThreadId);
	if (NULL == m_hThreadSend)
	{
		return FALSE;
	}
	else
	{
		CloseHandle(m_hThreadSend);
	}
	return TRUE;
}

DWORD CClient::RecvDataThread(void* pParam)
{
	CClient* pClient = (CClient*)pParam;
	int reVal;
	char temp[MAX_NUM_BUF];

	memset(temp, 0, MAX_NUM_BUF);

	for (; pClient->m_bConning;)
	{
		reVal = recv(pClient->m_socket, temp, MAX_NUM_BUF, 0);

		if (SOCKET_ERROR == reVal)
		{
			int nErrCode = WSAGetLastError();

			if (WSAEWOULDBLOCK == nErrCode)
			{
				continue;
			}
			else if (WSAENETDOWN == nErrCode ||
				WSAETIMEDOUT == nErrCode ||
				WSAECONNRESET == nErrCode)
			{
				break;
			}
		}
		if (reVal == 0)
		{
			break;
		}
		if (reVal > HEADERLEN)
		{
			pClient->HandleData(temp);
			SetEvent(pClient->m_hEvent);
			memset(temp, 0, MAX_NUM_BUF);
		}

		Sleep(TIMEFOR_THREAD_CLIENT);
	}

	pClient->m_bConning = FALSE;
	SetEvent(pClient->m_hEvent);

	return 0;
}

DWORD CClient::SendDataThread(void* pParam)
{
	CClient* pClient = (CClient*)pParam;

	for (; pClient->m_bConning;)
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(pClient->m_hEvent, INFINITE))
		{
			if (!pClient->m_bConning)
			{
				pClient->m_bExit = TRUE;
				break;
			}

			EnterCriticalSection(&pClient->m_cs);

			phdr pHeader = (phdr)pClient->m_data.buf;
			int nSendlen = pHeader->len;


			int val = send(pClient->m_socket, pClient->m_data.buf, nSendlen, 0);

			if (SOCKET_ERROR == val)
			{
				int nErrCode = WSAGetLastError();
				if (nErrCode == WSAEWOULDBLOCK)
				{
					continue;
				}
				else if (WSAENETDOWN == nErrCode ||
					WSAETIMEDOUT == nErrCode ||
					WSAECONNRESET == nErrCode)
				{
					LeaveCriticalSection(&pClient->m_cs);
					pClient->m_bConning = FALSE;
					pClient->m_bExit = TRUE;
					break;
				}
				else
				{
					LeaveCriticalSection(&pClient->m_cs);
					pClient->m_bConning = FALSE;
					pClient->m_bExit = TRUE;
					break;
				}
			}
			LeaveCriticalSection(&pClient->m_cs);
			ResetEvent(&pClient->m_hEvent);
		}
	}
	return 0;
}

void CClient::HandleData(const char* pExpr)
{
	memset(m_data.buf, 0, MAX_NUM_BUF);


	if (BYEBYE == ((phdr)pExpr)->type)
	{
		EnterCriticalSection(&m_cs);
		phdr pHeaderSend = (phdr)m_data.buf;
		pHeaderSend->type = BYEBYE;
		pHeaderSend->len = HEADERLEN + strlen("OK");
		memcpy(m_data.buf + HEADERLEN, "OK", strlen("OK"));
		LeaveCriticalSection(&m_cs);
	}
	else
	{
		int nFirNum;
		int nSecNum;
		char cOper;
		int nResult;

		sscanf(pExpr + HEADERLEN, "%d%c%d", &nFirNum, &cOper, &nSecNum);

		switch (cOper)
		{
		case '+':
			{
				nResult = nFirNum + nSecNum;
				break;
			}
		case '-':
			{
				nResult = nFirNum - nSecNum;
				break;
			}
		case '*':
			{
				nResult = nFirNum * nSecNum;
				break;
			}
		case '/':
			{
				if (ZERO == nSecNum)
				{
					nResult = INVALID_NUM;
				}
				else
				{
					nResult = nFirNum / nSecNum;
				}			
				break;
			}
		default:
			nResult = INVALID_OPERATOR;
			break;
		}
		char temp[MAX_NUM_BUF];
		char cEqu = '=';
		sprintf(temp, "%d%c%d%c%d", nFirNum, cOper, nSecNum, cEqu, nResult);


		EnterCriticalSection(&m_cs);
		phdr pHeaderSend = (phdr)m_data.buf;
		pHeaderSend->type = EXPRESSION;
		pHeaderSend->len = HEADERLEN + strlen(temp);
		memcpy(m_data.buf + HEADERLEN, temp, strlen(temp));
		LeaveCriticalSection(&m_cs);
	}
}