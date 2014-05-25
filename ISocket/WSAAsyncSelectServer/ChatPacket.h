#pragma once

#include "UserInfo.h"

class CUserInfo;

// CChatPacket ÃüÁîÄ¿±ê

class CChatPacket : public CObject
{
public:
	CChatPacket();
	virtual ~CChatPacket();
	DECLARE_SERIAL(CChatPacket)

	enum PACKETTYPE
	{
		MESSAGE,
		USERLIST,
		SERVERMSG,
		UNKNOWN
	};

public:
	void Init(void);
	virtual void Serialize(CArchive& ar);

public:
	PACKETTYPE m_type;
	CUserInfo m_UserInfo;
	CString m_strMsg;
	CObject* m_pUserList;
	CUserInfo m_OfflineUserInfo;
	CTime m_time;
};


