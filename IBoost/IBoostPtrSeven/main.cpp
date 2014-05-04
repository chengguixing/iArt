#include <boost/shared_ptr.hpp> 
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;

class abstract
{
public:
	virtual void f() = 0;
	virtual void g() = 0;
protected:
	virtual ~abstract(){}
};

class impl : public abstract
{
public:
	virtual void f()
	{
		cout << "class impl f" << endl;
	}
	virtual void g()
	{
		cout << "class impl g" << endl;
	}
};

shared_ptr<abstract> create()
{
	return shared_ptr<abstract>(new impl);
}


int main() 
{ 
	shared_ptr<abstract> p = create();
	p->f();
	p->g();

	//abstract* q = p.get();
	//delete q;

	impl* q = (impl*)(p.get());
	delete q;

	system("pause");
	return 0;
} 