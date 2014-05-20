#include <Winsock2.h>
#include <iostream>
using namespace std;

#pragma comment(lib, "wsock32.lib")


int main(void) 
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
		WSACleanup();
		return 2;
	}

	SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(12345);

	sendto(sockClient, "Hello izhuxin", strlen("Hello izhuxin") + 1, 0, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	closesocket(sockClient);

	system("pause");
	return 0;
} 