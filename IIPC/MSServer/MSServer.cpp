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

	hMailslot = CreateMailslot(
		g_szMailslot,
		BUFFER_SIZE,
		MAILSLOT_WAIT_FOREVER,
		NULL);

	if (INVALID_HANDLE_VALUE == hMailslot)
	{
		cout << "Error occurred while creating the mailslot : " << GetLastError() << endl;
		return 1;
	}
	else
	{
		cout << "CreateMailslot() was successful." << endl;
	}

	char szBuffer[BUFFER_SIZE];
	DWORD cbBytes;
	BOOL bResult;

	cout << "Waiting for client connection..." << endl;


	//Infinite, till user terminates the console app
	while (1)
	{
		bResult = ReadFile(
			hMailslot,
			szBuffer,
			sizeof(szBuffer),
			&cbBytes,
			NULL);

		if ((!bResult) || (0 == cbBytes))
		{
			cout << "Error occured while reading from the client : " << GetLastError() << endl;
			CloseHandle(hMailslot);
			return 1;
		}
		else
		{
			cout << "ReadFile() was successful." << endl;
		}

		cout << "Client sent the following message : " << szBuffer << endl;
	}


	CloseHandle(hMailslot);

	system("pause");
	return 0;
}