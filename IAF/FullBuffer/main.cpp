#include <fstream>　
using namespace std;

int main(void)
{
	//创建文件test.txt并打开
	ofstream outfile("test.txt");

	//向test.txt文件中写入4096个字符'a'
	for(int n = 0; n < 4096; ++n)
	{
		outfile << 'a';
	}

	//暂停，按任意键继续
	system("PAUSE");

	//继续向test.txt文件中写入字符’b’，也就是说，第4097个字符是’b’
	outfile << 'b';

	//暂停，按任意键继续
	system("PAUSE");

	return 0;
}