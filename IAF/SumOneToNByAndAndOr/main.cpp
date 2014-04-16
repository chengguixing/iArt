#include <iostream>
#include <stdio.h>
using namespace std;

/*
功能：通过逻辑与计算从1到n的和
返回值：计算所得和
*/
int AccumulationByAnd(int number);

/*
功能：通过逻辑或计算从1到n的和
返回值：计算所得和
*/
int AccumulationByOr(int number);

int main(void)
{
	cout << AccumulationByAnd(100) << endl;
	cout << AccumulationByOr(100) << endl;
	getchar();
	return 0;
}


/*
功能：通过逻辑与计算从1到n的和
返回值：计算所得和
*/
int AccumulationByAnd(int number)
{
	unsigned int sum = 0;
	number && (sum = AccumulationByAnd(number - 1) + number);
	return sum;
}



/*
功能：通过逻辑或计算从1到n的和
返回值：计算所得和
*/
int AccumulationByOr(int number)
{
	unsigned int sum = 0;
	!number || (sum = AccumulationByOr(number - 1) + number);
	return sum;
}