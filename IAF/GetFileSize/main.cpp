#include <stdlib.h>
#include <iostream>
#include "File.h"
using namespace std;
using namespace IAF;



int main() 
{ 
	cout << iFile::GetFileSize("D:\\Ѹ������\\ϲ��֮������DVD����.rmvb") << endl;
	cout << iFile::GetFileSize("D:\\Ѹ������\\[iPlaySoft.com]VS2013_RTM_ULT_CHS.iso") << endl;

	system("pause");
	return 0;
} 