#ifndef _DIRECTORY_H_64AA3803_D006_4561_A5FB_B7B6907621C6
#define _DIRECTORY_H_64AA3803_D006_4561_A5FB_B7B6907621C6

#include <string>


namespace IAF
{

	class iDirectory
	{
	private:
		static const int BUFSIZE = 1024; 
	public:
		static void GetRootDirectory(std::wstring& strRootDirectory);
		static bool DeleteFileOrDirectory(std::string const& str);
	};

}



#endif