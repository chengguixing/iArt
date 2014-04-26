#include <iostream>
#include <stdlib.h>
using namespace std;

int fcmp(const void* elem1, const void* elem2);


int main(void)
{

	int iArray[] = {999, 0, 68, -2, 1024, 2048, 500, 600, 128};

	for (int i = 0; i < sizeof(iArray) / sizeof(iArray[0]); ++i)
	{
		cout << iArray[i] << ' ';
	}
	cout << endl;

	qsort(iArray, sizeof(iArray) / sizeof(iArray[0]), sizeof(iArray[0]), fcmp);


	for (int i = 0; i < sizeof(iArray) / sizeof(iArray[0]); ++i)
	{
		cout << iArray[i] << ' ';
	}
	cout << endl;

	system("pause");
	return 0;
}


int fcmp(const void* elem1, const void* elem2)
{
	const int* i1 = (const int*)elem1;
	const int* i2 = (const int*)elem2;


	if (*i1 < *i2)
	{
		return -1;
	}
	else if (*i1 == *i2)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}