
// SClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <string>
#include <list>
using namespace std;

// CSClientDlg �Ի���
class CClientDlg : public CDialog
{
	// Construction
public:
	CClientDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CClientDlg();	
	enum{MAX_RECVFILE_THREAD = 3};

	enum REQTYPE{
		REQROOT,	//�����Ŀ¼
		REQDIRC,	//������Ŀ¼
		REQFSIZ,	//�����ļ���С
		REQFDAT,	//�����ļ�����
		REQNON		//û������
	};

	// Dialog Data
	//{{AFX_DATA(CClientDlg)
	enum { IDD = IDD_SCLIENT_DIALOG };
	CProgressCtrl	m_ctlProgress;	//�������ؼ�
	CTreeCtrl	m_ctlTree;			//���ؼ�
	CIPAddressCtrl	m_ctlIP;		//IP��ַ�ؼ�
	CString	m_strSaveFile;			//����·��+�ļ�
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientDlg) 
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void	GetCurFilePath(string &strCur)				//�õ���ǰ�ļ�·��
	{
		strCur = m_strPathCur;
	}

	BOOL	RecvDirInfor(void);							//����Ŀ¼���ļ���С��Ϣ
	BOOL	RecvDirString(int nDataLen);				//����Ŀ¼
	void	RecvFileSize(unsigned long ulFileLen);		//�����ļ���С
	int		RecvFileData(SOCKET s, unsigned short ID);	//�����ļ�����

	void	SaveFile(u_short nIndexThread, const char* pBuf, 
		u_long ulLen, u_long ulPos);							//������ʱ�ļ�
	void	AssemblyFile(void);												//�ϲ��ļ�
	u_long	AssembleLine(FILE* destFile, int startPos, FILE* resoFile);		//�ϲ��ļ��Ӻ���
	void	EnableSomeControl(BOOL bEnable);								//��ֹ/����ĳЩ�ؼ�
	void	ResetCurFileDL(void);											//���ؽ������ؼ�
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
	void InitSocket(void);										//��ʼ��socket
	void UnInitSocket(void);									//�ͷ��׽���ռ�õ���Դ
	void PaintTree(HTREEITEM hTreeItem, list<string> &listStr);	//���Ŀ¼
	void GetFilePath(HTREEITEM hTreeItem, string &strPath);		//�õ��ļ�·��
	BOOL SendReq(void);											//��������
	void PrepareFileDownload(void);								//׼�����������ļ��߳�
	BOOL CreateSocket(SOCKET &s);								//�����׽���

	static DWORD WINAPI CreateDownloadFileThread(void* pParam);	//�����ļ������߳�
	static DWORD WINAPI	SendAndRecvDirInforThread(void* pParam);//���ͺͽ���Ŀ¼�̺߳���
	static DWORD WINAPI	DownloadFileThread(void* pParam);		//�ļ������̺߳���	

	BOOL IsConning(void)			//����״̬
	{
		return m_bConning;
	}
	SOCKET GetHostSocket(void)		//��ȡ�׽���
	{
		return m_sHost;
	}

	u_long GetFileDLSize(void)		//��ȡ�ļ�����
	{
		return m_ulFileSize;
	}

	string GetFileDL()				//��ȡ�����ļ�·��
	{
		return m_strReqFile;
	}

protected:
	HICON m_hIcon;

protected:
	CStatic			m_ctlTip;							//��ʾ״̬��Ϣ
	CStatic			m_ctlFileDLTip;						//�����ļ�������Ϣ
	SOCKET			m_sHost;							//�����׽���
	BOOL			m_bConning;							//����״̬
	HANDLE			m_hThreadSR;						//���ͺͽ���Ŀ¼�߳̾��
	HANDLE			m_hThreadFileDL;					//�����ļ������߳�
	HTREEITEM		m_hTreeCur;							//���ĵ�ǰ������
	string			m_strPathCur;						//��ǰ�ļ�·��
	REQTYPE			m_nReqCur;							//��ǰ����������
	string			m_strReqDir;						//����Ŀ¼
	string			m_strReqFile;						//���󳤶ȵ��ļ�����
	u_long			m_ulFileSize;						//�����ļ��ĳ���
	string			m_strSaveFilePath;					//�����ļ�·��
	u_long			m_ulCurTotal;						//��ǰ�����ļ�����
};
