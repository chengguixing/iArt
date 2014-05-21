#include <winsock2.h>
#include <iostream>
using namespace std;



#pragma comment(lib, "ws2_32.lib")



#define CLIENT_SETUP_FAIL 1
#define CLIENT_CREATETHREAD_FAIL 2
#define CLIENT_CONNECT_FAIL 3
#define TIMEFOR_THREAD_EXIT 1000
#define TIMEFOR_THREAD_SLEEP 500


#define SERVERIP "127.0.0.1"
#define SERVERPORT 5556
#define MAX_NUM_BUF 48
#define ADD '+'
#define SUB '-'
#define MUT '*'
#define DIV '/'
#define EQU '='


#define EXPRESSION 'E'
#define BYEBYE 'B'
#define HEADERLEN (sizeof(hdr))


typedef struct _head
{
	char type;
	unsigned short len;
}hdr, *phdr;


typedef struct _data
{
	char buf[MAX_NUM_BUF];
}DATABUF, *pDataBuf;




SOCKET sClient;
HANDLE hThreadSend;
HANDLE hThreadRecv;
DATABUF bufSend;
DATABUF bufRecv;
CRITICAL_SECTION csSend;
CRITICAL_SECTION csRecv;
BOOL bSendData;
HANDLE hEventShowDataResult;
BOOL bConnecting;
HANDLE arrThread[2];


BOOL InitClient(void);
BOOL ConnectServer(void);
BOOL CreateSendAndRecvThread(void);
void InputAndOutput(void);
void ExitClient(void);


void InitMember(void);
BOOL InitSocket(void);



DWORD __stdcall RecvDataThread(void* pParam);
DWORD __stdcall SendDataThread(void* pParam);


BOOL PackByebye(const char* pExpr);
BOOL PackExpression(const char* pExpr);


void ShowConnectMsg(BOOL bSuc);
void ShowDataResultMsg(void);
void ShowTipMsg(BOOL bFirstInput);



int main(void)
{
	if (!InitClient())
	{
		ExitClient();
		return CLIENT_SETUP_FAIL;
	}


	if (ConnectServer())
	{
		ShowConnectMsg(TRUE);
	}
	else
	{
		ShowConnectMsg(FALSE);
		ExitClient();
		return CLIENT_SETUP_FAIL;
	}


	if (!CreateSendAndRecvThread())
	{
		ExitClient();
		return CLIENT_CREATETHREAD_FAIL;
	}

	InputAndOutput();


	ExitClient();


	return 0;
}




BOOL InitClient(void)
{
	InitMember();
	if (!InitSocket())
	{
		return FALSE;
	}
	return TRUE;
}


BOOL ConnectServer(void)
{
	int reVal;
	sockaddr_in serAddr;

	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(SERVERPORT);
	serAddr.sin_addr.S_un.S_addr = inet_addr(SERVERIP);


	for (;;)
	{
		reVal = connect(sClient, (struct sockaddr*)&serAddr, sizeof(serAddr));


		if (SOCKET_ERROR == reVal)
		{
			int nErrCode = WSAGetLastError();
			if (WSAEWOULDBLOCK == nErrCode ||
				WSAEINVAL == nErrCode)
			{
				continue;
			}
			else if (WSAEISCONN == nErrCode)
			{
				break;
			}
			else
			{
				return FALSE;
			}
		}
		if (0 == reVal)
		{
			break;
		}
	}
	bConnecting = TRUE;

	return TRUE;
}
BOOL CreateSendAndRecvThread(void)
{
	unsigned long ulThreadId;
	hThreadRecv = CreateThread(NULL, 0, RecvDataThread, NULL, 0, &ulThreadId);
	if (NULL == hThreadRecv)
	{
		return FALSE;
	}


	hThreadSend = CreateThread(NULL, 0, SendDataThread, NULL, 0, &ulThreadId);
	if (NULL == hThreadSend)
	{
		return FALSE;
	}

	arrThread[0] = hThreadRecv;
	arrThread[1] = hThreadSend;
	return TRUE;
}
void InputAndOutput(void)
{
	char cInput[MAX_NUM_BUF];
	BOOL bFirstInput = TRUE;

	for (;bConnecting;)
	{
		memset(cInput, 0, MAX_NUM_BUF);

		ShowTipMsg(bFirstInput);

		cin >> cInput;
		char* pTemp = cInput;
		if (bFirstInput)
		{
			if (!PackExpression(pTemp))
			{
				continue;
			}
			bFirstInput = FALSE;
		}
		else if (!PackByebye(pTemp))
		{
			if (!PackExpression(pTemp))
			{
				continue;
			}
		}
		if (WAIT_OBJECT_0 == WaitForSingleObject(hEventShowDataResult, INFINITE))
		{
			ResetEvent(hEventShowDataResult);
			if (!bConnecting)
			{
				break;
			}

			ShowDataResultMsg();


			if (0 == strcmp(bufRecv.buf, "OK"))
			{
				bConnecting = FALSE;
				Sleep(TIMEFOR_THREAD_EXIT);
			}
		}
	}

	if (!bConnecting)
	{
		ShowConnectMsg(FALSE);
	}


	DWORD reVal = WaitForMultipleObjects(2, arrThread, TRUE, INFINITE);
	if (WAIT_ABANDONED_0 == reVal)
	{
		int nErrCode = GetLastError();
	}
}
void ExitClient(void)
{
	DeleteCriticalSection(&csSend);
	DeleteCriticalSection(&csRecv);
	CloseHandle(hThreadRecv);
	CloseHandle(hThreadSend);
	closesocket(sClient);
	WSACleanup();
	return ;
}


