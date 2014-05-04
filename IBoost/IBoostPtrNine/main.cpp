#include <boost/shared_array.hpp> 
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;
using namespace boost;

int main(void) 
{
	int* p = new int[100];
	shared_array<int> sa(p);
	shared_array<int> sa2 = sa;

	sa[0] = 10;
	assert(sa2[0] == 10);

	system("pause");
	return 0;
} 