
#pragma once

//����ʵ��
typedef void* SIPC_INSTANCE;
typedef void* SIPC_STREAM;

//������
#define SIPC_SUCC	0	//�ɹ�

//����ˣ�����IPC�����
SIPC_INSTANCE SIPC_CreateIpcServer();
//����ˣ���ʼ��IPC�����
long SIPC_InitIpcServer(SIPC_INSTANCE hSIpcInstance);
//����ˣ�����
typedef long (__stdcall *LISTENCALLBACK)(DWORD dwErrCode, void* pUserData);
long SIPC_ListenClient(SIPC_INSTANCE hSIpcInstance, LISTENCALLBACK pfnOnListenCallback, void* pUserData);

//�ͻ��ˣ�����IPC�ͻ���
SIPC_INSTANCE SIPC_CreateIpcClient();
//�ͻ��ˣ���ʼ��IPC�ͻ���
long SIPC_InitIpcClient(SIPC_INSTANCE hSIpcInstance);
//�ͻ��ˣ�����
typedef long (__stdcall *CONNECTCALLBACK)(DWORD dwErrCode, void* pUserData);
long SIPC_ConnectServer(SIPC_INSTANCE hSIpcInstance, CONNECTCALLBACK pfnOnConnectCallback, void* pUserData);

//����
typedef long (__stdcall *SENDCALLBACK)(DWORD dwErrCode, void* pUserData);
long SIPC_Send(SIPC_INSTANCE hSIpcInstance, DWORD dwId, SIPC_STREAM hSIpcStream, SENDCALLBACK pfnOnSendCallback, void* pUserData);

//����
typedef long (__stdcall *RECVCALLBACK)(DWORD dwErrCode, DWORD dwId, SIPC_STREAM hSIpcStream, void* pUserData);
long SIPC_RegisterRecvCallback(SIPC_INSTANCE hSIpcInstance, RECVCALLBACK pfnOnRecvCallback, void* pUserData);

//�Ͽ��ص�
typedef long (__stdcall *DISLINKCALLBACK)(DWORD dwErrCode, void* pUserData);
long SIPC_RegisterDislinkCallback(SIPC_INSTANCE hSIpcInstance, DISLINKCALLBACK pfnDisconnectCallback, void* pUserData);

//����SocketIPC����������
SIPC_STREAM SIPC_CreateSocketIpcStream();
//д��string��SocketIPC������
long SIPC_StreamWriteString(SIPC_STREAM hSIpcStream, const char* szData);
//��SocketIPC�������ж�ȡstring
long SIPC_StreamReadString(SIPC_STREAM hSIpcStream, char* szData, int iLen);
