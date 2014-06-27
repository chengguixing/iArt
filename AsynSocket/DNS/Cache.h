#ifndef _CACHE_H_BD18DF29_4D0E_42CF_8E01_5DA5D4E54B2B
#define _CACHE_H_BD18DF29_4D0E_42CF_8E01_5DA5D4E54B2B

#include <windows.h>
#include <string>
#include <hash_map>
#include "DNSHelper.h"

namespace DNS
{
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

		bool AddCache(std::string& const hostName, std::string& hostAddr);
		bool RemoveCache(std::string& const hostName);
		bool QueryAddress(std::string const hostName, std::string& hostAddr);

	private:
		mapCache m_cache;
		CriticalSection m_cs;
		/* 过期时间，默认30分钟，(ulong)-1表示永不过期 */
		static unsigned long s_expiredTime;
	};
}

#endif