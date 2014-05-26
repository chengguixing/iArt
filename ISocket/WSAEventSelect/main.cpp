/************************************************************************/ 
/* �¼�����I/O�������ʵ��                                              */ 
/************************************************************************/ 

#include <WINSOCK2.H> 
#pragma comment(lib, "ws2_32") 
#include <stdio.h> 

int main() 
{ 
	printf("����˳���\n"); 
	//------�ټ���---------- 
	WSADATA wsaData; 
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
	{ 
		printf("WSAStartup Failed, Error=��%d��\n", WSAGetLastError()); 
		return 1; 
	} 
	else 
		printf("�ټ��سɹ�\n"); 
	//-------�ڴ�����ʽ�׽���------------ 
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	if (s == INVALID_SOCKET) 
	{ 
		printf("socket() Failed, Error=��%d��\n", WSAGetLastError()); 
		return 1; 
	} 
	else 
		printf("���Ѵ��������׽ӿڣ���%d��\n", s); 
	//���׽ӿ�s���ڡ�������ģʽ�� 
	u_long u1 = 1; 
	ioctlsocket(s, FIONBIO, (u_long*)&u1); 
	//-----------�۰󶨱��ص�ַ--------------------- 
	struct sockaddr_in Sadd; 
	Sadd.sin_family = AF_INET; 
	Sadd.sin_port = htons(5555); 
	Sadd.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); 
	if (bind(s, (sockaddr*)&Sadd, sizeof(Sadd)) == SOCKET_ERROR) 
	{ 
		printf("bind() Failed, Error=��%d��\n", WSAGetLastError()); 
		return 1; 
	} 
	else 
		printf("�۰󶨳ɹ�������IP��ַ����%s��, �˿ںţ���%d��\n", inet_ntoa(Sadd.sin_addr), ntohs(Sadd.sin_port)); 
	//--------------�ܽ������״̬----------------- 
	if (listen(s, 5) == SOCKET_ERROR) 
	{ 
		printf("listen Failed, Error=��%d��\n", WSAGetLastError()); 
		return 1; 
	} 
	else 
		printf("�ܽ������״̬\n"); 
	//--------------�ݴ����¼�����----------------- 
	WSAEVENT NewEvent = WSACreateEvent(); 
	if (NewEvent == WSA_INVALID_EVENT) 
	{ 
		printf("WSACreateEvent() Failed, Error=��%d��\n", WSAGetLastError()); 
		return 1; 
	} 
	else 
		printf("�ݴ���һ���¼����󣬷��ص��¼�������NewEvent=%d\n", NewEvent); 
	//--------------�������¼�ע��------------ 
	int WESerror=WSAEventSelect(s, NewEvent, FD_ACCEPT | FD_CLOSE); 
	if (WESerror == INVALID_SOCKET) 
	{ 
		printf("WSAEventSelect() Failed,Error=��%d��\n",WSAGetLastError()); 
		return -1; 
	} 
	else 
		printf("���׽ӿڡ�%d�����¼�����%d���������¼�FD_ACCEPT|FD_CLOSE�ѹ���\n", s, NewEvent); 
	//-----------׼������--------------- 
	int t = 1; 
	WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS]; 
	SOCKET sockArray[WSA_MAXIMUM_WAIT_EVENTS]; 
	int n = 0; 
	eventArray[n] = NewEvent; 
	sockArray[n] = s; 
	++n; 
	//------------ѭ������------------- 
	while (true) 
	{ 
		//---------------�ߵȴ��¼�����-------------- 
		int nIndex = WSAWaitForMultipleEvents(n, eventArray, FALSE, 40000, FALSE); 
		printf("nIndex=%d\n", nIndex); 
		if (nIndex == WSA_WAIT_FAILED)//------7.1����ʧ��--------- 
		{ 
			printf("����ʧ��\n"); 
			break; 
		} 
		else if (nIndex == WSA_WAIT_TIMEOUT)//-------7.2��ʱ--------- 
		{ 
			if (t<3) 
			{ 
				printf("�ڡ�%d���γ�ʱ\n",t); 
				t++; 
				continue; 
			} 
			else 
			{ 
				printf("�ڡ�%d���γ�ʱ���˳�\n",t); 
				break; 
			} 
		} 
		//---------------7.3�����¼������¼��������Ĺ���״̬-------- 
		else 
		{ 
			WSANETWORKEVENTS event;//�ýṹ��¼�����¼��Ͷ�Ӧ������� 
			//---------�������¼���ѯ----------- 
			WSAEnumNetworkEvents(sockArray[nIndex - WSA_WAIT_EVENT_0], NULL, &event); 
			WSAResetEvent(eventArray[nIndex - WSA_WAIT_EVENT_0]); 
			if ((event.lNetworkEvents & FD_ACCEPT) != 0)       //------8.1����FD_ACCEPT֪ͨ��Ϣ 
			{ 
				if (event.iErrorCode[FD_ACCEPT_BIT] == 0) 
				{ 
					if (n > WSA_MAXIMUM_WAIT_EVENTS) 
					{ 
						printf("Too many connections!\n"); 
						break; 
					} 
					SOCKET sNew = accept(sockArray[nIndex - WSA_WAIT_EVENT_0], NULL, NULL); 
					NewEvent = WSACreateEvent(); 
					WSAEventSelect(sNew, NewEvent, FD_READ | FD_CLOSE); 
					eventArray[n] = NewEvent; 
					sockArray[n] = sNew; 
					++n; 
				} 
			} 
			else if (event.lNetworkEvents & FD_READ)    //-------8.2����FD_READ֪ͨ��Ϣ 
			{ 
				if (event.iErrorCode[FD_READ_BIT] == 0) 
				{ 
					char buf[256]; 
					memset(buf, 0, 256); 
					int nRecv = recv(sockArray[nIndex - WSA_WAIT_EVENT_0], buf, sizeof(buf), 0); 
					if (nRecv > 0) 
					{ 
						printf("���յ��ͻ��ˡ�%d�����ݣ�%s\n", sockArray[nIndex - WSA_WAIT_EVENT_0], buf); 
					} 
				} 
			} 
			else if (event.lNetworkEvents & FD_CLOSE)  //---------8.3����FD_CLOSE֪ͨ��Ϣ 
			{ 
				if (event.iErrorCode[FD_CLOSE_BIT] == 0) 
				{ 
					closesocket(sockArray[nIndex - WSA_WAIT_EVENT_0]); 
					WSACloseEvent(eventArray[nIndex - WSA_WAIT_EVENT_0]); 
					printf("�׽���Ϊ��%d�����ѹر�����\n", sockArray[nIndex - WSA_WAIT_EVENT_0]); 
				} 
				else 
				{ 
					if (event.iErrorCode[FD_CLOSE_BIT] == 10053) 
					{ 
						closesocket(sockArray[nIndex - WSA_WAIT_EVENT_0]); 
						WSACloseEvent(eventArray[nIndex - WSA_WAIT_EVENT_0]); 
						printf("�ͻ��ˡ�%d���Ƿ��ر�����\n", sockArray[nIndex - WSA_WAIT_EVENT_0]); 
					} 
				} 
				for (int j = nIndex - WSA_WAIT_EVENT_0; j < n - 1; ++j) 
				{ 
					sockArray[j] = sockArray[j + 1]; 
					eventArray[j] = eventArray[j + 1]; 
				} 
				--n; 
			} 
		}// end �����¼����� 
	}//end while 
	printf("�˳�����������\n"); 
	closesocket(s); 
	WSACleanup(); 
	return 0; 
} 