#include <stdlib.h>
#include <vector>
#include <iostream>
using namespace std;

extern "C" typedef int (*C_int_FP)();

template <int (*FP)()>
class FunctionReturningIntWrapper
{
public:
	int operator() ()
	{
		return FP();
	}
};



int random_int()
{
	return std::rand();
}


template <typename FO>
//template <int (*FP)()>
void initialize(std::vector<int>& coll)
{
	FO fo;
	for (std::vector<int>::size_type i = 0; i < coll.size(); ++i)
	{
		coll[i] = fo();
	}
}
int main(void)
{
	std::vector<int> v(10);
	initialize<FunctionReturningIntWrapper<std::rand> >(v);
	//initialize<std::rand>(v);

	for (std::vector<int>::size_type i = 0; i < v.size(); ++i)
	{
		std::cout << "coll[" << i << "] : " << v[i] << endl; 
	}

	system("pause");
	return 0;
}