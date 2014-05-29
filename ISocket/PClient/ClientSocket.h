#pragma once
#include "afx.h"
#include "PClientDlg.h"


class CClientSocket :
	public CObject
{
public:
	CClientSocket(CPClientDlg*);
	virtual ~CClientSocket(void);

public:
	BOOL ConnectServer(DWORD& dwServIP, short& sServPort);
	BOOL SendPacket(PHDER pHdr, CPClientDlg::state state, char* pSend);
	BOOL SendPulsePacket(void);
	BOOL RecvPaper(CString& strName, CObList* pItemList);

private:
	SOCKET m_s;
	char m_buf[BUFFER_SIZE];
	CPClientDlg* m_pClientDlg;
private:
	CClientSocket(void);
};

