#include <boost/optional.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <stdlib.h>
#include <vector>
#include <iostream>
using namespace std;
using namespace boost;

optional<double> calc(int x)
{
	return optional<double>(x != 0, 1.0 / x);
}

optional<double> sqrt_op(double x)
{
	return optional<double>(x > 0, sqrt(x));
}

int main() 
{
	optional<double> d = calc(10);
	if (d)
	{
		cout << *d << endl;
	}
	d = sqrt_op(-10);
	if (!d)
	{
		cout << "no result" << endl;
	}
	optional<int> op0;
	optional<int> op1(none);

	assert(!op0);
	assert(op0 == op1);
	assert(op1.get_value_or(253) == 253);

	//optional<string> ops("test");
	//cout << ops << endl;

	vector<int> v(10);
	optional<vector<int>& > opv(v);
	assert(opv);

	opv->push_back(5);
	assert(opv->size() == 11);

	opv = none;
	assert(!opv);
	auto x = make_optional(5);
	assert(*x == 5);

	auto y = make_optional<double>((*x > 10), 1.0);
	assert(!y);

	optional<string> ops(in_place("test in_place_factory"));
	//cout << *ops;

	optional<vector<int> > opp(in_place(10, 3));
	assert(opp->size() == 10);
	assert((*opp)[0] == 3);

	system("pause");
	return 0;
} 