#include <stdlib.h>
#include <iostream>
using namespace std;

template <typename I, class T>
void func_impl(I iter, T t)
{
	cout << "Enter " << __FUNCTION__ << endl;
	cout << typeid(iter).name() << endl
		 << typeid(T).name() << endl;
	T temp = 666;
	cout << temp << endl;

}

template <typename I>
inline void func(I iter)
{
	cout << "Enter " << __FUNCTION__ << endl;
	cout << typeid(iter).name() << endl
		 << typeid(*iter).name() << endl;
	func_impl(iter, *iter);
}


int main() 
{ 
	cout << "Enter " << __FUNCTION__ << endl;
	int i;
	func(&i);


	system("pause");
	return 0;
} 