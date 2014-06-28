#ifndef _DNSWORKER_H_62102BD2_51D4_4A92_87AA_C1B12C27A9B0
#define _DNSWORKER_H_62102BD2_51D4_4A92_87AA_C1B12C27A9B0
#include "stdafx.h"
#include <list>
#include "Cache.h"
#include "HostsFile.h"




namespace DNS
{


	
	class DNSWorker;
	class DNSWorkerMsgHandler : public CWindowImpl<DNSWorkerMsgHandler>
	{
	public:
		DNSWorkerMsgHandler();
		~DNSWorkerMsgHandler();

		void SetOwner(DNSWorker* lpOwner);

		bool Init();
		bool UnInit();

	public:
		LRESULT OnTaskComplete(UINT, WPARAM, LPARAM, BOOL&);

	public:
		BEGIN_MSG_MAP(DNSWorkerMsgHandler)
			MESSAGE_HANDLER(WM_TASK_COMPLETE, OnTaskComplete)
		END_MSG_MAP()
	private:
		DNSWorker* m_lpOwner;
	};

	


	
	class DNSWorker
	{
	private:
		struct DNSRequest
		{
			std::string strHostName;
			std::string strHostAddr;
			unsigned long dwErrorCode;

			HWND m_recvWnd;

			DNSWorkerCallBack callBack;
			void *pvUserData;
		};

		typedef std::list<DNSRequest*> listRequest;
		typedef SafeWindowWrapper<DNSWorkerMsgHandler> msgHandler;

		friend class DNSWorkerMsgHandler;

	public:
		DNSWorker();
		~DNSWorker();

		bool Start();
		bool End();

		bool Request(std::string& strHostName, DNSWorkerCallBack callBack, void* pvUserData);

	private:
		static unsigned int __stdcall ThreadProc(void* lpUserData);
		unsigned int ThreadWorker();
		void DNSResolver(DNSRequest* lpRequest);

		void OnTaskComplete(DNSRequest* lpRequest);
		DNSRequest* PeekTask();

	private:
		CriticalSection m_cs;
		HANDLE m_waitObj;
		listRequest m_request;

		msgHandler* m_lpMsgHandler;
		bool m_end;

		Cache m_cache;
		HostsFile m_hostsFile;
	};
}

#endif