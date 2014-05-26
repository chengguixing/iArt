#include <WinSock2.h> 
#pragma comment(lib, "ws2_32") 
#include <stdio.h> 
int main(void) 
{ 
    printf("1�ſͻ��˳���\n"); 
    //-----------�ټ���----------------
    WSADATA wsaData; 
    WORD wVersionRequested = MAKEWORD(2 ,2); 
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    { 
        printf("WSAStartup Failed, Error=��%d��\n", WSAGetLastError()); 
        return 1;
    } 
    else 
        printf("�ټ��سɹ�/n"); 
    //-------------�ڴ�����ʽ�׽���-----------------
    SOCKET c1 = socket(AF_INET, SOCK_STREAM, 0); 
    if (c1 == INVALID_SOCKET) 
    { 
        printf("socket() Failed, Error=��%d��\n", WSAGetLastError()); 
        return 1 ;
    } 
    else 
        printf("���Ѵ��������׽���:��%d��\n", c1); 
    //-------------�󶨵�ַ---------------------
    struct sockaddr_in C1add; 
    C1add.sin_family = AF_INET; 
    C1add.sin_port = htons( 2222 ); 
    C1add.sin_addr.s_addr = inet_addr( "127.0.0.1" ); 
    if (bind(c1, (sockaddr*)&C1add, sizeof(C1add)) == SOCKET_ERROR) 
    { 
        printf("bind() Failed, Error=��%d��\n", WSAGetLastError()); 
        return 1 ;
    } 
    else 
        printf("�󶨳ɹ�������IP��ַ����%s��, �˿ںţ���%d��\n", inet_ntoa(C1add.sin_addr), ntohs(C1add.sin_port)); 
    //------------����������---------------
    struct sockaddr_in Sadd; 
    Sadd.sin_family = AF_INET; 
    Sadd.sin_port = htons( 1111 ); 
    Sadd.sin_addr.s_addr= inet_addr( "127.0.0.1" ); 
    if (connect(c1, (sockaddr*)&Sadd, sizeof(Sadd))==- 1 )
    { 
        printf("Failed connect(),Error=��%d��\n", WSAGetLastError()); 
        return 1 ;
    } 
    else //*********���ӳɹ������Կ�ʼ���͡�����************
    { 
        printf("�����ӳɹ���������IP��ַ:��%s��,�˿ںţ���%d��\n", inet_ntoa(Sadd.sin_addr), ntohs(Sadd.sin_port)); 
        int a; 
        printf("ϣ������������(���롰1�����ͣ���������ֵ���˳�)"); 
        scanf("%d", &a); 
        while (a == 1)
        { 
            //---����-----
            char S_buf[] = "Hello!I am a client 1" ; 
            int isend = send(c1, S_buf, strlen(S_buf), 0); 
            if (isend == SOCKET_ERROR) 
            { 
                printf("Failed send(), Error=��%d��, ���߷���������ر�\n", WSAGetLastError()); 
                return 1;
            } 
            else if ( isend != 0 )
                printf( "��Ϣ��%s���ѷ���\n" , S_buf); 
            else 
                printf( "��Ϣ�޷����͸��ͻ���\n" ); 
            //---����----
            char R_buf[ 256 ]; 
            int RRecv; 
            memset(R_buf, 0, sizeof(R_buf)); 
            RRecv = recv(c1, R_buf, 256 ,0 ); 
            if ( RRecv == SOCKET_ERROR) 
            { 
                printf("Failed recv(), Error=��%d��\n", WSAGetLastError()); 
                return 1 ;
            } 
            else if ( RRecv == 0 )
                printf("�޷��յ����Է��������κ����ݣ� ���߷���������ر�\n"); 
            else 
            { 
                printf("���յ����Է����������ݣ���%s��\n", R_buf); 
                printf("ϣ���������������𣿣����롱1���������ͣ���������ֵ���˳�)"); 
                scanf("%d", &a); 
                if (a != 1)
                    break ; 
            } 
        } //-------end ����
        printf("��ѡ���˳�����\n"); 
    } //------end ����
    //-------------------�ܹرա��ͷ�------------
    closesocket( c1); 
    WSACleanup(); 
    printf( "����������������/n" ); 
    return 0 ;
}