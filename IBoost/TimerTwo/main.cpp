#include <boost/progress.hpp>
#include <iostream> 
#include <stdlib.h>
#include <iostream>
#include <sstream>
using namespace std;
using namespace boost;

int main() 
{ 
	stringstream ss;
	{
		progress_timer t(ss);

		for (int i = 0; i < 100000000; ++i)
		{
			;
		}
	}
	cout << ss.str();


	system("pause");
	return 0;
} 