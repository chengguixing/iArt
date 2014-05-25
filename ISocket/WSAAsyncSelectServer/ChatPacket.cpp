m_time
// ChatPacket.cpp : 实现文件
//

#include "stdafx.h"
#include "WSAAsyncSelectServer.h"
#include "ChatPacket.h"


// CChatPacket
IMPLEMENT_SERIAL(CChatPacket, CObject, 1)
CChatPacket::CChatPacket()
{
	Init();
}

CChatPacket::~CChatPacket()
{
}


void CChatPacket::Init(void)
{
	m_type = UNKNOWN;
	m_strMsg = _T("");
	m_pUserList = NULL;
	m_time = CTime::GetCurrentTime();
}


void CChatPacket::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		BYTE byType = m_type;
		ar << byType;
		ar << m_strMsg;


		long lTime = m_time.GetTime();
		ar << lTime;
	}
	else
	{
		BYTE byType;
		ar >> byType;
		m_type = (PACKETTYPE)byType;
		ar >> m_strMsg;

		long lTime;
		ar >> lTime;
		m_time = CTime((time_t)lTime);
	}

	m_UserInfo.Serialize(ar);
	m_OfflineUserInfo.Serialize(ar);

	if (m_type == USERLIST && NULL != m_pUserList)
	{
		m_pUserList->Serialize(ar);
	}
}



// CChatPacket 成员函数
