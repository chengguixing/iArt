/*
	Windows �ṩ��һ���ر�Ĺ����ڴ棬�����������һ��ʹ������Ӧ�ó�����ֹ���Զ�ɾ���� 
	Ϊ��ʹ�������ṩ�� boost::interprocess::windows_shared_memory �࣬
	������ boost/interprocess/windows_shared_memory.hpp �ļ��С�
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