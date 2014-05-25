#include "stdafx.h"
#include "Client.h"
#include "SServerDlg.h"

typedef struct _fileuploadparam
{
	u_long	ulFileSize;		//文件长度
	u_long	ulFilePos;		//文件位置
	CClient* ptr;			//CClient类指针
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
 * 获取第三级子目录
 */
void CClient::GetThirdSubDir(string &strDirPath, string &strThirdDir)
{
	string strTemp = strDirPath + "\\*.*";				//添加通配符
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strTemp.c_str());	//打开目录准备文件查找
	while( bWorking)
	{
		bWorking = finder.FindNextFile();				//查找文件
		CString filename = finder.GetFileName();		//获取文件名称
		CString strExt = filename.Right(filename.GetLength()- filename.ReverseFind('.') -1);
		if (!finder.IsSystem()		&&	//不是系统文件
			!finder.IsHidden()		&&	//不是隐藏文件
			!finder.IsDots()		&&	//不是. ..
			!finder.IsTemporary()	&&	//不是临时文件
			strExt.Compare("SYS")	&&	//不是.sys文件
			strExt.Compare("sys")	&&	//不是.sys文件
			strExt.Compare("BAT")	&&	//不是.bat文件
			strExt.Compare("bat"))		//不是.bat文件
		{
			filename += '|';		//添加分隔符
			strThirdDir += filename;//添加到返回值
		}
	}
	finder.Close();//关闭文件
}

/*
 * 获取第二级子目录
 */
void CClient::GetSecondSubDir(string &strDir)
{
	string strTemp = strDir + "*.*";//添加通配符
	string strParentPath = strDir;	//局部变量
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strTemp.c_str());//打开目录
	while( bWorking)
	{
		bWorking = finder.FindNextFile();		//查找文件
		CString filename = finder.GetFileName();//文件名
		CString strExt = filename.Right(filename.GetLength()- filename.ReverseFind('.') -1);
		
		if (!finder.IsSystem()		&&	//不是系统文件
			!finder.IsHidden()		&&	//不是隐藏文件
			!finder.IsDots()		&&	//不是. ..
			!finder.IsTemporary()	&&	//不是临时文件
			strExt.Compare("SYS")	&&	//不是.sys文件
			strExt.Compare("sys")	&&	//不是.sys文件
			strExt.Compare("BAT")	&&	//不是.bat文件
			strExt.Compare("bat"))		//不是.bat文件
		{
			//CString 转换为sting		
			string fileTemp = filename.GetBuffer(12);
			filename.ReleaseBuffer();
			
			//获取第三级目录
			string strThirdDir;
			GetThirdSubDir(strParentPath+ fileTemp,strThirdDir);
			
			fileTemp += "\\";									//添加路径符号
			fileTemp += strThirdDir;
			fileTemp.insert(fileTemp.begin(), '<');				//二级目录左分隔标记
			fileTemp +='>';										//二级目录右分隔标记
			
			strDir += fileTemp;//添加到输出字符串
			fileTemp.erase(fileTemp.begin(), fileTemp.end());	//清除字符串
		}		
	}

	finder.Close();//关闭文件
}

/*
 * 获取到根目录
 */
void CClient::GetRootDir(string &strRoot)
{
	char  szDrives[128];//临时变量
	if (GetLogicalDriveStrings(sizeof(szDrives), szDrives))			//获取根目录
	{
		char *pTemp = szDrives;	//临时字符指针
		string strSubDir;		//临时变量
		
		while(TRUE)
		{
			if (*pTemp)
			{
				strSubDir += *pTemp;
			}else if (!*pTemp && *(pTemp+1))
			{
				strSubDir.erase(strSubDir.end());					//删除最后的'\'
				string strThirdDir;
				GetThirdSubDir(strSubDir,strThirdDir);				//获取子目录
				strSubDir += strThirdDir;
				
				strSubDir.insert(strSubDir.begin(), '<');			//左分隔标记
				strSubDir +='>';									//右分隔标记
				
				strRoot += strSubDir;								//加入发送字符变量
				
				strSubDir.erase(strSubDir.begin(), strSubDir.end());//清空该变量
				
			}else
			{	
				strSubDir.erase(strSubDir.end());					//删除最后的'\'
				
				string strThirdDir;
				GetThirdSubDir(strSubDir,strThirdDir);				//获取子目录
				strSubDir += strThirdDir;
				
				strSubDir.insert(strSubDir.begin(), '<');			//左分隔标记
				strSubDir +='>';									//右分隔标记
				
				strRoot += strSubDir;								//加入发送字符变量
				
				strSubDir.erase(strSubDir.begin(), strSubDir.end());//清空该变量
				break;												//结束
			}			
			pTemp++;
		}		
	}
	return;
}


