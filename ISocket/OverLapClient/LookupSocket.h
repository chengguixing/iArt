#pragma once
#include "afx.h"


class CClientDlg;

class CLookupSocket :
	public CObject
{
public:
	CLookupSocket(CClientDlg* pDlg, SOCKET s, WSAEVENT hEventOut, WSAEVENT hEventIn);
	~CLookupSocket(void);


public:
	void HandleData(void);
	BOOL Lookup(CString& strWord);

protected:
	BOOL Send(void);
	BOOL RecvHeader(void);
	BOOL RecvPacket(void);


private:
	CLookupSocket();


public:
	WSAOVERLAPPED m_oIO;
	WSAOVERLAPPED m_iIO;


protected:
	CClientDlg* m_pClientDlg;
	SOCKET m_s;
	CString m_strWord;
	CString m_StrMeaning;
	PACKETHDR m_hdrRecv;
};

