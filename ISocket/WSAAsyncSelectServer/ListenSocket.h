#pragma once

// CListenSocket ����Ŀ��

class CWSAAsyncSelectServerDlg;

class CListenSocket : public CSocket
{
public:
	CListenSocket(CWSAAsyncSelectServerDlg*);
	virtual ~CListenSocket();

public:
public:
protected:
	virtual void OnAccept(int nErrorCode);
protected:
	CWSAAsyncSelectServerDlg* m_pServerDlg;
};


