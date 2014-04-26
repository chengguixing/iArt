#include <iostream>
#include <stdlib.h>
using namespace std;


class INT
{
	friend ostream& operator<< (ostream& os, const INT& i);
public:
	INT(int i) : m_i(i)
	{
		;
	}
	const INT operator++ (int)
	{
		INT temp = *this;
		++(*this);
		return temp;
	}
	INT& operator++ ()
	{
		++(this->m_i);
		return *this;
	}
	const INT operator-- (int)
	{
		INT temp = *this;
		--(*this);
		return temp;
	}
	INT& operator-- ()
	{
		--(this->m_i);
		return *this;
	}
	int& operator* () const
	{
		return (int&)m_i;
	}
private:
	int m_i;
};


ostream& operator<< (ostream& os, const INT& i)
{
	os << '[' << i.m_i << ']';
	return os;
}

int main(void)
{
	INT I(5);
	cout << I++;
	cout << ++I;
	cout << I--;
	cout << --I;
	cout << *I;

	system("pause");
	return 0;
}