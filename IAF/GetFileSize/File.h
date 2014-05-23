#ifndef _FILE_H_886AE816_39F9_4372_9341_2FF23CFBC46B
#define _FILE_H_886AE816_39F9_4372_9341_2FF23CFBC46B

#include <string>

namespace IAF
{

class iFile
{
public:
	static __int64 GetFileSize(std::string strFileName);
};

}


#endif