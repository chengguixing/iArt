#include <windows.h>
#include <stdlib.h>
#include <iostream>
#include <tchar.h>
using namespace std;

#define g_szPipeName _T("\\\\.\\pipe\\iNamedPipe")

#define BUFFER_SIZE 1024 //1k


int main(void)
{
	HANDLE hPipe;

	//Connect to the server pipe using CreateFile()
	hPipe = CreateFile(
		g_szPipeName,
		GENERIC_READ | 
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
		);

	if (INVALID_HANDLE_VALUE == hPipe)
	{
		cout << "Error occurred while connecting to the server : " << GetLastError() << endl;
		return 1;
	}
	else
	{
		cout << "CreateFile() was successful." << endl;
	}


	//We are done connecting to the server pipe,
	//we can start communicating with the server using ReadFile() / WriteFile()
	//on handle - hPipe

	char szBuffer[BUFFER_SIZE];

	cout << "Enter a message to be sent to the server :";
	gets(szBuffer);

	DWORD cbBytes;


	BOOL bResult = WriteFile(
		hPipe,
		szBuffer,
		strlen(szBuffer) + 1,
		&cbBytes,
		NULL);

	if ((!bResult) || (strlen(szBuffer) + 1) != cbBytes)
	{
		cout << "Error occurred while writing to the server : " << GetLastError() << endl;
		CloseHandle(hPipe);
		return 1;
	}
	else
	{
		cout << "WriteFile() was successful." << endl;
	}

	//Read server response
	bResult = ReadFile(
		hPipe,
		szBuffer,
		sizeof(szBuffer),
		&cbBytes,
		NULL
		);

	if ((!bResult) || (0 == cbBytes))
	{
		cout << "Error occurred while reading from the server : " << GetLastError() << endl;
		CloseHandle(hPipe);
		return 1;
	}
	else
	{
		cout << "ReadFile() was successful." << endl;
	}

	cout << "Server sent the following message : " << szBuffer << endl;


	CloseHandle(hPipe);
	system("pause");
	return 0;
}