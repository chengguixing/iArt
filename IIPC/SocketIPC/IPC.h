
/*///////////////////////////////////////////////////////////////////

SocketIPC上层IPC逻辑封装模块，使用DataCommunication来实现IPC通信
通信数据格式：
unsigned char ucFlags;		//0为SocketIPC数据，1为用户数据
unsigned long ulType;		//数据类型
char* szData;				//具体数据

注：目前假设只有一个客户端

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
	//监听回调数据
	typedef struct  
	{
		ONLINKCALLBACK		pfnOnLinkCallback;
		void*				pUserData;
	} IPC_LINK_CALLBACK, * LPIPC_LINK_CALLBACK;
	//发送回调数据
	typedef struct
	{
		ONSENDCALLBACK	pfnOnSendCallback;
		void*			pUserData;
		DWORD			dwId;
	} IPC_SEND_CALLBACK, * LPIPC_SEND_CALLBACK;
	//接收数据回调
	typedef struct
	{
		ONRECVCALLBACK	pfnRecvCallback;
		void*			pUserData;
	} IPC_RECV_CALLBACK, * LPIPC_RECV_CALLBACK;
	//断开回调数据
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
	//初始化
	virtual long Init(IpcType ipcType);
	//监听
	virtual long Link(ONLINKCALLBACK pfnOnListenCallback, void* pUserData);

	//发送数据
	virtual long Send(DWORD dwId, LPSOCKETIPC_STREAM lpSocketIpcStream, ONSENDCALLBACK pfnOnSendCallback, void* pUserData);
	//注册接收数据回调函数
	virtual long RegisterRecvCallback(ONRECVCALLBACK pfnOnRecvCallback, void* pUserData);
	//设置断开回调函数
	virtual long RegisterDislinkCallback(ONDISCONNECTCALLBACK pfnDisconnectCallback, void* pUserData);

private:
	DataCommunicationInterface*			m_DataCommunicationInterface;	//DataCommunicationInterface通信模块实例
	BOOL								m_bLinkCompletely;			//是否完成连接
	SOCKETIPC_DATA						m_SocketIpcData;				//SocketIPC数据
	IPC_LINK_CALLBACK					m_LinkCallbackInfo;			//监听回调信息
	IPC_RECV_CALLBACK					m_RecvCallbackInfo;				//接收回调信息
	IPC_DISCONNECT_CALLBACK				m_DisconnectCallbackInfo;
	std::map<DWORD, IPC_SEND_CALLBACK>	m_mSendCallbackInfo;			//发送回调信息
};