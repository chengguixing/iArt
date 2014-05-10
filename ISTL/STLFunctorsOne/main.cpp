#include <functional>
#include <iostream>
using namespace std;

int main(void)
{
	greater<int> ig;
	cout << boolalpha << ig(4, 6) << endl;
	cout << greater<int>()(6, 4) << endl;


	system("pause");
	return 0;
}