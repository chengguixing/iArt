#ifndef _DNSWORKER_H_62102BD2_51D4_4A92_87AA_C1B12C27A9B0
#define _DNSWORKER_H_62102BD2_51D4_4A92_87AA_C1B12C27A9B0

#include <string>
#include <list>
#include <atlbase.h>
#include <atlwin.h>
#include "Cache.h"
#include "HostsFile.h"




namespace DNS
{


	#define WM_TASK_COMPLETE WM_USER + 666
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

	typedef void (__stdcall* DNSWorkerCallBack)(std::string& hostName, std::string& hostAddr, unsigned long errorCode, void* userData);


	template<typename BaseWindow>
	class SafeWindowWrapper : public BaseWindow
	{
		typedef BaseWindow base_class;

	public:

		BEGIN_MSG_MAP(SafeWindowWrapper)
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
			CHAIN_MSG_MAP(base_class)
		END_MSG_MAP()

		SafeWindowWrapper()
			:m_bFinalDel(false),
			m_bOnDestroy(false)
		{}

		void SafeDelete()
		{
			assert(!m_bFinalDel);

			if (base_class::IsWindow()) 
			{
				m_bFinalDel = true;

				if (!m_bOnDestroy)
				{
					End();
				}
			}
			else if (m_dwState & WINSTATE_DESTROYED)
			{
				// 窗口已经销毁，但是还没有到达最后一条消息
				m_bFinalDel = true;
			}
			else
			{
				m_bFinalDel = false;
				delete this;
			}
		}

	protected:

		// 对象的删除只可以通过DeleteSelf来删除，不可以直接调用delete来删除！
		virtual ~SafeWindowWrapper()
		{

		}

	private:

		LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
		{
			bHandled = FALSE;

			MarkOnDestroy();

			return 0;
		}

		void End()
		{
			// CDialogImplBaseT
			__if_exists(base_class::StartDialogProc)
			{
				base_class::EndDialog(0);
			}

			// CWindowImplBaseT
			__if_exists(base_class::StartWindowProc)
			{
				base_class::DestroyWindow();
			}
		}

		virtual void OnFinalMessage(HWND)
		{
			m_bOnDestroy = false;

			if (m_bFinalDel)
			{
				m_bFinalDel = false;
				delete this;
			}
		}

		void MarkOnDestroy()
		{
			assert(!m_bOnDestroy);
			m_bOnDestroy = true;
		}

	private:

		bool    m_bFinalDel;        // 是否需要在onfinalmessage里面删除自己
		bool    m_bOnDestroy;       // 是否已经调用到Destroy或者EndDialog，防止重入
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