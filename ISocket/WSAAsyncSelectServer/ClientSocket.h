#pragma once

// CClientSocket ÃüÁîÄ¿±ê


class CWSAAsyncSelectServerDlg;


class CClientSocket : public CSocket
{
public:
	CClientSocket(CWSAAsyncSelectServerDlg*);
	virtual ~CClientSocket();

public:
	void Init();
	void SendUserList(CChatPacket*);
	void SendUserMsg(CChatPacket*);
	void SaveUserInfo(SOCKADDR_IN clientAddr);
	void SaveUserInfo(const CUserInfo& userInfo);
	CUserInfo GetUserInfo(void);

protected:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);


protected:
	CWSAAsyncSelectServerDlg* m_pServDlg;
	CSocketFile* m_pFile;
	CUserInfo m_userInfo;
};


