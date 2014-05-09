
/*///////////////////////////////////////////////////////////////////

SocketIPC�ײ�ͨ��ģ�飬��Socket����IO��ɶ˿ڷ�װ

///////////////////////////////////////////////////////////////////*/

#pragma once
#include "SocketIPCDefine.h"


class ServerDataCommunication : public DataCommunicationInterface
{
public:
	//������ɶ˿ڴ����̵߳Ľṹ��
	typedef struct  
	{
		HANDLE		hIoCompletionPort;
		ServerDataCommunication*	pThis;
	} SERVERWORKER_THREADPARAM, * LPSERVERWORKER_THREADPARAM;

	//�ص��˿���չ�ṹ��
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

	//��ɶ˿�KEY
	typedef struct 
	{
		SOCKET Socket;
	} PER_HANDLE_DATA, * LPPER_HANDLE_DATA;

public:
	ServerDataCommunication();
	~ServerDataCommunication();

public:
	// ��ʼ��
	long Init(HWND hWnd);
	// ��������
	long Link();

	// ��������
	long SendData(DWORD dwId, const char* szData, const long& lDataLen);

private:
	//��ɶ˿ڴ����߳�
	static unsigned __stdcall ServerWorkerThread(void* pParam);
	//�����߳�
	static unsigned __stdcall ListenThread(void* pParam);

	//���������Ϣ
	long PostListenCompleteMsg(BOOL bRet = TRUE);
	//���ͷ��������Ϣ
	long PostSendCompleteMsg(DWORD dwId, DWORD dwErrCode);
	//���ͽ��������Ϣ
	long PostRecvCompleteMsg(const char* szData, DWORD dwNumberOfBytesRecv);
	//�������ӶϿ���Ϣ
	long PostDisconnectMsg();

private:
	HWND		m_hWnd;					//����ServerDataCommunication��Ϣ�Ĵ��ھ��
	HANDLE		m_hIoCompletionPort;	//��ɶ˿ھ��
	SOCKET		m_serverSocket;			//�����SOCKET
	SOCKET		m_localSocket;			//��ʱĬ��ֻ��һ���ͻ�������
};