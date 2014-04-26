#include "IDefineClass.h"

ostream& operator<< (ostream& os, const DefineClass& dc)
{
	os << dc.m_str << endl;
	return os;
}