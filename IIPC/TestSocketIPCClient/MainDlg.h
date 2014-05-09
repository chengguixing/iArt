// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "tslog/tslog.h"
#include "../SocketIPC/SocketIPC.h"

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
		COMMAND_HANDLER(IDC_BUTTON_CONNECT, BN_CLICKED, OnBnClickedButtonConnect)
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
		m_IPCInterface = SIPC_CreateIpcClient();
		SIPC_InitIpcClient(m_IPCInterface);

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

	LRESULT CMainDlg::OnBnClickedButtonConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: 在此添加控件通知处理程序代码
		m_listboxLog.InsertString(-1, L"Start to connect server");
		SIPC_ConnectServer(m_IPCInterface, OnConnect, this);

		return 0;
	}

	LRESULT CMainDlg::OnBnClickedButtonSenddata(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: 在此添加控件通知处理程序代码

		//m_IPCClient.SendData(1, "1234567890");
		SIPC_STREAM hSIpcStream = SIPC_CreateSocketIpcStream();
		SIPC_StreamWriteString(hSIpcStream, "--DataFromIPCClient1");
		SIPC_StreamWriteString(hSIpcStream, "--DataFromIPCClient2");

		//测试
		/*long lLen = CommonTool::StreamReadString(lpSocketIpcStream, NULL, 0);
		TSDEBUG(L"lLen = %d", lLen);
		char* szData = new char[lLen];
		CommonTool::StreamReadString(lpSocketIpcStream, szData, lLen);
		std::wstring wstr = CommonTool::StringToWstring(szData);
		TSDEBUG(L"wstr = %s", wstr.c_str());*/

		SIPC_Send(m_IPCInterface, 0, hSIpcStream, OnSend, this);

		return 0;
	}

private:
	static long __stdcall OnConnect(DWORD dwErrCode, void* pUserData)
	{
		TSDEBUG(L"Enter, dwErrCode = %u", dwErrCode);

		CMainDlg* pThis = (CMainDlg*)pUserData;
		pThis->m_listboxLog.InsertString(-1, L"Connect completely");

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

		long lLen = SIPC_StreamReadString(hSIpcStream, NULL, 0);
		char* szData = new char[lLen];
		long lRet = SIPC_StreamReadString(hSIpcStream, szData, lLen);

		std::string str = szData;
		//获取缓冲区大小，并申请空间，缓冲区大小按字符计算   
		int iLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);   
		wchar_t* wszBuffer = new wchar_t[iLen + 1];   

		//多字节编码转换成宽字节编码   
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), wszBuffer, iLen);   
		wszBuffer[iLen] = '\0';             //添加字符串结尾   

		std::wstring wstr = wszBuffer;
		TSDEBUG(L"wstr = %s", wstr.c_str());

		delete[] wszBuffer;

		pThis->m_listboxLog.InsertString(-1, wstr.c_str());

		return 0;
	}

private:
	SIPC_INSTANCE m_IPCInterface;
	SOCKET m_localSocket;
	CListBox m_listboxLog;
};
