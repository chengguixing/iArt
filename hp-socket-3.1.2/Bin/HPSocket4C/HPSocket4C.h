/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 3.1.2
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 75375912
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
Module:  HPSocket for C DLL

Desc: ������ C ���������������ԣ��磺C / C# / Delphi �ȣ��ܷ����ʹ�� HPSocket

Usage:
		����һ��
		--------------------------------------------------------------------------------------
		0. ��C/C++ ���򣩰��� HPSocket4C.h ͷ�ļ�
		1. ���� ::Create_HP_XxxListener() ������������������
		2. ���� ::Create_HP_Xxx(pListener) �������� HPSocket ����
		3. ���� ::HP_Set_FN_Xxx_OnYyy(pListener, ...) �������ü������Ļص�����
		4. ������ص����������� HPSocket ����
		5. ...... ......
		6. ���� ::Destroy_HP_Xxx(pSocket) �������� HPSocket ����
		7. ���� ::Destroy_HP_XxxListener(pListener) �������ټ���������

		��������
		--------------------------------------------------------------------------------------
		1. Ӧ�ó������Ҫ�õ��ĵ���������װ���ض����Եİ�װ����
		2. ͨ����װ���װ�����������ķ�ʽʹ�� HPSocket

Release:
		1. x86/HPSocket4C.dll		- (32λ/MBCS/Release)
		2. x86/HPSocket4C_D.dll		- (32λ/MBCS/DeBug)
		3. x86/HPSocket4C_U.dll		- (32λ/UNICODE/Release)
		4. x86/HPSocket4C_UD.dll	- (32λ/UNICODE/DeBug)
		5. x64/HPSocket4C.dll		- (64λ/MBCS/Release)
		6. x64/HPSocket4C_D.dll		- (64λ/MBCS/DeBug)
		7. x64/HPSocket4C_U.dll		- (64λ/UNICODE/Release)
		8. x64/HPSocket4C_UD.dll	- (64λ/UNICODE/DeBug)

******************************************************************************/

#pragma once

/**************************************************/
/********* imports / exports HPSocket.dll *********/

#ifdef HPSOCKET_EXPORTS
	#define HPSOCKET_API extern "C" __declspec(dllexport)
#else
	#define HPSOCKET_API extern "C" __declspec(dllimport)
#endif

/************************************************************************
���ƣ����� ID ��������
�������������� ID ����������
************************************************************************/
typedef ULONG_PTR	HP_CONNID;

/************************************************************************
���ƣ����� Socket ����ָ�����ͱ���
�������� Socket ����ָ�붨��Ϊ��ֱ�۵ı���
************************************************************************/

typedef PVOID		HP_Object;

typedef HP_Object	HP_Server;
typedef HP_Object	HP_Client;
typedef HP_Object	HP_TcpServer;
typedef HP_Object	HP_TcpClient;
typedef HP_Object	HP_TcpPullServer;
typedef HP_Object	HP_TcpPullClient;
typedef HP_Object	HP_UdpServer;
typedef HP_Object	HP_UdpClient;

typedef HP_Object	HP_Listener;
typedef HP_Object	HP_ServerListener;
typedef HP_Object	HP_ClientListener;
typedef HP_Object	HP_TcpServerListener;
typedef HP_Object	HP_TcpClientListener;
typedef HP_Object	HP_TcpPullServerListener;
typedef HP_Object	HP_TcpPullClientListener;
typedef HP_Object	HP_UdpServerListener;
typedef HP_Object	HP_UdpClientListener;

/*****************************************************************************************************/
/******************************************** �����ࡢ�ӿ� ********************************************/
/*****************************************************************************************************/

/************************************************************************
���ƣ�ͨ���������״̬
������Ӧ�ó������ͨ��ͨ������� GetState() ������ȡ�����ǰ����״̬
************************************************************************/
enum En_HP_ServiceState
{
	HP_SS_STARTING	= 0,	// ��������
	HP_SS_STARTED	= 1,	// �Ѿ�����
	HP_SS_STOPING	= 2,	// ����ֹͣ
	HP_SS_STOPED	= 3,	// �Ѿ�����
};

