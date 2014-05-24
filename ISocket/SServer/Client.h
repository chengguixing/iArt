#pragma once

class CSServerDlg;



class CClient
{
private:
	CClient(void);
public:
	CClient(SOCKET s, CSServerDlg* pServer);
	~CClient(void);


	enum REQTYPE
	{
		REQROOT,
		REQDIRC,
		REQFSIZ,
		REQFDAT,
		REQNON
	};

public:
	BOOL SendData(void);
	BOOL RecvData(void);


	SOCKET GetSocket(void)
	{
		return m_sClient;
	}


private:
	void GetRootDir(string& strRoot);
	void GetThirdSubDir(string& strDirPath, string& strThirdDir);
	void GetSecondSubDir(string& strDir);
	u_long GetFileSize(string strFile);


	BOOL RecvDirPath(u_short usLen);
	BOOL RecvFilePathForSize(u_short usLen);
	BOOL RecvUploadInfor(u_short usLen, u_long ulFileLen, u_long ulFilePos);


	BOOL SendRootDir(void);
	BOOL SendSubDir(void);
	BOOL SendFileSize(void);


	static DWORD WINAPI UploadFileThread(void* pParam);


	u_long GetFileUploadLen(void)
	{
		return m_ulFile;
	}

	string GetFileUpload(void)
	{
		return m_strFileUL;
	}
	CRITICAL_SECTION* GetFileUploadCS(void)
	{
		return &m_csFile;
	}
	CSServerDlg* GetServer(void)
	{
		return m_pServer;
	}


private:
	CSServerDlg* m_pServer;
	SOCKET m_sClient;
	REQTYPE m_nReqCur;
	string m_strDir;
	string m_strFile;
	string m_strFileUL;
	HANDLE m_hThreadFileUL;
	u_long m_ulFile;
	CRITICAL_SECTION m_csFile;
};

