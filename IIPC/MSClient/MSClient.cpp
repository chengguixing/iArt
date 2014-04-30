#include <windows.h>
#include <stdlib.h>
#include <iostream>
#include <tchar.h>
using namespace std;


#define g_szMailslot _T("\\\\.\\mailslot\\iMailSlot")

#define BUFFER_SIZE 1024


int main(void)
{
	HANDLE hMailslot;

	hMailslot = CreateFile(
		g_szMailslot,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hMailslot)
	{
		cout << "Error occurred while connecting to the server : " << GetLastError() << endl; 
		return 1;
	}
	else
	{
		cout << "CreateFile() was successful." << endl;
	}


	char szBuffer[BUFFER_SIZE];

	cout << "Enter a message to be sent to the server : " << endl;
	gets(szBuffer);

	DWORD cbBytes;


	BOOL bResult = WriteFile(
		hMailslot,
		szBuffer,
		strlen(szBuffer) + 1,
		&cbBytes,
		NULL);

	if ((!bResult) || (strlen(szBuffer) + 1 != cbBytes))
	{
		cout << "Error occurred while writing to the server : " << GetLastError() << endl;
		CloseHandle(hMailslot);
		return 1;
	}
	else
	{
		cout << "WriteFile() was successful." << endl;
	}

	CloseHandle(hMailslot);

	return 0;
}