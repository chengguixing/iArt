#include <iterator>
#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
using namespace std;


class Shape
{
public:
	virtual void display() = 0;
};

class Rect : public Shape
{
public:
	virtual void display()
	{
		cout << "Rect" << endl;
	}
};
class Circle : public Shape
{
public:
	virtual void display()
	{
		cout << "Circle" << endl;
	}
};
class Square : public Shape
{
public:
	virtual void display()
	{
		cout << "Square" << endl;
	}
};

int main(void)
{
	vector<Shape*> V;
	V.push_back(new Rect);
	V.push_back(new Circle);
	V.push_back(new Square);
	V.push_back(new Circle);
	V.push_back(new Rect);


	for (int i = 0; i < V.size(); ++i)
	{
		(V[i])->display();
	}
	cout << endl;

	for_each(V.begin(), V.end(), mem_fun(&Shape::display));
	cout << endl;



	system("pause");
	return 0;

}