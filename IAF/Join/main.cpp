#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;


#define JOIN(X, Y) DO_JOIN(X, Y)
#define DO_JOIN(X, Y) DO_JOIN2(X, Y)
#define DO_JOIN2(X, Y) X##Y

#define IZX "izhuxin "
#define ILY "i love you"


int main() 
{ 
	cout << JOIN("izhuxin ", "i love you") << endl;
	cout << JOIN("izhuxin ", ILY) << endl;
	cout << JOIN(IZX, ILY) << endl;
	system("pause");
	return 0;
} 