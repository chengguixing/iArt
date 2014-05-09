#include <stdio.h>
#include <stdlib.h>
#include "accum1.h"
#include <iostream>
using namespace std;



class MultiPolicy
{
public:
	template <typename T1, typename T2>
	static void accumulate(T1& total, T2 const& value)
	{
		total *= value;
	}
};


int main(void)
{
	int num[] = {1, 2, 3, 4, 5};
	cout << Accum<int, MultiPolicy>::accum(&num[0], &num[5]) << endl;



	system("pause");
	return 0;
}