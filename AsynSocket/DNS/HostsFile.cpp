#include "stdafx.h"
#include <vector>


#pragma comment(lib, "Ws2_32.lib")


#include "HostsFile.h"
#include "DNSHelper.h"


namespace DNS
{
	HostsFile::HostsFile()
	{
		NULL;
	}
	HostsFile::~HostsFile()
	{
		NULL;
	}
	bool HostsFile::Query(const std::string& domain, std::string& ip)
	{
		if (m_mapNameIp.find(domain) != m_mapNameIp.end())
		{
			ip = m_mapNameIp[domain];
			return true;
		}
		return false;
	}
	void HostsFile::Init()
	{
		;
	}

	void HostsFile::ParseLine(const std::string& strLine)
	{
		std::string strTemp(strLine);
		std::string::size_type pos = strTemp.find_first_of("#");
		if (std::string::npos != pos)
		{
			strTemp = strTemp.substr(0, pos);
		}
		if (!strTemp.empty())
		{
			std::vector<std::string> divide_result;
			DNSHelper::DivideString(strTemp, "\t", divide_result);
			std::string ip;
			std::vector<std::string>::const_iterator cit = divide_result.begin();
			if (cit != divide_result.end())
			{
				if (inet_addr((*cit).c_str()) != INADDR_NONE)
				{
					ip = *cit;
					++cit;
					for (; cit != divide_result.end(); ++cit)
					{
						if (DNSHelper::IsValidDomainName(*cit))
						{
							if (m_mapNameIp.find(*cit) == m_mapNameIp.end())
							{
								m_mapNameIp.insert(make_pair(*cit, ip));
							}
						}
					}
				}
			}
		}
	}
}