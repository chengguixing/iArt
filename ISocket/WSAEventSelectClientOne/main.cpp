//Client 1
#include <winsock2.h>
#pragma comment(lib,"WS2_32")
#include <stdio.h>
int main()
{  	
	printf("�ͻ��˳���/n"); 	
//----------------�ټ���---------------------------------------------------------
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);  
	if(WSAStartup(wVersionRequested, &wsaData) != 0)  			
	{   
		printf("WSAStartup() Failed, Error=\n", WSAGetLastError());	
		return 1;
	}
	else 
		printf("�ټ��سɹ�\n");		
//----------------�ڴ�����ʽ�׽���------------------------------------------------	
	SOCKET c1;
	c1 = socket(AF_INET, SOCK_STREAM, 0);
	if(c1 == INVALID_SOCKET)	
	{   printf("socket() Failed, Error=\n", WSAGetLastError());
		return 1;
	}
	else 
		printf("���Ѵ��������׽���:�� %d ��\n", c1);	
//----------------�󶨱��ص�ַ------------------------------------------------
    struct  sockaddr_in  C1add;  
    C1add.sin_family = AF_INET;
	C1add.sin_port = htons(2222);
	C1add.sin_addr.s_addr = inet_addr("127.0.0.1");	
	if(bind(c1, (sockaddr*)&C1add, sizeof(C1add)) == SOCKET_ERROR)	
	{   
		printf("bind() Failed, Error=/n", WSAGetLastError());	
		return 1;
	}
	else 
		printf("�󶨳ɹ�,����IP��ַ:�� %s ��, �˿ں�:�� %d ��\n", inet_ntoa(C1add.sin_addr), ntohs(C1add.sin_port));	
//------------------����������------------------------------------------------		
	struct  sockaddr_in  Sadd;  
	Sadd.sin_family = AF_INET;
	Sadd.sin_port = htons(5555);  
	Sadd.sin_addr.s_addr = inet_addr("127.0.0.1");	
	if(connect(c1, (sockaddr*)&Sadd, sizeof(Sadd)) == -1)
	{   
		printf(" Failed connect(), Error=�� %d ��/n", WSAGetLastError());		
		return 1;
	}
	else //*************************���ӳɹ�,���� ��ʼ���͡�����*************************
	{	
		printf("�����ӳɹ�,������IP��ַ:�� %s ��, �˿ں�:�� %d ��\n", inet_ntoa(Sadd.sin_addr), ntohs(Sadd.sin_port));
	    int a;
	    printf("ϣ������������(���롰1������)");
		scanf("%d", &a);	 			
	    while(a == 1)  //------ѭ������------
		{   //-----����------     			    
            char S_buf[] = "Hello!I am a client 1";	
		    int  isend = send(c1, S_buf, strlen(S_buf), 0);		
		    if(isend == SOCKET_ERROR)	
			{  
				printf("Failed  send(), Error=�� %d ��, ���߷���������ر�\n", WSAGetLastError());
		   		return 1;
			}
		    else if(isend != 0)
			   printf("��Ϣ�� %s ���ѷ���\n", S_buf);			 
		    else
			   printf("��Ϣ�޷����͸������\n");

			printf("ϣ����������������(���롰1����������)");
				scanf("%d", &a);
    	        if(a != 1)			
		        break;
		} //------end ѭ������------	   
	} //*************************end ��ʼ���͡�����*************************
//------------------�ܹرա��ͷ�------------------------------------------------	
    closesocket(c1);		
	WSACleanup();
	printf("����������������/n");
	return 0;
	
}