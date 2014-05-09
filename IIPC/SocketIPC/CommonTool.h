
#pragma once
#include <string>
#include "tslog/tslog.h"
#include "SocketIPCDefine.h"

class CommonTool
{
public:
	//string转换为wstring
	static std::wstring StringToWstring(const std::string& str);
	//wstring转换为string
	static std::string WstringToString(const std::wstring& wstr);

	//从SocketIPC数据中解释出ucFlags和ulType
	static long ParseHeaderFromData(const char* szData, unsigned char &ucFlags, unsigned long &ulType, unsigned long &ulDataLen, LPSOCKETIPC_STREAM lpSocketIpcStream);
	//组合SocketIPC数据
	static long CombineSocketIpcData(const unsigned char &ucFlag, const unsigned long &ulType, LPSOCKETIPC_STREAM lpSocketIpcStream, char* szSocketIpcData);

	//创建SocketIPC数据流对象
	static LPSOCKETIPC_STREAM CreateSocketIpcStream();

	//写入string到SocketIPC数据流，以\r\n为分割
	static long StreamWriteString(LPSOCKETIPC_STREAM lpSocketIpcStream, const char* szData);

	//从SocketIPC数据流中读取string
	static long StreamReadString(LPSOCKETIPC_STREAM lpSocketIpcStream, char* szData, int iLen);
};

