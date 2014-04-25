#include <boost/interprocess/managed_shared_memory.hpp> 
#include <iostream> 
#include <stdlib.h>
#include <iostream>
using namespace std;

int main() 
{ 
	try 
	{ 
		boost::interprocess::shared_memory_object::remove("Highscore"); 
		boost::interprocess::managed_shared_memory managed_shm(boost::interprocess::open_or_create, "Highscore", 1 << 15); 
		int *i = managed_shm.construct<int>("Integer")[4096](99); 
	} 
	catch (boost::interprocess::bad_alloc &ex) 
	{ 
		std::cerr << ex.what() << std::endl; 
	} 

	system("pause");
	return 0;
} 