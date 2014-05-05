#include <boost/optional.hpp>
#include <stdlib.h>
#include <vector>
#include <iostream>
using namespace std;
using namespace boost;

int main() 
{ 
	optional<int> op0;
	optional<int> op1(none);

	assert(!op0);
	assert(op0 == op1);
	assert(op1.get_value_or(253) == 253);

	optional<string> ops("test");
	cout << ops << endl;

	vector<int> v(10);
	optional<vector<int>& > opv(v);
	assert(opv);

	opv->push_back(5);
	assert(opv->size() == 11);

	opv = none;
	assert(!opv);

	system("pause");
	return 0;
} 