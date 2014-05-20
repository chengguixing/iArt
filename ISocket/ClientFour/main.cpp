#include <Winsock2.h>
#include <iostream>
using namespace std;
#define PORT 4000
#define IP_ADDRESS "127.0.0.1"

#pragma comment(lib, "wsock32.lib")



int main() 
{ 
	WSADATA wsa;
	SOCKET ClientSocket;
	struct sockaddr_in ServerAddr;
	int Ret = 0;
	int AddrLen = 0;
	HANDLE hThread = NULL;
	char SendBuffer[MAX_PATH];

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "WSAStartup Failed" << GetLastError() << endl;
		return 1;
	}


	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == ClientSocket)
	{
		cout << "socket Failed" << endl;
		return 2;
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	ServerAddr.sin_port = htons(PORT);
	memset(ServerAddr.sin_zero, 0x00, 8);

	Ret = connect(ClientSocket, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr));
	if (Ret == SOCKET_ERROR)
	{
		cout << "connect Failed" << endl;
		return 3;
	}
	else
	{
		cout << "succeed" << endl;
	}


	while (true)
	{
		cin.getline(SendBuffer, sizeof(SendBuffer));
		Ret = send(ClientSocket, SendBuffer, (int)strlen(SendBuffer), 0);
		if (SOCKET_ERROR == Ret)
		{
			cout << "send Failed" << endl;
			break;
		}
	}

	closesocket(ClientSocket);
	WSACleanup();
	system("pause");
	return 0;
} 