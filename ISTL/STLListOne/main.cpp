#include <stdlib.h>
#include <iostream>
#include <list>
#include <algorithm>
using namespace std;


int main(void)
{
	int i;
	list<int> ilist;
	cout << "size = " << ilist.size() << endl;


	ilist.push_back(0);
	ilist.push_back(1);
	ilist.push_back(2);
	ilist.push_back(3);
	ilist.push_back(4);

	cout << "size = " << ilist.size() << endl;


	list<int>::iterator ite;
	for (ite = ilist.begin(); ite != ilist.end(); ++ite)
	{
		cout << *ite << ' ';
	}
	cout << endl;

	ite = find(ilist.begin(), ilist.end(), 3);
	if (ite != ilist.end())
	{
		ilist.insert(ite, 99);
	}

	cout << "size = " << ilist.size() << endl;
	cout << *ite << endl;


	for (ite = ilist.begin(); ite != ilist.end(); ++ite)
	{
		cout << *ite << ' ';
	}
	cout << endl;

	ite = find(ilist.begin(), ilist.end(), 1);
	if (ite != ilist.end())
	{
		cout << *(ilist.erase(ite)) << endl;
	}

	for (ite = ilist.begin(); ite != ilist.end(); ++ite)
	{
		cout << *ite << ' ';
	}
	cout << endl;



	cout << endl << endl << endl;
	int iv[5] = {5, 6, 7, 8, 9};
	list<int> ilist2(iv, iv + 5);

	ite = find(ilist.begin(), ilist.end(), 99);
	ilist.splice(ite, ilist2);
	ilist.reverse();
	ilist.sort();

	cout << "ilist" << endl;
	for (ite = ilist.begin(); ite != ilist.end(); ++ite)
	{
		cout << *ite << ' ';
	}
	cout << endl;


	cout << "ilist2" << endl;
	for (ite = ilist2.begin(); ite != ilist2.end(); ++ite)
	{
		cout << *ite << ' ';
	}
	cout << endl;


	system("pause");
	return 0;
}