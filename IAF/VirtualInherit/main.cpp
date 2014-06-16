#include <stdlib.h>
#include <iostream>
using namespace std;

class a
{
	virtual void func()
	{
		;
	}
};
class b : public virtual a
{
	virtual void foo()
	{
		;
	}
};

class a1
{
	virtual void func()
	{
		;
	}
};
class b1 : public a1
{
	virtual void foo()
	{
		;
	}
};

class a2
{
	virtual void func()
	{
		;
	}
	char x;
};
class b2 : public virtual a2
{
	virtual void foo()
	{
		;
	}
};


class a3
{
	virtual void func()
	{
		;
	}
	char x;
};
class b3 : public a3
{
	virtual void foo()
	{
		;
	}
};




int main(void) 
{ 
	cout << sizeof(a) << endl;
	cout << sizeof(b) << endl;

	b ib;

	cout << sizeof(a1) << endl;
	cout << sizeof(b1) << endl;

	b1 ib1;

	cout << sizeof(a2) << endl;
	cout << sizeof(b2) << endl;

	b2 ib2;

	cout << sizeof(a3) << endl;
	cout << sizeof(b3) << endl;

	b3 ib3;

	system("pause");
	return 0;
} 