#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

/*
class Empty
{
	typedef int Int;
};

class EmptyTwo : public Empty
{

};

class EmptyThree : public EmptyTwo
{

};
*/

class Empty
{
	typedef int Int;
};

class EmptyTwo : public Empty
{
	;
};

class EmptyThree : public Empty, public EmptyTwo
{
	;
};
int main(void)
{
	cout << sizeof(Empty) << endl;
	cout << sizeof(EmptyTwo) << endl;
	cout << sizeof(EmptyThree) << endl;

	system("pause");
	return 0;
}