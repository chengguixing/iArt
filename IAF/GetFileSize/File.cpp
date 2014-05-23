#include "File.h"
#include <stdio.h>

namespace IAF
{

__int64 iFile::GetFileSize(std::string strFileName)
{
	fpos_t ulFileLength;
	FILE* fp = NULL;
	if (NULL == (fp = fopen(strFileName.c_str(), "rb")))
	{
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	/*ftell可能会返回-1*/
	//ulFileLength = ftell(fp);
	
	
	fgetpos(fp, &ulFileLength);
	fclose(fp);

	return ulFileLength;
}


}