#include <iostream>
#include <windows.h>
#include <process.h>
using namespace std;

VOID WINAPI APCFunc(ULONG_PTR dwParam)
{
	cout << "APCFunc������APC����ing" << endl;
	Sleep(2000);
	cout << "APCFunc��Sleep��2000, ������APC����" << endl;
}

VOID WINAPI APCFunc2(ULONG_PTR dwParam)
{
	cout << "APCFunc2������APC����2ing" << endl;
	Sleep(100);
	cout << "APCFunc2��������APC����2" << endl;
}

DWORD WINAPI ThreadFun(PVOID pvParam)
{
	HANDLE hEvent = (HANDLE)pvParam;
	Sleep(100);
	cout << "ThreadFun��WaitForSingleObjectEx(hEvent, INFINITE, TRUE); " << endl;
    DWORD dw = WaitForSingleObjectEx(hEvent, INFINITE, TRUE);
	//*Ex�������߳�����Ϊalertable״̬
    if(dw == WAIT_OBJECT_0)
    {
        cout << "ThreadFun���¼�����" << endl;
    }
    if(dw == WAIT_IO_COMPLETION)
    {
        //����߳����ٴ�����APC�����е�һ��
        cout << "ThreadFun��APC������APC����ִ���꣬�ȴ���������WAIT_IO_COMPLETION������̼߳���ִ��" << endl;
    }
    return 0;
}
int main(int argc, char* argv[])
{
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))ThreadFun, (PVOID)hEvent, 0, NULL);
	//ִ��һЩ��������
	cout << "main����ʼ" << endl;
	//��ʱ�û���Ҫ��ֹ���߳�
	Sleep(1000); //����ʱ���������߳̽���alertable״̬��
	cout << "main������Sleep(1000)��ʱ�̵߳���WaitForSingleObjectEx����alertable״̬" << endl;
	QueueUserAPC(APCFunc, hThread, NULL);
	cout << "main��QueueUserAPC(APCFunc, hThread, NULL); " << endl;
	Sleep(1500);
	cout << "main������Sleep��1500ʱ���APCFunc��2000�̣����߳���ִ����APC����ǰ����������µ�APCFunc2" << endl;
	QueueUserAPC(APCFunc2, hThread, NULL);
	cout << "QueueUserAPC(APCFunc2, hThread, NULL);" << endl;
	WaitForSingleObject(hThread, INFINITE);
	cout << "main��WaitForSingleObject(hThread, INFINITE);" << endl;
	system("pause");
	return 0;
}