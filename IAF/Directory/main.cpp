#include <stdlib.h>
#include <iostream>
#include "Directory.h"
using namespace std;
using namespace IAF;



int main() 
{ 

	wstring wstr;
	iDirectory::GetRootDirectory(wstr);
	wcout.imbue(locale("chs"));
	wcout << L"[" << wstr.c_str() << L"]" << endl;


	iDirectory::DeleteFileOrDirectory("e:\\temp\\");
	iDirectory::DeleteFileOrDirectory("e:\\�ļ���\\");
	iDirectory::DeleteFileOrDirectory("e:\\����.txt");

	system("pause");
	return 0;
} 