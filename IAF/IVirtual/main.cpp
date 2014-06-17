#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;


/*单一的继承*/
class Parent
{
public:
	int iparent;
	Parent() : iparent(10)
	{
		;
	}
	virtual void f()
	{
		cout << "Parent::f()" << endl;
	}
	virtual void g()
	{
		cout << "Parent::g()" << endl;
	}
	virtual void h()
	{
		cout << "Parent::h()" << endl;
	}
};


class Child : public Parent
{
public:
	int ichild;
	Child() : ichild(100)
	{
		;
	}
	virtual void f()
	{
		cout << "Child::f()" << endl;
	}
	virtual void g_child()
	{
		cout << "Child::g_child()" << endl;
	}
	virtual void h_child()
	{
		cout << "Child::h_child()" << endl;
	}
};


class GrandChild : public Child
{
public:
	int igrandchild;
	GrandChild() : igrandchild(1000)
	{
		;
	}
	virtual void f()
	{
		cout << "GrandChild::f()" << endl;
	}
	virtual void g_child()
	{
		cout << "GrandChild::g_child()" << endl;
	}
	virtual void h_grandchild()
	{
		cout << "GrandChild::h_grandchild()" << endl;
	}
};


/*多重继承*/
class Base1
{
public:
	int ibase1;
	Base1() : ibase1(10)
	{
		;
	}
	virtual void f()
	{
		cout << "Base1::f()" << endl;
	}
	virtual void g()
	{
		cout << "Base1::g()" << endl;
	}
	virtual void h()
	{
		cout << "Base1::h()" << endl;
	}
};

class Base2
{
public:
	int ibase2;
	Base2() : ibase2(20)
	{
		;
	}
	virtual void f()
	{
		cout << "Base2::f()" << endl;
	}
	virtual void g()
	{
		cout << "Base2::g()" << endl;
	}
	virtual void h()
	{
		cout << "Base2::h()" << endl;
	}
};


class Base3
{
public:
	int ibase3;
	Base3() : ibase3(30)
	{
		;
	}
	virtual void f()
	{
		cout << "Base3::f()" <<endl;
	}
	virtual void g()
	{
		cout << "Base3::g()" << endl;
	}
	virtual void h()
	{
		cout << "Base3::h()" << endl;
	}
};

class Derive : public Base1, public Base2, public Base3
{
public:
	int iderive;
	Derive() : iderive(100)
	{
		;
	}
	virtual void f()
	{
		cout << "Derive::f()" << endl;
	}
	virtual void g1()
	{
		cout << "Derive::g1()" << endl;
	}
};


//重复继承
/*
class B
{
public:
	int ib;
	char cb;
public:
	B() : ib(0), cb('B')
	{
		;
	}
	virtual void f()
	{
		cout << "B::f()" << endl;
	}
	virtual void Bf()
	{
		cout << "B::Bf()" << endl;
	}
};

class B1 : public B
{
public:
	int ib1;
	char cb1;
public:
	B1() : ib1(11), cb1('1')
	{
		;
	}
	virtual void f()
	{
		cout << "B1::f()" << endl;
	}
	virtual void f1()
	{
		cout << "B1::f1()" << endl;
	}
	virtual void Bf1()
	{
		cout << "B1::Bf1()" << endl;
	}
};

class B2 : public B
{
public:
	int ib2;
	char cb2;
public:
	B2() : ib2(12), cb2('2')
	{
		;
	}
	virtual void f()
	{
		cout << "B2::f()" << endl;
	}
	virtual void f2()
	{
		cout << "B2::f2()" << endl;
	}
	virtual void Bf2()
	{
		cout << "B2::Bf2()" << endl;
	}
};


class D : public B1, public B2
{
public:
	int id;
	char cd;
public:
	D() : id(100), cd('D')
	{
		;
	}

	virtual void f()
	{
		cout << "D::f()" << endl;
	}
	virtual void f1()
	{
		cout << "D::f1()" << endl;
	}
	virtual void f2()
	{
		cout << "D::f2()" << endl;
	}
	virtual void Df()
	{
		cout << "D::Df()" << endl;
	}
};
*/

//钻石型继承

class B
{
public:
	int ib;
	char cb;
public:
	B() : ib(0), cb('B')
	{
		;
	}
	virtual void f()
	{
		cout << "B::f()" << endl;
	}
	virtual void Bf()
	{
		cout << "B::Bf()" << endl;
	}
};

