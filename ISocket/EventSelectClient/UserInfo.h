#pragma once
#include "afx.h"
class CUserInfo :
	public CObject
{
public:
	CUserInfo(void);
	CUserInfo(const CUserInfo& userInfo);
	~CUserInfo(void);

	enum USERSTATE
	{
		PLAY,
		WAIT,
		LOGIN,
		UNKNOWN
	};

public:
	CUserInfo& operator= (consts CUserInfo& userInfo);

public:
	CString m_strName;
	u_long m_lIP;
	u_short m_uPort;
	USERSTATE m_state;
};