/************************************************************************
���ƣ�Socket ��������
������Ӧ�ó���� OnErrror() �¼���ͨ���ò�����ʶ�����ֲ������µĴ���
************************************************************************/
enum En_HP_SocketOperation
{
	HP_SO_UNKNOWN	= 0,	// Unknown
	HP_SO_ACCEPT	= 1,	// Acccept
	HP_SO_CONNECT	= 2,	// Connnect
	HP_SO_SEND		= 3,	// Send
	HP_SO_RECEIVE	= 4,	// Receive
};

/************************************************************************
���ƣ��¼�֪ͨ������
�������¼�֪ͨ�ķ���ֵ����ͬ�ķ���ֵ��Ӱ��ͨ������ĺ�����Ϊ
************************************************************************/
enum En_HP_HandleResult
{
	HP_HR_OK		= 0,	// �ɹ�
	HP_HR_IGNORE	= 1,	// ����
	HP_HR_ERROR		= 2,	// ����
};

/************************************************************************
���ƣ������������
������Start() / Stop() ����ִ��ʧ��ʱ����ͨ�� GetLastError() ��ȡ�������
************************************************************************/
enum En_HP_ServerError
{
	HP_SE_OK					= 0,	// �ɹ�
	HP_SE_ILLEGAL_STATE			= 1,	// ��ǰ״̬���������
	HP_SE_INVALID_PARAM			= 2,	// �Ƿ�����
	HP_SE_SOCKET_CREATE			= 3,	// �������� SOCKET ʧ��
	HP_SE_SOCKET_BIND			= 4,	// �󶨼�����ַʧ��
	HP_SE_SOCKET_PREPARE		= 5,	// ���ü��� SOCKET ʧ��
	HP_SE_SOCKET_LISTEN			= 6,	// ��������ʧ��
	HP_SE_CP_CREATE				= 7,	// ������ɶ˿�ʧ��
	HP_SE_WORKER_THREAD_CREATE	= 8,	// ���������߳�ʧ��
	HP_SE_DETECT_THREAD_CREATE	= 9,	// ��������߳�ʧ��
	HP_SE_SOCKE_ATTACH_TO_CP	= 10,	// ���� SOCKET �󶨵���ɶ˿�ʧ��
};

/************************************************************************
���ƣ������������
������Start() / Stop() ����ִ��ʧ��ʱ����ͨ�� GetLastError() ��ȡ�������
************************************************************************/
enum En_HP_ClientError
{
	HP_CE_OK					= 0,	// �ɹ�
	HP_CE_ILLEGAL_STATE			= 1,	// ��ǰ״̬���������
	HP_CE_INVALID_PARAM			= 2,	// �Ƿ�����
	HP_CE_SOCKET_CREATE_FAIL	= 3,	// ���� Client Socket ʧ��
	HP_CE_SOCKET_PREPARE_FAIL	= 4,	// ���� Client Socket ʧ��
	HP_CE_CONNECT_SERVER_FAIL	= 5,	// ���ӷ�����ʧ��
	HP_CE_WORKER_CREATE_FAIL	= 6,	// ���������߳�ʧ��
	HP_CE_DETECTOR_CREATE_FAIL	= 7,	// ��������߳�ʧ��
	HP_CE_NETWORK_ERROR			= 8,	// �������
	HP_CE_DATA_PROC_ERROR		= 9,	// ���ݴ������
};

/************************************************************************
���ƣ�����ץȡ���
����������ץȡ�����ķ���ֵ
************************************************************************/
enum En_HP_FetchResult
{
	HP_FR_OK				= 0,	// �ɹ�
	HP_FR_LENGTH_TOO_LONG	= 1,	// ץȡ���ȹ���
	HP_FR_DATA_NOT_FOUND	= 2,	// �Ҳ��� ConnID ��Ӧ������
};

/****************************************************/
/************** HPSocket4C.dll �ص����� **************/

/* �����ص����� */
typedef En_HP_HandleResult (__stdcall *HP_FN_OnSend)			(HP_CONNID dwConnID, const BYTE* pData, int iLength);
typedef En_HP_HandleResult (__stdcall *HP_FN_OnReceive)			(HP_CONNID dwConnID, const BYTE* pData, int iLength);
typedef En_HP_HandleResult (__stdcall *HP_FN_OnPullReceive)		(HP_CONNID dwConnID, int iLength);
typedef En_HP_HandleResult (__stdcall *HP_FN_OnClose)			(HP_CONNID dwConnID);
typedef En_HP_HandleResult (__stdcall *HP_FN_OnError)			(HP_CONNID dwConnID, En_HP_SocketOperation enOperation, int iErrorCode);

