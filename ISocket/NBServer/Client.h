#ifndef _CLIENT_H_FB7A11A6_1B10_488C_911B_D14C851B24EC
#define _CLIENT_H_FB7A11A6_1B10_488C_911B_D14C851B24EC


#include <winsock2.h>

#define TIMEFOR_THREAD_CLIENT 500


#define MAX_NUM_CLIENT 10
#define MAX_NUM_BUF 48
#define INVALID_OPERATOR 1
#define INVALID_NUM 2
#define ZERO 0


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


class CClient
{
public:
	CClient(const SOCKET sClient, const sockaddr_in &addrClient);
	virtual ~CClient();
public:
	BOOL StartRunning(void);
	void HandleData(const char* pExpr);
	BOOL IsConning(void)
	{
		return m_bConning;
	}
	void DisConning(void)
	{
		m_bConning = FALSE;
	}
	BOOL IsExit(void)
	{
		return m_bExit;
	}
public:
	static DWORD __stdcall RecvDataThread(void* pParam);
	static DWORD __stdcall SendDataThread(void* pParam);
private:
	CClient();
private:
	SOCKET m_socket;
	sockaddr_in m_addr;
	DATABUF m_data;
	HANDLE m_hEvent;
	HANDLE m_hThreadSend;
	HANDLE m_hThreadRecv;
	CRITICAL_SECTION m_cs;
	BOOL m_bConning;
	BOOL m_bExit;
};

#endif