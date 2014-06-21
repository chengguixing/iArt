#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

/*
	����
	��1����ͬ�ķ�Χ����ͬһ�����У���
	��2������������ͬ��
	��3��������ͬ��
	��4��virtual �ؼ��ֿ��п��ޡ�
	ע�⣺������ֵ��ͬ�������أ�������ʾ���Ǳ��벻���ģ�
	int max(int, int);
	double max(int, int);
*/

/*
	����
	��1����ͬ�ķ�Χ���ֱ�λ������������ࣩ��
	��2������������ͬ��
	��3��������ͬ��
	��4�����ຯ��������virtual �ؼ��֡�
*/

/*
	����
	������ء���ָ������ĺ�������������ͬ���Ļ��ຯ�����������£�
	��1�����������ĺ��������ĺ���ͬ�������ǲ�����ͬ����ʱ����������virtual
	�ؼ��֣�����ĺ����������أ�ע��������ػ�������
	��2�����������ĺ��������ĺ���ͬ�������Ҳ���Ҳ��ͬ�����ǻ��ຯ��û��virtual
	�ؼ��֡���ʱ������ĺ��������أ�ע����븲�ǻ�������
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