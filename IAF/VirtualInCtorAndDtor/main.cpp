#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

class A
{
public:
	A()
	{
		go();
	}
	virtual ~A()
	{
		out();
	}
public:
	virtual void print()
	{
		OutputDebugString(L"A print\n");
	}
	virtual void go()
	{
		OutputDebugString(L"I'm A\n");
	}
	virtual void out()
	{
		OutputDebugString(L"Bye A~\n");
	}
};

class B : public A
{
public:
	B()
	{
		;
	}
	~B()
	{
		;
	}
	virtual void print()
	{
		OutputDebugString(L"B print\n");
	}
	virtual void go()
	{
		OutputDebugString(L"I'm B\n");
	}
	virtual void out()
	{
		OutputDebugString(L"Bye B~\n");
	}
};


int main(void)
{
	A* a = new B;
	a->print();
	delete a;

	/*
		I'm A
		B print
		Bye A~
	*/

	system("pause");
	return 0;
}