void InitMember(void)
{
	InitializeCriticalSection(&csSend);
	InitializeCriticalSection(&csRecv);


	sClient = INVALID_SOCKET;
	hThreadRecv = NULL;
	hThreadSend = NULL;
	bConnecting = FALSE;
	bSendData = FALSE;


	memset(bufSend.buf, 0, MAX_NUM_BUF);
	memset(bufRecv.buf, 0, MAX_NUM_BUF);
	memset(arrThread, 0, 2);


	hEventShowDataResult = (HANDLE)CreateEvent(NULL, TRUE, FALSE, NULL);
}
BOOL InitSocket(void)
{
	int reVal;
	WSADATA wsData;
	reVal = WSAStartup(MAKEWORD(2, 2), &wsData);


	sClient = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == sClient)
	{
		return FALSE;
	}


	unsigned long ul = 1;
	reVal = ioctlsocket(sClient, FIONBIO, (unsigned long*)&ul);
	if (reVal == SOCKET_ERROR)
	{
		return FALSE;
	}


	return TRUE;
}



DWORD __stdcall RecvDataThread(void* pParam)
{
	int reVal;
	char temp[MAX_NUM_BUF];
	memset(temp, 0, MAX_NUM_BUF);


	while (bConnecting)
	{
		reVal = recv(sClient, temp, MAX_NUM_BUF, 0);

		if (SOCKET_ERROR == reVal)
		{
			int nErrCode = WSAGetLastError();
			if (WSAEWOULDBLOCK == nErrCode)
			{
				Sleep(TIMEFOR_THREAD_SLEEP);
				continue;
			}
			else
			{
				bConnecting = FALSE;
				SetEvent(hEventShowDataResult);
				return 0;
			}
		}


		if (0 == reVal)
		{
			bConnecting = FALSE;
			SetEvent(hEventShowDataResult);
			return 0;
		}

		if (reVal > HEADERLEN && -1 != reVal)
		{
			phdr header = (phdr)(temp);
			EnterCriticalSection(&csRecv);
			memset(bufRecv.buf, 0, MAX_NUM_BUF);
			memcpy(bufRecv.buf, temp + HEADERLEN, header->len - HEADERLEN);
			LeaveCriticalSection(&csRecv);


			SetEvent(hEventShowDataResult);
			memset(temp, 0, MAX_NUM_BUF);
		}

		Sleep(TIMEFOR_THREAD_SLEEP);
	}


	return 0;
}
DWORD __stdcall SendDataThread(void* pParam)
{
	while (bConnecting)
	{
		if (bSendData)
		{
			EnterCriticalSection(&csSend);
			
			for(;;)
			{
				int nBuflen = ((phdr)(bufSend.buf))->len;
				int val = send(sClient, bufSend.buf, nBuflen, 0);


				if (SOCKET_ERROR == val)
				{
					int nErrCode = WSAGetLastError();

					if (WSAEWOULDBLOCK == nErrCode)
					{
						continue;
					}
					else
					{
						LeaveCriticalSection(&csSend);
						bConnecting = FALSE;
						SetEvent(hEventShowDataResult);
						return 0;
					}
				}
				bSendData = FALSE;
				break;
			}
			LeaveCriticalSection(&csSend);
		}

		Sleep(TIMEFOR_THREAD_SLEEP);

	}

	return 0;
}


