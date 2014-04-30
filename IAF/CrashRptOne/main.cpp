#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdarg.h>
#include "CrashRpt.h"
#include "tslog/tslog.h"



// �������ʱ��crashrpt���õĻص�����
BOOL WINAPI CrashCallback(LPVOID /*lpvState*/)
{  
	TSDEBUG(L"Enter %s", __FUNCTIONW__);

	// ����TRUE, ��crashrpt���ɴ��󱨸�

	TSDEBUG(L"Leave %s", __FUNCTIONW__);
	return TRUE;
}




// �̴߳�����
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	TSDEBUG(L"Enter %s", __FUNCTIONW__);
	// �ڸ��߳��а�װcrashrpt���δ�����쳣�Ĵ���  
	crInstallToCurrentThread2(0);


	for(;;)
	{
		// ������ģ��һ���ڴ�Խ��
		int* p = NULL;
		*p = 13;
	}    


	// ����crashrpt��Դ
	crUninstallFromCurrentThread();    


	TSDEBUG(L"Leave %s", __FUNCTIONW__);
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{  
	TSDEBUG(L"Enter %s", __FUNCTIONW__);
	// ����crashrpt�ĸ������
	CR_INSTALL_INFO info;  
	memset(&info, 0, sizeof(CR_INSTALL_INFO));  
	info.cb = sizeof(CR_INSTALL_INFO);    
	info.pszAppName = _T("MyApp");  
	info.pszAppVersion = _T("1.0.0");  
	info.pszEmailSubject = _T("MyApp 1.0.0 Error Report");  
	info.pszEmailTo = _T("izhuxin@gmail.com");    
	info.pszUrl = _T("http://myapp.com/tools/crashrpt.php");  
	info.pfnCrashCallback = CrashCallback;   
	info.uPriorities[CR_HTTP] = 3;  // ����ʹ��HTTP�ķ�ʽ���ʹ��󱨸�
	info.uPriorities[CR_SMTP] = 2;  // Ȼ��ʹ��SMTP�ķ�ʽ���ʹ��󱨸�  
	info.uPriorities[CR_SMAPI] = 1; //�����ʹ��SMAPI�ķ�ʽ���ʹ��󱨸�    
	// ���������ܹ�������쳣, ʹ��HTTP�����Ʊ���ķ�ʽ����
	info.dwFlags |= CR_INST_ALL_POSSIBLE_HANDLERS;
	info.dwFlags |= CR_INST_HTTP_BINARY_ENCODING; 
	info.dwFlags |= CR_INST_APP_RESTART; 
	info.dwFlags |= CR_INST_SEND_QUEUED_REPORTS; 
	info.pszRestartCmdLine = _T("/restart");
	// ��˽����URL
	info.pszPrivacyPolicyURL = _T("http://myapp.com/privacypolicy.html"); 

	int nResult = crInstall(&info);    
	if(nResult != 0)  
	{    
		TCHAR szErrorMsg[512] = _T("");        
		crGetLastErrorMsg(szErrorMsg, 512);    
		_tprintf_s(_T("%s\n"), szErrorMsg);    
		return 1;
	} 

	// �����־�ļ������󱨸���
	crAddFile2(_T("log.txt"), NULL, _T("Log File"), CR_AF_MAKE_FILE_COPY);    

	// ��ӳ������ʱ�Ľ��������󱨸���
	crAddScreenshot(CR_AS_VIRTUAL_SCREEN);   

	// ����������Ϣ�����󱨸��У��������Կ���Ϣ��Ϊʾ��
	crAddProperty(_T("VideoCard"), _T("nVidia GeForce 8600 GTS"));



	HANDLE hWorkingThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)NULL, 0, NULL);


	TCHAR* szFormatString = NULL;
	_tprintf_s(szFormatString);

	WaitForSingleObject(hWorkingThread, INFINITE);



	crUninstall();


	TSDEBUG(L"Leave %s", __FUNCTIONW__);
	return 0;
}