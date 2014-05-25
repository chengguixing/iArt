#include "stdafx.h"
#include "Client.h"
#include "SServerDlg.h"

typedef struct _fileuploadparam
{
	u_long	ulFileSize;		//�ļ�����
	u_long	ulFilePos;		//�ļ�λ��
	CClient* ptr;			//CClient��ָ��
}FileUploadParam, *pFileUploadParam;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClient::CClient(SOCKET s, CSServerDlg* pServer)
{
	m_sClient = s;
	m_nReqCur = REQNON;
	m_hThreadFileUL = NULL;
	InitializeCriticalSection(&m_csFile);
	m_pServer = pServer;
}

CClient::~CClient()
{
	m_pServer = NULL;
	closesocket(m_sClient);
	DeleteCriticalSection(&m_csFile);
}


/*
 * ��ȡ��������Ŀ¼
 */
void CClient::GetThirdSubDir(string &strDirPath, string &strThirdDir)
{
	string strTemp = strDirPath + "\\*.*";				//���ͨ���
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strTemp.c_str());	//��Ŀ¼׼���ļ�����
	while( bWorking)
	{
		bWorking = finder.FindNextFile();				//�����ļ�
		CString filename = finder.GetFileName();		//��ȡ�ļ�����
		CString strExt = filename.Right(filename.GetLength()- filename.ReverseFind('.') -1);
		if (!finder.IsSystem()		&&	//����ϵͳ�ļ�
			!finder.IsHidden()		&&	//���������ļ�
			!finder.IsDots()		&&	//����. ..
			!finder.IsTemporary()	&&	//������ʱ�ļ�
			strExt.Compare("SYS")	&&	//����.sys�ļ�
			strExt.Compare("sys")	&&	//����.sys�ļ�
			strExt.Compare("BAT")	&&	//����.bat�ļ�
			strExt.Compare("bat"))		//����.bat�ļ�
		{
			filename += '|';		//��ӷָ���
			strThirdDir += filename;//��ӵ�����ֵ
		}
	}
	finder.Close();//�ر��ļ�
}

/*
 * ��ȡ�ڶ�����Ŀ¼
 */
void CClient::GetSecondSubDir(string &strDir)
{
	string strTemp = strDir + "*.*";//���ͨ���
	string strParentPath = strDir;	//�ֲ�����
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strTemp.c_str());//��Ŀ¼
	while( bWorking)
	{
		bWorking = finder.FindNextFile();		//�����ļ�
		CString filename = finder.GetFileName();//�ļ���
		CString strExt = filename.Right(filename.GetLength()- filename.ReverseFind('.') -1);
		
		if (!finder.IsSystem()		&&	//����ϵͳ�ļ�
			!finder.IsHidden()		&&	//���������ļ�
			!finder.IsDots()		&&	//����. ..
			!finder.IsTemporary()	&&	//������ʱ�ļ�
			strExt.Compare("SYS")	&&	//����.sys�ļ�
			strExt.Compare("sys")	&&	//����.sys�ļ�
			strExt.Compare("BAT")	&&	//����.bat�ļ�
			strExt.Compare("bat"))		//����.bat�ļ�
		{
			//CString ת��Ϊsting		
			string fileTemp = filename.GetBuffer(12);
			filename.ReleaseBuffer();
			
			//��ȡ������Ŀ¼
			string strThirdDir;
			GetThirdSubDir(strParentPath+ fileTemp,strThirdDir);
			
			fileTemp += "\\";									//���·������
			fileTemp += strThirdDir;
			fileTemp.insert(fileTemp.begin(), '<');				//����Ŀ¼��ָ����
			fileTemp +='>';										//����Ŀ¼�ҷָ����
			
			strDir += fileTemp;//��ӵ�����ַ���
			fileTemp.erase(fileTemp.begin(), fileTemp.end());	//����ַ���
		}		
	}

	finder.Close();//�ر��ļ�
}

