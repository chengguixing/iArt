#include<windows.h>
#include<wininet.h>
#include<iostream>
using namespace std;

#pragma comment(lib, "wininet.lib")


DWORD dwNumKSent;
DWORD dwNumKToSend;
DWORD dwNumBytesComplete = 0;
char lpOutBuf[1024];
HANDLE hConnectedEvent, hRequestCompleteEvent;
HINTERNET hInstance, hConnect, hRequest;
char *lpszUrl, *lpszServer;
BOOL bAllDone = FALSE;
void __stdcall Callback(HINTERNET hInternet,
						DWORD dwContext,
						DWORD dwInternetStatus,
						LPVOID lpStatusInfo,
						DWORD dwStatusInfoLen);
int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		cout << "Usage: sendreqexasync <server> <url> <size in kilobytes>" << endl;
		cout << "   Example: sendreqexasync www.foo.com /postfolder/upload.exe 256" << endl;
		return 1;
	}
	lpszServer = argv[1];
	lpszUrl = argv[2];
	dwNumKToSend = atoi(argv[3]);
	FillMemory(lpOutBuf, 1024, 'A');
	hConnectedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hRequestCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hInstance = InternetOpen("sendreqexasync", 
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		INTERNET_FLAG_ASYNC);
	if (hInstance == NULL)
	{
		cout << "InternetOpen failed, error " << GetLastError();
		return 2;
	}
	if (InternetSetStatusCallback(hInstance,
		(INTERNET_STATUS_CALLBACK)&Callback) == INTERNET_INVALID_STATUS_CALLBACK)
	{
		cout << "InternetSetStatusCallback failed, error " << GetLastError();
		return 3;
	}
	hConnect = InternetConnect(hInstance, 
		lpszServer, 
		INTERNET_DEFAULT_HTTP_PORT,
		NULL,
		NULL,
		INTERNET_SERVICE_HTTP,
		0,
		1);
	if (hConnect == NULL)
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			cout << "InternetConnect failed, error " << GetLastError();
			return 4;
		}
		WaitForSingleObject(hConnectedEvent, INFINITE);
	}
	hRequest = HttpOpenRequest(hConnect, 
		"POST", 
		lpszUrl,
		NULL,
		NULL,
		NULL,
		INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE,
		2);
	if (hRequest == NULL)
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			cout << "HttpOpenRequest failed, error " << GetLastError();
			return 5;
		}
		WaitForSingleObject(hRequestCompleteEvent, INFINITE);
	}
	INTERNET_BUFFERS IntBuff;
	FillMemory(&IntBuff, sizeof(IntBuff), 0);
	IntBuff.dwStructSize= sizeof(IntBuff);
	IntBuff.dwBufferTotal = 1024*dwNumKToSend;
	IntBuff.lpcszHeader = "Content-Type: text/text\r\n";
	IntBuff.dwHeadersLength = lstrlen(IntBuff.lpcszHeader);
	if (!HttpSendRequestEx(hRequest, 
		&IntBuff, 
		NULL, 
		0,
		2))
	{
		if (GetLastError() != ERROR_IO_PENDING)
		{
			cout << "HttpSendRequestEx failed, error " << GetLastError();
			return 6;
		}
		cout << "HttpSendRequestEx called successfully" << endl;
		cout.flush();
		WaitForSingleObject(hRequestCompleteEvent, INFINITE);
	}
	for (dwNumKSent = 0; dwNumKSent < dwNumKToSend; ++dwNumKSent)
	{
		DWORD dwBytesWritten;
		if(!InternetWriteFile(hRequest,
			lpOutBuf,
			1024,
			&dwBytesWritten))
		{
			if (GetLastError() != ERROR_IO_PENDING)
			{
				cout << "InternetWriteFile failed, error " << GetLastError();
				return 7;
			}
			else
			{
				cout << "InternetWriteFile completing asynchronously" << endl;
				cout.flush();
				WaitForSingleObject(hRequestCompleteEvent, INFINITE);
			}
		}
	}
	cout << "Calling HttpEndRequest" << endl;
	cout.flush();
	if (!HttpEndRequest(hRequest, NULL, HSR_INITIATE, 2))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			cout << "HttpEndRequest called" << endl;
			cout.flush();
			WaitForSingleObject(hRequestCompleteEvent, INFINITE);
		}
		else
		{
			cout << "HttpEndRequest failed, error " << GetLastError() << endl;
			return 8;
		}
	}

	cout << "------------------- Read the response -------------------" << endl;
	char lpReadBuff[256];
	do
	{
		INTERNET_BUFFERS InetBuff;
		FillMemory(&InetBuff, sizeof(InetBuff), 0);
		InetBuff.dwStructSize = sizeof(InetBuff);
		InetBuff.lpvBuffer = lpReadBuff;
		InetBuff.dwBufferLength = sizeof(lpReadBuff) - 1;

		cout << "Calling InternetReadFileEx" << endl;
		cout.flush();
		if (!InternetReadFileEx(hRequest,
			&InetBuff,
			0, 2))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				cout << "Waiting for InternetReadFile to complete" << endl;
				cout.flush();
				WaitForSingleObject(hRequestCompleteEvent, INFINITE);
			}
			else
			{
				cout << "InternetReadFileEx failed, error " << GetLastError();
				cout.flush();
				return 9;
			}
		}
		lpReadBuff[InetBuff.dwBufferLength] = 0;
		cout << lpReadBuff;
		cout.flush();
		if (InetBuff.dwBufferLength == 0) 
			bAllDone = TRUE;
	} while (bAllDone == FALSE);
	cout << endl << endl << "------------------- Request Complete ----------------" << endl;


	return 0;
}
void __stdcall Callback(HINTERNET hInternet,
						DWORD dwContext,
						DWORD dwInternetStatus,
						LPVOID lpStatusInfo,
						DWORD dwStatusInfoLen)
{
	cout << "Callback dwInternetStatus: " << dwInternetStatus << " Context: " << dwContext << endl;
	cout.flush();
	switch(dwContext)
	{
	case 1: // Connection handle
		if (dwInternetStatus == INTERNET_STATUS_HANDLE_CREATED)
		{
			INTERNET_ASYNC_RESULT *pRes = (INTERNET_ASYNC_RESULT *)lpStatusInfo;
			hConnect = (HINTERNET)pRes->dwResult;
			cout << "Connect handle created" << endl;
			cout.flush();
			SetEvent(hConnectedEvent);
		}
		break;
	case 2: // Request handle
		switch(dwInternetStatus)
		{
		case INTERNET_STATUS_HANDLE_CREATED:
			{
				INTERNET_ASYNC_RESULT *pRes = (INTERNET_ASYNC_RESULT *)lpStatusInfo;
				hRequest = (HINTERNET)pRes->dwResult;
				cout << "Request handle created" << endl;
				cout.flush();
			}
			break;
		case INTERNET_STATUS_REQUEST_SENT:
			{
				DWORD *lpBytesSent = (DWORD*)lpStatusInfo;
				cout << "Bytes Sent: " << *lpBytesSent << endl;
				dwNumBytesComplete += *lpBytesSent;
			}
			break;
		case INTERNET_STATUS_REQUEST_COMPLETE:
			{
				INTERNET_ASYNC_RESULT *pAsyncRes = (INTERNET_ASYNC_RESULT *)lpStatusInfo;
				cout << "Function call finished" << endl;
				cout << "dwResult: " << pAsyncRes->dwResult << endl;
				cout << "dwError:  " << pAsyncRes->dwError << endl;
				cout.flush();
				SetEvent(hRequestCompleteEvent);
			}
			break;
		case INTERNET_STATUS_RECEIVING_RESPONSE:
			cout << "Receiving Response" << endl;
			cout.flush();
			break;
		case INTERNET_STATUS_RESPONSE_RECEIVED:
			{
				DWORD *dwBytesReceived = (DWORD*)lpStatusInfo;
				cout << "Received " << *dwBytesReceived << endl;
				cout.flush();
			}
		}
	}
}