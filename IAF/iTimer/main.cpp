#include "iTimer.h"
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;
using namespace IAF::ITime;

int main() 
{ 
	iTimer t;

	Sleep(1000);

	cout << t.elapse() << "us" << endl;

	iTimer t1;
	int wi = 0;
	while (true)
	{
		++wi;
		if (100000000 == wi)
		{
			break;
		}
	}
	cout << t1.elapse() << "us" << endl;

	iTimer t2;
	int fi = 0;
	for (;;)
	{
		++fi;
		if (100000000 == fi)
		{
			break;
		}
	}
	cout << t2.elapse() << "us" << endl;

	system("pause");
	return 0;
} 