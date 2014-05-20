#include <Winsock2.h>
#include <iostream>
using namespace std;

#pragma comment(lib, "wsock32.lib")


int main() 
{ 

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);

	if (0 != err)
	{
		cout << "WSAStartup Failed" << endl;
		return 1;
	}
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		cout << "LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1 Failed" << endl;
		WSACleanup();
		return 2;
	}

	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addSrv;
	addSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addSrv.sin_family = AF_INET;
	addSrv.sin_port = htons(6000);

	bind(sockSrv, (SOCKADDR*)&addSrv, sizeof(SOCKADDR));
	listen(sockSrv, 5);

	SOCKADDR_IN addClient;
	int len = sizeof(SOCKADDR);

	while (1)
	{
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addClient, &len);
		cout << "sockConn is " << (int)sockConn << endl;
		char sendBuf[1000];
		sprintf(sendBuf, "Welcome %s", inet_ntoa(addClient.sin_addr));
		send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);

		char receiveBuf[1000];
		recv(sockConn, receiveBuf, 1000, 0);
		printf("%s\n", receiveBuf);
		closesocket(sockConn);
	}

	return 0;
} 