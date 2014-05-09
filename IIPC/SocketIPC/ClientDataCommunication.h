
#pragma once
#include "SocketIPCDefine.h"


class ClientDataCommunication : public DataCommunicationInterface
{
public:
	//Socket��Ϣ
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
	// ��ʼ��
	long Init(HWND hWnd);
	//����
	long Link();
	//��������
	long SendData(DWORD dwId, const char* szData, const long& lDataLen);

private:
	//����SOCKET��Ϣ
	long CreateSocketInformation(SOCKET localSocket);
	//����SOCKET��Ϣ
	long FreeSocketInformation(DWORD dwEvent);

	//SOCKET�¼������߳�
	static unsigned __stdcall ClientWorkerThread(void* pParam);

	//�������������Ϣ
	long PostConnectCompleteMsg(BOOL bRet = TRUE);
	//���ͷ��������Ϣ
	long PostSendCompleteMsg(DWORD dwId, DWORD dwErrCode);
	//���ͽ��������Ϣ
	long PostRecvCompleteMsg(const char* szData, DWORD dwNumberOfBytesRecv);
	//�������ӶϿ���Ϣ
	long PostDisconnectMsg();

private:
	HWND					m_hWnd;										//����DataDataCommunication��Ϣ�Ĵ��ھ��
	SOCKET					m_localSocket;								//����SOCKET
	DWORD					m_dwEventTotal;								//�¼�����
	WSAEVENT				m_aEventArray[WSA_MAXIMUM_WAIT_EVENTS];
	LPSOCKET_INFORMATION	m_aSocketInfo[WSA_MAXIMUM_WAIT_EVENTS];
};