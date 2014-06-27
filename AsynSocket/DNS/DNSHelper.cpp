#include <windows.h>
#include "DNSHelper.h"


namespace DNS
{
	std::string DNSHelper::GetWinDir()
	{
		std::string win_dir;
		UINT ret = GetWindowsDirectory(NULL, 0);
		if (ret > 0)
		{
			unsigned int buff_len = ret;
			char* buff_ptr = new char[buff_len];
			if (buff_ptr)
			{
				ret = GetWindowsDirectoryA(buff_ptr, buff_len);
				if (ret > 0)
				{
					win_dir.assign(buff_ptr, ret);
				}
				delete[] buff_ptr;
			}
		}
		return win_dir;
	}
	std::string DNSHelper::GetHostsPath()
	{
		std::string strTemp = GetWinDir();
		if ("" != strTemp)
		{
			OSVERSIONINFO info;
			info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			if (FALSE != GetVersionEx(&info))
			{
				if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
				{
					strTemp += "\\";
				}
				else
				{
					strTemp += "\\system32\\drivers\\etc\\"; 
				}
			}
			else
			{
				strTemp += "system32\\drivers\\etc\\";
			}
			return strTemp;
		}
		return "";
	}
	std::string DNSHelper::FindHostsFile()
	{
		std::string strFilePath = GetHostsPath();
		if ("" == strFilePath)
		{
			return "";
		}
		std::vector<std::string> vstrFileName;
		vstrFileName.push_back("Hosts");
		vstrFileName.push_back("Hosts");

		WIN32_FIND_DATAA find_data;
		HANDLE find_handle;


		std::vector<std::string>::const_iterator cit = vstrFileName.begin();
		for (; cit != vstrFileName.end(); ++cit)
		{
			std::string temp = strFilePath + *cit;
			find_handle = FindFirstFileA(temp.c_str(), &find_data);
			if (INVALID_HANDLE_VALUE)
			{
				FindClose(find_handle);
				return temp;
			}
			else
			{
				DWORD error_code = GetLastError();
			}
		}
		return "";
	}

	bool DNSHelper::IsValidDomainName(std::string const &str)
	{
		for (unsigned int i = 0; i < str.length(); ++i)
		{
			char c = str.at(i);
			if (!(::isalpha(c) || ::isdigit(c) || c == '-' || c == '.' || c == '_'))
			{
				return false;
			}
		}
		return true;
	}
	bool DNSHelper::IsNetbiosName(std::string const & domain_name)
	{
		int len = domain_name.length();
		if (len > 15)
		{
			return false;
		}
		for (int i = 0; i < len; ++i)
		{
			char c = domain_name.at(i);
			if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '-'))
			{
				return false;
			}
		}
		return true;
	}
	void DNSHelper::DivideString(std::string const &strSrc, std::string const & divide_chars, std::vector<std::string>& result)
	{
		unsigned pos = 0;
		do 
		{
			pos = strSrc.find_first_not_of(divide_chars, pos);
			if (std::string::npos != pos)
			{
				unsigned pos_end = strSrc.find_first_of(divide_chars, pos);
				if (std::string::npos != pos_end)
				{
					result.push_back(strSrc.substr(pos, pos_end - pos));
					pos = pos_end;
				}
				else
				{
					result.push_back(strSrc.substr(pos));
					break;
				}
			}
		} while (pos != std::string::npos);
	}
}