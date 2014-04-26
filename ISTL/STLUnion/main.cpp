#include <iostream>
#include <stdlib.h>
using namespace std;

typedef union iUnion
{
	struct {
		int x;
		int y;
		int z;
	}u;
	int k;
}iUnion;

static union{
	char c[4];
	unsigned long mylong;
}endian_test = {{'l', '?', '?', 'b'}};

#define ENDIANNESS ((char)endian_test.mylong)


const int endian = 1;
#define is_bigendian() (*(char*)&endian == 0)
#define is_littlendian (*(char*)&endian == 1)

union
{
	int val;
	char cval[sizeof(int)];
}u;

u.val = 1;


int main(void)
{
	cout << ENDIANNESS << endl;

	iUnion iUn;
	iUn.u.x = 4;
	iUn.u.y = 5;
	iUn.u.z = 6;
	iUn.k = 3;

	cout << iUn.u.x << endl;
	cout << iUn.u.y << endl;
	cout << iUn.u.z << endl;
	cout << iUn.k << endl;



	system("pause");
	return 0;
}