/* ����˻ص����� */
typedef En_HP_HandleResult (__stdcall *HP_FN_OnPrepareListen)	(UINT_PTR soListen);
// ���Ϊ TCP ���ӣ�pClientΪ SOCKET ��������Ϊ UDP ���ӣ�pClientΪ SOCKADDR_IN ָ�룻
typedef En_HP_HandleResult (__stdcall *HP_FN_OnAccept)			(HP_CONNID dwConnID, UINT_PTR pClient);
typedef En_HP_HandleResult (__stdcall *HP_FN_OnServerShutdown)	();

/* �ͻ��˻ص����� */
typedef En_HP_HandleResult (__stdcall *HP_FN_OnPrepareConnect)	(HP_CONNID dwConnID, UINT_PTR socket);
typedef En_HP_HandleResult (__stdcall *HP_FN_OnConnect)			(HP_CONNID dwConnID);

/****************************************************/
/************** HPSocket4C.dll �������� **************/

// ���� HP_TcpServer ����
HPSOCKET_API HP_TcpServer __stdcall Create_HP_TcpServer(HP_TcpServerListener pListener);
// ���� HP_TcpClient ����
HPSOCKET_API HP_TcpClient __stdcall Create_HP_TcpClient(HP_TcpClientListener pListener);
// ���� HP_TcpPullServer ����
HPSOCKET_API HP_TcpPullServer __stdcall Create_HP_TcpPullServer(HP_TcpPullServerListener pListener);
// ���� HP_TcpPullClient ����
HPSOCKET_API HP_TcpPullClient __stdcall Create_HP_TcpPullClient(HP_TcpPullClientListener pListener);
// ���� HP_UdpServer ����
HPSOCKET_API HP_UdpServer __stdcall Create_HP_UdpServer(HP_UdpServerListener pListener);
// ���� HP_UdpClient ����
HPSOCKET_API HP_UdpClient __stdcall Create_HP_UdpClient(HP_UdpClientListener pListener);

// ���� HP_TcpServer ����
HPSOCKET_API void __stdcall Destroy_HP_TcpServer(HP_TcpServer pServer);
// ���� HP_TcpClient ����
HPSOCKET_API void __stdcall Destroy_HP_TcpClient(HP_TcpClient pClient);
// ���� HP_TcpPullServer ����
HPSOCKET_API void __stdcall Destroy_HP_TcpPullServer(HP_TcpPullServer pServer);
// ���� HP_TcpPullClient ����
HPSOCKET_API void __stdcall Destroy_HP_TcpPullClient(HP_TcpPullClient pClient);
// ���� HP_UdpServer ����
HPSOCKET_API void __stdcall Destroy_HP_UdpServer(HP_UdpServer pServer);
// ���� HP_UdpClient ����
HPSOCKET_API void __stdcall Destroy_HP_UdpClient(HP_UdpClient pClient);

// ���� HP_TcpServerListener ����
HPSOCKET_API HP_TcpServerListener __stdcall Create_HP_TcpServerListener();
// ���� HP_TcpClientListener ����
HPSOCKET_API HP_TcpClientListener __stdcall Create_HP_TcpClientListener();
// ���� HP_TcpPullServerListener ����
HPSOCKET_API HP_TcpPullServerListener __stdcall Create_HP_TcpPullServerListener();
// ���� HP_TcpPullClientListener ����
HPSOCKET_API HP_TcpPullClientListener __stdcall Create_HP_TcpPullClientListener();
// ���� HP_UdpServerListener ����
HPSOCKET_API HP_UdpServerListener __stdcall Create_HP_UdpServerListener();
// ���� HP_UdpClientListener ����
HPSOCKET_API HP_UdpClientListener __stdcall Create_HP_UdpClientListener();

// ���� HP_TcpServerListener ����
HPSOCKET_API void __stdcall Destroy_HP_TcpServerListener(HP_TcpServerListener pListener);
// ���� HP_TcpClientListener ����
HPSOCKET_API void __stdcall Destroy_HP_TcpClientListener(HP_TcpClientListener pListener);
// ���� HP_TcpPullServerListener ����
HPSOCKET_API void __stdcall Destroy_HP_TcpPullServerListener(HP_TcpPullServerListener pListener);
// ���� HP_TcpPullClientListener ����
HPSOCKET_API void __stdcall Destroy_HP_TcpPullClientListener(HP_TcpPullClientListener pListener);
// ���� HP_UdpServerListener ����
HPSOCKET_API void __stdcall Destroy_HP_UdpServerListener(HP_UdpServerListener pListener);
// ���� HP_UdpClientListener ����
HPSOCKET_API void __stdcall Destroy_HP_UdpClientListener(HP_UdpClientListener pListener);