BOOL PackByebye(const char* pExpr)
{
	BOOL reVal = FALSE;
	if (!strcmp("Byebye", pExpr) || !strcmp("byebye", pExpr))
	{
		EnterCriticalSection(&csSend);
		phdr pHeader = (phdr)bufSend.buf;
		pHeader->type = BYEBYE;
		pHeader->len = HEADERLEN + strlen("Byebye");
		memcpy(bufSend.buf + HEADERLEN, pExpr, strlen(pExpr));
		LeaveCriticalSection(&csSend);


		pHeader = NULL;
		bSendData = TRUE;
	}

	return reVal;
}
BOOL PackExpression(const char* pExpr)
{
	char* pTemp = (char*)pExpr;	//�������ʽ���ֿ�ʼ��λ��
	while (!*pTemp)				//��һ������λ��
		pTemp++;		

	char* pos1 = pTemp;	//��һ������λ��
	char* pos2 = NULL;	//�����λ��
	char* pos3 = NULL;	//�ڶ�������λ��
	int len1 = 0;		//��һ�����ֳ���
	int len2 = 0;		//���������
	int len3 = 0;		//�ڶ������ֳ���

	//��һ���ַ���+ - ����������
	if ((*pTemp != '+') && 
		(*pTemp != '-') &&
		((*pTemp < '0') || (*pTemp > '9')))
	{
		return FALSE;
	}


	if ((*pTemp++ == '+')&&(*pTemp < '0' || *pTemp > '9'))	//��һ���ַ���'+'���ڶ���������	
		return FALSE;										//��������
	--pTemp;												//����ָ��


	if ((*pTemp++ == '-')&&(*pTemp < '0' || *pTemp > '9'))	//��һ���ַ���'-',�ڶ���������	
		return FALSE;										//��������
	--pTemp;												//����ָ��

	char* pNum = pTemp;						//���ֿ�ʼ��λ��					
	if (*pTemp == '+'||*pTemp == '-')		//+ -
		pTemp++;

	while (*pTemp >= '0' && *pTemp <= '9')	//����
		pTemp++;							

	len1 = pTemp - pNum;//���ֳ���						

	//�����пո�
	while(!*pTemp)							
		pTemp++;

	//���������
	if ((ADD != *pTemp)&&			
		(SUB != *pTemp)&&
		(MUT != *pTemp)&&
		(DIV != *pTemp))
		return FALSE;

	pos2 = pTemp;
	len2 = 1;

	//����ָ��
	pTemp++;
	//�����пո�
	while(!*pTemp)
		pTemp++;

	//��2������λ��
	pos3 = pTemp;
	if (*pTemp < '0' || *pTemp > '9')
		return FALSE;//��������			

	while (*pTemp >= '0' && *pTemp <= '9')//����
		pTemp++;

	if (EQU != *pTemp)	//����ǵ��ں�
		return FALSE;	//��������

	len3 = pTemp - pos3;//���ֳ���


	int nExprlen = len1 + len2 + len3;	//������ʾ����

	//���ʽ���뷢�����ݻ�����
	EnterCriticalSection(&csSend);		//�����ٽ���
	//���ݰ�ͷ
	phdr pHeader = (phdr)(bufSend.buf);
	pHeader->type = EXPRESSION;			//����
	pHeader->len = nExprlen + HEADERLEN;//���ݰ�����
	//��������
	memcpy(bufSend.buf + HEADERLEN, pos1, len1);
	memcpy(bufSend.buf + HEADERLEN + len1, pos2, len2);
	memcpy(bufSend.buf + HEADERLEN + len1 + len2 , pos3,len3);
	LeaveCriticalSection(&csSend);		//�뿪�ٽ���
	pHeader = NULL;

	bSendData = TRUE;					//֪ͨ���������̷߳�������

	return TRUE;
}


void ShowConnectMsg(BOOL bSuc)
{
	if (bSuc)
	{
		cout << "**************************************" << endl;
		cout << "*                                    *" << endl;
		cout << "* Succeed to connect server!         *" << endl;
		cout << "*                                    *" << endl;
		cout << "**************************************" << endl;
	}
	else
	{
		cout << "**************************************" << endl;
		cout << "*                                    *" << endl;
		cout << "* Fail to connect server!            *" << endl;
		cout << "*                                    *" << endl;
		cout << "**************************************" << endl;
	}
	return ;
}
void ShowDataResultMsg(void)
{
	EnterCriticalSection(&csRecv);
	cout << "**********************************" << endl;
	cout << "*                                *" << endl;
	cout << "* Result :                       *" << endl;
	cout << bufRecv.buf << endl;
	cout << "*                                *" << endl;
	cout << "**********************************" << endl;
	LeaveCriticalSection(&csRecv);
}
void ShowTipMsg(BOOL bFirstInput)
{
	if (bFirstInput)
	{
		cout << "**************************************" << endl;
		cout << "*                                    *" << endl;
		cout << "* Please input expression.           *" << endl;
		cout << "* Usage : NumberOperatorNumber =     *" << endl;
		cout << "*                                    *" << endl;
		cout << "**************************************" << endl;
	}
	else
	{
		cout << "**************************************" << endl;
		cout << "*                                    *" << endl;
		cout << "* Please input expression.           *" << endl;
		cout << "* Usage : NumberOperatorNumber =     *" << endl;
		cout << "*                                    *" << endl;
		cout << "* If you want to exit.               *" << endl;
		cout << "* Usage : Byebye or byebye           *" << endl;
		cout << "*                                    *" << endl;
		cout << "**************************************" << endl;
	}
}