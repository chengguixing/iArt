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