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
	iDirectory::DeleteFileOrDirectory("e:\\ÎÄ¼þ¼Ð\\");
	iDirectory::DeleteFileOrDirectory("e:\\ÖìöÎ.txt");

	system("pause");
	return 0;
} 