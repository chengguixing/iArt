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
	system("pause");
	return 0;
} 