/**********************************************************************************/
/***************************** Server �ص��������÷��� *****************************/

HPSOCKET_API void __stdcall HP_Set_FN_Server_OnPrepareListen(HP_ServerListener pListener, HP_FN_OnPrepareListen fn);
HPSOCKET_API void __stdcall HP_Set_FN_Server_OnAccept(HP_ServerListener pListener, HP_FN_OnAccept fn);
HPSOCKET_API void __stdcall HP_Set_FN_Server_OnSend(HP_ServerListener pListener, HP_FN_OnSend fn);
HPSOCKET_API void __stdcall HP_Set_FN_Server_OnReceive(HP_ServerListener pListener, HP_FN_OnReceive fn);
HPSOCKET_API void __stdcall HP_Set_FN_Server_OnPullReceive(HP_ServerListener pListener, HP_FN_OnPullReceive fn);
HPSOCKET_API void __stdcall HP_Set_FN_Server_OnClose(HP_ServerListener pListener, HP_FN_OnClose fn);
HPSOCKET_API void __stdcall HP_Set_FN_Server_OnError(HP_ServerListener pListener, HP_FN_OnError fn);
HPSOCKET_API void __stdcall HP_Set_FN_Server_OnServerShutdown(HP_ServerListener pListener, HP_FN_OnServerShutdown fn);

/**********************************************************************************/
/***************************** Client �ص��������÷��� *****************************/

HPSOCKET_API void __stdcall HP_Set_FN_Client_OnPrepareConnect(HP_ClientListener pListener, HP_FN_OnPrepareConnect fn);
HPSOCKET_API void __stdcall HP_Set_FN_Client_OnConnect(HP_ClientListener pListener, HP_FN_OnConnect fn);
HPSOCKET_API void __stdcall HP_Set_FN_Client_OnSend(HP_ClientListener pListener, HP_FN_OnSend fn);
HPSOCKET_API void __stdcall HP_Set_FN_Client_OnReceive(HP_ClientListener pListener, HP_FN_OnReceive fn);
HPSOCKET_API void __stdcall HP_Set_FN_Client_OnPullReceive(HP_ClientListener pListener, HP_FN_OnPullReceive fn);
HPSOCKET_API void __stdcall HP_Set_FN_Client_OnClose(HP_ClientListener pListener, HP_FN_OnClose fn);
HPSOCKET_API void __stdcall HP_Set_FN_Client_OnError(HP_ClientListener pListener, HP_FN_OnError fn);

/**************************************************************************/
/***************************** Server �������� *****************************/

/*
* ���ƣ�����ͨ�����
* ���������������ͨ�������������ɺ�ɿ�ʼ���տͻ������Ӳ��շ�����
*		
* ������		pszBindAddress	-- ������ַ
*			usPort			-- �����˿�
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __stdcall HP_Server_Start(HP_Server pServer, LPCTSTR pszBindAddress, USHORT usPort);

/*
* ���ƣ��ر�ͨ�����
* �������رշ����ͨ��������ر���ɺ�Ͽ����пͻ������Ӳ��ͷ�������Դ
*		
* ������	
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __stdcall HP_Server_Stop(HP_Server pServer);

/*
* ���ƣ���������
* �������û�ͨ���÷�����ָ���ͻ��˷�������
*		
* ������		dwConnID	-- ���� ID
*			pBuffer		-- �������ݻ�����
*			iLength		-- �������ݳ���
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __stdcall HP_Server_Send(HP_Server pServer, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength);

/*
* ���ƣ��Ͽ�����
* �������Ͽ���ĳ���ͻ��˵�����
*		
* ������		dwConnID	-- ���� ID
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __stdcall HP_Server_Disconnect(HP_Server pServer, HP_CONNID dwConnID, BOOL bForce);

/*
* ���ƣ��Ͽ���ʱ����
* �������Ͽ�����ָ��ʱ��������
*		
* ������		dwPeriod	-- ʱ�������룩
*			bForce		-- �Ƿ�ǿ�ƶϿ�����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API BOOL __stdcall HP_Server_DisconnectLongConnections(HP_Server pServer, DWORD dwPeriod, BOOL bForce);

/******************************************************************************/
/***************************** Server ���Է��ʷ��� *****************************/

