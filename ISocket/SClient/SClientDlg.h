
// SClientDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <string>
#include <list>
using namespace std;

// CSClientDlg 对话框
class CClientDlg : public CDialog
{
	// Construction
public:
	CClientDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CClientDlg();	
	enum{MAX_RECVFILE_THREAD = 3};

	enum REQTYPE{
		REQROOT,	//请求根目录
		REQDIRC,	//请求子目录
		REQFSIZ,	//请求文件大小
		REQFDAT,	//请求文件数据
		REQNON		//没有请求
	};

	// Dialog Data
	//{{AFX_DATA(CClientDlg)
	enum { IDD = IDD_SCLIENT_DIALOG };
	CProgressCtrl	m_ctlProgress;	//进度条控件
	CTreeCtrl	m_ctlTree;			//树控件
	CIPAddressCtrl	m_ctlIP;		//IP地址控件
	CString	m_strSaveFile;			//保存路径+文件
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientDlg) 
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void	GetCurFilePath(string &strCur)				//得到当前文件路径
	{
		strCur = m_strPathCur;
	}

	BOOL	RecvDirInfor(void);							//接收目录和文件大小信息
	BOOL	RecvDirString(int nDataLen);				//接收目录
	void	RecvFileSize(unsigned long ulFileLen);		//接收文件大小
	int		RecvFileData(SOCKET s, unsigned short ID);	//接收文件数据

	void	SaveFile(u_short nIndexThread, const char* pBuf, 
		u_long ulLen, u_long ulPos);							//保存临时文件
	void	AssemblyFile(void);												//合并文件
	u_long	AssembleLine(FILE* destFile, int startPos, FILE* resoFile);		//合并文件子函数
	void	EnableSomeControl(BOOL bEnable);								//禁止/激活某些控件
	void	ResetCurFileDL(void);											//隐藏进度条控件
	// Generated message map functions
	//{{AFX_MSG(CClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonConnect();
	afx_msg void OnButtonSetFilePath();
	afx_msg void OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnFileDownload();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void InitSocket(void);										//初始化socket
	void UnInitSocket(void);									//释放套接字占用的资源
	void PaintTree(HTREEITEM hTreeItem, list<string> &listStr);	//填充目录
	void GetFilePath(HTREEITEM hTreeItem, string &strPath);		//得到文件路径
	BOOL SendReq(void);											//发送请求
	void PrepareFileDownload(void);								//准备创建下载文件线程
	BOOL CreateSocket(SOCKET &s);								//创建套接字

	static DWORD WINAPI CreateDownloadFileThread(void* pParam);	//创建文件下载线程
	static DWORD WINAPI	SendAndRecvDirInforThread(void* pParam);//发送和接收目录线程函数
	static DWORD WINAPI	DownloadFileThread(void* pParam);		//文件下载线程函数	

	BOOL IsConning(void)			//连接状态
	{
		return m_bConning;
	}
	SOCKET GetHostSocket(void)		//获取套接字
	{
		return m_sHost;
	}

	u_long GetFileDLSize(void)		//获取文件长度
	{
		return m_ulFileSize;
	}

	string GetFileDL()				//获取下载文件路径
	{
		return m_strReqFile;
	}

protected:
	HICON m_hIcon;

protected:
	CStatic			m_ctlTip;							//显示状态信息
	CStatic			m_ctlFileDLTip;						//下载文件进度信息
	SOCKET			m_sHost;							//主机套接字
	BOOL			m_bConning;							//连接状态
	HANDLE			m_hThreadSR;						//发送和接收目录线程句柄
	HANDLE			m_hThreadFileDL;					//创建文件下载线程
	HTREEITEM		m_hTreeCur;							//树的当前子项句柄
	string			m_strPathCur;						//当前文件路径
	REQTYPE			m_nReqCur;							//当前的请求类型
	string			m_strReqDir;						//请求目录
	string			m_strReqFile;						//请求长度的文件名称
	u_long			m_ulFileSize;						//接收文件的长度
	string			m_strSaveFilePath;					//保存文件路径
	u_long			m_ulCurTotal;						//当前接收文件长度
};
