
// SClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CSClientDlg �Ի���
class CSClientDlg : public CDialogEx
{
// ����
public:
	CSClientDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CSClientDlg();
	enum{MAX_RECVFILE_THREAD = 3};

	enum REQTYPE
	{
		REQROOT,
		REQDIRC,
		REQFSIZ,
		REQFDAT,
		REQNON
	};

// �Ի�������
	enum { IDD = IDD_SCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


public:
	void GetCurFilePath(string& strCur)
	{
		strCur = m_strPathCur;
	}

	BOOL RecvDirInfor(void);
	BOOL RecvDirString(int nDataLen);
	void RecvFileSize(unsigned long ulFileLen);
	int RecvFileData(SOCKET s, unsigned short ID);

	void SaveFile(u_short nIndexThread, const char* pBuf, u_long ulLen, u_long ulPos);

	void AssemblyFile(void);
	u_long AssembleLine(FILE* destFile, int startPos, FILE* resoFile);
	void EnableSomeControl(BOOL bEnable);
	void ResetCurFileDL(void);



// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();


	DECLARE_MESSAGE_MAP()
public:
	CIPAddressCtrl m_ctlIP;
	CProgressCtrl m_ctlProgress;
	CString m_strSaveFile;
	CTreeCtrl m_ctlTree;

	afx_msg void OnButtonConnect();
	afx_msg void OnButtonSetFilePath();
	afx_msg void OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnFileDownload();

protected:
	void InitSocket(void);
	void UnInitSocket(void);
	void PaintTree(HTREEITEM hTreeItem, list<string>& listStr);
	void GetFilePath(HTREEITEM hTreeItem, string& strPath);
	BOOL SendReq(void);
	void PrepareFileDownload(void);
	BOOL CreateSocket(SOCKET& s);

	static DWORD WINAPI CreateDownloadFileThread(void* pParam);
	static DWORD WINAPI SendAndRecvDirInforThread(void* pParam);
	static DWORD WINAPI DownloadFileThread(void* pParam);


	BOOL IsConning(void)
	{
		return m_bConning;
	}

	SOCKET GetHostSocket(void)
	{
		return m_sHost;
	}
	u_long GetFileDLSize(void)
	{
		return m_ulFileSize;
	}
	string GetFileDL()
	{
		return m_strReqFile;
	}

protected:
	CStatic m_ctlTip;
	CStatic m_ctlFileDLTip;
	SOCKET m_sHost;
	BOOL m_bConning;
	HANDLE m_hThreadSR;
	HANDLE m_hThreadFileDL;
	HTREEITEM m_hTreeCur;
	string m_strPathCur;
	REQTYPE m_nReqCur;
	string m_strReqDir;
	string m_strReqFile;
	u_long m_ulFileSize;
	string m_strSaveFilePath;
	u_long m_ulCurTotal;
};
