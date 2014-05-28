#include "stdafx.h"
#include "LookupSocket.h"
#include "OverLapClientDlg.h"


CLookupSocket::CLookupSocket(COverLapClientDlg* pDlg, SOCKET s, WSAEVENT hEventOut, WSAEVENT hEventIn)
{
	m_pClientDlg = pDlg;
	m_s = s;
	ZeroMemory(&m_oIO, sizeof(m_oIO));
	m_oIO.hEvent = hEventOut;
	ZeroMemory(&m_iIO, sizeof(m_iIO));
	m_iIO.hEvent = hEventIn;

	ZeroMemory(&m_hdrRecv, HEADERLEN);
}

CLookupSocket::~CLookupSocket(void)
{
	closesocket(m_s);
	WSACloseEvent(m_iIO.hEvent);
	WSACloseEvent(m_oIO.hEvent);
	m_s = INVALID_SOCKET;
}



void CLookupSocket::HandleData(void)
{
	if (m_hdrRecv.len != 0)
	{
		RecvPacket();
		ZeroMemory(&m_hdrRecv, HEADERLEN);
	}
	else
	{
		m_pClientDlg->ShowMeaning((TCHAR*)(LPCTSTR)m_StrMeaning);
		m_StrMeaning.ReleaseBuffer();
	}
}

BOOL CLookupSocket::Lookup(CString& strWord)
{
	m_strWord.Empty();
	m_strWord = strWord;
	if (m_strWord.IsEmpty())
	{
		return TRUE;
	}
	if (FALSE == Send())
	{
		return FALSE;
	}
	if (FALSE == RecvHeader())
	{
		return FALSE;
	}
	return TRUE;
}


BOOL CLookupSocket::Send(void)
{
	DWORD dwNumberOfBytesSent;
	DWORD dwFlags = 0;
	int nErrCode;


	if (m_strWord.IsEmpty())
	{
		return TRUE;
	}

	WSABUF wsaSendBuf[2];
	PACKETHDR hdr;
	hdr.type = ETOC;
	hdr.len = m_strWord.GetLength();
	wsaSendBuf[0].buf = (char*)&hdr;
	wsaSendBuf[0].len = HEADERLEN;


	wsaSendBuf[1].buf = (char*)(LPCTSTR)m_strWord;
	wsaSendBuf[1].len = m_strWord.GetLength();


	nErrCode = WSASend(m_s, wsaSendBuf, 2, &dwNumberOfBytesSent, dwFlags, &m_oIO, NULL);

	if (SOCKET_ERROR == nErrCode)
	{
		if (WSA_IO_PENDING == WSAGetLastError())
		{
			return TRUE;
		}
		else
		{
			AfxMessageBox(TEXT("WSASendº¯Êýµ÷ÓÃÊ§°Ü!"));
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CLookupSocket::RecvHeader(void)
{
	DWORD dwBytesRecved;
	DWORD dwFlags = 0;

	ZeroMemory(&m_hdrRecv, HEADERLEN);

	WSABUF wsaRecv;
	wsaRecv.buf = (char*)&m_hdrRecv;
	wsaRecv.len = HEADERLEN;

	if (SOCKET_ERROR == WSARecv(m_s, &wsaRecv, 1, &dwBytesRecved,&dwFlags, &m_iIO, NULL))
	{
		int err = WSAGetLastError();
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			return FALSE;
		}
	}
	return TRUE;
}
BOOL CLookupSocket::RecvPacket(void)
{
	DWORD dwBytesRecved;
	DWORD dwFlags = 0;

	m_StrMeaning.Empty();

	WSABUF wsaRecv;
	wsaRecv.buf = (char*)m_StrMeaning.GetBuffer(m_hdrRecv.len);
	wsaRecv.len = m_hdrRecv.len;


	if (SOCKET_ERROR == WSARecv(m_s, &wsaRecv, 1, &dwBytesRecved, &dwFlags, &m_iIO, NULL))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			return FALSE;
		}
	}
	return TRUE;
}

