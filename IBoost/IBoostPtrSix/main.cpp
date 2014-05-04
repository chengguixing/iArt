#include <boost/shared_ptr.hpp> 
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;
using namespace boost;

class sample
{
private:
	class impl;
	boost::shared_ptr<impl> p;
public:
	sample();
	void print();
};

class sample::impl
{
public:
	void print()
	{
		cout << "impl print" << endl;
	}
};
sample::sample():p(new impl)
{
}

void sample::print()
{
	p->print();
}

int main() 
{ 
	sample s;
	s.print();

	system("pause");
	return 0;
} 