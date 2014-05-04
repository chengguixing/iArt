#include <boost/weak_ptr.hpp> 
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;

int main() 
{ 
	shared_ptr<int> sp(new int(10));
	assert(sp.use_count() == 1);

	weak_ptr<int> wp(sp);
	assert(wp.use_count() == 1);


	if (!wp.expired())
	{
		shared_ptr<int> sp2 = wp.lock();
		*sp2 = 100;
		assert(wp.use_count() == 2);
	}

	assert(wp.use_count() == 1);
	sp.reset();
	assert(wp.expired());
	assert(!wp.lock());


	system("pause");
	return 0;
} 