/*
 * ��ȡ����Ŀ¼
 */
void CClient::GetRootDir(string &strRoot)
{
	char  szDrives[128];//��ʱ����
	if (GetLogicalDriveStrings(sizeof(szDrives), szDrives))			//��ȡ��Ŀ¼
	{
		char *pTemp = szDrives;	//��ʱ�ַ�ָ��
		string strSubDir;		//��ʱ����
		
		while(TRUE)
		{
			if (*pTemp)
			{
				strSubDir += *pTemp;
			}else if (!*pTemp && *(pTemp+1))
			{
				strSubDir.erase(strSubDir.end());					//ɾ������'\'
				string strThirdDir;
				GetThirdSubDir(strSubDir,strThirdDir);				//��ȡ��Ŀ¼
				strSubDir += strThirdDir;
				
				strSubDir.insert(strSubDir.begin(), '<');			//��ָ����
				strSubDir +='>';									//�ҷָ����
				
				strRoot += strSubDir;								//���뷢���ַ�����
				
				strSubDir.erase(strSubDir.begin(), strSubDir.end());//��ոñ���
				
			}else
			{	
				strSubDir.erase(strSubDir.end());					//ɾ������'\'
				
				string strThirdDir;
				GetThirdSubDir(strSubDir,strThirdDir);				//��ȡ��Ŀ¼
				strSubDir += strThirdDir;
				
				strSubDir.insert(strSubDir.begin(), '<');			//��ָ����
				strSubDir +='>';									//�ҷָ����
				
				strRoot += strSubDir;								//���뷢���ַ�����
				
				strSubDir.erase(strSubDir.begin(), strSubDir.end());//��ոñ���
				break;												//����
			}			
			pTemp++;
		}		
	}
	return;
}


/*
 * ���͸�Ŀ¼
 */
BOOL CClient::SendRootDir(void)
{
	//��ø�Ŀ¼
	string strRoot;
	GetRootDir(strRoot);
	
	int nErrCode;	//����ֵ
	hdr header;		//��ͷ
	
	memset(&header, 0, sizeof(header));
	header.type = ROOT;
	header.len = HEADLEN + strRoot.size();
	
	//���Ͱ�ͷ
	nErrCode = send(m_sClient, (char*)&header, HEADLEN, 0);
	if (SOCKET_ERROR == nErrCode)
	{
	return FALSE;
	}
	
	//����Ŀ¼
	nErrCode = send(m_sClient, (char*)strRoot.c_str(), strRoot.size(),0);
	if (SOCKET_ERROR == nErrCode)
	{
		return FALSE;
	}

	return TRUE;
}

/*
 * ������Ŀ¼
 */
BOOL CClient::SendSubDir(void)
{
	//�����Ŀ¼
	GetSecondSubDir(m_strDir);
	
	int nErrCode;	//����ֵ
	hdr header;		//��ͷ
	
	memset(&header, 0, sizeof(header));
	header.type = DIRC;
	header.len = HEADLEN + m_strDir.size();
	
	//���Ͱ�ͷ
	nErrCode = send(m_sClient, (char*)&header, HEADLEN, 0);
	if (SOCKET_ERROR ==nErrCode)
	{
		return FALSE;
	}	
	//����Ŀ¼
	nErrCode = send(m_sClient, (char*)m_strDir.c_str(), m_strDir.size(),0);
	if (SOCKET_ERROR ==nErrCode)
	{
		return FALSE;
	}
	
	//���
	m_strDir.erase(m_strDir.begin(), m_strDir.end());
	return TRUE;
}

/*
 * �����ļ���С
 */
BOOL CClient::SendFileSize(void)
{
	int nErrCode;	//����ֵ
	
	m_ulFile = GetFileSize(m_strFile);//�õ��ļ���С

	//��ͷ
	hdr header;		
	memset(&header, 0, sizeof(header));
	header.type = FSIZ;
	header.len = HEADLEN;
	header.flen = m_ulFile;
	
	//���Ͱ���
	nErrCode = send(m_sClient, (char*)&header, HEADLEN, 0);
	if (SOCKET_ERROR ==nErrCode)
	{
		return FALSE;
	}
	return TRUE;
}




