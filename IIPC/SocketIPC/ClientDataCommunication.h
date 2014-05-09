
#pragma once
#include "SocketIPCDefine.h"


class ClientDataCommunication : public DataCommunicationInterface
{
public:
	//Socket信息
	typedef struct 
	{
		//CHAR Buffer[DATA_BUFSIZE];
		CHAR* Buffer;
		WSABUF DataBuf;
		SOCKET Socket;
		DWORD BytesSEND;
		DWORD BytesRECV;
		DWORD Id;
		DWORD BufferSize;
	} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;

public:
	ClientDataCommunication();
	~ClientDataCommunication();

public:
	// 初始化
	long Init(HWND hWnd);
	//连接
	long Link();
	//发送数据
	long SendData(DWORD dwId, const char* szData, const long& lDataLen);

private:
	//创建SOCKET信息
	long CreateSocketInformation(SOCKET localSocket);
	//销毁SOCKET信息
	long FreeSocketInformation(DWORD dwEvent);

	//SOCKET事件处理线程
	static unsigned __stdcall ClientWorkerThread(void* pParam);

	//发送连接完成消息
	long PostConnectCompleteMsg(BOOL bRet = TRUE);
	//发送发送完成消息
	long PostSendCompleteMsg(DWORD dwId, DWORD dwErrCode);
	//发送接收完成消息
	long PostRecvCompleteMsg(const char* szData, DWORD dwNumberOfBytesRecv);
	//发送连接断开消息
	long PostDisconnectMsg();

private:
	HWND					m_hWnd;										//接收DataDataCommunication信息的窗口句柄
	SOCKET					m_localSocket;								//本地SOCKET
	DWORD					m_dwEventTotal;								//事件总数
	WSAEVENT				m_aEventArray[WSA_MAXIMUM_WAIT_EVENTS];
	LPSOCKET_INFORMATION	m_aSocketInfo[WSA_MAXIMUM_WAIT_EVENTS];
};