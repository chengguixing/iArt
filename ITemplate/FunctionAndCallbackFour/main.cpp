#include <stdlib.h>
#include <set>
#include <iostream>
using namespace std;

class Person
{
	;
};

class PersonSortCriterion
{
public:
	bool operator() (Person const& p1, Person const& p2) const
	{
		;
	}
};

namespace IAF
{
template <typename T>
class less
{
public:
	bool operator() (T const& x, T const& y) const
	{
		return x < y;
	}
};

template <typename T>
class greater
{
public:
	bool operator() (T const& x, T const& y) const
	{
		return x > y;
	}
};
}

void foo()
{
	set<Person, IAF::less<Person> > c0, c1;
	set<Person, IAF::greater<Person> > c2;
	set<Person, PersonSortCriterion> c3;

	c0 = c1;
	//c1 = c2;
	//if (c1 == c3)
	//{
	//	;
	//}
}

int main(void)
{

	system("pause");
	return 0;
}