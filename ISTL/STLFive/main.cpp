#include <iostream>
#include <stdlib.h>
using namespace std;

template <class T>
struct Plus
{
	T operator()(const T& x, const T& y) const
	{
		return x + y;
	}
};

template <class T>
struct Minus
{
	T operator()(const T& x, const T& y) const
	{
		return x - y;
	}
};

int main(void)
{
	Plus<int> PlusObj;
	Minus<int> MinusObj;

	cout << PlusObj(3, 5) << endl;
	cout << MinusObj(3, 5) << endl;


	cout << Plus<int>()(43, 50) << endl;
	cout << Minus<int>()(43, 50) << endl;

	system("pause");
	return 0;
}