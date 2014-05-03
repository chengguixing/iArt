#include <iostream>
using namespace std;


struct B{};
struct D1 : public B{};
struct D2 : public B{};


template <typename I>
void func(I& p, B)
{
	cout << "B version" << endl;
}

template <typename I>
void func(I& p, D2)
{
	cout << "D2 version" << endl;
}


int main(void)
{
	int* p;
	func(p, B());
	func(p, D1());
	func(p, D2());

	system("pause");
	return 0;
}