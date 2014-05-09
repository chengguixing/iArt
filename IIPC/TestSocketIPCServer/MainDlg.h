// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "tslog/tslog.h"
#include "../SocketIPC/SocketIPC.h"
#include "../SocketIPC/CommonTool.h"

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		COMMAND_HANDLER(IDC_BUTTON_LISTEN, BN_CLICKED, OnBnClickedButtonListen)
		COMMAND_HANDLER(IDC_BUTTON_SENDDATA, BN_CLICKED, OnBnClickedButtonSenddata)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
		SetIcon(hIconSmall, FALSE);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		UIAddChildWindowContainer(m_hWnd);

		m_listboxLog = GetDlgItem(IDC_LIST_LOG);

		//初始化
		m_IPCInterface = SIPC_CreateIpcServer();
		SIPC_InitIpcServer(m_IPCInterface);

		return TRUE;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		return 0;
	}

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CloseDialog(0);
		return 0;
	}

	void CloseDialog(int nVal)
	{
		DestroyWindow();
		::PostQuitMessage(nVal);
	}

	LRESULT CMainDlg::OnBnClickedButtonListen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: 在此添加控件通知处理程序代码
		m_listboxLog.InsertString(-1, L"Start to listen");
		SIPC_ListenClient(m_IPCInterface, OnListen, this);

		return 0;
	}

	LRESULT CMainDlg::OnBnClickedButtonSenddata(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: 在此添加控件通知处理程序代码

		//m_IPCServer.SendData(1, "DataFromTestSocketIPCServer");
		SIPC_STREAM hSIpcStream = SIPC_CreateSocketIpcStream();
		SIPC_StreamWriteString(hSIpcStream, "DataFromTestSocketIPCServer");
		SIPC_Send(m_IPCInterface, 0, hSIpcStream, OnSend, this);

		return 0;
	}

private:
	static long __stdcall OnListen(DWORD dwErrCode, void* pUserData)
	{
		TSDEBUG(L"Enter, dwErrCode = %u", dwErrCode);

		CMainDlg* pThis = (CMainDlg*)pUserData;
		pThis->m_listboxLog.InsertString(-1, L"Listen completely");

		SIPC_RegisterRecvCallback(pThis->m_IPCInterface, OnRecv, pThis);

		return 0;
	}

	static long __stdcall OnSend(DWORD dwErrCode, void* pUserData)
	{
		TSDEBUG(L"Enter, dwErrCode = %u", dwErrCode);

		CMainDlg* pThis = (CMainDlg*)pUserData;
		pThis->m_listboxLog.InsertString(-1, L"Send completely");

		return 0;
	}

	static long __stdcall OnRecv(DWORD dwErrCode, DWORD dwId, SIPC_STREAM hSIpcStream, void* pUserData)
	{
		TSDEBUG(L"Enter, dwErrCode = %u, dwId = %u", dwErrCode, dwId);

		CMainDlg* pThis = (CMainDlg*)pUserData;

		long lLen1 = SIPC_StreamReadString(hSIpcStream, NULL, 0);
		char* szData1 = new char[lLen1];
		long lRet = SIPC_StreamReadString(hSIpcStream, szData1, lLen1);
		long lLen2 = SIPC_StreamReadString(hSIpcStream, NULL, 0);
		char* szData2 = new char[lLen2];
		lRet = SIPC_StreamReadString(hSIpcStream, szData2, lLen2);

		std::wstring wstr1 = CommonTool::StringToWstring(szData1);
		std::wstring wstr2 = CommonTool::StringToWstring(szData2);
		std::wstring wstr = L"Recv: " + wstr1 + L" " + wstr2;

		pThis->m_listboxLog.InsertString(-1, wstr.c_str());

		return 0;
	}

private:
	SIPC_INSTANCE m_IPCInterface;
	SOCKET m_serverSocket;
	CListBox m_listboxLog;
};
