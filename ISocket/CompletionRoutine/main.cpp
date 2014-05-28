#include <stdio.h> 
#include <Winsock2.h> 
#pragma comment(lib, "ws2_32.lib") 

#define PORT 5050 
#define MSGSIZE 1024 

typedef struct 
{ 
	WSAOVERLAPPED overlap; 
	WSABUF Buffer; 
	char szMessage[MSGSIZE]; 
	DWORD NumberOfBytesRecvd; 
	DWORD Flags; 
	SOCKET sClient; 
}PER_IO_OPERATION_DATA, *LPPER_IO_OPERATION_DATA; 

int                     g_iTotalConn = 0; 
SOCKET                  g_CliSocketArr[MAXIMUM_WAIT_OBJECTS]; 
WSAEVENT                g_CliEventArr[MAXIMUM_WAIT_OBJECTS]; 
LPPER_IO_OPERATION_DATA g_pPerIoDataArr[MAXIMUM_WAIT_OBJECTS]; 
SOCKET g_sNewClientConnection = NULL; 
BOOL g_bNewConnectionArrived = FALSE; 

DWORD WINAPI WorkerThread(LPVOID lpParam); 
void CALLBACK CompletionRoutine(DWORD dwError, DWORD cbTransferred,  
								LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags); 

int main() 
{ 
	WSADATA wsaData; 
	SOCKET sListen; 
	SOCKADDR_IN local, client; 
	int iAddrSize = sizeof(SOCKADDR_IN); 
	DWORD dwThreadId; 
	WSAStartup(MAKEWORD(2,2), &wsaData); 
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	memset(&local, 0, sizeof(SOCKADDR_IN)); 
	local.sin_family = AF_INET; 
	local.sin_port = htons(PORT); 
	local.sin_addr.s_addr = htonl(INADDR_ANY); 
	bind(sListen, (SOCKADDR*)&local, sizeof(SOCKADDR_IN)); 
	listen(sListen, 5); 

	CreateThread(NULL, 0, WorkerThread, NULL, 0, &dwThreadId); 

	while(TRUE) 
	{ 
		g_sNewClientConnection = accept(sListen, (SOCKADDR*)&client, &iAddrSize); 
		g_bNewConnectionArrived = TRUE; 
		printf("Accepted Client : %s:%d \n", inet_ntoa(client.sin_addr), ntohs(client.sin_port)); 
	} 
	return 0; 
} 

DWORD WINAPI WorkerThread(LPVOID lpParam) 
{ 
	LPPER_IO_OPERATION_DATA lpPerIOData = NULL; 
	while(TRUE) 
	{ 
		if(g_bNewConnectionArrived) 
		{ 
			lpPerIOData = (LPPER_IO_OPERATION_DATA)HeapAlloc( 
				GetProcessHeap(), 
				HEAP_ZERO_MEMORY, 
				sizeof(PER_IO_OPERATION_DATA)); 
			lpPerIOData->Buffer.len = MSGSIZE; 
			lpPerIOData->Buffer.buf = lpPerIOData->szMessage; 
			lpPerIOData->sClient = g_sNewClientConnection; 
			WSARecv(lpPerIOData->sClient, 
				&lpPerIOData->Buffer, 
				1, 
				&lpPerIOData->NumberOfBytesRecvd, 
				&lpPerIOData->Flags, 
				&lpPerIOData->overlap, 
				CompletionRoutine); 
			g_bNewConnectionArrived = FALSE; 
		} 
		SleepEx(1000, TRUE); 
	} 
	return 0; 
} 

void CALLBACK CompletionRoutine(DWORD dwError, DWORD cbTransferred,  
								LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags) 
{ 
	LPPER_IO_OPERATION_DATA lpPerIOData = (LPPER_IO_OPERATION_DATA)lpOverlapped; 
	if(dwError!=0||cbTransferred==0) 
	{ 
		closesocket(lpPerIOData->sClient); 
		HeapFree(GetProcessHeap(),0,lpPerIOData); 
	} 
	else 
	{ 
		lpPerIOData->szMessage[cbTransferred] = '\0'; 
		send(lpPerIOData->sClient, lpPerIOData->szMessage, cbTransferred, 0); 

		memset(&lpPerIOData->overlap, 0, sizeof(WSAOVERLAPPED)); 
		lpPerIOData->Buffer.len = MSGSIZE; 
		lpPerIOData->Buffer.buf = lpPerIOData->szMessage; 
		WSARecv(lpPerIOData->sClient, 
			&lpPerIOData->Buffer, 
			1, 
			&lpPerIOData->NumberOfBytesRecvd, 
			&lpPerIOData->Flags, 
			&lpPerIOData->overlap, 
			CompletionRoutine); 
	} 
} 