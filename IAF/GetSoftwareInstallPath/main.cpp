#include <stdlib.h>
#include <iostream>
#include <Shlwapi.h>
using namespace std;
#pragma comment(lib, "Shlwapi.lib")


#define		REGEDIT_THUNDER_SUB_KEY						L"SOFTWARE\\Thunder Network\\ThunderOem\\thunder_backwnd"
#define		REGEDIT_THUNDER_KEY_VALUE_PATH				L"Path"



BOOL GetThunderPath(std::wstring&);

int main(void) 
{ 
	std::wstring path;
	GetThunderPath(path);
	wcout << path.c_str() << std::endl;


	system("pause");
	return 0;
} 



BOOL GetThunderPath(std::wstring& wszInstallDir)
{
	const size_t cdwBufferSize = 512;
	wchar_t szBuffer[cdwBufferSize] = {0};
	DWORD dwBufferSize = cdwBufferSize;
	DWORD dwType = (DWORD)-1;

	if (ERROR_SUCCESS == SHGetValue(HKEY_LOCAL_MACHINE, REGEDIT_THUNDER_SUB_KEY, REGEDIT_THUNDER_KEY_VALUE_PATH, &dwType, szBuffer, &dwBufferSize))
	{
		if (wcslen(szBuffer) > 0)
		{
			wszInstallDir = szBuffer;
			return TRUE;
		}

		return FALSE;
	}

	return FALSE;

}