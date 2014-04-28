#include "noncopyable.h"
#include <stdlib.h>
#include <iostream>
using namespace std;
using namespace IAF::IBase;

class Derived : public noncopyable
{
	;
};

int main(void)
{
	Derived a;
	//Derived b(a);
	//Derived c = a;
	system("pause");
	return 0;
}