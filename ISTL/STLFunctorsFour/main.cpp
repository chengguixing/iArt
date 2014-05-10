#include <functional>
#include <iostream>
using namespace std;

int main(void)
{
	logical_and<int> and_obj;
	logical_or<int> or_obj;
	logical_not<int> not_obj;


	cout << and_obj(3, 5) << endl;
	cout << or_obj(3, 5) << endl;
	cout << not_obj(3) << endl;


	cout << logical_and<int>()(3, 5) << endl;
	cout << logical_or<int>()(3, 5) << endl;
	cout << logical_not<int>()(3) << endl;

	system("pause");
	return 0;
}