/*
 * �������� ������FALSEʱ�ڷ�������ɾ����ʵ�� 
 */
BOOL CClient::RecvData(void)
{
	//�������ݵİ�ͷ��������Ӧ�ĺ�����������
	hdr	header;		//��ͷ
	int	nReadData;	//�����ֽ���
	
	//���հ�ͷ
	nReadData = recv(m_sClient, (char*)&header, HEADLEN, 0);
	
	if (SOCKET_ERROR == nReadData)					//����
	{
		return FALSE;							 						 
	}else if (0 == nReadData)						//�ͻ��˹ر����׽���
	{
		return FALSE;
	}else if (nReadData > 0 && HEADLEN == nReadData)
	{
		switch(header.type)//�ͻ������������
		{
		case ROOT://�����Ŀ¼
			{
				m_pServer->ShowReqDirInfor("�����Ŀ¼��");
				m_nReqCur = REQROOT;
				break;
			}
		case DIRC://������Ŀ¼
			{
				m_nReqCur = REQDIRC;
				RecvDirPath(header.len - HEADLEN);
				break;
			}
		case FSIZ://�����ļ���С
			{
				m_nReqCur = REQFSIZ;
				RecvFilePathForSize(header.len - HEADLEN);
				break;
			}
		case FDAT://�����ļ�
			{
				m_nReqCur = REQFDAT;
				RecvUploadInfor(header.len - HEADLEN, header.flen, header.fpos);				
				break;
			}
		default:
			break;
		}
	}	
	return TRUE;
}

/*
 * �����ϴ��ļ���Ϣ
 */
BOOL CClient::RecvUploadInfor(u_short usLen, u_long ulFileLen, u_long ulFilePos)
{
	char	cRecv;			//�����ַ�
	int		nReadLen;		//һ�ν����ֽ���
	u_short	nTotalLen = 0;	//���յ�����
	m_strFileUL.erase(m_strFileUL.begin(), m_strFileUL.end());	//�������
	
	//���հ��壬��ȡ�ϴ��ļ���·��
	while (nTotalLen != usLen)
	{		
		nReadLen = recv(m_sClient, &cRecv, 1, 0);				//ÿ�ν���һ���ַ�
		if (SOCKET_ERROR == nReadLen)
		{
			AfxMessageBox(_T("��ȡĿ¼ʧ�ܣ�"));
			return FALSE;
		}else if (0 == nReadLen)
		{
			AfxMessageBox(_T("�ͻ��˹ر������ӣ�"));
			return FALSE;
		}else if (nReadLen > 0)
		{
			m_strFileUL += cRecv;
		}
		nTotalLen += nReadLen;
	}
	
	//�����ϴ��ļ��̲߳���
	pFileUploadParam pThreadParm = new FileUploadParam();
	pThreadParm->ptr = this;
	pThreadParm->ulFileSize = ulFileLen;	//�ϴ��ļ�����
	pThreadParm->ulFilePos = ulFilePos;		//�ϴ��ļ��Ŀ�ʼλ��
	
	//�����ϴ��ļ��߳�
	DWORD dwThreadID;
	m_hThreadFileUL = CreateThread(NULL, 0, UploadFileThread, pThreadParm, 0, &dwThreadID);
	while (NULL == m_hThreadFileUL)
	{
		m_hThreadFileUL = CreateThread(NULL, 0, UploadFileThread, pThreadParm, 0, &dwThreadID);
		Sleep(THREAD_SLEEP);
	}
	return TRUE;	
}

/*
 * �ϴ��ļ��̺߳���
 */
