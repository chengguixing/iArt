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

	SOCKET sockSrv = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(12345);

	bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	int ret = WSAGetLastError();
	printf("%d\n", ret);

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	char recvBuf[1000];

	memset(recvBuf, 0, 1000);


	recvfrom(sockSrv, recvBuf, 1000, 0, (SOCKADDR*)&addrClient, &len);

	printf("%s\n", recvBuf);

	closesocket(sockSrv);
	WSACleanup();

	system("pause");
	return 0;
} 