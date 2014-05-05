#include <stdlib.h>
#include <typeinfo>
#include <iostream>
using namespace std;


void foo()
{
	cout << "foo() called" << endl;
}

typedef void FooT();


int main(void)
{
	foo();
	cout << "Types of foo: " << typeid(foo).name() << endl;
	cout << "Types of FooT: " << typeid(FooT).name() << endl;


	FooT* pf = foo;
	pf();
	(*pf)();

	cout << "Types of pf: " << typeid(pf).name() << endl;

	FooT& rf = foo;
	rf();

	cout << "Types of rf: " << typeid(rf).name() << endl;

	system("pause");
	return 0;
}