#include <boost/interprocess/shared_memory_object.hpp> 
#include <boost/interprocess/mapped_region.hpp> 
#include <iostream>
#include <stdlib.h>
using namespace std;


int main() 
{ 
	boost::interprocess::shared_memory_object shdmem(boost::interprocess::open_or_create, "izhuxin", boost::interprocess::read_write); 
	shdmem.truncate(4096); 
	boost::interprocess::mapped_region region(shdmem, boost::interprocess::read_write); 
	std::cout << std::hex << "0x" << region.get_address() << std::endl; 
	std::cout << std::dec << region.get_size() << std::endl; 
	boost::interprocess::mapped_region region1(shdmem, boost::interprocess::read_only); 
	std::cout << std::hex << "0x" << region1.get_address() << std::endl; 
	std::cout << std::dec << region1.get_size() << std::endl; 

	system("pause");
	return 0;
} 