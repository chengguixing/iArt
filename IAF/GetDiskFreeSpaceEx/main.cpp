/*
#include <windows.h>
#include <stdlib.h>
#include <iostream>
#include <string>
using namespace std;

int main(void)
{
	wstring strname = L"E:\\X\\";
	ULARGE_INTEGER nTotalNumberOfBytes;
	ULARGE_INTEGER nTotalNumberOfFreeBytes;

	GetDiskFreeSpaceEx(strname.c_str(), NULL, (PULARGE_INTEGER)&nTotalNumberOfBytes, (PULARGE_INTEGER)&nTotalNumberOfFreeBytes );

	cout << (nTotalNumberOfBytes.QuadPart >> 30) << endl;
	cout << (nTotalNumberOfFreeBytes.QuadPart >> 30) << endl;

	system("pause");
	return 0;
}
*/
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
using namespace std;


int main(void)
{
	/*
    BOOL bFlag;
    TCHAR Buf[256];    

    bFlag = GetVolumeNameForVolumeMountPoint(
        _T("E:\\X\\"),
        Buf,
        256
        );

    if (bFlag != TRUE) 
    {
        return (-2);
    }
	else
	{
		printf("%s\n", Buf);
		ULARGE_INTEGER nTotalNumberOfBytes;
		ULARGE_INTEGER nTotalNumberOfFreeBytes;

		GetDiskFreeSpaceEx(Buf, NULL, (PULARGE_INTEGER)&nTotalNumberOfBytes, (PULARGE_INTEGER)&nTotalNumberOfFreeBytes );

		cout << (nTotalNumberOfBytes.QuadPart >> 30) << endl;
		cout << (nTotalNumberOfFreeBytes.QuadPart >> 30) << endl;
	}
	*/
	ULARGE_INTEGER nTotalNumberOfBytes;
	ULARGE_INTEGER nTotalNumberOfFreeBytes;

	GetDiskFreeSpaceEx(L"E:\\X\\", NULL, (PULARGE_INTEGER)&nTotalNumberOfBytes, (PULARGE_INTEGER)&nTotalNumberOfFreeBytes );
	signed __int64 DiskFreeSpace = nTotalNumberOfFreeBytes.QuadPart;
	cout << nTotalNumberOfFreeBytes.QuadPart << endl;
	cout << nTotalNumberOfBytes.QuadPart << endl;
	cout << DiskFreeSpace << endl;
	
	system("pause");
    return 0;
}