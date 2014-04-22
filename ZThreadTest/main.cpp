/*
#include <windows.h>
#include <iostream>
#include "time.h"
using namespace std;

int main(void)
{
	Time time;
	cout << time.seconds() << endl;
	cout << time.milliseconds() << endl;


	Sleep(5500);


	Time time1;
	cout << time1.seconds() << endl;
	cout << time1.milliseconds() << endl;



	Sleep(11100);


	Time time2;
	cout << time2.seconds() << endl;
	cout << time2.milliseconds() << endl;
	



	system("pause");
	return 0;
}
*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

int callee()
{
	printf("Callee.\n");
	return 0;
}

int caller()
{
	static int i = (callee(),1);
	printf("Caller.\n");
	return 0;
}

void foo()
{
	;
}

int main(void)
{
	cout << _MSC_VER << endl;

	cout << foo << endl;
	cout << &foo << endl;
	cout << *foo << endl;

	printf("Main.\n");
	caller();
	caller();
	caller();

	system("pause");
	return 0;
}
