#include <boost/interprocess/managed_shared_memory.hpp> 
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;

int main() 
{ 
	boost::interprocess::shared_memory_object::remove("izhuxin"); 
	boost::interprocess::managed_shared_memory managed_shm(boost::interprocess::open_or_create, "izhuxin", 4096); 
    int *i = managed_shm.construct<int>("Integer")(99); 
	std::cout << *i << std::endl; 

	std::pair<int*, std::size_t> p = managed_shm.find<int>("Integer"); 
	if (p.first) 
    {
		std::cout << *p.first << std::endl; 
	}

	system("pause");
	return 0;
} 