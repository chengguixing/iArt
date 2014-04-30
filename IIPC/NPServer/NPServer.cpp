#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#define g_szPipeName _T("\\\\.\\Pipe\\iNamedPipe")

#define BUFFER_SIZE 1024

#define ACK_MESG_RECV "Message received successfully"

int main(void)
{
	HANDLE hPipe;

	hPipe = CreateNamedPipe( 
		g_szPipeName,             // pipe name 
		PIPE_ACCESS_DUPLEX,       // read/write access 
		PIPE_TYPE_MESSAGE |       // message type pipe 
		PIPE_READMODE_MESSAGE |   // message-read mode 
		PIPE_WAIT,                // blocking mode 
		PIPE_UNLIMITED_INSTANCES, // max. instances  
		BUFFER_SIZE,              // output buffer size 
		BUFFER_SIZE,              // input buffer size 
		NMPWAIT_USE_DEFAULT_WAIT, // client time-out 
		NULL);                    // default security attribute 

	if (INVALID_HANDLE_VALUE == hPipe) 
	{
		cout << "Error occurred while creating the pipe: " << GetLastError() << endl; 
		return 1;  //Error
	}
	else
	{
		cout << "CreateNamedPipe() was successful." << endl;
	}

	cout << "Waiting for client connection..." << endl;

	//Wait for the client to connect
	BOOL bClientConnected = ConnectNamedPipe(hPipe, NULL);

	if (FALSE == bClientConnected)
	{
		cout << "Error occurred while connecting to the client: " << GetLastError() << endl; 
		CloseHandle(hPipe);
		return 1;  //Error
	}
	else
	{
		cout << "ConnectNamedPipe() was successful." << endl;
	}

	char szBuffer[BUFFER_SIZE];
	DWORD cbBytes;

	//We are connected to the client.
	//To communicate with the client we will use ReadFile()/WriteFile() 
	//on the pipe handle - hPipe

	//Read client message
	BOOL bResult = ReadFile( 
		hPipe,                // handle to pipe 
		szBuffer,             // buffer to receive data 
		sizeof(szBuffer),     // size of buffer 
		&cbBytes,             // number of bytes read 
		NULL);                // not overlapped I/O 

	if ( (!bResult) || (0 == cbBytes)) 
	{
		cout << "Error occurred while reading from the client: " << GetLastError() << endl; 
		CloseHandle(hPipe);
		return 1;  //Error
	}
	else
	{
		cout << "ReadFile() was successful." << endl;
	}

	cout << "Client sent the following message: " << szBuffer << endl;

	strcpy(szBuffer, ACK_MESG_RECV);

	//Reply to client
	bResult = WriteFile( 
		hPipe,                // handle to pipe 
		szBuffer,             // buffer to write from 
		strlen(szBuffer) + 1,   // number of bytes to write, include the NULL 
		&cbBytes,             // number of bytes written 
		NULL);                // not overlapped I/O 

	if ( (!bResult) || (strlen(szBuffer) + 1 != cbBytes))
	{
		cout << "Error occurred while writing to the client: " << GetLastError() << endl; 
		CloseHandle(hPipe);
		return 1;  //Error
	}
	else
	{
		cout << "WriteFile() was successful." << endl;
	}

	CloseHandle(hPipe);
	return 0; //Success
}