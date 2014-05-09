
#pragma once
#include <string>
#include "tslog/tslog.h"
#include "SocketIPCDefine.h"

class CommonTool
{
public:
	//stringת��Ϊwstring
	static std::wstring StringToWstring(const std::string& str);
	//wstringת��Ϊstring
	static std::string WstringToString(const std::wstring& wstr);

	//��SocketIPC�����н��ͳ�ucFlags��ulType
	static long ParseHeaderFromData(const char* szData, unsigned char &ucFlags, unsigned long &ulType, unsigned long &ulDataLen, LPSOCKETIPC_STREAM lpSocketIpcStream);
	//���SocketIPC����
	static long CombineSocketIpcData(const unsigned char &ucFlag, const unsigned long &ulType, LPSOCKETIPC_STREAM lpSocketIpcStream, char* szSocketIpcData);

	//����SocketIPC����������
	static LPSOCKETIPC_STREAM CreateSocketIpcStream();

	//д��string��SocketIPC����������\r\nΪ�ָ�
	static long StreamWriteString(LPSOCKETIPC_STREAM lpSocketIpcStream, const char* szData);

	//��SocketIPC�������ж�ȡstring
	static long StreamReadString(LPSOCKETIPC_STREAM lpSocketIpcStream, char* szData, int iLen);
};

