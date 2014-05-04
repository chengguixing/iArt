#include <boost/make_shared.hpp> 
#include <iostream> 
#include <stdlib.h>
#include <iostream>
#include <vector>
using namespace std;

int main() 
{ 
	shared_ptr<string> sp = make_shared<string>("make_shared");
	shared_ptr<vector<int> > spv = make_shared<vector<int> >(10, 2);
	assert(spv->size() == 10);


	system("pause");
	return 0;
} 