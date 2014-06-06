#include<iostream>
using namespace std;

int main(void)
{
	char c;
	//第一次调用getchar()函数,程序执行时，您可以输入一串字符并按下回车键，按下回车键后该函数返回。返回值是用户输入的第一个字符　(假设用户输入了 abcdef，函数返回a)
	c = getchar();
	//显示getchar()函数的返回值
	cout<< c << endl; // 输出 a

	// 循环多次调用getchar()函数，将每次调用getchar()函数的返回值显示出来,直到遇到回车符才结束。 这时函数执行不会让用户输入而是顺序读取缓冲区字符内容。第一个字符用户输入结束后已经读取，所以会从第二个字符开始读
	while ((c = getchar()) != '\n')
	{
		cout<< "," << c <<endl;
	}

	system("pause");
	return 0;
}