#include <stdlib.h>
#include <iostream>
using namespace std;


class ConstantIntFunctor
{
private:
	int value;
public:
	ConstantIntFunctor(int c) : value(c)
	{
		;
	}
	int operator()() const
	{
		return value;
	}
};

void client(ConstantIntFunctor const& cif)
{
	cout << "calling back functor yields " << cif() << endl;
}


int main(void)
{
	ConstantIntFunctor seven(7);
	ConstantIntFunctor fortytwo(42);
	client(seven);
	client(fortytwo);


	system("pause");
	return 0;
}