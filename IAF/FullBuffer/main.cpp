#include <fstream>��
using namespace std;

int main(void)
{
	//�����ļ�test.txt����
	ofstream outfile("test.txt");

	//��test.txt�ļ���д��4096���ַ�'a'
	for(int n = 0; n < 4096; ++n)
	{
		outfile << 'a';
	}

	//��ͣ�������������
	system("PAUSE");

	//������test.txt�ļ���д���ַ���b����Ҳ����˵����4097���ַ��ǡ�b��
	outfile << 'b';

	//��ͣ�������������
	system("PAUSE");

	return 0;
}