
#include <winsock2.h>
#include <iostream>
using namespace std;


#pragma comment(lib, "wsock32.lib")
#define SERVER_EXIT_OK 0
#define SERVER_DLL_ERROR 1
#define SERVER_API_ERROR 2
#define SERVERPORT 5555
#define MAX_NUM_BUF 64

char bufRecv[MAX_NUM_BUF];
char bufSend[MAX_NUM_BUF];
SOCKET sServer;
SOCKET sClient;
BOOL bConning;


void InitMember(void);
int ExitClient(int nExit);
BOOL RecvLine(SOCKET s, char* buf);
BOOL SendLine(SOCKET s, char* buf);
int HandleSocketError(char* str);
void ShowSocketMsg(char* str);

int main(void) 
{ 
	InitMember();
	WORD wVersionRequested;
	/*
	typedef struct WSAData {
	WORD                    wVersion;
	WORD                    wHighVersion;
	#ifdef _WIN64
	unsigned short          iMaxSockets;
	unsigned short          iMaxUdpDg;
	char FAR *              lpVendorInfo;
	char                    szDescription[WSADESCRIPTION_LEN+1];
	char                    szSystemStatus[WSASYS_STATUS_LEN+1];
	#else
	char                    szDescription[WSADESCRIPTION_LEN+1];
	char                    szSystemStatus[WSASYS_STATUS_LEN+1];
	unsigned short          iMaxSockets;
	unsigned short          iMaxUdpDg;
	char FAR *              lpVendorInfo;
	#endif
	} WSADATA, FAR * LPWSADATA;
	*/
	WSADATA wsaData;
	int retVal;

	wVersionRequested = MAKEWORD(1, 1);
	retVal = WSAStartup(wVersionRequested, &wsaData);
	if (0 != retVal)
	{
		ShowSocketMsg("Can not find a usable windows sockets dll!");
		return SERVER_DLL_ERROR;
	}
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		ShowSocketMsg("Can not find a usable windows sockets dll!");
		WSACleanup();
		return SERVER_DLL_ERROR;
	}


	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sServer)
	{
		return HandleSocketError("Failed socket()!");
	}
	/*
	typedef struct sockaddr_in {

	#if(_WIN32_WINNT < 0x0600)
	short   sin_family;
	#else //(_WIN32_WINNT < 0x0600)
	ADDRESS_FAMILY sin_family;
	#endif //(_WIN32_WINNT < 0x0600)

	USHORT sin_port;
	IN_ADDR sin_addr;
	CHAR sin_zero[8];
	} SOCKADDR_IN, *PSOCKADDR_IN;
	*/
	SOCKADDR_IN addrServ;
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(SERVERPORT);
	/*
	//
	// IPv4 Internet address
	// This is an 'on-wire' format structure.
	//
	typedef struct in_addr {
	union {
	struct { UCHAR s_b1,s_b2,s_b3,s_b4; } S_un_b;
	struct { USHORT s_w1,s_w2; } S_un_w;
	ULONG S_addr;
	} S_un;
	#define s_addr  S_un.S_addr // can be used for most tcp & ip code
	#define s_host  S_un.S_un_b.s_b2    // host on imp
	#define s_net   S_un.S_un_b.s_b1    // network
	#define s_imp   S_un.S_un_w.s_w2    // imp
	#define s_impno S_un.S_un_b.s_b4    // imp #
	#define s_lh    S_un.S_un_b.s_b3    // logical host
	} IN_ADDR, *PIN_ADDR, FAR *LPIN_ADDR;
	*/
	addrServ.sin_addr.s_addr = INADDR_ANY;

	retVal = bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == retVal)
	{
		closesocket(sServer);
		return HandleSocketError("Failed bind()!");
	}

	retVal = listen(sServer, 1);
	if (SOCKET_ERROR == retVal)
	{
		closesocket(sServer);
		return HandleSocketError("Failed listen()!");
	}
	cout << "Server socceeded!" << endl;
	cout << "Waiting for new clients..." << endl;

	sockaddr_in addrClient;
	int addrClientlen = sizeof(addrClient);
	sClient = accept(sServer, (sockaddr FAR*)&addrClient, &addrClientlen);
	if (INVALID_SOCKET == sClient)
	{
		closesocket(sServer);
		return HandleSocketError("Failed accept()!");
	}
	else
	{
		bConning = TRUE;
	}

	char* pClientIP = inet_ntoa(addrClient.sin_addr);
	u_short clientPort = ntohs(addrClient.sin_port);
	cout << "Accept a client." << endl;
	cout << "IP:" << pClientIP << endl;
	cout << "PORT: " << clientPort << endl;


	if (!RecvLine(sClient, bufRecv))
	{
		return ExitClient(SERVER_API_ERROR);
	}
	cout << bufRecv << endl;

	strcpy(bufSend, "Hello, Client!\n");
	if (!SendLine(sClient, bufSend))
	{
		return ExitClient(SERVER_API_ERROR);
	}
	cout << "Server exiting..." << endl;

	return ExitClient(SERVER_EXIT_OK);
} 

