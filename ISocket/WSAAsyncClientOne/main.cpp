#include <WinSock2.h> 
#pragma comment(lib, "ws2_32") 
#include <stdio.h> 
int main(void) 
{ 
    printf("1号客户端程序\n"); 
    //-----------①加载----------------
    WSADATA wsaData; 
    WORD wVersionRequested = MAKEWORD(2 ,2); 
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    { 
        printf("WSAStartup Failed, Error=【%d】\n", WSAGetLastError()); 
        return 1;
    } 
    else 
        printf("①加载成功/n"); 
    //-------------②创建流式套接字-----------------
    SOCKET c1 = socket(AF_INET, SOCK_STREAM, 0); 
    if (c1 == INVALID_SOCKET) 
    { 
        printf("socket() Failed, Error=【%d】\n", WSAGetLastError()); 
        return 1 ;
    } 
    else 
        printf("②已创建连接套接字:【%d】\n", c1); 
    //-------------绑定地址---------------------
    struct sockaddr_in C1add; 
    C1add.sin_family = AF_INET; 
    C1add.sin_port = htons( 2222 ); 
    C1add.sin_addr.s_addr = inet_addr( "127.0.0.1" ); 
    if (bind(c1, (sockaddr*)&C1add, sizeof(C1add)) == SOCKET_ERROR) 
    { 
        printf("bind() Failed, Error=【%d】\n", WSAGetLastError()); 
        return 1 ;
    } 
    else 
        printf("绑定成功，本地IP地址：【%s】, 端口号：【%d】\n", inet_ntoa(C1add.sin_addr), ntohs(C1add.sin_port)); 
    //------------③连接请求---------------
    struct sockaddr_in Sadd; 
    Sadd.sin_family = AF_INET; 
    Sadd.sin_port = htons( 1111 ); 
    Sadd.sin_addr.s_addr= inet_addr( "127.0.0.1" ); 
    if (connect(c1, (sockaddr*)&Sadd, sizeof(Sadd))==- 1 )
    { 
        printf("Failed connect(),Error=【%d】\n", WSAGetLastError()); 
        return 1 ;
    } 
    else //*********连接成功，可以开始发送、接收************
    { 
        printf("③连接成功，服务器IP地址:【%s】,端口号：【%d】\n", inet_ntoa(Sadd.sin_addr), ntohs(Sadd.sin_port)); 
        int a; 
        printf("希望发送数据吗？(键入“1”发送，键入其他值则退出)"); 
        scanf("%d", &a); 
        while (a == 1)
        { 
            //---发送-----
            char S_buf[] = "Hello!I am a client 1" ; 
            int isend = send(c1, S_buf, strlen(S_buf), 0); 
            if (isend == SOCKET_ERROR) 
            { 
                printf("Failed send(), Error=【%d】, 或者服务器意外关闭\n", WSAGetLastError()); 
                return 1;
            } 
            else if ( isend != 0 )
                printf( "信息【%s】已发送\n" , S_buf); 
            else 
                printf( "信息无法发送给客户端\n" ); 
            //---接收----
            char R_buf[ 256 ]; 
            int RRecv; 
            memset(R_buf, 0, sizeof(R_buf)); 
            RRecv = recv(c1, R_buf, 256 ,0 ); 
            if ( RRecv == SOCKET_ERROR) 
            { 
                printf("Failed recv(), Error=【%d】\n", WSAGetLastError()); 
                return 1 ;
            } 
            else if ( RRecv == 0 )
                printf("无法收到来自服务器的任何数据， 或者服务器意外关闭\n"); 
            else 
            { 
                printf("接收到来自服务器的数据：【%s】\n", R_buf); 
                printf("希望继续发送数据吗？（键入”1“继续发送，键入其他值则退出)"); 
                scanf("%d", &a); 
                if (a != 1)
                    break ; 
            } 
        } //-------end 结束
        printf("已选择退出连接\n"); 
    } //------end 结束
    //-------------------④关闭、释放------------
    closesocket( c1); 
    WSACleanup(); 
    printf( "④与服务器连接完毕/n" ); 
    return 0 ;
}