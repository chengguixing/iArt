#include <boost/interprocess/shared_memory_object.hpp> 
#include <boost/interprocess/mapped_region.hpp> 
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;

int main() 
{ 
	boost::interprocess::shared_memory_object shdmem(boost::interprocess::open_or_create, "izhuxin", boost::interprocess::read_write); 
	shdmem.truncate(4096); 
	boost::interprocess::mapped_region region(shdmem, boost::interprocess::read_write); 
	std::cout << std::hex << "0x" << region.get_address() << std::endl; 
	std::cout << std::dec << region.get_size() << std::endl; 
	int *i1 = static_cast<int*>(region.get_address()); 
	*i1 = 99; 



	boost::interprocess::mapped_region region1(shdmem, boost::interprocess::read_only); 
	std::cout << std::hex << "0x" << region1.get_address() << std::endl; 
	std::cout << std::dec << region1.get_size() << std::endl; 
	int *i2 = static_cast<int*>(region1.get_address()); 
	std::cout << *i2 << std::endl; 


	bool removed = boost::interprocess::shared_memory_object::remove("izhuxin"); 
	std::cout << removed << std::endl; 


	system("pause");
	return 0;
} 