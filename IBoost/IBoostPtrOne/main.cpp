#include <boost/smart_ptr.hpp> 
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;
using namespace boost;


struct posix_file
{
	posix_file(const char* filename)
	{
		cout << "open file : " << filename << endl;
	}
	~posix_file()
	{
		cout << "close file" << endl;
	}
};

int main() 
{ 
	/*
	scoped_ptr<int> p(new int);

	if (p)
	{
		*p = 100;
		cout << *p << endl;
	}

	p.reset();

	assert(p == 0);

	if (!p)
	{
		cout << "scoped_ptr == null" << endl;
	}



	{
		scoped_ptr<posix_file> fp(new posix_file("/tmp/a.txt"));
	}
	*/

	{
		int* arr = new int[100];
		scoped_array<int> sa(arr);

		fill_n(&sa[0], 100, 5);
		sa[10] = sa[20] + sa[30];

		cout << sa[10] << endl;
		cout << sa[20] << endl;
		cout << sa[30] << endl;
	}


	system("pause");
	return 0;
} 