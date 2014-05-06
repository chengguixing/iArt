#include <stdio.h>
#include <stdlib.h>
#include "sqrt1.h"
#include <iostream>
using namespace std;


int main(void)
{
	cout << Sqrt<16>::result << endl;
	cout << Sqrt<25>::result << endl;
	cout << Sqrt<42>::result << endl;
	cout << Sqrt<1>::result << endl;

	system("pause");
	return 0;
}