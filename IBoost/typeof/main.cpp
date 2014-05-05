#include <boost/typeof/typeof.hpp>
#include <stdlib.h>
#include <vector>
#include <iostream>
using namespace std;


#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

namespace ex
{
	struct demo_class{int a, b;};
}

BOOST_TYPEOF_REGISTER_TYPE(ex::demo_class)



vector<string> func()
{
	vector<string> v(10);
	return std::move(v);
}

int main() 
{ 
	//BOOST_TYPEOF(2.0 * 3) x = 2.0 * 3;
	BOOST_AUTO(y, 2+ 3);
	
	BOOST_AUTO(p, make_pair(1, "string"));

	BOOST_AUTO(v, func());

	BOOST_AUTO(x, make_pair("test", ex::demo_class()));
	cout << typeid(x).name() <<endl;

	x.second.a = 10;
	x.second.b = 20;

	cout << x.second.a << endl << x.second.b << endl;


	system("pause");
	return 0;
} 