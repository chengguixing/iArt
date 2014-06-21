#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;


class EmptyClass
{
	;
};
struct EmptyStruct
{
	;
};


int main(void)
{
	EmptyClass ec;
	EmptyStruct es;

	cout << sizeof(ec) << endl;
	cout << sizeof(es) << endl;

	cout << sizeof(EmptyClass) << endl;
	cout << sizeof(EmptyStruct) << endl;

	system("pause");
	return 0;
}