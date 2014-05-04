#include <boost/shared_ptr.hpp> 
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;

void any_func(void* p)
{
	cout << "some operate" << endl;
}

int main() 
{
	{
		shared_ptr<void> p((void*)0, any_func);
	}
	system("pause");
	return 0;
} 