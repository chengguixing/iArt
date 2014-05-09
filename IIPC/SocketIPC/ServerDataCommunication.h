
/*///////////////////////////////////////////////////////////////////

SocketIPC底层通信模块，对Socket进行IO完成端口封装

///////////////////////////////////////////////////////////////////*/

#pragma once
#include "SocketIPCDefine.h"


class ServerDataCommunication : public DataCommunicationInterface
{
public:
	//传递完成端口处理线程的结构体
	typedef struct  
	{
		HANDLE		hIoCompletionPort;
		ServerDataCommunication*	pThis;
	} SERVERWORKER_THREADPARAM, * LPSERVERWORKER_THREADPARAM;

	//重叠端口扩展结构体
	typedef struct
	{
		OVERLAPPED Overlapped;
		WSABUF DataBuf;
		//CHAR Buffer[DATA_BUFSIZE];
		CHAR* Buffer;
		DWORD BytesSEND;
		DWORD BytesRECV;
		BOOL IsSEND;
		DWORD Id;
	} PER_IO_OPERATION_DATA, * LPPER_IO_OPERATION_DATA;

	//完成端口KEY
	typedef struct 
	{
		SOCKET Socket;
	} PER_HANDLE_DATA, * LPPER_HANDLE_DATA;

public:
	ServerDataCommunication();
	~ServerDataCommunication();

public:
	// 初始化
	long Init(HWND hWnd);
	// 监听连接
	long Link();

	// 发送数据
	long SendData(DWORD dwId, const char* szData, const long& lDataLen);

private:
	//完成端口处理线程
	static unsigned __stdcall ServerWorkerThread(void* pParam);
	//监听线程
	static unsigned __stdcall ListenThread(void* pParam);

	//监听完成消息
	long PostListenCompleteMsg(BOOL bRet = TRUE);
	//发送发送完成消息
	long PostSendCompleteMsg(DWORD dwId, DWORD dwErrCode);
	//发送接收完成消息
	long PostRecvCompleteMsg(const char* szData, DWORD dwNumberOfBytesRecv);
	//发送连接断开消息
	long PostDisconnectMsg();

private:
	HWND		m_hWnd;					//接收ServerDataCommunication信息的窗口句柄
	HANDLE		m_hIoCompletionPort;	//完成端口句柄
	SOCKET		m_serverSocket;			//服务端SOCKET
	SOCKET		m_localSocket;			//暂时默认只有一个客户端连接
};