DWORD CClient::UploadFileThread(void *pParam)
{
	pFileUploadParam	pFileUpload = (pFileUploadParam)pParam;

	CClient				*pClient = pFileUpload->ptr;			//CClient��ָ��
	u_long				ulFileSize = pFileUpload->ulFileSize;	//�ϴ��ļ�����
	u_long				ulFilePos = pFileUpload->ulFilePos;		//�ļ���ʼλ��
	CRITICAL_SECTION	*pCS = pClient->GetFileUploadCS();		//�ٽ�������
	CSServerDlg			*pServer = pClient->GetServer();

	SOCKET s = pClient->GetSocket();//�׽���
	
	delete pFileUpload;//ɾ��
	pFileUpload = NULL;	
	
	EnterCriticalSection(pCS);//�����ٽ���
	string strFile = pClient->GetFileUpload();//��ȡ�����ļ�·��
	FILE *stream = NULL;
	
	if((stream  = fopen( strFile.c_str(), "rb" )) == NULL)//���ļ�
	{
		LeaveCriticalSection(pCS);
		AfxMessageBox("���ļ�ʧ�ܣ�");
		return 0;
	}
	
	int		nSendTimes = ulFileSize / BUF_SIZE +1;	//���㷢�͵Ĵ���
	char	cSend[BUF_SIZE];						//�����ļ����ݵĻ�����
	int		nErrCode;								//�������
	int		i = 0;									//���ʹ���
	u_long	nlTotal = 0;							//��ȡ�ļ�����
	u_long	nlRead = 0;								//ÿ�ζ�ȡ�ļ�����
	int		nReadIndex = 0;							//�����

	while (nlTotal != ulFileSize &&
					!feof(stream) &&
					pServer->IsConnecting())		//���㷢���ļ��ĳ��ȣ����߶����ļ�β
	{
		nReadIndex++;//��ȡ������1
		
		memset(cSend, 0, BUF_SIZE);
		fseek(stream, ulFilePos +  nlTotal, SEEK_SET);		//�ƶ��ļ�ָ��ָ��λ��	
		
		if (nReadIndex != nSendTimes)
		{
			nlRead = fread(cSend, 1, BUF_SIZE, stream);		//��ȡ����
		}else{
			u_long ulLeft = ulFileSize - nlTotal;			//���һ��
			nlRead = fread(cSend, 1, ulLeft, stream);		//��ȡ���� 
		}
		
		//�����ͷ��ͷ
		hdr header;
		header.type = FDAT;
		header.len = HEADLEN + nlRead;
		header.flen = nlRead;
		header.fpos = nlTotal;
		
		//���Ͱ�ͷ
		nErrCode = send(s,(char*)&header, HEADLEN, 0);
		if (SOCKET_ERROR == nErrCode)
		{
			AfxMessageBox("�����ļ�ʧ�ܣ�");			
			fclose(stream);
			LeaveCriticalSection(pCS);
			return 0;
		}
		
		//���Ͱ���
		nErrCode = send(s, cSend, nlRead, 0);
		if (SOCKET_ERROR == nErrCode)
		{
			AfxMessageBox("�����ļ�ʧ�ܣ�");
			fclose(stream);
			LeaveCriticalSection(pCS);
			return 0;
		}
		
		nlTotal += nlRead;//�����Ѷ��ļ�����
	}	
	
	//��ʵ����
	if (nlTotal != ulFileSize)
	{
		AfxMessageBox("�����ļ�ʧ�ܣ�");
		fclose(stream);	
		LeaveCriticalSection(pCS);
		return 0;		
	}
	
	LeaveCriticalSection(pCS);
	fclose( stream );				//�ر��ļ�	
	
	//���ͽ�����
	//��ͷ
	hdr header;
	header.type = FDAT;
	header.len = HEADLEN;
	header.flen = 0;//�����ļ�������ʶ
	header.fpos = 0;//�����ļ�������ʶ
	
	//���Ͱ�ͷ
	nErrCode = send(s,(char*)&header, HEADLEN, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("�����ļ�ʧ�ܣ�");
		return 0;
	}

	//֪ͨ�ͻ��ˣ����������ٷ�������
	shutdown(s, SD_SEND);
	return 0;
}	


