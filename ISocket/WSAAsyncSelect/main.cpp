#define WM_SOCKET WM_USER + 101 
#include <WINSOCK2.H> 
#pragma comment(lib,"WS2_32") 
//----------------窗口过程函数的声明------------- 
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); 
//----------------WinMain()函数------------------ 
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ) 
{ 
	WNDCLASS wc; 
	wc.style = CS_HREDRAW | CS_VREDRAW; 
	wc.lpfnWndProc = WindowProc; 
	wc.cbClsExtra = 0; 
	wc.cbWndExtra = 0; 
	wc.hInstance = hInstance; 
	wc.hIcon = LoadIcon(NULL, IDI_EXCLAMATION); 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); 
	wc.lpszMenuName = NULL; 
	wc.lpszClassName = TEXT("Test"); 
	//---注册窗口类---- 
	RegisterClass(&wc); 
	//---创建窗口---- 
	HWND hwnd=CreateWindow(TEXT("Test"), TEXT("窗口标题"), WS_SYSMENU, 300, 0, 600, 400, NULL, NULL, hInstance, NULL); 
	if (hwnd == NULL) 
	{ 
		MessageBox(hwnd, TEXT("创建窗口出错"), TEXT("标题栏提升"), MB_OK); 
		return 1; 
	} 
	//---显示窗口---- 
	ShowWindow(hwnd, SW_SHOWNORMAL); 
	UpdateWindow(hwnd); 
	//---socket----- 
	WSADATA wsaData; 
	WORD wVersionRequested = MAKEWORD(2, 2); 
	if (WSAStartup(wVersionRequested, &wsaData) != 0) 
	{ 
		MessageBox(NULL, TEXT("WSAStartup() Failed"), TEXT("调用失败"), 0); 
		return 1; 
	} 
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	if (s == INVALID_SOCKET) 
	{ 
		MessageBox(NULL, TEXT("socket() Failed"), TEXT("调用失败"), 0); 
		return 1; 
	}     
	sockaddr_in sin; 
	sin.sin_family = AF_INET; 
	sin.sin_port = htons(1111); 
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); 
	if (bind(s, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR) 
	{ 
		MessageBox(NULL, TEXT("bind() Failed"), TEXT("调用失败"), 0); 
		return 1; 
	} 
	if (listen(s, 3) == SOCKET_ERROR) 
	{ 
		MessageBox(NULL, TEXT("listen() Failed"), TEXT("调用失败"), 0); 
		return 1; 
	} 
	else 
		MessageBox(hwnd, TEXT("进入监听状态！"), TEXT("标题栏提示"), MB_OK); 
	WSAAsyncSelect(s, hwnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE); 
	//---消息循环---- 
	MSG msg; 
	while (GetMessage(&msg, 0, 0, 0)) 
	{ 
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	} 
	closesocket(s); 
	WSACleanup(); 
	return msg.wParam; 
} 
//-------------------窗口过程---------------------- 
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
	switch(uMsg) 
	{ 
	case WM_SOCKET: 
		{ 
			SOCKET ss = wParam;   //wParam参数标志了网络事件发生的套接口 
			if (WSAGETSELECTERROR(lParam)) 
			{ 
				closesocket(ss); 
				return 0; 
			} 
			switch (WSAGETSELECTEVENT(lParam)) 
			{ 
			case FD_ACCEPT:   //-----①连接请求到来 
				{ 
					sockaddr_in Cadd; 
					int Cadd_len = sizeof(Cadd); 
					SOCKET sNew = accept(ss, (sockaddr*)&Cadd, &Cadd_len); 
					if (ss == INVALID_SOCKET)                     
						MessageBox(hwnd,TEXT("调用accept()失败！"),TEXT("标题栏提示"),MB_OK);                     
					WSAAsyncSelect(sNew, hwnd, WM_SOCKET, FD_READ | FD_CLOSE); 
				}break; 
			case FD_READ:   //-----②数据发送来 
				{ 
					char cbuf[256]; 
					memset(cbuf, 0, 256); 
					int cRecv=recv(ss, cbuf, 256, 0); 
					if ((cRecv == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)|| cRecv == 0) 
					{ 
						MessageBox(hwnd, TEXT("调用recv()失败！"), TEXT("标题栏提示"), MB_OK); 
						closesocket(ss); 
					} 
					else if (cRecv > 0) 
					{ 
						MessageBox(hwnd, cbuf, TEXT("收到的信息"), MB_OK);     
						char Sbuf[] = "Hello client!I am server"; 
						int isend = send(ss, Sbuf, sizeof(Sbuf), 0); 
						if (isend == SOCKET_ERROR || isend <= 0) 
						{ 
							MessageBox(hwnd, TEXT("发送消息失败！"), TEXT("标题栏提示"), MB_OK);                             
						} 
						else 
							MessageBox(hwnd, TEXT("已经发信息到客户端！"), TEXT("标题栏提示"), MB_OK); 
					} 
				}break; 
			case FD_CLOSE:    //----③关闭连接 
				{ 
					closesocket(ss); 
				} 
				break; 
			} 
		} 
		break; 
	case WM_CLOSE: 
		if (IDYES == MessageBox(hwnd, TEXT("是否确定退出？"), TEXT("message"), MB_YESNO)) 
			DestroyWindow(hwnd); 
		break; 
	case WM_DESTROY: 
		PostQuitMessage(0); 
		break; 
	default: 
		return DefWindowProc(hwnd,uMsg,wParam,lParam); 
	} 
	return 0; 
} 