
#include "stdafx.h"
#include "DNS.h"

namespace DNS
{
DNS::DNS(void)
{
	;
}
DNS::~DNS(void)
{
	;
}
DNS* DNS::GetInstance()
{
	static DNS* s_lpInstance = NULL;
	static CriticalSection cs;
	if (NULL == s_lpInstance)
	{
		AutoCriticalSection AutoLock(cs);
		s_lpInstance = new DNS();
		s_lpInstance->Init();
	}
	return s_lpInstance;
}
bool DNS::Init()
{
	m_worker.Start();
	return true;
}
long DNS::AsynGetHostByName(const char* hostName,
							ONGETHOSTBYNAME lpCallBack,
							LPVOID lpUserData)
{
	assert(hostName);
	assert(lpCallBack);

	if (NULL == hostName || NULL == lpCallBack || 0 == strlen(hostName))
	{
		return 1;
	}
	DNSData* lpData = new DNSData();
	lpData->m_lpThis = this;
	lpData->m_callBack = lpCallBack;
	lpData->m_userData = lpUserData;

	std::string strHostName = hostName;

	m_worker.Request(strHostName, WorkerCallBack, lpData);
	return 0;
}
void DNS::WorkerCallBack(std::string& hostName,
						 std::string& hostAddr, 
						 unsigned long errorCode,
						 void* userData)
{
	DNSData* lpData = (DNSData*)userData;
	assert(lpData);
	assert(lpData->m_lpThis);

	lpData->m_lpThis->WorkComplete(lpData, hostName, hostAddr, errorCode);
}
void DNS::WorkComplete(DNSData* lpData,
					   std::string& hostName,
					   std::string& hostAddr,
					   unsigned long errorCode)
{
	assert(lpData);
	assert(lpData->m_callBack);

	lpData->m_callBack(hostName.c_str(), hostAddr.c_str(), lpData->m_userData, errorCode);
}
}