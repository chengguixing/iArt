#include <windows.h>
#include <Winhttp.h>
#include <iostream>
using namespace std;




int main(void) 
{ 
	SYSTEMTIME sTime;
	LPCWSTR pwszTimeStr = L"Tue, 21 Nov 2000 01:06:53 GMT";

	if (!WinHttpTimeToSystemTime(pwszTimeStr, &sTime))
	{
		printf("Error %u in WinHttpTimeToSystemTime.\n", GetLastError());
	}
	else
	{
		printf("The U.S. formatted date is (%u/%u/%u)\n", sTime.wMonth,
			sTime.wDay,
			sTime.wYear);
	}

	system("pause");
	return 0;
} 