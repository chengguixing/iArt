#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdarg.h>
#include "CrashRpt.h"
#include "tslog/tslog.h"



// 程序崩溃时由crashrpt调用的回调函数
BOOL WINAPI CrashCallback(LPVOID /*lpvState*/)
{  
	TSDEBUG(L"Enter %s", __FUNCTIONW__);

	// 返回TRUE, 由crashrpt生成错误报告

	TSDEBUG(L"Leave %s", __FUNCTIONW__);
	return TRUE;
}




// 线程处理函数
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	TSDEBUG(L"Enter %s", __FUNCTIONW__);
	// 在该线程中安装crashrpt库对未处理异常的处理  
	crInstallToCurrentThread2(0);


	for(;;)
	{
		// 在这里模拟一处内存越界
		int* p = NULL;
		*p = 13;
	}    


	// 清理crashrpt资源
	crUninstallFromCurrentThread();    


	TSDEBUG(L"Leave %s", __FUNCTIONW__);
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{  
	TSDEBUG(L"Enter %s", __FUNCTIONW__);
	// 设置crashrpt的各项参数
	CR_INSTALL_INFO info;  
	memset(&info, 0, sizeof(CR_INSTALL_INFO));  
	info.cb = sizeof(CR_INSTALL_INFO);    
	info.pszAppName = _T("MyApp");  
	info.pszAppVersion = _T("1.0.0");  
	info.pszEmailSubject = _T("MyApp 1.0.0 Error Report");  
	info.pszEmailTo = _T("izhuxin@gmail.com");    
	info.pszUrl = _T("http://myapp.com/tools/crashrpt.php");  
	info.pfnCrashCallback = CrashCallback;   
	info.uPriorities[CR_HTTP] = 3;  // 首先使用HTTP的方式发送错误报告
	info.uPriorities[CR_SMTP] = 2;  // 然后使用SMTP的方式发送错误报告  
	info.uPriorities[CR_SMAPI] = 1; //最后尝试使用SMAPI的方式发送错误报告    
	// 捕获所有能够捕获的异常, 使用HTTP二进制编码的方式传输
	info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS;
	info.dwFlags |= CR_INST_HTTP_BINARY_ENCODING; 
	info.dwFlags |= CR_INST_APP_RESTART; 
	info.dwFlags |= CR_INST_SEND_QUEUED_REPORTS; 
	info.pszRestartCmdLine = _T("/restart");
	// 隐私策略URL
	info.pszPrivacyPolicyURL = _T("http://myapp.com/privacypolicy.html"); 

	int nResult = crInstall(&info);    
	if(nResult != 0)  
	{    
		TCHAR szErrorMsg[512] = _T("");        
		crGetLastErrorMsg(szErrorMsg, 512);    
		_tprintf_s(_T("%s\n"), szErrorMsg);    
		return 1;
	} 

	// 添加日志文件到错误报告中
	crAddFile2(_T("log.txt"), NULL, _T("Log File"), CR_AF_MAKE_FILE_COPY);    

	// 添加程序崩溃时的截屏到错误报告中
	crAddScreenshot(CR_AS_VIRTUAL_SCREEN);   

	// 添加任意的信息到错误报告中，这里以显卡信息作为示例
	crAddProperty(_T("VideoCard"), _T("nVidia GeForce 8600 GTS"));



	HANDLE hWorkingThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)NULL, 0, NULL);


	TCHAR* szFormatString = NULL;
	_tprintf_s(szFormatString);

	WaitForSingleObject(hWorkingThread, INFINITE);



	crUninstall();


	TSDEBUG(L"Leave %s", __FUNCTIONW__);
	return 0;
}