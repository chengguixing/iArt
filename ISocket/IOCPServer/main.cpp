#include<stdio.h> 
#include<winsock2.h> 
#pragma comment(lib, "ws2_32.lib") 

#define PORT 5050 
#define MSGSIZE 1024 

typedef enum 
{ 
	RECV_POSTED 
}OPERATION_TYPE; 

typedef struct 
{ 
	OVERLAPPED overlap; 
	WSABUF     Buffer; 
	char       szMessage[MSGSIZE]; 
	DWORD      NumberOfBytesRecvd; 
	DWORD      Flags; 
	OPERATION_TYPE OpetationType; 
}PER_IO_OPERATION_DATA, *LPPER_IO_OPERATION_DATA; 

DWORD WINAPI WorkerThread(LPVOID); 

int main(void) 
{ 
	WSADATA wsaData; 
	SOCKET sListen, sClient; 
	SOCKADDR_IN local, client; 
	DWORD i, dwThreadId; 
	int iAddrSize = sizeof(SOCKADDR_IN); 
	HANDLE CompletionPort = INVALID_HANDLE_VALUE; 
	SYSTEM_INFO sysinfo; 
	LPPER_IO_OPERATION_DATA lpPerIoData = NULL; 

	WSAStartup(MAKEWORD(2, 2), &wsaData); 
	CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); 
	GetSystemInfo(&sysinfo); 
	for(i = 0; i < sysinfo.dwNumberOfProcessors; ++i) 
	{ 
		CreateThread(NULL, 0, WorkerThread, CompletionPort, 0, &dwThreadId); 
	} 

	sListen = socket(AF_INET, SOCK_STREAM, 0); 
	memset(&local, 0, sizeof(SOCKADDR_IN)); 
	local.sin_family = AF_INET; 
	local.sin_port = htons(PORT); 
	local.sin_addr.s_addr = htonl(INADDR_ANY); 
	bind(sListen, (SOCKADDR*)&local, sizeof(SOCKADDR_IN)); 
	listen(sListen, 5); 
	while(TRUE) 
	{ 
		sClient = accept(sListen, (SOCKADDR*)&client, &iAddrSize); 
		printf("Accept Client : %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port)); 
		CreateIoCompletionPort((HANDLE)sClient, CompletionPort, (DWORD)sClient, 0); 
		lpPerIoData = (LPPER_IO_OPERATION_DATA)HeapAlloc( 
			GetProcessHeap(), 
			HEAP_ZERO_MEMORY, 
			sizeof(PER_IO_OPERATION_DATA)); 
		lpPerIoData->Buffer.len = MSGSIZE; 
		lpPerIoData->Buffer.buf = lpPerIoData->szMessage; 
		lpPerIoData->OpetationType = RECV_POSTED; 
		WSARecv(sClient, 
			&lpPerIoData->Buffer, 
			1, 
			&lpPerIoData->NumberOfBytesRecvd, 
			&lpPerIoData->Flags, 
			&lpPerIoData->overlap, 
			NULL); 
	} 

	PostQueuedCompletionStatus(CompletionPort, 0xFFFFFFFF, 0, NULL); 
	CloseHandle(CompletionPort); 
	closesocket(sListen); 
	WSACleanup(); 
	return 0;     
} 

DWORD WINAPI WorkerThread(LPVOID lpParam) 
{ 
	HANDLE CompletionPort = (HANDLE)lpParam; 
	DWORD dwBytesTransferred; 
	SOCKET sClient; 
	LPPER_IO_OPERATION_DATA lpPerIoData = NULL; 
	while(TRUE) 
	{ 
		GetQueuedCompletionStatus(CompletionPort, 
			&dwBytesTransferred, 
			(DWORD*)sClient, 
			(LPOVERLAPPED*)&lpPerIoData, 
			INFINITE); 
		if(dwBytesTransferred == 0xFFFFFFFF) 
		{ 
			return 0; 
		} 
		if(lpPerIoData->OpetationType == RECV_POSTED) 
		{ 
			if(dwBytesTransferred == 0) 
			{ 
				closesocket(sClient); 
				HeapFree(GetProcessHeap(), 0, lpPerIoData); 
			} 
			else 
			{ 
				lpPerIoData->szMessage[dwBytesTransferred] = '\0'; 
				send(sClient, lpPerIoData->szMessage, dwBytesTransferred, 0); 

				memset(lpPerIoData, 0, sizeof(PER_IO_OPERATION_DATA)); 
				lpPerIoData->Buffer.len = MSGSIZE; 
				lpPerIoData->Buffer.buf = lpPerIoData->szMessage; 
				lpPerIoData->OpetationType = RECV_POSTED; 
				WSARecv(sClient, 
					&lpPerIoData->Buffer, 
					1, 
					&lpPerIoData->NumberOfBytesRecvd, 
					&lpPerIoData->Flags, 
					&lpPerIoData->overlap, 
					NULL); 
			} 
		} 
	} 
	return 0; 
} 