/*
 * 发送根目录
 */
BOOL CClient::SendRootDir(void)
{
	//获得根目录
	string strRoot;
	GetRootDir(strRoot);
	
	int nErrCode;	//返回值
	hdr header;		//包头
	
	memset(&header, 0, sizeof(header));
	header.type = ROOT;
	header.len = HEADLEN + strRoot.size();
	
	//发送包头
	nErrCode = send(m_sClient, (char*)&header, HEADLEN, 0);
	if (SOCKET_ERROR == nErrCode)
	{
	return FALSE;
	}
	
	//发送目录
	nErrCode = send(m_sClient, (char*)strRoot.c_str(), strRoot.size(),0);
	if (SOCKET_ERROR == nErrCode)
	{
		return FALSE;
	}

	return TRUE;
}

/*
 * 发送子目录
 */
BOOL CClient::SendSubDir(void)
{
	//获得子目录
	GetSecondSubDir(m_strDir);
	
	int nErrCode;	//返回值
	hdr header;		//包头
	
	memset(&header, 0, sizeof(header));
	header.type = DIRC;
	header.len = HEADLEN + m_strDir.size();
	
	//发送包头
	nErrCode = send(m_sClient, (char*)&header, HEADLEN, 0);
	if (SOCKET_ERROR ==nErrCode)
	{
		return FALSE;
	}	
	//发送目录
	nErrCode = send(m_sClient, (char*)m_strDir.c_str(), m_strDir.size(),0);
	if (SOCKET_ERROR ==nErrCode)
	{
		return FALSE;
	}
	
	//清空
	m_strDir.erase(m_strDir.begin(), m_strDir.end());
	return TRUE;
}

/*
 * 发送文件大小
 */
BOOL CClient::SendFileSize(void)
{
	int nErrCode;	//返回值
	
	m_ulFile = GetFileSize(m_strFile);//得到文件大小

	//包头
	hdr header;		
	memset(&header, 0, sizeof(header));
	header.type = FSIZ;
	header.len = HEADLEN;
	header.flen = m_ulFile;
	
	//发送包体
	nErrCode = send(m_sClient, (char*)&header, HEADLEN, 0);
	if (SOCKET_ERROR ==nErrCode)
	{
		return FALSE;
	}
	return TRUE;
}




/*
 * 接收数据 当返回FALSE时在服务器端删除该实例 
 */
