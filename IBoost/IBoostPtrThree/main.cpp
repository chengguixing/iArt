#include <boost/shared_ptr.hpp> 
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;
using namespace boost;


class shared
{
private:
	boost::shared_ptr<int> p;
public:
	shared(boost::shared_ptr<int> p_) : p(p_)
	{
		;
	}
	void print()
	{
		cout << "count : " << p.use_count() << " v = " << *p << endl;
	}
};

void print_func(boost::shared_ptr<int> p)
{
	cout << "count : " << p.use_count() << " v = " << *p << endl;
}


int main(void) 
{ 
	{
		boost::shared_ptr<int> p(new int(100));
		cout << p.use_count() << endl;
		shared s1(p), s2(p);

		s1.print();
		s2.print();

		*p = 20;
		print_func(p);

		s1.print();
		s1.print();
	}
	system("pause");
	return 0;
} 