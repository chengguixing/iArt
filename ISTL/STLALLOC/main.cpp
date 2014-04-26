#include "IAlloc.h"
#include "IDefineClass.h"
#include <stdlib.h>
#include <vector>
#include <iostream>
using namespace std;

int main(void)
{
	int ia[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	vector<int, ISTL::iAllocator<int> > iv(ia, ia + 10);

	for (unsigned int i = 0; i < iv.size(); ++i)
	{
		cout << iv[i] << ' ';
	}
	cout << endl;


	std::string str[10] = {
		"i love you",
		"izhuxin",
		"andy",
		"liudehua",
		"IT",
		"iArt",
		"iWindows",
		"iLinux",
		"iMac",
		"iDea"
	};

	vector<DefineClass, ISTL::iAllocator<DefineClass> > sv(str, str + 10);

	
	for (unsigned int i = 0; i < sv.size(); ++i)
	{
		cout << sv[i].GetString() << endl;
	}


	system("pause");
	return 0;
}