BOOL CClient::RecvData(void)
{
	//接收数据的包头，调用相应的函数接收数据
	hdr	header;		//包头
	int	nReadData;	//接收字节数
	
	//接收包头
	nReadData = recv(m_sClient, (char*)&header, HEADLEN, 0);
	
	if (SOCKET_ERROR == nReadData)					//错误
	{
		return FALSE;							 						 
	}else if (0 == nReadData)						//客户端关闭了套接字
	{
		return FALSE;
	}else if (nReadData > 0 && HEADLEN == nReadData)
	{
		switch(header.type)//客户端请求的类型
		{
		case ROOT://请求根目录
			{
				m_pServer->ShowReqDirInfor("请求根目录。");
				m_nReqCur = REQROOT;
				break;
			}
		case DIRC://请求子目录
			{
				m_nReqCur = REQDIRC;
				RecvDirPath(header.len - HEADLEN);
				break;
			}
		case FSIZ://请求文件大小
			{
				m_nReqCur = REQFSIZ;
				RecvFilePathForSize(header.len - HEADLEN);
				break;
			}
		case FDAT://下载文件
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
 * 接收上传文件信息
 */
BOOL CClient::RecvUploadInfor(u_short usLen, u_long ulFileLen, u_long ulFilePos)
{
	char	cRecv;			//接收字符
	int		nReadLen;		//一次接收字节数
	u_short	nTotalLen = 0;	//接收的总数
	m_strFileUL.erase(m_strFileUL.begin(), m_strFileUL.end());	//清除内容
	
	//接收包体，获取上传文件的路径
	while (nTotalLen != usLen)
	{		
		nReadLen = recv(m_sClient, &cRecv, 1, 0);				//每次接收一个字符
		if (SOCKET_ERROR == nReadLen)
		{
			AfxMessageBox(_T("读取目录失败！"));
			return FALSE;
		}else if (0 == nReadLen)
		{
			AfxMessageBox(_T("客户端关闭了连接！"));
			return FALSE;
		}else if (nReadLen > 0)
		{
			m_strFileUL += cRecv;
		}
		nTotalLen += nReadLen;
	}
	
	//定义上传文件线程参数
	pFileUploadParam pThreadParm = new FileUploadParam();
	pThreadParm->ptr = this;
	pThreadParm->ulFileSize = ulFileLen;	//上传文件长度
	pThreadParm->ulFilePos = ulFilePos;		//上传文件的开始位置
	
	//创建上传文件线程
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
 * 上传文件线程函数
 */
DWORD CClient::UploadFileThread(void *pParam)
{
	pFileUploadParam	pFileUpload = (pFileUploadParam)pParam;

	CClient				*pClient = pFileUpload->ptr;			//CClient类指针
	u_long				ulFileSize = pFileUpload->ulFileSize;	//上传文件长度
	u_long				ulFilePos = pFileUpload->ulFilePos;		//文件开始位置
	CRITICAL_SECTION	*pCS = pClient->GetFileUploadCS();		//临界区对象
	CSServerDlg			*pServer = pClient->GetServer();

	SOCKET s = pClient->GetSocket();//套接字
	
	delete pFileUpload;//删除
	pFileUpload = NULL;	
	
	EnterCriticalSection(pCS);//进入临界区
	string strFile = pClient->GetFileUpload();//获取下载文件路径
	FILE *stream = NULL;
	
	if((stream  = fopen( strFile.c_str(), "rb" )) == NULL)//打开文件
	{
		LeaveCriticalSection(pCS);
		AfxMessageBox("打开文件失败！");
		return 0;
	}
	
	int		nSendTimes = ulFileSize / BUF_SIZE +1;	//计算发送的次数
	char	cSend[BUF_SIZE];						//发送文件数据的缓冲区
	int		nErrCode;								//错误代码
	int		i = 0;									//发送次数
	u_long	nlTotal = 0;							//读取文件长度
	u_long	nlRead = 0;								//每次读取文件长度
	int		nReadIndex = 0;							//读序号

	while (nlTotal != ulFileSize &&
					!feof(stream) &&
					pServer->IsConnecting())		//满足发送文件的长度，或者读到文件尾
	{
		nReadIndex++;//读取次数加1
		
		memset(cSend, 0, BUF_SIZE);
		fseek(stream, ulFilePos +  nlTotal, SEEK_SET);		//移动文件指针指定位置	
		
		if (nReadIndex != nSendTimes)
		{
			nlRead = fread(cSend, 1, BUF_SIZE, stream);		//读取数据
		}else{
			u_long ulLeft = ulFileSize - nlTotal;			//最后一次
			nlRead = fread(cSend, 1, ulLeft, stream);		//读取数据 
		}
		
		//定义包头包头
		hdr header;
		header.type = FDAT;
		header.len = HEADLEN + nlRead;
		header.flen = nlRead;
		header.fpos = nlTotal;
		
		//发送包头
		nErrCode = send(s,(char*)&header, HEADLEN, 0);
		if (SOCKET_ERROR == nErrCode)
		{
			AfxMessageBox("发送文件失败！");			
			fclose(stream);
			LeaveCriticalSection(pCS);
			return 0;
		}
		
		//发送包体
		nErrCode = send(s, cSend, nlRead, 0);
		if (SOCKET_ERROR == nErrCode)
		{
			AfxMessageBox("发送文件失败！");
			fclose(stream);
			LeaveCriticalSection(pCS);
			return 0;
		}
		
		nlTotal += nlRead;//增加已读文件长度
	}	
	
	//核实数据
	if (nlTotal != ulFileSize)
	{
		AfxMessageBox("发送文件失败！");
		fclose(stream);	
		LeaveCriticalSection(pCS);
		return 0;		
	}
	
	LeaveCriticalSection(pCS);
	fclose( stream );				//关闭文件	
	
	//发送结束包
	//包头
	hdr header;
	header.type = FDAT;
	header.len = HEADLEN;
	header.flen = 0;//发送文件结束标识
	header.fpos = 0;//发送文件结束标识
	
	//发送包头
	nErrCode = send(s,(char*)&header, HEADLEN, 0);
	if (SOCKET_ERROR == nErrCode)
	{
		AfxMessageBox("发送文件失败！");
		return 0;
	}

	//通知客户端，服务器不再发送数据
	shutdown(s, SD_SEND);
	return 0;
}	


/*
 * 发送数据
 */
BOOL CClient::SendData(void)
{
	BOOL bReVal = TRUE;	//返回值
	switch(m_nReqCur)	//当前请求
	{
	case REQROOT://请求根目录
		{
			if (FALSE == SendRootDir())				
			{
				bReVal = FALSE;
			}			
			m_nReqCur = REQNON;
			break;
		}
	case REQDIRC://请求子目录
		{		
			if(FALSE == SendSubDir())
			{
				bReVal = FALSE;				
			}
			m_nReqCur = REQNON;
			break;
		}
	case REQFSIZ://请求文件大小
		{
			if(FALSE == SendFileSize())
			{
				bReVal = FALSE;
			}	
			m_nReqCur = REQNON;
			break;
		}
	case REQFDAT://下载文件
		{	
			break;
		}
	default:
		break;
	}
	return bReVal;
}

/*
 * 接收子目录路径
 */
BOOL CClient::RecvDirPath(u_short usLen)
{	
	u_short	nTotalLen = 0;	//接收数据的长度
	int		nReadLen;		//一次接收数据的长度
	char	cRecv;			//接收的数据

	while (nTotalLen != usLen)//判断接收是否结束
	{		
		nReadLen = recv(m_sClient, &cRecv, 1, 0);//每次接收一个字符
		if (SOCKET_ERROR == nReadLen)
		{
			AfxMessageBox(_T("读取目录失败！"));
			return FALSE;
		}else if (0 == nReadLen)
		{
			AfxMessageBox(_T("客户端关闭了连接！"));
			return FALSE;
		}else if (nReadLen > 0)
		{
			m_strDir += cRecv;//保存数据
		}
		nTotalLen += nReadLen;
	}

	//更新界面信息
	CString strReq;
	strReq.Format("客户端请求%s子目录。",m_strDir.c_str());
	m_pServer->ShowReqDirInfor(strReq);
	return TRUE;
}


/*
 * 接收文件路径
 */
BOOL CClient::RecvFilePathForSize(u_short usLen)
{
	char	cRecv;			//接收的字符
	int		nReadLen;		//一次接收字节数
	u_short	nTotalLen = 0;	//接收的总数
	m_strFile.erase(m_strFile.begin(), m_strFile.end());
	while (nTotalLen != usLen)
	{		
		nReadLen = recv(m_sClient, &cRecv, 1, 0);//每次接收一个字符
		if (SOCKET_ERROR == nReadLen)
		{
			AfxMessageBox(_T("读取目录失败！"));
			return FALSE;
		}else if (0 == nReadLen)
		{
			AfxMessageBox(_T("客户端关闭了连接！"));
			return FALSE;
		}else if (nReadLen > 0)
		{
			m_strFile += cRecv;
		}
		nTotalLen += nReadLen;
	}
	
	//更新界面信息
	CString strReq;
	strReq.Format("客户端请求下载%s文件。",m_strFile.c_str());	
	m_pServer->ShowReqDirInfor(strReq);
	return TRUE;
}


/*
 * 获取文件长度
 */
u_long CClient::GetFileSize(string strFile)
{
	long retFileLen;//文件长度
	FILE *stream;	//文件指针
	if((stream  = fopen( strFile.c_str(), "rb" )) == NULL)//打开文件
		return 0;
	fseek( stream, 0, SEEK_END);	//指针移动到文件尾
	retFileLen = ftell( stream );	//获取文件长度
	fclose( stream );				//关闭文件
	return retFileLen;
}
