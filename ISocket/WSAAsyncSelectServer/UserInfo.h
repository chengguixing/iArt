#pragma once

// CUserInfo ÃüÁîÄ¿±ê

class CUserInfo : public CObject
{
public:
	CUserInfo();
	CUserInfo(const CUserInfo& userInfo);
	virtual ~CUserInfo();
	DECLARE_SERIAL(CUserInfo)

	enum USERSTATUS
	{
		ONLINE,
		OFFLINE,
		LOGIN,
		UNKNOWN
	};

public:
	void Init(void);
	virtual void Serialize(CArchive& ar);
	CUserInfo& operator= (const CUserInfo& userInfo);

public:
	CString m_strName;
	CString m_strPassword;
	USERSTATUS m_eStatus;
	DWORD m_lIP;
	SHORT m_nPort;
	CTime m_time;
};


