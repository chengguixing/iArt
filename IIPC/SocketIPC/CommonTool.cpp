
#include "stdafx.h"
#include "CommonTool.h"


std::wstring CommonTool::StringToWstring(const std::string& str)
{
	TSDEBUG(L"Enter");

	//获取缓冲区大小，并申请空间，缓冲区大小按字符计算   
	int iLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);   
	wchar_t* wszBuffer = new wchar_t[iLen + 1];   

	//多字节编码转换成宽字节编码   
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), wszBuffer, iLen);   
	wszBuffer[iLen] = '\0';             //添加字符串结尾   

	std::wstring wstr = wszBuffer;
	TSDEBUG(L"wstr = %s", wstr.c_str());

	delete[] wszBuffer;

	return wstr;
}

std::string CommonTool::WstringToString(const std::wstring& wstr)
{
	TSDEBUG(L"Enter, wstr = %s", wstr.c_str());

	//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的   
	int iLen = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);   
	char* szBuffer = new char[iLen + 1];  

	//宽字节编码转换成多字节编码   
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), szBuffer, iLen, NULL, NULL);   
	szBuffer[iLen] = '\0'; 

	std::string str = szBuffer;

	delete[] szBuffer;  

	return str; 
}

long CommonTool::ParseHeaderFromData(const char* szData, unsigned char &ucFlags, unsigned long &ulType, unsigned long &ulDataLen, LPSOCKETIPC_STREAM lpSocketIpcStream)
{
	TSDEBUG(L"Enter");

	LPSOCKETIPC_DATA_HEADER lpSocketIpcDataHeader = (LPSOCKETIPC_DATA_HEADER)szData;
	ucFlags = lpSocketIpcDataHeader->ucFlags;
	ulType = lpSocketIpcDataHeader->ulType;
	ulDataLen = lpSocketIpcDataHeader->ulDataLen;
	TSDEBUG(L"ucFlags = %u, ulType = %u, ulDataLen = %u", ucFlags, ulType, ulDataLen);

	int iHeaderSize = sizeof(SOCKETIPC_DATA_HEADER);
	TSDEBUG(L"iHeaderSize = %d", iHeaderSize);

	long lRet = StreamWriteString(lpSocketIpcStream, &(szData[iHeaderSize]));
	lpSocketIpcStream->uiCurPos = 0;

	return lRet;
}

long CommonTool::CombineSocketIpcData(const unsigned char &ucFlag, const unsigned long &ulType, LPSOCKETIPC_STREAM lpSocketIpcStream, char* szSocketIpcData)
{
	TSDEBUG(L"Enter");

	if (NULL == szSocketIpcData)
	{
		//计算所需大小
		long lNeedLen = sizeof(SOCKETIPC_DATA_HEADER) + lpSocketIpcStream->uiCurPos;
		TSDEBUG(L"lNeedLen = %d", lNeedLen);
		return lNeedLen;
	}

	//协议头
	LPSOCKETIPC_DATA_HEADER lpSocketIpcDataHeader = (LPSOCKETIPC_DATA_HEADER)szSocketIpcData;
	memset(lpSocketIpcDataHeader, 0, sizeof(SOCKETIPC_DATA_HEADER));
	lpSocketIpcDataHeader->ucFlags = ucFlag;
	lpSocketIpcDataHeader->ulType = ulType;
	lpSocketIpcDataHeader->ulDataLen = lpSocketIpcStream->uiCurPos;
	//数据
	int iHeaderSize = sizeof(SOCKETIPC_DATA_HEADER);
	TSDEBUG(L"iHeaderSize = %d, uiCurPost = %u", iHeaderSize, lpSocketIpcStream->uiCurPos);
	memcpy(&(szSocketIpcData[iHeaderSize]), lpSocketIpcStream->szBuffer, lpSocketIpcStream->uiCurPos);

	return 0;
}

LPSOCKETIPC_STREAM CommonTool::CreateSocketIpcStream()
{
	TSDEBUG(L"Enter");

	LPSOCKETIPC_STREAM lpSocketIpcStream = new SOCKETIPC_STREAM;
	lpSocketIpcStream->uiTotalSize = IPCSTREAM_SIZE;
	lpSocketIpcStream->uiCurPos = 0;
	lpSocketIpcStream->uiUseSize = 0;
	lpSocketIpcStream->szBuffer = new char[IPCSTREAM_SIZE];

	return lpSocketIpcStream;
}

long CommonTool::StreamWriteString(LPSOCKETIPC_STREAM lpSocketIpcStream, const char* szData)
{
	TSDEBUG(L"Enter");

	if (NULL == szData)
	{
		TSDEBUG(L"Invalided param, szData = NULL");
		return -1;
	}

	int iDataLen = strlen(szData);
	TSDEBUG(L"iDataLen = %d, uiCurPos = %u, uiTotalSize = %u", iDataLen, lpSocketIpcStream->uiCurPos, lpSocketIpcStream->uiTotalSize);

	if (lpSocketIpcStream->uiCurPos + iDataLen + 2 > lpSocketIpcStream->uiTotalSize)
	{
		lpSocketIpcStream->uiTotalSize = ((lpSocketIpcStream->uiCurPos + iDataLen + 2) / IPCSTREAM_SIZE + 1) * IPCSTREAM_SIZE;
		TSDEBUG(L"lpSocketIpcStream->uiBufferSize = %u", lpSocketIpcStream->uiTotalSize);

		char* szNewBuffer = new char[lpSocketIpcStream->uiTotalSize];
		memcpy(szNewBuffer, lpSocketIpcStream->szBuffer, lpSocketIpcStream->uiCurPos);
		delete lpSocketIpcStream->szBuffer;
		lpSocketIpcStream->szBuffer = szNewBuffer;
	}

	memcpy(&(lpSocketIpcStream->szBuffer[lpSocketIpcStream->uiCurPos]), szData, iDataLen);
	lpSocketIpcStream->uiCurPos += iDataLen;
	lpSocketIpcStream->szBuffer[lpSocketIpcStream->uiCurPos] = '\r';
	lpSocketIpcStream->uiCurPos += 1;
	lpSocketIpcStream->szBuffer[lpSocketIpcStream->uiCurPos] = '\n';
	lpSocketIpcStream->uiCurPos += 1;
	TSDEBUG(L"uiCurPos = %u", lpSocketIpcStream->uiCurPos);

	return 0;
}

long CommonTool::StreamReadString(LPSOCKETIPC_STREAM lpSocketIpcStream, char* szData, int iLen)
{
	TSDEBUG(L"Enter");

	if (NULL == szData)
	{
		std::string str = lpSocketIpcStream->szBuffer;
		std::size_t uiPos = str.find("\r\n", lpSocketIpcStream->uiCurPos);
		TSDEBUG(L"str: size = %d, length = %d, uiPos = %d", str.size(), str.length(), uiPos);
		if (std::string::npos == uiPos)
		{
			TSDEBUG(L"Can not find decollator");
			return -1;
		}

		return uiPos - lpSocketIpcStream->uiCurPos + 1;
	}

	memcpy(szData, &(lpSocketIpcStream->szBuffer[lpSocketIpcStream->uiCurPos]), iLen - 1);
	szData[iLen - 1] = '\0';
	lpSocketIpcStream->uiCurPos += iLen;
	lpSocketIpcStream->uiCurPos += 1;

	return 0;
}