#include <stdlib.h>
#include <iostream>
using namespace std;



template <class T>
class B1
{
public: 
	void SayHi() 
	{
		// 此处是关键技巧。
		T* pT = static_cast<T*>(this);   
		pT->PrintClassName();
	}
	void PrintClassName() 
	{ 
		cout << "This is B1" << endl; 
	}
};



class D1 : public B1<D1>
{
	// No overridden functions at all
};



class D2 : public B1<D2>
{
public:
	void PrintClassName() 
	{ 
		cout << "This is D2" << endl; 
	}
};



int main(void)
{
	D1 d1;
	D2 d2;

	d1.SayHi();    // prints "This is B1"
	d2.SayHi();    // prints "This is D2"

	system("pause");
	return 0;
}
