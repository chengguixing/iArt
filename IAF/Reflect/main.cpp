#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "DynBase.h"
using namespace std;


class A : public Object
{
	DECLARE_CLASS(A)
public:
	A()
	{
		cout << hex << long(this) << "A constructor!" << endl;
	}
	~A()
	{
		cout << hex << long(this) << "A destructor!" << endl;
	}
};
IMPLEMENT_CLASS(A)

/*
class A : public Object
{
protected:
	static ClassInfo ms_classinfo;
public:
	virtual ClassInfo* GetClassInfo() const;
	static Object* CreateObject();
	A()
	{
		cout << hex << long(this) << "A constructor!" << endl;
	}
	~A()
	{
		cout << hex << long(this) << "A destructor!" << endl;
	}
};
ClassInfo A::ms_classinfo("A", (ObjectConstructorFn)A::CreateObject);

ClassInfo* A::GetClassInfo() const
{
	return &A::ms_classinfo;
}
Object* A::CreateObject()
{
	return new A;
}
*/

class B : public Object
{
	DECLARE_CLASS(B)
public:
	B()
	{
		cout << hex << (long)this << "B constructor!" << endl;
	}
	~B()
	{
		cout << hex << (long)this << "B destructor!" << endl;
	}
};
IMPLEMENT_CLASS(B)

int main(void)
{
	Object* p = Object::CreateObject("A");
	delete p;



	system("pause");
	return 0;
}