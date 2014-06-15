#include <iostream>
#include <windows.h>
#include <process.h>
using namespace std;

VOID WINAPI APCFunc(ULONG_PTR dwParam)
{
	cout << "APCFunc：处理APC函数ing" << endl;
	Sleep(2000);
	cout << "APCFunc：Sleep了2000, 处理完APC函数" << endl;
}

VOID WINAPI APCFunc2(ULONG_PTR dwParam)
{
	cout << "APCFunc2：处理APC函数2ing" << endl;
	Sleep(100);
	cout << "APCFunc2：处理完APC函数2" << endl;
}

DWORD WINAPI ThreadFun(PVOID pvParam)
{
	HANDLE hEvent = (HANDLE)pvParam;
	Sleep(100);
	cout << "ThreadFun：WaitForSingleObjectEx(hEvent, INFINITE, TRUE); " << endl;
    DWORD dw = WaitForSingleObjectEx(hEvent, INFINITE, TRUE);
	//*Ex函数把线程设置为alertable状态
    if(dw == WAIT_OBJECT_0)
    {
        cout << "ThreadFun：事件触发" << endl;
    }
    if(dw == WAIT_IO_COMPLETION)
    {
        //如果线程至少处理了APC队列中的一项
        cout << "ThreadFun：APC队列中APC函数执行完，等待函数返回WAIT_IO_COMPLETION激活该线程继续执行" << endl;
    }
    return 0;
}
int main(int argc, char* argv[])
{
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))ThreadFun, (PVOID)hEvent, 0, NULL);
	//执行一些其他代码
	cout << "main：开始" << endl;
	//此时用户想要终止子线程
	Sleep(1000); //留出时间先让子线程进入alertable状态。
	cout << "main：主程Sleep(1000)此时线程调用WaitForSingleObjectEx处于alertable状态" << endl;
	QueueUserAPC(APCFunc, hThread, NULL);
	cout << "main：QueueUserAPC(APCFunc, hThread, NULL); " << endl;
	Sleep(1500);
	cout << "main：主程Sleep了1500时间比APCFunc的2000短，让线程在执行完APC函数前往其中添加新的APCFunc2" << endl;
	QueueUserAPC(APCFunc2, hThread, NULL);
	cout << "QueueUserAPC(APCFunc2, hThread, NULL);" << endl;
	WaitForSingleObject(hThread, INFINITE);
	cout << "main：WaitForSingleObject(hThread, INFINITE);" << endl;
	system("pause");
	return 0;
}