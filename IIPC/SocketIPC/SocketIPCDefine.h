
#pragma once

#define WM_DATACOMMUNICATION_SENDCALLBACK		(WM_USER + 1)
#define WM_DATACOMMUNICATION_RECVCALLBACK		(WM_USER + 2)
#define WM_DATACOMMUNICATION_LINKCALLBACK		(WM_USER + 3)
#define WM_DATACOMMUNICATION_DISCONNECTCALLBACK	(WM_USER + 4)

#define SOCKETIPC_SERVERIP	"127.0.0.1"
#define SOCKETIPC_PORT	20248

#define DATA_BUFSIZE	256
#define IPCSTREAM_SIZE	256


//发送连接完成消息时所带数据
typedef struct
{
	BOOL bRet;
} LISTEN_MSG_DATA, * LPLISTEN_MSG_DATA;
typedef struct
{
	BOOL bRet;
} CONNECT_MSG_DATA, * LPCONNECT_MSG_DATA;
//发送发送完成消息时所带数据
typedef struct
{
	DWORD dwId;
	DWORD dwErrCode;
} SEND_MSG_DATA, * LPSEND_MSG_DATA;
//发送接收完成消息时所带数据
typedef struct
{
	char	szData[DATA_BUFSIZE];
	DWORD	dwNumberOfBytesRecv;
} RECV_MSG_DATA, * LPRECV_MSG_DATA;

//SocketIPC数据文件头
typedef struct
{
	unsigned char	ucFlags;	//0为SocketIPC数据，1为用户数据
	unsigned long	ulType;		//数据类型
	unsigned long	ulDataLen;	//数据长度
} SOCKETIPC_DATA_HEADER, * LPSOCKETIPC_DATA_HEADER;

//接收SocketIPC数据包格式
typedef struct  
{
	char*			szHeader;
	char*			szData;
	unsigned long	ulHeaderSize;
	unsigned long	ulHeaderFillCount;
	unsigned long	ulDataSize;
	unsigned long	ulDataFillCount;
} SOCKETIPC_DATA, * LPSOCKETIPC_DATA;

//SocketIPC数据流
typedef struct
{
	unsigned int	uiTotalSize;	//szBuffer分配空间大小
	unsigned int	uiUseSize;		//szBuffer已使用空间大小
	unsigned int	uiCurPos;		//流的当前读写位置
	char*			szBuffer;		//缓冲区
} SOCKETIPC_STREAM, *LPSOCKETIPC_STREAM;

//连接回调
typedef long (__stdcall *ONLINKCALLBACK)(DWORD dwErrCode, void* pUserData);
//发送回调
typedef long (__stdcall *ONSENDCALLBACK)(DWORD dwErrCode, void* pUserData);
//接收回调
typedef long (__stdcall *ONRECVCALLBACK)(DWORD dwErrCode, DWORD dwId, LPSOCKETIPC_STREAM lpSocketIpcStream, void* pUserData);
//断开回调
typedef long (__stdcall *ONDISCONNECTCALLBACK)(DWORD dwErrCode, void* pUserData);

//SocketIPC接口
class IPCInterface
{
public:
	typedef enum tagIpcType
	{
		IpcType_Client = 0,
		IpcType_Server
	}IpcType;
public:
	virtual long Init(IpcType ipcType) = 0;
	virtual long Link(ONLINKCALLBACK pfnOnListenCallback, void* pUserData) = 0;
	virtual long Send(DWORD dwId, LPSOCKETIPC_STREAM lpSocketIpcStream, ONSENDCALLBACK pfnOnSendCallback, void* pUserData) = 0;
	virtual long RegisterRecvCallback(ONRECVCALLBACK pfnOnRecvCallback, void* pUserData) = 0;
	virtual long RegisterDislinkCallback(ONDISCONNECTCALLBACK pfnDisconnectCallback, void* pUserData) = 0;
};

//SocketIPC底层通信接口
class DataCommunicationInterface
{
public:
	virtual long Init(HWND hWnd) = 0;
	virtual long Link() = 0;
	virtual long SendData(DWORD dwId, const char* szData, const long& lDataLen) = 0;
};