#include <boost/smart_ptr.hpp> 
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;

int main() 
{ 
	shared_ptr<int> sp(new int(10));
	assert(sp.unique());

	shared_ptr<int> sp2 = sp;
	assert(sp == sp2 && sp.use_count() == 2);

	*sp2 = 100;
	assert(*sp == 100);

	sp.reset();
	assert(!sp);



	system("pause");
	return 0;
} 