/*
* ���ƣ��������ӵĸ�������
* �������Ƿ�Ϊ���Ӱ󶨸������ݻ��߰�ʲô�������ݣ�����Ӧ�ó���ֻ�����
*		
* ������		dwConnID	-- ���� ID
*			pv			-- ����
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���Ч������ ID��
*/
HPSOCKET_API BOOL __stdcall HP_Server_SetConnectionExtra(HP_Server pServer, HP_CONNID dwConnID, PVOID pExtra);

/*
* ���ƣ���ȡ���ӵĸ�������
* �������Ƿ�Ϊ���Ӱ󶨸������ݻ��߰�ʲô�������ݣ�����Ӧ�ó���ֻ�����
*		
* ������		dwConnID	-- ���� ID
*			ppv			-- ����ָ��
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���Ч������ ID��
*/
HPSOCKET_API BOOL __stdcall HP_Server_GetConnectionExtra(HP_Server pServer, HP_CONNID dwConnID, PVOID* ppExtra);

/* ���ͨ������Ƿ������� */
HPSOCKET_API BOOL __stdcall HP_Server_HasStarted(HP_Server pServer);
/* �鿴ͨ�������ǰ״̬ */
HPSOCKET_API En_HP_ServiceState __stdcall HP_Server_GetState(HP_Server pServer);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������ */
HPSOCKET_API En_HP_ServerError __stdcall HP_Server_GetLastError(HP_Server pServer);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������� */
HPSOCKET_API LPCTSTR __stdcall HP_Server_GetLastErrorDesc(HP_Server pServer);
/* ��ȡ�ͻ��������� */
HPSOCKET_API DWORD __stdcall HP_Server_GetConnectionCount(HP_Server pServer);
/* ��ȡĳ���ͻ�������ʱ�������룩 */
HPSOCKET_API BOOL __stdcall HP_Server_GetConnectPeriod(HP_Server pServer, HP_CONNID dwConnID, DWORD* pdwPeriod);
/* ��ȡ���� Socket �ĵ�ַ��Ϣ */
HPSOCKET_API BOOL __stdcall HP_Server_GetListenAddress(HP_Server pServer, LPTSTR lpszAddress, int* piAddressLen, USHORT* pusPort);
/* ��ȡĳ���ͻ������ӵĵ�ַ��Ϣ */
HPSOCKET_API BOOL __stdcall HP_Server_GetClientAddress(HP_Server pServer, HP_CONNID dwConnID, LPTSTR lpszAddress, int* piAddressLen, USHORT* pusPort);

/* ���� Socket �����������ʱ�䣨���룬�������ڼ�� Socket ��������ܱ���ȡʹ�ã� */
HPSOCKET_API void __stdcall HP_Server_SetFreeSocketObjLockTime(HP_Server pServer, DWORD dwFreeSocketObjLockTime);
/* ���� Socket ����ش�С��ͨ������Ϊƽ���������������� 1/3 - 1/2�� */
HPSOCKET_API void __stdcall HP_Server_SetFreeSocketObjPool(HP_Server pServer, DWORD dwFreeSocketObjPool);
/* �����ڴ�黺��ش�С��ͨ������Ϊ Socket ����ش�С�� 2 - 3 ���� */
HPSOCKET_API void __stdcall HP_Server_SetFreeBufferObjPool(HP_Server pServer, DWORD dwFreeBufferObjPool);
/* ���� Socket ����ػ��շ�ֵ��ͨ������Ϊ Socket ����ش�С�� 3 ���� */
HPSOCKET_API void __stdcall HP_Server_SetFreeSocketObjHold(HP_Server pServer, DWORD dwFreeSocketObjHold);
/* �����ڴ�黺��ػ��շ�ֵ��ͨ������Ϊ�ڴ�黺��ش�С�� 3 ���� */
HPSOCKET_API void __stdcall HP_Server_SetFreeBufferObjHold(HP_Server pServer, DWORD dwFreeBufferObjHold);
/* ���ù����߳�������ͨ������Ϊ 2 * CPU + 2�� */
HPSOCKET_API void __stdcall HP_Server_SetWorkerThreadCount(HP_Server pServer, DWORD dwWorkerThreadCount);
/* ���ùرշ���ǰ�ȴ����ӹرյ��ʱ�ޣ����룬0 �򲻵ȴ��� */
HPSOCKET_API void __stdcall HP_Server_SetMaxShutdownWaitTime(HP_Server pServer, DWORD dwMaxShutdownWaitTime);