class B1 : virtual public B
{
public:
	int ib1;
	char cb1;
public:
	B1() : ib1(11), cb1('1')
	{
		;
	}
	virtual void f()
	{
		cout << "B1::f()" << endl;
	}
	virtual void f1()
	{
		cout << "B1::f1()" << endl;
	}
	virtual void Bf1()
	{
		cout << "B1::Bf1()" << endl;
	}
};

class B2 : virtual public B
{
public:
	int ib2;
	char cb2;
public:
	B2() : ib2(12), cb2('2')
	{
		;
	}
	virtual void f()
	{
		cout << "B2::f()" << endl;
	}
	virtual void f2()
	{
		cout << "B2::f2()" << endl;
	}
	virtual void Bf2()
	{
		cout << "B2::Bf2()" << endl;
	}
};


class D : public B1, public B2
{
public:
	int id;
	char cd;
public:
	D() : id(100), cd('D')
	{
		;
	}

	virtual void f()
	{
		cout << "D::f()" << endl;
	}
	virtual void f1()
	{
		cout << "D::f1()" << endl;
	}
	virtual void f2()
	{
		cout << "D::f2()" << endl;
	}
	virtual void Df()
	{
		cout << "D::Df()" << endl;
	}
};


int main(void)
{
	/*
	typedef void (*Fun)(void);

	GrandChild gc;

	Fun pFun;

	int** pVtab = (int**)&gc;

	cout << "[0] GrandChild::_vptr->" << endl;
	
	for (int i = 0; (Fun)pVtab[0][i] != NULL; ++i)
	{
		pFun = (Fun)pVtab[0][i];
		cout << " [" << i << "]";
		pFun();
	}
	

	pFun = (Fun)*(int*)*(int*)(&gc);
	pFun();

	pFun = (Fun)*((int*)*(int*)(&gc) + 1);
	pFun();

	pFun = (Fun)*((int*)*(int*)(&gc) + 2);
	pFun();

	pFun = (Fun)*((int*)*(int*)(&gc) + 3);
	pFun();

	pFun = (Fun)*((int*)*(int*)(&gc) + 4);
	pFun();

	pFun = (Fun)*((int*)*(int*)(&gc) + 5);
	pFun();

	cout << "[1] Parent.iparent = " << (int)*((int*)(&gc) + 1) << endl;
	cout << "[2] Child.ichild = " << (int)*((int*)(&gc) + 2) << endl;
	cout << "[3] GrandChild.igrandchild = " << (int)*((int*)(&gc) + 3) << endl;
	*/


	/*
	typedef void(*Fun)(void);

	Derive d;

	int** pVtab = (int**)&d;
	Fun pFun;

	pFun = (Fun)*(int*)(*(int*)(&d));
	pFun();


	pFun = (Fun)*((int*)(*(int*)(&d)) + 1);
	pFun();

	pFun = (Fun)*((int*)(*(int*)(&d)) + 2);
	pFun();

	pFun = (Fun)*((int*)(*(int*)(&d)) + 3);
	pFun();

	cout << *(int*)((int*)(&d) + 1) << endl;





	pFun = (Fun)*(int*)(*((int*)(&d) + 2));
	pFun();


	pFun = (Fun)*((int*)(*((int*)(&d) + 2)) + 1);
	pFun();

	pFun = (Fun)*((int*)(*((int*)(&d) + 2)) + 2);
	pFun();


	cout << *(int*)((int*)(&d) + 3) << endl;





	pFun = (Fun)*(int*)(*((int*)(&d) + 4));
	pFun();


	pFun = (Fun)*((int*)(*((int*)(&d) + 4)) + 1);
	pFun();

	pFun = (Fun)*((int*)(*((int*)(&d) + 4)) + 2);
	pFun();


	cout << *(int*)((int*)(&d) + 5) << endl;




	cout << *(int*)((int*)(&d) + 6) << endl;

	*/

	/*
	typedef void(*Fun)(void);
	int** pVtab = NULL;
	Fun pFun = NULL;


	D d;
	pVtab = (int**)&d;




	pFun = (Fun)*(int*)(*(int*)(&d));
	pFun();

	pFun = (Fun)*((int*)(*(int*)(&d)) + 1);
	pFun();

	pFun = (Fun)*((int*)(*(int*)(&d)) + 2);
	pFun();

	pFun = (Fun)*((int*)(*(int*)(&d)) + 3);
	pFun();

	pFun = (Fun)*((int*)(*(int*)(&d)) + 4);
	pFun();

	pFun = (Fun)*((int*)(*(int*)(&d)) + 5);
	cout << pFun << endl;


	cout << (int)*(int*)((int*)(&d) + 1) << endl;
	cout << (char)*(char*)((int*)(&d) + 2) << endl;
	cout << (int)*(int*)((int*)(&d) + 3) << endl;
	cout << (char)*(char*)((int*)(&d) + 4) << endl;

	
	
	
	pFun = (Fun)*(int*)*(int*)((int*)(&d) + 5);
	pFun();

	pFun = (Fun)*((int*)*((int*)(&d) + 5) + 1);
	pFun();

	pFun = (Fun)*((int*)*((int*)(&d) + 5) + 2);
	pFun();

	pFun = (Fun)*((int*)*((int*)(&d) + 5) + 3);
	pFun();

	pFun = (Fun)*((int*)*((int*)(&d) + 5) + 4);
	cout << pFun << endl;
 





	cout << (int)*(int*)((int*)(&d) + 6) << endl;
	cout << (char)*(char*)((int*)(&d) + 7) << endl;
	cout << (int)*(int*)((int*)(&d) + 8) << endl;
	cout << (char)*(char*)((int*)(&d) + 9) << endl;


	cout << (int)*(int*)((int*)(&d) + 10) << endl;
	cout << (char)*(char*)((int*)(&d) + 11) << endl;
	*/
	/*
	typedef void(*Fun)(void);
	int** pVtab = NULL;
	Fun pFun = NULL;


	B1 bb1;


	pVtab = (int**)&bb1;
	pFun = (Fun)*(int*)(*(int*)(&bb1));
	pFun();

	pFun = (Fun)*((int*)(*(int*)(&bb1)) + 1);
	pFun();

	pFun = (Fun)*((int*)(*(int*)(&bb1)) + 2);
	cout << pFun << endl;

	//下面一行输出-4
	cout << *(int*)(*((int*)(&bb1) + 1)) << endl;


	cout << (int)*((int*)(&bb1) + 2) << endl;
	cout << (char)*((int*)(&bb1) + 3) << endl;

	pFun = (Fun)*((int*)(&bb1) + 4);
	cout << pFun << endl;


	pFun = (Fun)*((int*)*((int*)(&bb1) + 5));
	pFun();

	pFun = (Fun)*((int*)*((int*)(&bb1) + 5) + 1);
	pFun();

	pFun = (Fun)*((int*)*((int*)(&bb1) + 5) + 2);
	cout << pFun << endl;

	cout << (int)*((int*)(&bb1) + 6) << endl;
	cout << (char)*((int*)(&bb1) + 7) << endl;
	*/

	/*
	D d;
	typedef void(*Fun)(void);
	Fun pFun = NULL;

	pFun = (Fun)*(int*)(*(int*)(&d));
	pFun();

	pFun = (Fun)*((int*)(*(int*)(&d)) + 1);
	pFun();

	pFun = (Fun)*((int*)(*(int*)(&d)) + 2);
	pFun();

	pFun = (Fun)*((int*)(*(int*)(&d)) + 3);
	cout << pFun << endl;
	//-4
	cout << *((int*)(*(int*)(&d)) + 3) << endl;



	//-4
	cout << *(int*)*((int*)(&d) + 1) << endl;


	cout << (int)*((int*)(&d) + 2) << endl;
	cout << (char)*((int*)(&d) + 3) << endl;

	pFun = (Fun)*(int*)*((int*)(&d) + 4);
	pFun();

	pFun = (Fun)*((int*)*((int*)(&d) + 4) + 1);
	pFun();

	cout << ((int*)*((int*)(&d) + 4) + 2) << endl;


	//-4
	cout << *(int*)*((int*)(&d) + 5) << endl;


	cout << (int)*((int*)(&d) + 6) << endl;
	cout << (char)*((int*)(&d) + 7) << endl;


	cout << (int)*((int*)(&d) + 8) << endl;
	cout << (char)*((int*)(&d) + 9) << endl;

	cout << *((int*)(&d) + 10) << endl;



	pFun = (Fun)*((int*)*((int*)(&d) + 11));
	pFun();


	pFun = (Fun)*((int*)*((int*)(&d) + 11) + 1);
	pFun();
	
	cout << *((int*)*((int*)(&d) + 11) + 2) << endl;

	cout << (int)*((int*)(&d) + 12) << endl;
	cout << (char)*((int*)(&d) + 13) << endl;
	*/

	/*
	Parent p;
	Child c;
	GrandChild gc;
	*/
	/*
	Base1 b1;
	Base2 b2;
	Base3 b3;
	Derive d;
	*/

	B b;
	B1 b1;
	B2 b2;
	D d;

	system("pause");
	return 0;
}