void InitMember(void)
{
	memset(bufRecv, 0, MAX_NUM_BUF);
	memset(bufSend, 0, MAX_NUM_BUF);

	sServer = INVALID_SOCKET;
	sClient = INVALID_SOCKET;

	bConning = FALSE;
}

int ExitClient(int nExit)
{
	closesocket(sServer);
	closesocket(sClient);
	WSACleanup();
	return nExit;
}

BOOL RecvLine(SOCKET s, char* buf)
{
	BOOL retVal = TRUE;
	BOOL bLineEnd = FALSE;
	int nReadLen = 0;
	int nDataLen = 0;
	while (!bLineEnd && bConning)
	{
		nReadLen = recv(s, buf + nDataLen, 1, 0);
		if (SOCKET_ERROR == nReadLen)
		{
			int nErrCode = WSAGetLastError();
			if (WSAENOTCONN == nErrCode)
			{
				ShowSocketMsg("The socket is not connected!");
			}
			else if (WSAESHUTDOWN == nErrCode)
			{
				ShowSocketMsg("The socket has been shutdown!");
			}
			else if (WSAETIMEDOUT == nErrCode)
			{
				ShowSocketMsg("The connection has been dropped!");
			}
			else if (WSAECONNRESET == nErrCode)
			{
				ShowSocketMsg("The virtual circuit was reset by the remote side!");
			}
			else
			{
				;
			}
			retVal = FALSE;
			break;
		}
		if (0 == nReadLen)
		{
			retVal = FALSE;
			break;
		}
		if ('\n' == *(buf + nDataLen))
		{
			bLineEnd = TRUE;
		}
		else
		{
			nDataLen += nReadLen;
		}
	}
	return retVal;
}

BOOL SendLine(SOCKET s, char* buf)
{
	int retVal;
	retVal = send(s, buf, strlen(buf), 0);

	if (SOCKET_ERROR == retVal)
	{
		int nErrCode = WSAGetLastError();
		if (WSAENOTCONN == nErrCode)
		{
			ShowSocketMsg("The socket is not connected!");
		}
		else if (WSAESHUTDOWN == nErrCode)
		{
			ShowSocketMsg("The socket has been shutdown!");
		}
		else if (WSAETIMEDOUT == nErrCode)
		{
			ShowSocketMsg("The connection has been dropped!");
		}
		else
		{
			;
		}
		return FALSE;
	}
	return TRUE;
}

int HandleSocketError(char* str)
{
	ShowSocketMsg(str);
	WSACleanup();
	return SERVER_API_ERROR;
}

void ShowSocketMsg(char* str)
{
	MessageBoxA(NULL, str, "Server Error", MB_OK);
}
