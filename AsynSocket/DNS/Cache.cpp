
#include <assert.h>
#include "Cache.h"
namespace DNS
{
	unsigned long Cache::s_expiredTime = 1000 * 60 * 30;

	Cache::Cache()
	{
		;
	}
	Cache::~Cache()
	{
		;
	}

	bool Cache::AddCache(std::string& const hostName, std::string& hostAddr)
	{
		assert(!hostName.empty());
		assert(!hostName.empty());

		AutoCriticalSection autoLock(m_cs);

		mapCache::iterator cp = m_cache.find(hostName);
		if (cp != m_cache.end())
		{
			CacheData& data = cp->second;
			data.m_addr = hostAddr;
			data.m_beginTime = ::GetTickCount();
		}
		else
		{
			CacheData data;
			data.m_addr = hostAddr;
			data.m_beginTime = ::GetTickCount();
		}
		return TRUE;
	}
	bool Cache::RemoveCache(std::string& const hostName)
	{
		AutoCriticalSection autoLock(m_cs);

		mapCache::iterator cp = m_cache.find(hostName);
		if (cp == m_cache.end())
		{
			return false;
		}
		m_cache.erase(cp);

		return true;
	}
	bool Cache::QueryAddress(std::string const hostName, std::string& hostAddr)
	{
		AutoCriticalSection autoLock(m_cs);

		mapCache::iterator cp = m_cache.find(hostName);
		if (cp == m_cache.end())
		{
			return false;
		}

		CacheData& data = cp->second;
		if (s_expiredTime == (unsigned long)-1)
		{
			hostAddr = data.m_addr;
			return true;
		}

		unsigned long curTime = ::GetTickCount();
		if (curTime - data.m_beginTime < s_expiredTime)
		{
			hostAddr = data.m_addr;
			return true;
		}

		m_cache.erase(cp);

		return false;
	}
}