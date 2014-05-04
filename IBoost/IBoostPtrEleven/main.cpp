#include <boost/enable_shared_from_this.hpp> 
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;
using namespace boost;

class self_shared: public boost::enable_shared_from_this<self_shared>
{
public:
	self_shared(int n) : x(n)
	{
		NULL;
	}
	int x;
	void print()
	{
		cout << "self_shared : " << x << endl;
	}
};


int main() 
{ 
	boost::shared_ptr<self_shared> sp = boost::make_shared<self_shared>(314);
	sp->print();

	boost::shared_ptr<self_shared> p = sp->shared_from_this();

	p->x = 1000;
	p->print();

	system("pause");
	return 0;
} 