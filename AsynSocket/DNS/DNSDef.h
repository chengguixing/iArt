#ifndef _DNSDEF_H_BF7B00B7_8ED6_4518_A6B2_7C25643F3658
#define _DNSDEF_H_BF7B00B7_8ED6_4518_A6B2_7C25643F3658

#include <atlbase.h>
#include <atlwin.h>


namespace DNS
{
	#define WM_TASK_COMPLETE WM_USER + 666


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
				// �����Ѿ����٣����ǻ�û�е������һ����Ϣ
				m_bFinalDel = true;
			}
			else
			{
				m_bFinalDel = false;
				delete this;
			}
		}

	protected:

		// �����ɾ��ֻ����ͨ��DeleteSelf��ɾ����������ֱ�ӵ���delete��ɾ����
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

		bool    m_bFinalDel;        // �Ƿ���Ҫ��onfinalmessage����ɾ���Լ�
		bool    m_bOnDestroy;       // �Ƿ��Ѿ����õ�Destroy����EndDialog����ֹ����
	};


	typedef void (__stdcall* DNSWorkerCallBack)(std::string& hostName, std::string& hostAddr, unsigned long errorCode, void* userData);
}


#endif