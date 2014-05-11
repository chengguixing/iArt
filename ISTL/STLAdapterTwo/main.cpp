#include <iterator>
#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
using namespace std;

int main(void)
{
	ostream_iterator<int> outite(cout, "");
	int ia[] = {2, 21, 12, 7, 19, 23};
	vector<int> iv(ia, ia + 6);
	

	for_each(iv.begin(), iv.end(), compose1(bind2nd(multiplies<int>(), 3), bind2nd(plus<int>(), 2)));

	copy(iv.begin(), iv.end(), outite);
	cout << endl;

	transform(iv.begin(), iv.end(), outite, compose1(bind2nd(multiplies<int>(), 3), bind2nd(plus<int>(), 2)));
	cout << endl;

	system("pause");
	return 0;

}