#include <boost/interprocess/shared_memory_object.hpp> 
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;

int main() 
{ 
	boost::interprocess::shared_memory_object shdmem(boost::interprocess::open_or_create, "izhuxin", boost::interprocess::read_write); 
	shdmem.truncate(4096); 
	std::cout << shdmem.get_name() << std::endl; 
	boost::interprocess::offset_t size; 
	if (shdmem.get_size(size)) 
    {
		cout << size << endl; 
	}


	system("pause");
	return 0;
} 