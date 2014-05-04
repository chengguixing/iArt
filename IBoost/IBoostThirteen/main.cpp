#include <iostream> 
#include <stdlib.h>
#include <iostream>
#include <boost/weak_ptr.hpp>
using namespace std;

class node
{
public:
	typedef weak_ptr<node> ptr_type;
	ptr_type next;
};


int main(void) 
{
	auto p1 = make_shared<node>();
	auto p2 = make_shared<node>();

	p1->next = p2;
	p2->next = p1;

	assert(p1.use_count() == 1);
	assert(p2.use_count() == 1);

	if (!p1->next.expired())
	{
		auto p3 = p1->next.lock();
	}

	system("pause");
	return 0;
} 