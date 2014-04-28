#include <boost/timer.hpp>
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;
using namespace boost;

int main() 
{ 
	timer t;
	cout << t.elapsed_max() << endl;
	cout << t.elapsed_min() << endl;
	cout << t.elapsed() << endl;

	system("pause");
	return 0;
} 