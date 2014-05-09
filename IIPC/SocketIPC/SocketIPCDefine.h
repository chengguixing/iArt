
#pragma once

#define WM_DATACOMMUNICATION_SENDCALLBACK		(WM_USER + 1)
#define WM_DATACOMMUNICATION_RECVCALLBACK		(WM_USER + 2)
#define WM_DATACOMMUNICATION_LINKCALLBACK		(WM_USER + 3)
#define WM_DATACOMMUNICATION_DISCONNECTCALLBACK	(WM_USER + 4)

#define SOCKETIPC_SERVERIP	"127.0.0.1"
#define SOCKETIPC_PORT	20248

#define DATA_BUFSIZE	256
#define IPCSTREAM_SIZE	256


//�������������Ϣʱ��������
typedef struct
{
	BOOL bRet;
} LISTEN_MSG_DATA, * LPLISTEN_MSG_DATA;
typedef struct
{
	BOOL bRet;
} CONNECT_MSG_DATA, * LPCONNECT_MSG_DATA;
//���ͷ��������Ϣʱ��������
typedef struct
{
	DWORD dwId;
	DWORD dwErrCode;
} SEND_MSG_DATA, * LPSEND_MSG_DATA;
//���ͽ��������Ϣʱ��������
typedef struct
{
	char	szData[DATA_BUFSIZE];
	DWORD	dwNumberOfBytesRecv;
} RECV_MSG_DATA, * LPRECV_MSG_DATA;

//SocketIPC�����ļ�ͷ
typedef struct
{
	unsigned char	ucFlags;	//0ΪSocketIPC���ݣ�1Ϊ�û�����
	unsigned long	ulType;		//��������
	unsigned long	ulDataLen;	//���ݳ���
} SOCKETIPC_DATA_HEADER, * LPSOCKETIPC_DATA_HEADER;

//����SocketIPC���ݰ���ʽ
typedef struct  
{
	char*			szHeader;
	char*			szData;
	unsigned long	ulHeaderSize;
	unsigned long	ulHeaderFillCount;
	unsigned long	ulDataSize;
	unsigned long	ulDataFillCount;
} SOCKETIPC_DATA, * LPSOCKETIPC_DATA;

//SocketIPC������
typedef struct
{
	unsigned int	uiTotalSize;	//szBuffer����ռ��С
	unsigned int	uiUseSize;		//szBuffer��ʹ�ÿռ��С
	unsigned int	uiCurPos;		//���ĵ�ǰ��дλ��
	char*			szBuffer;		//������
} SOCKETIPC_STREAM, *LPSOCKETIPC_STREAM;

//���ӻص�
typedef long (__stdcall *ONLINKCALLBACK)(DWORD dwErrCode, void* pUserData);
//���ͻص�
typedef long (__stdcall *ONSENDCALLBACK)(DWORD dwErrCode, void* pUserData);
//���ջص�
typedef long (__stdcall *ONRECVCALLBACK)(DWORD dwErrCode, DWORD dwId, LPSOCKETIPC_STREAM lpSocketIpcStream, void* pUserData);
//�Ͽ��ص�
typedef long (__stdcall *ONDISCONNECTCALLBACK)(DWORD dwErrCode, void* pUserData);

//SocketIPC�ӿ�
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

//SocketIPC�ײ�ͨ�Žӿ�
class DataCommunicationInterface
{
public:
	virtual long Init(HWND hWnd) = 0;
	virtual long Link() = 0;
	virtual long SendData(DWORD dwId, const char* szData, const long& lDataLen) = 0;
};