#include "StringConversion.h"
#include <windows.h>


namespace IAF
{
	namespace IString
	{


void StringConversion::Ansi2Unicode(const char* in, unsigned int len, std::wstring& out)
{
	int wbufferlen = (int)::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, in, (int)len, NULL, 0);
	wchar_t* pwbuffer = new wchar_t[wbufferlen + 4];
	if ( NULL == pwbuffer )
	{
		return;
	}
	wbufferlen = (int)::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, in, (int)len, pwbuffer, wbufferlen + 2);
	pwbuffer[wbufferlen] = '\0';
	out.assign( pwbuffer, wbufferlen );
	delete[] pwbuffer;
	return;
}



void StringConversion::Unicode2Ansi(const wchar_t* in, unsigned int len, std::string& out)
{
	int bufferlen = (int)::WideCharToMultiByte(CP_ACP, 0, in, (int)len, NULL, 0, NULL, NULL);
	char* pBuffer = new char[bufferlen + 4];
	if ( NULL == pBuffer )
	{
		return;
	}
	int out_len = ::WideCharToMultiByte(CP_ACP, 0, in, (int)len, pBuffer, bufferlen + 2, NULL, NULL);   
	pBuffer[bufferlen] = '\0';
	out.assign( pBuffer, out_len );
	delete[] pBuffer;
}



void StringConversion::Utf82Unicode(const char* in, unsigned int len, std::wstring& out)
{
	wchar_t* pBuf = new wchar_t[len + 1];
	if (NULL == pBuf)
	{
		return;
	}
	size_t out_len = (len + 1) * sizeof(wchar_t);
	memset(pBuf, 0, (len + 1) * sizeof(wchar_t));
	wchar_t* pResult = pBuf;

	out_len = ::MultiByteToWideChar(CP_UTF8, 0, in, len, pBuf, len * sizeof(wchar_t));
	out.assign( pResult, out_len );


	delete [] pResult;
	pResult = NULL;
}




void StringConversion::Unicode2Utf8(const wchar_t* in, std::string& out)
{
	unsigned int length = WideCharToMultiByte(CP_UTF8, 0, in, -1, NULL, 0, NULL, NULL);
	size_t out_len = length * 3 + 1;
	char* pBuf = new char[out_len];
	if ( NULL == pBuf )
	{
		return;
	}
	char* pResult = pBuf;
	memset(pBuf, 0, out_len);

	out_len = ::WideCharToMultiByte(CP_UTF8, 0, in, length, pBuf, length * 3, NULL, NULL);
	out.assign( pResult, out_len );

	delete [] pResult;
	pResult = NULL;
	return;
}




void StringConversion::wstring2string(const std::wstring& wstr, std::string& str)
{
    std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
    setlocale(LC_ALL, "chs");
    const wchar_t* _Source = wstr.c_str();
    size_t _Dsize = 2 * wstr.size() + 1;
    char *_Dest = new char[_Dsize];
    memset(_Dest,0,_Dsize);
    wcstombs(_Dest,_Source,_Dsize);
    str = _Dest;
    delete []_Dest;
    setlocale(LC_ALL, curLocale.c_str());
}



void StringConversion::string2wstring(const std::string& str, std::wstring& wstr)
{
    setlocale(LC_ALL, "chs"); 
    const char* _Source = str.c_str();
    size_t _Dsize = str.size() + 1;
    wchar_t *_Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
    mbstowcs(_Dest,_Source,_Dsize);
    wstr = _Dest;
    delete []_Dest;
    setlocale(LC_ALL, "C");
}

	}
}
