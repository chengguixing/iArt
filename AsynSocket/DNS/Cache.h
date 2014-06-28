#ifndef _CACHE_H_BD18DF29_4D0E_42CF_8E01_5DA5D4E54B2B
#define _CACHE_H_BD18DF29_4D0E_42CF_8E01_5DA5D4E54B2B

#include <hash_map>
#include "DNSHelper.h"

namespace DNS
{
	/* ���� */
	class Cache
	{
	private:
		struct CacheData
		{
			std::string m_addr;
			unsigned long m_beginTime;
		};

		typedef stdext::hash_map<std::string, CacheData> mapCache;

	public:
		Cache();
		~Cache();

		bool AddCache(const std::string& hostName, std::string& hostAddr);
		bool RemoveCache(const std::string& hostName);
		bool QueryAddress(const std::string hostName, std::string& hostAddr);

	private:
		mapCache m_cache;
		CriticalSection m_cs;
		/* ����ʱ�䣬Ĭ��30���ӣ�(ulong)-1��ʾ�������� */
		static unsigned long s_expiredTime;
	};
}

#endif