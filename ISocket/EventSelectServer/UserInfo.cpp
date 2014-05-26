#include "stdafx.h"
#include "UserInfo.h"


CUserInfo::CUserInfo(void) : m_strName(""), m_lIP(0), m_uPort(0), m_state(UNKNOWN)
{
}
CUserInfo::CUserInfo(const CUserInfo& userInfo)
{
	m_strName = userInfo.m_strName;
	m_lIP = userInfo.m_lIP;
	m_uPort = userInfo.m_uPort;
	m_state = userInfo.m_state;
}

CUserInfo::~CUserInfo(void)
{

}

CUserInfo& CUserInfo::operator = (const CUserInfo& userInfo)
{
	m_strName = userInfo.m_strName;
	m_lIP = userInfo.m_lIP;
	m_uPort = userInfo.m_uPort;
	m_state = userInfo.m_state;

	return *this;
}
