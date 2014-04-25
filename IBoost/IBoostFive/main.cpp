/*
	Windows 提供了一种特别的共享内存，它可以在最后一个使用它的应用程序终止后自动删除。 
	为了使用它，提供了 boost::interprocess::windows_shared_memory 类，
	定义在 boost/interprocess/windows_shared_memory.hpp 文件中。
*/



#include <boost/interprocess/windows_shared_memory.hpp> 
#include <boost/interprocess/mapped_region.hpp> 
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;

int main() 
{ 
	boost::interprocess::windows_shared_memory shdmem(boost::interprocess::open_or_create, "izhuxin", boost::interprocess::read_write, 1024); 
	boost::interprocess::mapped_region region(shdmem, boost::interprocess::read_write); 
	int *i1 = static_cast<int*>(region.get_address()); 
	*i1 = 99; 

	boost::interprocess::mapped_region region1(shdmem, boost::interprocess::read_only); 
	int *i2 = static_cast<int*>(region1.get_address()); 
	std::cout << *i2 << std::endl; 


	system("pause");
	return 0;
} 