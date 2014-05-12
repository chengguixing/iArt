#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;


class NotVirtual
{

};

class Virtual
{
public:
	virtual void foo()
	{
		
	}
};

template <typename VBase>
class Base : private VBase
{
public:
	void foo()
	{
		cout << "Base::foo()" << endl;
	}
};


template <typename V>
class Derived : public Base<V>
{
public:
	void foo()
	{
		cout << "Derived::foo()" << endl;
	}
};

int main(void)
{
	Base<NotVirtual>* p1 = new Derived<NotVirtual>;
	p1->foo();

	Base<Virtual>* p2 = new Derived<Virtual>;
	p2->foo();

	system("pause");
	return 0;
}