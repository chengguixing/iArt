#define BOOST_DATE_TIME_SOURCE
#include <boost/date_time/gregorian/gregorian.hpp>
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;
using namespace boost;
using namespace boost::gregorian;

int main() 
{
	date d1;
	date d2(2010, 1, 1);
	date d3(2000, Jan, 1);
	date d4(d2);

	assert(d1 == date(not_a_date_time));
	assert(d2 == d4);
	assert(d3 < d4);



	system("pause");
	return 0;
} 