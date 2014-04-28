#ifndef _DELETER_H_7EE1CE25_F9B4_447F_8C01_6E282AF995E3
#define _DELETER_H_7EE1CE25_F9B4_447F_8C01_6E282AF995E3

class to_be_deleted;

class deleter
{
public:
	void delete_it(to_be_deleted* p);
};


#endif