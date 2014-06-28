#ifndef _DNSHELPER_H_6D8C44A1_EB64_47DB_8877_BDA9E4935876
#define _DNSHELPER_H_6D8C44A1_EB64_47DB_8877_BDA9E4935876

#include <vector>

namespace DNS
{
	/* Helper */
	class DNSHelper
	{
	public:
		static std::string GetWinDir();
		static std::string GetHostsPath();
		static std::string FindHostsFile();
		static bool IsValidDomainName(std::string const &str);
		static bool IsNetbiosName(std::string const & domain_name);
		static void DivideString(std::string const &strSrc, std::string const & divide_chars, std::vector<std::string>& result);
	};

	class CriticalSection
	{
	public:
		CriticalSection()
		{
			Init();
		}
		~CriticalSection()
		{
			Term();
		}
		void Lock()
		{
			::EnterCriticalSection(&m_cs);
		}
		void Unlock()
		{
			::LeaveCriticalSection(&m_cs);
		}
		void Init()
		{
			::InitializeCriticalSection(&m_cs);
		}
		void Term()
		{
			::DeleteCriticalSection(&m_cs);
		}
	private:
		CRITICAL_SECTION m_cs;
	};

	class AutoCriticalSection
	{
	public:
		AutoCriticalSection(CriticalSection& cs) : m_cs(cs)
		{
			m_cs.Lock();
		}
		~AutoCriticalSection()
		{
			m_cs.Unlock();
		}
	private:
		AutoCriticalSection(const AutoCriticalSection& other);
		AutoCriticalSection& operator= (const AutoCriticalSection& other);
	private:
		CriticalSection& m_cs;
	};


	typedef void (__stdcall* ONGETHOSTBYNAME)(
		const char* requestHostName,
		const char* hostAddr,
		void* pvUserData,
		DWORD dwErrCode
		);
}


#endif