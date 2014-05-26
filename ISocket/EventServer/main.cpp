#include<winsock2.h> 
#include<stdio.h> 
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
}PER_IO_OPERATION_DATA, *LPPER_IO_OPERATION_DATA; 




int                     g_iTotalConn = 0; 
SOCKET                  g_CliSocketArr[MAXIMUM_WAIT_OBJECTS]; 
WSAEVENT                g_CliEventArr[MAXIMUM_WAIT_OBJECTS]; 
LPPER_IO_OPERATION_DATA g_pPerIoDataArr[MAXIMUM_WAIT_OBJECTS]; 

DWORD WINAPI WorkerThread(LPVOID); 

void Cleanup(int); 

int main(void) 
{ 
	WSADATA wsaData; 
	SOCKET sListen, sClient; 
	SOCKADDR_IN client, local; 
	DWORD dwThreadId; 
	int iAddrSize = sizeof(SOCKADDR_IN); 

	WSAStartup(MAKEWORD(2, 2), &wsaData); 
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
		sClient = accept(sListen, (SOCKADDR*)&client, &iAddrSize); 
		printf("Accepted Client : %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port)); 
		g_CliSocketArr[g_iTotalConn] = sClient; 
		g_pPerIoDataArr[g_iTotalConn] = (LPPER_IO_OPERATION_DATA)HeapAlloc( 
			GetProcessHeap(), 
			HEAP_ZERO_MEMORY, 
			sizeof(PER_IO_OPERATION_DATA) 
			); 
		g_pPerIoDataArr[g_iTotalConn]->Buffer.len = MSGSIZE; 
		g_pPerIoDataArr[g_iTotalConn]->Buffer.buf = g_pPerIoDataArr[g_iTotalConn]->szMessage; 
		g_pPerIoDataArr[g_iTotalConn]->overlap.hEvent = WSACreateEvent(); 
		g_CliEventArr[g_iTotalConn] = g_pPerIoDataArr[g_iTotalConn]->overlap.hEvent; 

		WSARecv(g_CliSocketArr[g_iTotalConn], 
			&g_pPerIoDataArr[g_iTotalConn]->Buffer, 
			1, 
			&g_pPerIoDataArr[g_iTotalConn]->NumberOfBytesRecvd, 
			&g_pPerIoDataArr[g_iTotalConn]->Flags, 
			&g_pPerIoDataArr[g_iTotalConn]->overlap, 
			NULL); 
		++g_iTotalConn; 
	} 
	closesocket(sListen); 
	WSACleanup(); 

	return 0; 
} 

DWORD WINAPI WorkerThread(LPVOID lpParam) 
{ 
	int ret, index; 
	DWORD cbTransferred; 
	while(TRUE) 
	{ 
		ret = WSAWaitForMultipleEvents(g_iTotalConn, g_CliEventArr, FALSE, 1000, FALSE); 
		if(ret == WSA_WAIT_FAILED || ret == WSA_WAIT_TIMEOUT) 
		{ 
			//如果当前没有客户端的话，要sleep一下，要不然CUP会占50%以上
			if(g_iTotalConn == 0)
			{
				Sleep(1000);
			}
			continue; 
		} 
		index = ret - WSA_WAIT_EVENT_0; 
		WSAResetEvent(g_CliEventArr[index]); 
		WSAGetOverlappedResult(g_CliSocketArr[index], 
			&g_pPerIoDataArr[index]->overlap, 
			&cbTransferred, 
			TRUE, 
			&g_pPerIoDataArr[index]->Flags); 
		if(cbTransferred == 0) 
		{ 
			Cleanup(index); 
		} 
		else 
		{ 
			g_pPerIoDataArr[index]->szMessage[cbTransferred] = '\0'; 
			send(g_CliSocketArr[index], g_pPerIoDataArr[index]->szMessage, cbTransferred, 0); 

			WSARecv(g_CliSocketArr[index], 
				&g_pPerIoDataArr[index]->Buffer, 
				1, 
				&g_pPerIoDataArr[index]->NumberOfBytesRecvd, 
				&g_pPerIoDataArr[index]->Flags, 
				&g_pPerIoDataArr[index]->overlap, 
				NULL); 
		} 
	} 
	return 0; 
} 

void Cleanup(int index) 
{ 
	closesocket(g_CliSocketArr[index]); 
	WSACloseEvent(g_CliEventArr[index]); 
	HeapFree(GetProcessHeap(), 0, g_pPerIoDataArr[index]); 
	if(index < g_iTotalConn - 1) 
	{ 
		g_CliSocketArr[index] = g_CliSocketArr[g_iTotalConn - 1]; 
		g_CliEventArr[index] = g_CliEventArr[g_iTotalConn - 1]; 
		g_pPerIoDataArr[index] = g_pPerIoDataArr[g_iTotalConn - 1]; 
	} 
	g_pPerIoDataArr[--g_iTotalConn] = NULL; 
} 