/* ��ȡ Socket �����������ʱ�� */
HPSOCKET_API DWORD __stdcall HP_Server_GetFreeSocketObjLockTime(HP_Server pServer);
/* ��ȡ Socket ����ش�С */
HPSOCKET_API DWORD __stdcall HP_Server_GetFreeSocketObjPool(HP_Server pServer);
/* ��ȡ�ڴ�黺��ش�С */
HPSOCKET_API DWORD __stdcall HP_Server_GetFreeBufferObjPool(HP_Server pServer);
/* ��ȡ Socket ����ػ��շ�ֵ */
HPSOCKET_API DWORD __stdcall HP_Server_GetFreeSocketObjHold(HP_Server pServer);
/* ��ȡ�ڴ�黺��ػ��շ�ֵ */
HPSOCKET_API DWORD __stdcall HP_Server_GetFreeBufferObjHold(HP_Server pServer);
/* ��ȡ�����߳����� */
HPSOCKET_API DWORD __stdcall HP_Server_GetWorkerThreadCount(HP_Server pServer);
/* ��ȡ�رշ���ǰ�ȴ����ӹرյ��ʱ�� */
HPSOCKET_API DWORD __stdcall HP_Server_GetMaxShutdownWaitTime(HP_Server pServer);

/**********************************************************************************/
/***************************** TCP Server ���Է��ʷ��� *****************************/

/* ���� Accept ԤͶ�� Socket ������ͨ������Ϊ�����߳����� 1 - 2 ���� */
HPSOCKET_API void __stdcall HP_TcpServer_SetAcceptSocketCount(HP_TcpServer pServer, DWORD dwAcceptSocketCount);
/* ����ͨ�����ݻ�������С������ƽ��ͨ�����ݰ���С�������ã�ͨ������Ϊ 1024 �ı����� */
HPSOCKET_API void __stdcall HP_TcpServer_SetSocketBufferSize(HP_TcpServer pServer, DWORD dwSocketBufferSize);
/* ���ü��� Socket �ĵȺ���д�С�����ݲ������������������ã� */
HPSOCKET_API void __stdcall HP_TcpServer_SetSocketListenQueue(HP_TcpServer pServer, DWORD dwSocketListenQueue);
/* ������������������룬0 �򲻷����������� */
HPSOCKET_API void __stdcall HP_TcpServer_SetKeepAliveTime(HP_TcpServer pServer, DWORD dwKeepAliveTime);
/* ��������ȷ�ϰ�����������룬0 ������������������������ɴ� [Ĭ�ϣ�WinXP 5 ��, Win7 10 ��] ��ⲻ������ȷ�ϰ�����Ϊ�Ѷ��ߣ� */
HPSOCKET_API void __stdcall HP_TcpServer_SetKeepAliveInterval(HP_TcpServer pServer, DWORD dwKeepAliveInterval);

/* ��ȡ Accept ԤͶ�� Socket ���� */
HPSOCKET_API DWORD __stdcall HP_TcpServer_GetAcceptSocketCount(HP_TcpServer pServer);
/* ��ȡͨ�����ݻ�������С */
HPSOCKET_API DWORD __stdcall HP_TcpServer_GetSocketBufferSize(HP_TcpServer pServer);
/* ��ȡ���� Socket �ĵȺ���д�С */
HPSOCKET_API DWORD __stdcall HP_TcpServer_GetSocketListenQueue(HP_TcpServer pServer);
/* ��ȡ���������� */
HPSOCKET_API DWORD __stdcall HP_TcpServer_GetKeepAliveTime(HP_TcpServer pServer);
/* ��ȡ��������� */
HPSOCKET_API DWORD __stdcall HP_TcpServer_GetKeepAliveInterval(HP_TcpServer pServer);

/**********************************************************************************/
/***************************** UDP Server ���Է��ʷ��� *****************************/

/* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
HPSOCKET_API void __stdcall HP_UdpServer_SetMaxDatagramSize(HP_UdpServer pServer, DWORD dwMaxDatagramSize);
/* ��ȡ���ݱ�����󳤶� */
HPSOCKET_API DWORD __stdcall HP_UdpServer_GetMaxDatagramSize(HP_UdpServer pServer);

