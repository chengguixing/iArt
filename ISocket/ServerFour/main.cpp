#include <Winsock2.h>
#include <iostream>
using namespace std;

#define PORT 4000
#define MSGSIZE 1024

#pragma comment(lib, "wsock32.lib")


int g_iTotalConn = 0;
SOCKET g_CliSocketArray[FD_SETSIZE];
DWORD WINAPI WorkerThreadFunc(LPVOID);


int main(void) 
{ 
	WSADATA wsaData;
	SOCKET sListen, sClient;
	SOCKADDR_IN local, client;
	int iaddrSize = sizeof(SOCKADDR_IN);
	DWORD dwThreadId;

	WSAStartup(0x0202, &wsaData);
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	local.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	local.sin_family = AF_INET;
	local.sin_port = htons(PORT);
	bind(sListen, (struct sockaddr*)&local, sizeof(SOCKADDR_IN));

	listen(sListen, 3);

	HANDLE hHandle = CreateThread(NULL, 0, WorkerThreadFunc, NULL, 0, &dwThreadId);
	CloseHandle(hHandle);


	while (TRUE)
	{
		sClient = accept(sListen, (struct sockaddr*)&client, &iaddrSize);
		cout << inet_ntoa(client.sin_addr) << ":" <<ntohs(client.sin_port) << endl;
		g_CliSocketArray[g_iTotalConn++] = sClient;
	}



	system("pause");
	return 0;
} 


DWORD WINAPI WorkerThreadFunc(LPVOID lpParameter)
{
	int i;
	fd_set fdread;
	int ret;
	struct timeval tv = {1, 0};
	char szMessage[MSGSIZE];
	while (TRUE)
	{
		FD_ZERO(&fdread);
		for (i = 0; i < g_iTotalConn; ++i)
		{
			FD_SET(g_CliSocketArray[i], &fdread);
		}
		ret = select(0, &fdread, NULL, NULL, &tv);
		if (0 == ret)
		{
			continue;
		}
		for (i = 0; i < g_iTotalConn; ++i)
		{
			if (FD_ISSET(g_CliSocketArray[i], &fdread))
			{
				ret = recv(g_CliSocketArray[i], szMessage, MSGSIZE, 0);
				if (0 == ret || (ret == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET))
				{
					cout << g_CliSocketArray[i] << endl;
					closesocket(g_CliSocketArray[i]);
					if (i < g_iTotalConn - 1)
					{
						g_CliSocketArray[i--] = g_CliSocketArray[--g_iTotalConn];
					}
					else if(i == g_iTotalConn - 1)
					{
						--g_iTotalConn;
					}
				}
				else
				{
					szMessage[ret] = '\0';
					send(g_CliSocketArray[i], szMessage, strlen(szMessage), 0);
				}
			}
		}
	}
	return 0;
}