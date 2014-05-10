#include <functional>
#include <iostream>
using namespace std;

int main(void)
{
	plus<int> plusobj;
	minus<int> minusobj;
	multiplies<int> multipliesobj;
	divides<int> dividesobj;
	modulus<int> modulusobj;
	negate<int> negateobj;

	cout << plusobj(3, 5) << endl;
	cout << minusobj(3, 5) << endl;
	cout << multipliesobj(3, 5) << endl;
	cout << dividesobj(3, 5) << endl;
	cout << modulusobj(3, 5) << endl;
	cout << negateobj(3) << endl;


	cout << plus<int>()(3, 5) << endl;
	cout << minus<int>()(3, 5) << endl;
	cout << multiplies<int>()(3, 5) << endl;
	cout << divides<int>()(3, 5) << endl;
	cout << modulus<int>()(3, 5) << endl;
	cout << negate<int>()(3) << endl;

	system("pause");
	return 0;
}