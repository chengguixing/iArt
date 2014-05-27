
// EventSelectClientDlg.h : 头文件
//

#pragma once


// CEventSelectClientDlg 对话框
class CEventSelectClientDlg : public CDialogEx
{
// 构造
public:
	CEventSelectClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CLIENT_LOGIN_DIALOG };
	CIPAddressCtrl m_ctlServIP;
	short m_servPort;
	DWORD m_lservIP;
	CString m_strName;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual void OnOK(void);
	DECLARE_MESSAGE_MAP()
};
