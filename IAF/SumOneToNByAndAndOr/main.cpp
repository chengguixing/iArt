#include <iostream>
#include <stdio.h>
using namespace std;

/*
���ܣ�ͨ���߼�������1��n�ĺ�
����ֵ���������ú�
*/
int AccumulationByAnd(int number);

/*
���ܣ�ͨ���߼�������1��n�ĺ�
����ֵ���������ú�
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
���ܣ�ͨ���߼�������1��n�ĺ�
����ֵ���������ú�
*/
int AccumulationByAnd(int number)
{
	unsigned int sum = 0;
	number && (sum = AccumulationByAnd(number - 1) + number);
	return sum;
}



/*
���ܣ�ͨ���߼�������1��n�ĺ�
����ֵ���������ú�
*/
int AccumulationByOr(int number)
{
	unsigned int sum = 0;
	!number || (sum = AccumulationByOr(number - 1) + number);
	return sum;
}