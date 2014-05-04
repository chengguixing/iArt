#include <boost/make_shared.hpp> 
#include <iostream> 
#include <stdlib.h>
#include <iostream>
#include <vector>
using namespace std;

int main() 
{ 
	typedef vector<shared_ptr<int> > vs;
	vs v(10);

	int i = 0;
	for (vs::iterator pos = v.begin(); pos != v.end(); ++pos)
	{
		(*pos) = make_shared<int>(++i);
		cout << *(*pos) << ", ";
	}
	cout << endl;

	shared_ptr<int> p = v[9];
	*p = 100;
	cout << *v[9] << endl;

	system("pause");
	return 0;
} 