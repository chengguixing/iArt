#include <stdlib.h>
#include <iostream>
using namespace std;

class B1
{
public:
	B1() : b1(0)
	{
		;
	}
private:
	int b1;
public:
	void mf1();
};

void B1::mf1()
{
	cout << "b1 = " << b1 << endl;
}

class B2
{
public:
	B2() : b2(0)
	{
		;
	}
private:
	int b2;
public:
	void mf2();
};

void B2::mf2()
{
	cout << "b2 = " << b2 << endl;
}

class D : public B1, public B2
{
public:
	D() : d(0)
	{
		;
	}
private:
	int d;
};

void call_memfun(D obj, void(D::*pmf)())
{
	(obj.*pmf)();
}

int main(void)
{
	D obj;
	//obj.mf1();
	//obj.mf2();
	call_memfun(obj, &D::mf1);
	call_memfun(obj, &D::mf2);

	system("pause");
	return 0;
}