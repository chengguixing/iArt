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
		cout << "LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1 Failed" << endl;
		WSACleanup();
		return 2;
	}

	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
	cout << "sockClient is " << (int)sockClient << endl;
	SOCKADDR_IN addSrv;
	addSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addSrv.sin_family = AF_INET;
	addSrv.sin_port = htons(6000);

	connect(sockClient, (SOCKADDR*)&addSrv, sizeof(SOCKADDR));

	char receiveBuf[1000];
	recv(sockClient, receiveBuf, 1000, 0);
	printf("%s\n", receiveBuf);

	send(sockClient, "designed by izhuxin", strlen("designed by izhuxin") + 1, 0);
	closesocket(sockClient);

	WSACleanup();


	return 0;
} 