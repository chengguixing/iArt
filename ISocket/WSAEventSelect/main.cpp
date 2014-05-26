/************************************************************************/ 
/* 事件对象I/O管理程序实例                                              */ 
/************************************************************************/ 

#include <WINSOCK2.H> 
#pragma comment(lib, "ws2_32") 
#include <stdio.h> 

int main() 
{ 
	printf("服务端程序\n"); 
	//------①加载---------- 
	WSADATA wsaData; 
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
	{ 
		printf("WSAStartup Failed, Error=【%d】\n", WSAGetLastError()); 
		return 1; 
	} 
	else 
		printf("①加载成功\n"); 
	//-------②创建流式套接字------------ 
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	if (s == INVALID_SOCKET) 
	{ 
		printf("socket() Failed, Error=【%d】\n", WSAGetLastError()); 
		return 1; 
	} 
	else 
		printf("②已创建监听套接口：【%d】\n", s); 
	//将套接口s置于”非阻塞模式“ 
	u_long u1 = 1; 
	ioctlsocket(s, FIONBIO, (u_long*)&u1); 
	//-----------③绑定本地地址--------------------- 
	struct sockaddr_in Sadd; 
	Sadd.sin_family = AF_INET; 
	Sadd.sin_port = htons(5555); 
	Sadd.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); 
	if (bind(s, (sockaddr*)&Sadd, sizeof(Sadd)) == SOCKET_ERROR) 
	{ 
		printf("bind() Failed, Error=【%d】\n", WSAGetLastError()); 
		return 1; 
	} 
	else 
		printf("③绑定成功，本地IP地址：【%s】, 端口号：【%d】\n", inet_ntoa(Sadd.sin_addr), ntohs(Sadd.sin_port)); 
	//--------------④进入监听状态----------------- 
	if (listen(s, 5) == SOCKET_ERROR) 
	{ 
		printf("listen Failed, Error=【%d】\n", WSAGetLastError()); 
		return 1; 
	} 
	else 
		printf("④进入监听状态\n"); 
	//--------------⑤创建事件对象----------------- 
	WSAEVENT NewEvent = WSACreateEvent(); 
	if (NewEvent == WSA_INVALID_EVENT) 
	{ 
		printf("WSACreateEvent() Failed, Error=【%d】\n", WSAGetLastError()); 
		return 1; 
	} 
	else 
		printf("⑤创建一个事件对象，返回的事件对象句柄NewEvent=%d\n", NewEvent); 
	//--------------⑥网络事件注册------------ 
	int WESerror=WSAEventSelect(s, NewEvent, FD_ACCEPT | FD_CLOSE); 
	if (WESerror == INVALID_SOCKET) 
	{ 
		printf("WSAEventSelect() Failed,Error=【%d】\n",WSAGetLastError()); 
		return -1; 
	} 
	else 
		printf("⑥套接口【%d】、事件对象【%d】和网络事件FD_ACCEPT|FD_CLOSE已关联\n", s, NewEvent); 
	//-----------准备工作--------------- 
	int t = 1; 
	WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS]; 
	SOCKET sockArray[WSA_MAXIMUM_WAIT_EVENTS]; 
	int n = 0; 
	eventArray[n] = NewEvent; 
	sockArray[n] = s; 
	++n; 
	//------------循环处理------------- 
	while (true) 
	{ 
		//---------------⑦等待事件对象-------------- 
		int nIndex = WSAWaitForMultipleEvents(n, eventArray, FALSE, 40000, FALSE); 
		printf("nIndex=%d\n", nIndex); 
		if (nIndex == WSA_WAIT_FAILED)//------7.1调用失败--------- 
		{ 
			printf("调用失败\n"); 
			break; 
		} 
		else if (nIndex == WSA_WAIT_TIMEOUT)//-------7.2超时--------- 
		{ 
			if (t<3) 
			{ 
				printf("第【%d】次超时\n",t); 
				t++; 
				continue; 
			} 
			else 
			{ 
				printf("第【%d】次超时，退出\n",t); 
				break; 
			} 
		} 
		//---------------7.3网络事件触发事件对象句柄的工作状态-------- 
		else 
		{ 
			WSANETWORKEVENTS event;//该结构记录网络事件和对应出错代码 
			//---------⑧网络事件查询----------- 
			WSAEnumNetworkEvents(sockArray[nIndex - WSA_WAIT_EVENT_0], NULL, &event); 
			WSAResetEvent(eventArray[nIndex - WSA_WAIT_EVENT_0]); 
			if ((event.lNetworkEvents & FD_ACCEPT) != 0)       //------8.1处理FD_ACCEPT通知消息 
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
			else if (event.lNetworkEvents & FD_READ)    //-------8.2处理FD_READ通知消息 
			{ 
				if (event.iErrorCode[FD_READ_BIT] == 0) 
				{ 
					char buf[256]; 
					memset(buf, 0, 256); 
					int nRecv = recv(sockArray[nIndex - WSA_WAIT_EVENT_0], buf, sizeof(buf), 0); 
					if (nRecv > 0) 
					{ 
						printf("接收到客户端【%d】数据：%s\n", sockArray[nIndex - WSA_WAIT_EVENT_0], buf); 
					} 
				} 
			} 
			else if (event.lNetworkEvents & FD_CLOSE)  //---------8.3处理FD_CLOSE通知消息 
			{ 
				if (event.iErrorCode[FD_CLOSE_BIT] == 0) 
				{ 
					closesocket(sockArray[nIndex - WSA_WAIT_EVENT_0]); 
					WSACloseEvent(eventArray[nIndex - WSA_WAIT_EVENT_0]); 
					printf("套接字为【%d】的已关闭连接\n", sockArray[nIndex - WSA_WAIT_EVENT_0]); 
				} 
				else 
				{ 
					if (event.iErrorCode[FD_CLOSE_BIT] == 10053) 
					{ 
						closesocket(sockArray[nIndex - WSA_WAIT_EVENT_0]); 
						WSACloseEvent(eventArray[nIndex - WSA_WAIT_EVENT_0]); 
						printf("客户端【%d】非法关闭连接\n", sockArray[nIndex - WSA_WAIT_EVENT_0]); 
					} 
				} 
				for (int j = nIndex - WSA_WAIT_EVENT_0; j < n - 1; ++j) 
				{ 
					sockArray[j] = sockArray[j + 1]; 
					eventArray[j] = eventArray[j + 1]; 
				} 
				--n; 
			} 
		}// end 网络事件触发 
	}//end while 
	printf("退出服务器程序\n"); 
	closesocket(s); 
	WSACleanup(); 
	return 0; 
} 