/* ���ü������Դ�����0 �򲻷��ͼ�������������������Դ�������Ϊ�Ѷ��ߣ� */
HPSOCKET_API void __stdcall HP_UdpServer_SetDetectAttempts(HP_UdpServer pServer, DWORD dwDetectAttempts);
/* ���ü������ͼ�����룬0 �����ͼ����� */
HPSOCKET_API void __stdcall HP_UdpServer_SetDetectInterval(HP_UdpServer pServer, DWORD dwDetectInterval);
/* ��ȡ���������� */
HPSOCKET_API DWORD __stdcall HP_UdpServer_GetDetectAttempts(HP_UdpServer pServer);
/* ��ȡ��������� */
HPSOCKET_API DWORD __stdcall HP_UdpServer_GetDetectInterval(HP_UdpServer pServer);

/******************************************************************************/
/***************************** Client ����������� *****************************/

/*
* ���ƣ�����ͨ�����
* �����������ͻ���ͨ����������ӷ���ˣ�������ɺ�ɿ�ʼ�շ�����
*		
* ������		pszRemoteAddress	-- ����˵�ַ
*			usPort				-- ����˶˿�
*			bAsyncConnect		-- �Ƿ�����첽 Connnect
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __stdcall HP_Client_Start(HP_Client pClient, LPCTSTR pszRemoteAddress, USHORT usPort, BOOL bAsyncConnect);

/*
* ���ƣ��ر�ͨ�����
* �������رտͻ���ͨ��������ر���ɺ�Ͽ������˵����Ӳ��ͷ�������Դ
*		
* ������	
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __stdcall HP_Client_Stop(HP_Client pClient);

/*
* ���ƣ���������
* �������û�ͨ���÷��������˷�������
*		
* ������		dwConnID	-- ���� ID������������Ŀǰ�ò�����δʹ�ã�
*			pBuffer		-- �������ݻ�����
*			iLength		-- �������ݳ���
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ�ܣ���ͨ�� GetLastError() ��ȡ�������
*/
HPSOCKET_API BOOL __stdcall HP_Client_Send(HP_Client pClient, HP_CONNID dwConnID, const BYTE* pBuffer, int iLength);

/******************************************************************************/
/***************************** Client ���Է��ʷ��� *****************************/

/* ���ͨ������Ƿ������� */
HPSOCKET_API BOOL __stdcall HP_Client_HasStarted(HP_Client pClient);
/* �鿴ͨ�������ǰ״̬ */
HPSOCKET_API En_HP_ServiceState	__stdcall HP_Client_GetState(HP_Client pClient);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������ */
HPSOCKET_API En_HP_ClientError	__stdcall HP_Client_GetLastError(HP_Client pClient);
/* ��ȡ���һ��ʧ�ܲ����Ĵ������� */
HPSOCKET_API LPCTSTR __stdcall HP_Client_GetLastErrorDesc(HP_Client pClient);
/* ��ȡ�������������� ID */
HPSOCKET_API HP_CONNID __stdcall HP_Client_GetConnectionID(HP_Client pClient);
/* ��ȡ Client Socket �ĵ�ַ��Ϣ */
HPSOCKET_API BOOL __stdcall HP_Client_GetLocalAddress(HP_Client pClient, LPTSTR lpszAddress, int* piAddressLen, USHORT* pusPort);
/* �����ڴ�黺��ش�С��ͨ������Ϊ -> PUSH ģ�ͣ�5 - 10��PULL ģ�ͣ�10 - 20 �� */
HPSOCKET_API void __stdcall HP_Client_SetFreeBufferPoolSize(HP_Client pClient, DWORD dwFreeBufferPoolSize);
/* �����ڴ�黺��ػ��շ�ֵ��ͨ������Ϊ�ڴ�黺��ش�С�� 3 ���� */
HPSOCKET_API void __stdcall HP_Client_SetFreeBufferPoolHold(HP_Client pClient, DWORD dwFreeBufferPoolHold);
/* ��ȡ�ڴ�黺��ش�С */
HPSOCKET_API DWORD __stdcall HP_Client_GetFreeBufferPoolSize(HP_Client pClient);
/* ��ȡ�ڴ�黺��ػ��շ�ֵ */
HPSOCKET_API DWORD __stdcall HP_Client_GetFreeBufferPoolHold(HP_Client pClient);

