#include <stdlib.h>
#include <iostream>
using namespace std;
/*
class SomeType;


class MyCriterion
{
public:
	virtual bool operator() (SomeType const&, SomeType const&) const = 0;
};


class LessThan : public MyCriterion
{
public:
	virtual bool operator() (SomeType const&, SomeType const&) const
	{
		return true;
	}
};

template <MyCriterion& F>
void sort();


LessThan order;

//sort<order>();

//sort<(MyCriterion&)order>();

*/
class SomeType;

class MyCriterion
{
public:
	bool operator() (SomeType const&, SomeType const&)const;
};

//emplate <MyCriterion F>
//void my_sort();


int main(void)
{

	system("pause");
	return 0;
}