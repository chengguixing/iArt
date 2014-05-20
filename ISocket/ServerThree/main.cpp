#include <Winsock2.h>
#include <iostream>
using namespace std;

#define PORT 4000
#define IP_ADDRESS "127.0.0.1"

#pragma comment(lib, "wsock32.lib")

DWORD WINAPI ClientThreadFunc(LPVOID);

int main() 
{ 
	WSADATA wsa;
	SOCKET ServerSocket, ClientSocket;
	struct sockaddr_in LocalAddr, ClientAddr;
	int Ret = 0;
	int AddrLen = 0;
	HANDLE hThread = NULL;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "WSAStartup Failed" << endl;
		return 1;
	}

	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == ServerSocket)
	{
		cout << "socket Failed" << endl;
		return 2;
	}

	LocalAddr.sin_family = AF_INET;
	LocalAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	LocalAddr.sin_port = htons(PORT);
	memset(LocalAddr.sin_zero, 0x00, 8);

	Ret = bind(ServerSocket, (struct sockaddr*)&LocalAddr, sizeof(LocalAddr));
	if (0 != Ret)
	{
		cout << "bind Failed" << endl;
		return 3;
	}

	Ret = listen(ServerSocket, 10);
	if (0 != Ret)
	{
		cout << "listen Failed" << endl;
		return 4;
	}

	cout << "Server Starting" << endl;

	while (true)
	{
		AddrLen = sizeof(ClientAddr);
		ClientSocket = accept(ServerSocket, (struct sockaddr*)&ClientAddr, &AddrLen);
		if (INVALID_SOCKET == ClientSocket)
		{
			cout << "accept Failed " << GetLastError() << endl;
			break;
		}
		cout << inet_ntoa(ClientAddr.sin_addr) << ":" << ClientAddr.sin_port << endl;

		hThread = CreateThread(NULL, 0, ClientThreadFunc, (LPVOID)ClientSocket, 0, NULL);
		if (NULL == hThread)
		{
			cout << "CreateThread Failed " << GetLastError() << endl;
			break;
		}
		CloseHandle(hThread);
	}

	closesocket(ServerSocket);
	WSACleanup();

	system("pause");
	return 0;
}


DWORD WINAPI ClientThreadFunc(LPVOID lpParameter)
{
	SOCKET ClientSocket = (SOCKET)lpParameter;
	int Ret = 0;
	char RecvBuffer[MAX_PATH];

	while (true)
	{
		memset(RecvBuffer, 0x00, sizeof(RecvBuffer));
		Ret = recv(ClientSocket, RecvBuffer, MAX_PATH, 0);
		if (0 == Ret || SOCKET_ERROR == Ret)
		{
			cout << Ret << endl;
			break;
		}
		cout << RecvBuffer << endl;
	}
	closesocket(ClientSocket);
	return 0;
}