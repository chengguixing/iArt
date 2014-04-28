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


	system("pause");
	return 0;
} 