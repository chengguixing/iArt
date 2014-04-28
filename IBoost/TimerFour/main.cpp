#include <boost/progress.hpp>
#include <iostream> 
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <windows.h>
#include <iostream>
using namespace std;
using namespace boost;

int main() 
{ 
	vector<int> v(100, 666);
	ofstream fs("./test.txt");

	progress_display pd(v.size());

	for (auto& x : v)
	{
		fs << x << endl;
		//Sleep(1000);
		++pd;
	}

	system("pause");
	return 0;
} 