/**********************************************************************************/
/***************************** TCP Client ���Է��ʷ��� *****************************/
	
/* ����ͨ�����ݻ�������С������ƽ��ͨ�����ݰ���С�������ã�ͨ������Ϊ��(N * 1024) - sizeof(TBufferObj)�� */
HPSOCKET_API void __stdcall HP_TcpClient_SetSocketBufferSize(HP_TcpClient pClient, DWORD dwSocketBufferSize);
/* ������������������룬0 �򲻷����������� */
HPSOCKET_API void __stdcall HP_TcpClient_SetKeepAliveTime(HP_TcpClient pClient, DWORD dwKeepAliveTime);
/* ��������ȷ�ϰ�����������룬0 ������������������������ɴ� [Ĭ�ϣ�WinXP 5 ��, Win7 10 ��] ��ⲻ������ȷ�ϰ�����Ϊ�Ѷ��ߣ� */
HPSOCKET_API void __stdcall HP_TcpClient_SetKeepAliveInterval(HP_TcpClient pClient, DWORD dwKeepAliveInterval);

/* ��ȡͨ�����ݻ�������С */
HPSOCKET_API DWORD __stdcall HP_TcpClient_GetSocketBufferSize(HP_TcpClient pClient);
/* ��ȡ���������� */
HPSOCKET_API DWORD __stdcall HP_TcpClient_GetKeepAliveTime(HP_TcpClient pClient);
/* ��ȡ��������� */
HPSOCKET_API DWORD __stdcall HP_TcpClient_GetKeepAliveInterval(HP_TcpClient pClient);

/**********************************************************************************/
/***************************** UDP Client ���Է��ʷ��� *****************************/

/* �������ݱ�����󳤶ȣ������ھ����������²����� 1472 �ֽڣ��ڹ����������²����� 548 �ֽڣ� */
HPSOCKET_API void __stdcall HP_UdpClient_SetMaxDatagramSize(HP_UdpClient pClient, DWORD dwMaxDatagramSize);
/* ��ȡ���ݱ�����󳤶� */
HPSOCKET_API DWORD __stdcall HP_UdpClient_GetMaxDatagramSize(HP_UdpClient pClient);

/* ���ü������Դ�����0 �򲻷��ͼ�������������������Դ�������Ϊ�Ѷ��ߣ� */
HPSOCKET_API void __stdcall HP_UdpClient_SetDetectAttempts(HP_UdpClient pClient, DWORD dwDetectAttempts);
/* ���ü������ͼ�����룬0 �����ͼ����� */
HPSOCKET_API void __stdcall HP_UdpClient_SetDetectInterval(HP_UdpClient pClient, DWORD dwDetectInterval);
/* ��ȡ���������� */
HPSOCKET_API DWORD __stdcall HP_UdpClient_GetDetectAttempts(HP_UdpClient pClient);
/* ��ȡ��������� */
HPSOCKET_API DWORD __stdcall HP_UdpClient_GetDetectInterval(HP_UdpClient pClient);

/***************************************************************************************/
/***************************** TCP Pull Server ����������� *****************************/

/*
* ���ƣ�ץȡ����
* �������û�ͨ���÷����� Socket �����ץȡ����
*		
* ������		dwConnID	-- ���� ID
*			pBuffer		-- ����ץȡ������
*			iLength		-- ץȡ���ݳ���
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullServer_Fetch(HP_TcpPullServer pServer, HP_CONNID dwConnID, BYTE* pBuffer, int iLength);

/***************************************************************************************/
/***************************** TCP Pull Server ���Է��ʷ��� *****************************/

/***************************************************************************************/
/***************************** TCP Pull Client ����������� *****************************/

/*
* ���ƣ�ץȡ����
* �������û�ͨ���÷����� Socket �����ץȡ����
*		
* ������		dwConnID	-- ���� ID
*			pBuffer		-- ����ץȡ������
*			iLength		-- ץȡ���ݳ���
* ����ֵ��	TRUE	-- �ɹ�
*			FALSE	-- ʧ��
*/
HPSOCKET_API En_HP_FetchResult __stdcall HP_TcpPullClient_Fetch(HP_TcpPullClient pClient, HP_CONNID dwConnID, BYTE* pBuffer, int iLength);

/***************************************************************************************/
/***************************** TCP Pull Client ���Է��ʷ��� *****************************/
