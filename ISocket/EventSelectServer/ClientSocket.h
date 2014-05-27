#pragma once
#include "afx.h"
#include "EventSelectServerDlg.h"
#include "UserInfo.h"


class CEventSelectServerDlg;

class CClientSocket : public CObject
{
public:
	CClientSocket(CEventSelectServerDlg* pServDlg, SOCKET s, WSAEVENT hEvent);
	virtual ~CClientSocket(void);

public:
	BOOL Send(const u_short type, const CString& strData);
	BOOL Recv(void);
	void SaveUserInfo(const CUserInfo& userInfo);

public:
	SOCKET m_s;
	WSAEVENT m_hEvent;
	CUserInfo m_UserInfo;

private:
	CClientSocket();

private:
	CEventSelectServerDlg* m_pServDlg;

};

