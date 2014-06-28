#ifndef _DNS_H_E37FBECC_B45B_4E52_89D0_CCFFAD7627AE
#define _DNS_H_E37FBECC_B45B_4E52_89D0_CCFFAD7627AE


#include "DNSWorker.h"
#include "DNSHelper.h"

namespace DNS
{
	
	/* DNS */
	class DNS
	{
	private:
		struct DNSData
		{
			DNS* m_lpThis;
			ONGETHOSTBYNAME m_callBack;
			void* m_userData;
		};
	public:
		DNS(void);
		~DNS(void);

		static DNS* GetInstance();

		long AsynGetHostByName(const char* hostName,
			ONGETHOSTBYNAME lpCallBack,
			LPVOID lpUserData);

	private:
		bool Init();
		static void __stdcall WorkerCallBack(std::string& hostName,
			std::string& hostAddr, 
			unsigned long errorCode,
			void* userData);
		void WorkComplete(DNSData* lpData, 
			std::string& hostName,
			std::string& hostAddr,
			unsigned long errorCode);

	private:
		DNSWorker m_worker;
	};
}


#endif