/*
 * ��������
 */
BOOL CClient::SendData(void)
{
	BOOL bReVal = TRUE;	//����ֵ
	switch(m_nReqCur)	//��ǰ����
	{
	case REQROOT://�����Ŀ¼
		{
			if (FALSE == SendRootDir())				
			{
				bReVal = FALSE;
			}			
			m_nReqCur = REQNON;
			break;
		}
	case REQDIRC://������Ŀ¼
		{		
			if(FALSE == SendSubDir())
			{
				bReVal = FALSE;				
			}
			m_nReqCur = REQNON;
			break;
		}
	case REQFSIZ://�����ļ���С
		{
			if(FALSE == SendFileSize())
			{
				bReVal = FALSE;
			}	
			m_nReqCur = REQNON;
			break;
		}
	case REQFDAT://�����ļ�
		{	
			break;
		}
	default:
		break;
	}
	return bReVal;
}

/*
 * ������Ŀ¼·��
 */
BOOL CClient::RecvDirPath(u_short usLen)
{	
	u_short	nTotalLen = 0;	//�������ݵĳ���
	int		nReadLen;		//һ�ν������ݵĳ���
	char	cRecv;			//���յ�����

	while (nTotalLen != usLen)//�жϽ����Ƿ����
	{		
		nReadLen = recv(m_sClient, &cRecv, 1, 0);//ÿ�ν���һ���ַ�
		if (SOCKET_ERROR == nReadLen)
		{
			AfxMessageBox(_T("��ȡĿ¼ʧ�ܣ�"));
			return FALSE;
		}else if (0 == nReadLen)
		{
			AfxMessageBox(_T("�ͻ��˹ر������ӣ�"));
			return FALSE;
		}else if (nReadLen > 0)
		{
			m_strDir += cRecv;//��������
		}
		nTotalLen += nReadLen;
	}

	//���½�����Ϣ
	CString strReq;
	strReq.Format("�ͻ�������%s��Ŀ¼��",m_strDir.c_str());
	m_pServer->ShowReqDirInfor(strReq);
	return TRUE;
}


/*
 * �����ļ�·��
 */
BOOL CClient::RecvFilePathForSize(u_short usLen)
{
	char	cRecv;			//���յ��ַ�
	int		nReadLen;		//һ�ν����ֽ���
	u_short	nTotalLen = 0;	//���յ�����
	m_strFile.erase(m_strFile.begin(), m_strFile.end());
	while (nTotalLen != usLen)
	{		
		nReadLen = recv(m_sClient, &cRecv, 1, 0);//ÿ�ν���һ���ַ�
		if (SOCKET_ERROR == nReadLen)
		{
			AfxMessageBox(_T("��ȡĿ¼ʧ�ܣ�"));
			return FALSE;
		}else if (0 == nReadLen)
		{
			AfxMessageBox(_T("�ͻ��˹ر������ӣ�"));
			return FALSE;
		}else if (nReadLen > 0)
		{
			m_strFile += cRecv;
		}
		nTotalLen += nReadLen;
	}
	
	//���½�����Ϣ
	CString strReq;
	strReq.Format("�ͻ�����������%s�ļ���",m_strFile.c_str());	
	m_pServer->ShowReqDirInfor(strReq);
	return TRUE;
}


/*
 * ��ȡ�ļ�����
 */
u_long CClient::GetFileSize(string strFile)
{
	long retFileLen;//�ļ�����
	FILE *stream;	//�ļ�ָ��
	if((stream  = fopen( strFile.c_str(), "rb" )) == NULL)//���ļ�
		return 0;
	fseek( stream, 0, SEEK_END);	//ָ���ƶ����ļ�β
	retFileLen = ftell( stream );	//��ȡ�ļ�����
	fclose( stream );				//�ر��ļ�
	return retFileLen;
}
