#ifndef _STRINGCONVERSION_H_176335C9_C2F2_4EBF_A74E_077C6FBF9BD4
#define _STRINGCONVERSION_H_176335C9_C2F2_4EBF_A74E_077C6FBF9BD4

#include <string>

namespace IAF
{
	namespace IString
	{

class StringConversion
{
public:
	static void Ansi2Unicode(const char* in, unsigned int len, std::wstring& out);
	static void Unicode2Ansi(const wchar_t* in, unsigned int len, std::string& out);
	
	static void Utf82Unicode(const char* in, unsigned int len, std::wstring& out);
	static void Unicode2Utf8(const wchar_t* in, std::string& out);

	static void wstring2string(const std::wstring& wstr, std::string& str);
	static void string2wstring(const std::string& str, std::wstring& wstr);
};

	}
}


#endif