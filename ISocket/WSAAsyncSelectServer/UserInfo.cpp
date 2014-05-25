m_time
// UserInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "WSAAsyncSelectServer.h"
#include "UserInfo.h"


// CUserInfo

CUserInfo::CUserInfo()
{
}

CUserInfo::~CUserInfo()
{
}


// CUserInfo 成员函数

void CUserInfo::Init(void)
{
	m_strName = _T("");
	m_strPassword = _T("");
	m_eStatus = UNKNOWN;
	m_lIP = 0;
	m_nPort = 0;
	m_time = CTime::GetCurrentTime();
}
void CUserInfo::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_strName;
		ar << m_strPassword;

		WORD byStatus = 0;
		byStatus = (WORD)m_eStatus;
		ar << byStatus;

		ar << m_lIP;
		ar << m_nPort;

		long lTime = m_time.GetTime();
		ar << lTime;
	}
	else
	{
		ar >> m_strName;
		ar >> m_strPassword;

		WORD byStatus = 0;
		ar >> byStatus;
		m_eStatus = (USERSTATUS)byStatus;

		ar >> m_lIP;
		ar >> m_nPort;

		long lTime;
		ar >> lTime;
		m_time = CTime((time_t)lTime);
	}
}

CUserInfo& CUserInfo::operator= (const CUserInfo& userInfo)
{
	m_strName = userInfo.m_strName;
	m_strPassword = userInfo.m_strPassword;
	m_eStatus = userInfo.m_eStatus;
	m_lIP = userInfo.m_lIP;
	m_nPort = userInfo.m_nPort;
	m_time = userInfo.m_time;

	return *this;
}



CUserInfo::CUserInfo(const CUserInfo& userInfo)
{
	m_strName = userInfo.m_strName;
	m_strPassword = userInfo.m_strPassword;
	m_eStatus = userInfo.m_eStatus;
	m_lIP = userInfo.m_lIP;
	m_nPort = userInfo.m_nPort;
	m_time = userInfo.m_time;
}