#include "checked_delete.h"
#include "deleter.h"
#include "to_be_deleted.h"
#include <stdlib.h>
#include <iostream>
using namespace std;
using namespace IAF::IBase;


class some_class;

some_class* create()
{
	return (some_class*)0;
}


int main() 
{ 
	
	//some_class* p = create();
	//�����������Ͷ��壬���������벻����
	//checked_delete(p);


	to_be_deleted* p = new to_be_deleted;

	deleter d;
	//d.delete_it(p);

	system("pause");
	return 0;
} 