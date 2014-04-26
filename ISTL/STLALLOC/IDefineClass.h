#ifndef _IDEFINE_H_F6F99371_26C4_4a54_83D9_67028BF3DB1A
#define _IDEFINE_H_F6F99371_26C4_4a54_83D9_67028BF3DB1A

#include <string>
#include <iostream>
using namespace std;

class DefineClass
{
	friend ostream& operator<<(ostream& os, const DefineClass& dc);
public:
	DefineClass() : m_str("")
	{
		;
	}
	DefineClass(const std::string& str) : m_str(str)
	{
		;
	}
	~DefineClass()
	{
		;
	}
public:
	std::string GetString()
	{
		return m_str;
	}
private:
	std::string m_str;
};


#endif