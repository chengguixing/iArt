#include <iostream>
#include <stdlib.h>
#include <vector>
using namespace std;

template <class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value);


template <class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f);

template <typename T>
class Function
{
public:
	void operator()(const T& elem)
	{
		cout << elem << endl;
	}
	
};

int main(void)
{
	double dArray[] = {0, 0.1, 0.01, 0.001, 0.0001};
	vector<double> dv(dArray, dArray + 5);

	vector<double>::iterator it = find(dv.begin(), dv.end(), 0.01);
	if (it != dv.end())
	{
		cout << *it << endl;
	}
	else
	{
		cout << "don't have what you want" << endl;
	}

	for_each(dv.begin(), dv.end(), Function<double>());

	system("pause");
	return 0;
}




template <class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value)
{
	while (first != last && *first != value)
	{
		++first;
	}
	return first;
}


template <class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f)
{
	for (; first != last; ++first)
	{
		f(*first);
	}
	return f;
}