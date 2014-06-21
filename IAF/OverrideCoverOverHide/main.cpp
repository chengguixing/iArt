#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

/*
	重载
	（1）相同的范围（在同一个类中）；
	（2）函数名字相同；
	（3）参数不同；
	（4）virtual 关键字可有可无。
	注意：（返回值不同不叫重载，如下所示，是编译不过的）
	int max(int, int);
	double max(int, int);
*/

/*
	覆盖
	（1）不同的范围（分别位于派生类与基类）；
	（2）函数名字相同；
	（3）参数相同；
	（4）基类函数必须有virtual 关键字。
*/

/*
	隐藏
	这里“隐藏”是指派生类的函数屏蔽了与其同名的基类函数，规则如下：
	（1）如果派生类的函数与基类的函数同名，但是参数不同。此时，不论有无virtual
	关键字，基类的函数将被隐藏（注意别与重载混淆）。
	（2）如果派生类的函数与基类的函数同名，并且参数也相同，但是基类函数没有virtual
	关键字。此时，基类的函数被隐藏（注意别与覆盖混淆）。
*/
/*
class Base
{
public:
	void f(int x)
	{ 
		cout << "Base::f(int) " << x << endl; 
	}
	void f(float x)
	{ 
		cout << "Base::f(float) " << x << endl; 
	}
	virtual void g(void)
	{ 
		cout << "Base::g(void)" << endl;
	}
};
class Derived : public Base
{
public:
	virtual void g(void){ cout << "Derived::g(void)" << endl;}
};

int main(void)
{
	Derived d;
	Base *pb = &d;
	pb->f(42); // Base::f(int) 42

	pb->f(3.14f); // Base::f(float) 3.14
	pb->g(); // Derived::g(void)

	system("pause");
	return 0;
}
*/

/*
#include <iostream>
using namespace std;

class Base
{
public:
	virtual void f(float x)
	{ 
		cout << "Base::f(float) " << x << endl; 
	}
	void g(float x)
	{ 
		cout << "Base::g(float) " << x << endl; 
	}
	void h(float x)
	{ 
		cout << "Base::h(float) " << x << endl; 
	}
};
class Derived : public Base
{
public:
	virtual void f(float x)
	{ 
		cout << "Derived::f(float) " << x << endl; 
	}
	void g(int x)
	{ 
		cout << "Derived::g(int) " << x << endl; 
	}
	void h(float x)
	{ 
		cout << "Derived::h(float) " << x << endl; 
	}
};

int main(void)
{
	Derived d;
	Base *pb = &d;
	Derived *pd = &d;
	// Good : behavior depends solely on type of the object
	pb->f(3.14f); // Derived::f(float) 3.14
	pd->f(3.14f); // Derived::f(float) 3.14
	// Bad : behavior depends on type of the pointer
	pb->g(3.14f); // Base::g(float) 3.14
	pd->g(3.14f); // Derived::g(int) 3 (surprise!)
	// Bad : behavior depends on type of the pointer
	pb->h(3.14f); // Base::h(float) 3.14 (surprise!)
	pd->h(3.14f); // Derived::h(float) 3.14

	system("pause");
	return 0;
}
*/


class Base
{
public:
	void f(int x);
};
class Derived : public Base
{
public:
	void f(char *str);
};
void Test(void)
{
	Derived *pd = new Derived;
	pd->f(10); // error
}
int main(void)
{
	Test();
	system("pause");
	return 0;
}