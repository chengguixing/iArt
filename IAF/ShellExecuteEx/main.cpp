#include<windows.h>
#include<tchar.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	SHELLEXECUTEINFO sei;
	memset(&sei, 0, sizeof(sei));
	sei.cbSize = sizeof(sei);
	sei.hwnd = NULL;
	sei.lpVerb = TEXT("runas");
	sei.lpFile = TEXT("XLServicePlatform.exe");
	

	sei.lpParameters = TEXT("-s");
	sei.nShow = SW_SHOW;
	
	if(!ShellExecuteEx(&sei))
	{
		DWORD dwStatus = GetLastError();
		if(dwStatus == ERROR_CANCELLED)
		{
			printf("提升权限被用户拒绝\n");
		}
		else if(dwStatus == ERROR_FILE_NOT_FOUND)
		{
			printf("所要执行的文件没有找到\n");
		}
	}
	else
	{
		printf("%d\n", GetLastError());
		printf("success\n");
	}


	system("pause");
	return 0;
}