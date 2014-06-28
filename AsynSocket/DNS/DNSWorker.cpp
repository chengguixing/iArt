#include "stdafx.h"
#include "DNSWorker.h"
#include "DNS.h"


namespace DNS
{
	DNSWorkerMsgHandler::DNSWorkerMsgHandler() : m_lpOwner(NULL)
	{
		;
	}
	DNSWorkerMsgHandler::~DNSWorkerMsgHandler()
	{
		assert(m_lpOwner);
		m_lpOwner = NULL;
	}

	bool DNSWorkerMsgHandler::Init()
	{
		assert(!IsWindow());
		Create(HWND_MESSAGE, NULL, NULL, WS_POPUP);
		return true;
	}
	bool DNSWorkerMsgHandler::UnInit()
	{
		assert(IsWindow());
		DestroyWindow();

		return true;
	}

	LRESULT DNSWorkerMsgHandler::OnTaskComplete(UINT, WPARAM wParam, LPARAM lParam, BOOL&)
	{
		DNSWorker::DNSRequest* lpRequest = (DNSWorker::DNSRequest*)wParam;
		m_lpOwner->OnTaskComplete(lpRequest);

		return 0;
	}
	void DNSWorkerMsgHandler::SetOwner(DNSWorker* lpOwner)
	{
		assert(m_lpOwner == NULL);
		assert(lpOwner);

		m_lpOwner = lpOwner;
	}


	DNSWorker::DNSWorker() : m_end(false), m_waitObj(NULL)
	{
		m_lpMsgHandler = new msgHandler();
		m_lpMsgHandler->SetOwner(this);
	}
	DNSWorker::~DNSWorker()
	{
		assert(m_waitObj == NULL);
		assert(m_lpMsgHandler);
		m_lpMsgHandler->SafeDelete();
		m_lpMsgHandler = NULL;
	}

	bool DNSWorker::Start()
	{
		m_hostsFile.Init();

		m_waitObj = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(m_waitObj);
		if (NULL ==m_waitObj)
		{
			return false;
		}
		m_end = false;

		HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, this, 0, NULL);
		if (NULL == handle)
		{
			::CloseHandle(m_waitObj);
			m_waitObj = NULL;

			return false;
		}

		::CloseHandle(handle);

		assert(m_lpMsgHandler);
		m_lpMsgHandler->Init();

		return true;
	}

	unsigned int __stdcall DNSWorker::ThreadProc(void* lpUserData)
	{
		DNSWorker* lpThis = (DNSWorker*)lpUserData;
		assert(lpThis);

		return lpThis->ThreadWorker();
	}

	unsigned int DNSWorker::ThreadWorker()
	{
		assert(m_waitObj);

		for (;;)
		{
			::WaitForSingleObject(m_waitObj, INFINITE);
			if (m_end)
			{
				break;
			}
			while (true)
			{
				if (m_end)
				{
					break;
				}
				DNSRequest* lpRequest = PeekTask();
				if (NULL == lpRequest)
				{
					break;
				}
				DNSResolver(lpRequest);
			}
			if (m_end)
			{
				break;
			}
		}
		return 0;
	}


	void DNSWorker::DNSResolver(DNSRequest* lpRequest)
	{
		assert(lpRequest);
		assert(!lpRequest->strHostName.empty());


		lpRequest->strHostAddr.clear();
		lpRequest->dwErrorCode = 403;

		if (m_cache.QueryAddress(lpRequest->strHostName, lpRequest->strHostAddr))
		{
			lpRequest->dwErrorCode = 0;
		}
		else if (m_hostsFile.Query(lpRequest->strHostName, lpRequest->strHostAddr))
		{
			lpRequest->dwErrorCode = 0;
			m_cache.AddCache(lpRequest->strHostName, lpRequest->strHostAddr);
		}
		else
		{
			struct sockaddr_in sockAddr;
			sockAddr.sin_family = AF_INET;
			sockAddr.sin_addr.s_addr = inet_addr(lpRequest->strHostName.c_str());

			if (INADDR_NONE == sockAddr.sin_addr.s_addr)
			{
				struct hostent* pEnt = gethostbyname(lpRequest->strHostName.c_str());
				if (pEnt)
				{
					sockAddr.sin_addr.s_addr = ((LPIN_ADDR)pEnt->h_addr)->s_addr;
					char *pAddr = inet_ntoa(sockAddr.sin_addr);
					if (pAddr)
					{
						lpRequest->strHostAddr = pAddr;
						lpRequest->dwErrorCode = 0;

						m_cache.AddCache(lpRequest->strHostName, lpRequest->strHostAddr);
					}
				}
			}
			else
			{
				lpRequest->strHostAddr = lpRequest->strHostName;
				lpRequest->dwErrorCode = 0;
			}
		}
		if (!m_end)
		{
			assert(lpRequest->m_recvWnd);
			::PostMessage(lpRequest->m_recvWnd, WM_TASK_COMPLETE, (WPARAM)lpRequest, (LPARAM)0);
		}
		else
		{
			delete lpRequest;
		}
	}

	void DNSWorker::OnTaskComplete(DNSRequest* lpRequest)
	{
		assert(lpRequest);
		assert(lpRequest->callBack);

		if (!m_end)
		{
			lpRequest->callBack(lpRequest->strHostName,
				lpRequest->strHostAddr,
				lpRequest->dwErrorCode,
				lpRequest->pvUserData);
		}
		delete lpRequest;
	}

	bool DNSWorker::End()
	{
		assert(!m_end);
		m_end = true;


		assert(m_waitObj);
		::CloseHandle(m_waitObj);
		m_waitObj = NULL;


		assert(m_lpMsgHandler);
		m_lpMsgHandler->UnInit();

		return true;
	}

	DNSWorker::DNSRequest* DNSWorker::PeekTask()
	{
		AutoCriticalSection AutoLock(m_cs);

		if (m_request.empty())
		{
			return NULL;
		}

		DNSRequest* lpRequest = m_request.front();
		m_request.pop_front();

		return lpRequest;
	}

	bool DNSWorker::Request(std::string& strHostName,
		DNSWorkerCallBack callBack,
		void* pvUserData)
	{
		assert(!strHostName.empty());
		assert(callBack);
		assert(m_lpMsgHandler);


		DNSRequest* lpRequest = new DNSRequest();

		lpRequest->callBack = callBack;
		lpRequest->pvUserData = pvUserData;
		lpRequest->strHostName = strHostName;

		lpRequest->dwErrorCode = 0;
		lpRequest->m_recvWnd = m_lpMsgHandler->m_hWnd;


		do 
		{
			AutoCriticalSection AutoLock(m_cs);

			m_request.push_back(lpRequest);

			assert(m_waitObj);
			::SetEvent(m_waitObj);
		} while (false);
		return true;
	}
}