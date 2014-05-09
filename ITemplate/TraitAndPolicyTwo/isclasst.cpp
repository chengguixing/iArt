#include "isclasst.h"

class MyClass
{
	;
}
;

struct MyStruct
{
	;
};

union MyUnion
{
	;
};

void MyFunc()
{
	;
}

enum E{e1}e;

template <typename T>
void check()
{
	if (IsClassT<T>::Yes)
	{
		cout << "IsClassT" << endl;
	}
	else
	{
		cout << "!IsClassT" << endl;
	}
}

template <typename T>
void checkT(T)
{
	check<T>();
}


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

int main(void)
{
	cout << "int: ";
	check<int>();

	cout << "MyClass: ";
	check<MyClass>();


	cout << "MyStruct: ";
	MyStruct s;
	checkT(s);


	cout << "MyUnion: ";
	check<MyUnion>();


	cout << "enum: ";
	checkT(e);


	cout << "MyFunc: ";
	checkT(MyFunc);

	system("pause");
	return 0;
}