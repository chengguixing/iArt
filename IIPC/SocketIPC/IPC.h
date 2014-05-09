
/*///////////////////////////////////////////////////////////////////

SocketIPC�ϲ�IPC�߼���װģ�飬ʹ��DataCommunication��ʵ��IPCͨ��
ͨ�����ݸ�ʽ��
unsigned char ucFlags;		//0ΪSocketIPC���ݣ�1Ϊ�û�����
unsigned long ulType;		//��������
char* szData;				//��������

ע��Ŀǰ����ֻ��һ���ͻ���

///////////////////////////////////////////////////////////////////*/

#pragma once
#include <atlbase.h>
#include <atlwin.h>
#include "ClientDataCommunication.h"
#include "ServerDataCommunication.h"
#include <map>

class IPC : public IPCInterface, public CWindowImpl<IPC>
{
public:
	//�����ص�����
	typedef struct  
	{
		ONLINKCALLBACK		pfnOnLinkCallback;
		void*				pUserData;
	} IPC_LINK_CALLBACK, * LPIPC_LINK_CALLBACK;
	//���ͻص�����
	typedef struct
	{
		ONSENDCALLBACK	pfnOnSendCallback;
		void*			pUserData;
		DWORD			dwId;
	} IPC_SEND_CALLBACK, * LPIPC_SEND_CALLBACK;
	//�������ݻص�
	typedef struct
	{
		ONRECVCALLBACK	pfnRecvCallback;
		void*			pUserData;
	} IPC_RECV_CALLBACK, * LPIPC_RECV_CALLBACK;
	//�Ͽ��ص�����
	typedef struct
	{
		ONDISCONNECTCALLBACK	pfnDisconnectCallback;
		void*					pUserData;
	} IPC_DISCONNECT_CALLBACK, * LPIPC_DISCONNECT_CALLBACK;

public:
	DECLARE_WND_CLASS(L"IPC_WndClass")

	BEGIN_MSG_MAP(IPC)
		MESSAGE_HANDLER(WM_DATACOMMUNICATION_LINKCALLBACK, OnLinkCallback)
		MESSAGE_HANDLER(WM_DATACOMMUNICATION_SENDCALLBACK, OnSendCallback)
		MESSAGE_HANDLER(WM_DATACOMMUNICATION_RECVCALLBACK, OnRecvCallback)
		MESSAGE_HANDLER(WM_DATACOMMUNICATION_DISCONNECTCALLBACK, OnDisconnectCallback)
	END_MSG_MAP()

	IPC();
	~IPC();

	LRESULT OnLinkCallback(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSendCallback(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRecvCallback(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDisconnectCallback(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	//��ʼ��
	virtual long Init(IpcType ipcType);
	//����
	virtual long Link(ONLINKCALLBACK pfnOnListenCallback, void* pUserData);

	//��������
	virtual long Send(DWORD dwId, LPSOCKETIPC_STREAM lpSocketIpcStream, ONSENDCALLBACK pfnOnSendCallback, void* pUserData);
	//ע��������ݻص�����
	virtual long RegisterRecvCallback(ONRECVCALLBACK pfnOnRecvCallback, void* pUserData);
	//���öϿ��ص�����
	virtual long RegisterDislinkCallback(ONDISCONNECTCALLBACK pfnDisconnectCallback, void* pUserData);

private:
	DataCommunicationInterface*			m_DataCommunicationInterface;	//DataCommunicationInterfaceͨ��ģ��ʵ��
	BOOL								m_bLinkCompletely;			//�Ƿ��������
	SOCKETIPC_DATA						m_SocketIpcData;				//SocketIPC����
	IPC_LINK_CALLBACK					m_LinkCallbackInfo;			//�����ص���Ϣ
	IPC_RECV_CALLBACK					m_RecvCallbackInfo;				//���ջص���Ϣ
	IPC_DISCONNECT_CALLBACK				m_DisconnectCallbackInfo;
	std::map<DWORD, IPC_SEND_CALLBACK>	m_mSendCallbackInfo;			//���ͻص���Ϣ
};