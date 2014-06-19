// DeadLockTest.cpp : Defines the entry point for the console application.
//

#include <afxwin.h>
#include <iostream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

CRITICAL_SECTION cs1;
CRITICAL_SECTION cs2;
CRITICAL_SECTION csprint;

//��ʼ���ؼ������
void InitMyCriticalSection();
//ɾ���ؼ������
void DeleteMyCriticalSection();
//��ӡ��Ϣ
void PrintString(const CString& strInfo);

DWORD WINAPI Thread1(LPVOID lpParameter);
DWORD WINAPI Thread2(LPVOID lpParameter);

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;

		return nRetCode;
	}

	//��ʼ���ؼ������
	InitMyCriticalSection();

	//�����߳�
	HANDLE hThread1 = CreateThread(NULL, 0, Thread1, NULL, 0, NULL);
	HANDLE hThread2 = CreateThread(NULL, 0, Thread2, NULL, 0, NULL);

	//�ȴ��߳̽���
	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);

	//�ر��߳̾��
	CloseHandle(hThread1);
	CloseHandle(hThread2);

	//�ͷŹؼ������
	DeleteMyCriticalSection();

	return nRetCode;
}

void InitMyCriticalSection()
{
	InitializeCriticalSection(&cs1);
	InitializeCriticalSection(&cs2);
	InitializeCriticalSection(&csprint);
}

void DeleteMyCriticalSection()
{
	DeleteCriticalSection(&cs1);
	DeleteCriticalSection(&cs2);
	DeleteCriticalSection(&csprint);
}

DWORD WINAPI Thread1(LPVOID lpParameter)
{
	CString strThreadID = _T("");
	strThreadID.Format(_T("%d"), GetCurrentThreadId());

	CString strPrintInfo = _T("");

	for (int i = 0; i < 5; i++)
	{
		EnterCriticalSection(&cs1);

		strPrintInfo = _T("");
		strPrintInfo += _T("Thread1 ");
		strPrintInfo += strThreadID;
		strPrintInfo += _T(" EnterCriticalSection(&cs1)");

		PrintString(strPrintInfo);

		Sleep(500);
		EnterCriticalSection(&cs2);

		strPrintInfo = _T("");
		strPrintInfo += _T("Thread1 ");
		strPrintInfo += strThreadID;
		strPrintInfo += _T(" EnterCriticalSection(&cs2)");

		PrintString(strPrintInfo);

		LeaveCriticalSection(&cs2);
		LeaveCriticalSection(&cs1);
	}

	return 1;
}

DWORD WINAPI Thread2(LPVOID lpParameter)
{
	CString strThreadID = _T("");
	strThreadID.Format(_T("%d"), GetCurrentThreadId());

	CString strPrintInfo = _T("");

	for (int i = 0; i < 5; i++)
	{
		EnterCriticalSection(&cs2);

		strPrintInfo = _T("");
		strPrintInfo += _T("Thread2 ");
		strPrintInfo += strThreadID;
		strPrintInfo += _T(" EnterCriticalSection(&cs2)");

		PrintString(strPrintInfo);

		Sleep(500);

		EnterCriticalSection(&cs1);

		strPrintInfo = _T("");
		strPrintInfo += _T("Thread2 ");
		strPrintInfo += strThreadID;
		strPrintInfo += _T(" EnterCriticalSection(&cs1)");

		PrintString(strPrintInfo);

		LeaveCriticalSection(&cs1);
		LeaveCriticalSection(&cs2);
	}

	return 1;
}


void PrintString(const CString& strInfo)
{
	EnterCriticalSection(&csprint);
	wcout<<(const TCHAR*)strInfo<<endl;
	LeaveCriticalSection(&csprint);
}
