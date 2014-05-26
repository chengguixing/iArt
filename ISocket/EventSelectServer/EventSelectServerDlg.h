
// EventSelectServerDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "resource.h"


class CUserInfo;

// CEventSelectServerDlg 对话框
class CEventSelectServerDlg : public CDialog
{
// 构造
public:
	CEventSelectServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_EVENTSELECTSERVER_DIALOG };
	CListCtrl m_ctlUserList;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStartService();
	afx_msg void OnStopService();

	afx_msg LRESULT OnSocketAccept(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSocketRead(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSocketClose(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	BOOL Init(void);
	BOOL UpdateUserList(const CString strUserInfo, CClientSocket* pClientSocket);
	void UpdateListCtrl(void);
	
};
