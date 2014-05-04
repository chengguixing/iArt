#include <iostream> 
#include <stdlib.h>
#include <iostream>
#include <boost/smart_ptr.hpp>
using namespace std;

class node
{
public:
	~node()
	{
		cout << "deleted" << endl;
	}
	int x;
	typedef shared_ptr<node> ptr_type;
	ptr_type next;
};


int main(void) 
{
	{
	auto p1 = make_shared<node>();
	auto p2 = make_shared<node>();

	p1->next = p2;
	p2->next = p1;

	assert(p1.use_count() == 2);
	assert(p2.use_count() == 2);
	}
	system("pause");
	return 0;
} 