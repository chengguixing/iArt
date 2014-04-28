#include <windows.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "StringConversion.h"
using namespace std;
using namespace IAF::IString;

#define MAX_BUF 256

void ReadAnsi();
void ReadUtf8();
void ReadUnicode();

int main(void)
{
	ReadAnsi();
	ReadUtf8();
	ReadUnicode();


	system("pause");
	return 0;
}

void ReadAnsi()
{
	char szBuf[MAX_BUF];	
	memset(szBuf, 0, sizeof(char) * MAX_BUF);
	
	std::string strMessage;
 
	FILE * fp = NULL;
	fp = fopen("ansi.txt", "rb");
	if (fp != NULL)
	{
		// common file do not offset.
			
		std::wstring wstr;
		while(fread(szBuf, sizeof(char), MAX_BUF, fp) > 0)
		{
			strMessage += szBuf;

			StringConversion::Ansi2Unicode(szBuf, strlen(szBuf), wstr);
			std::wcout.imbue(std::locale("chs"));
			wcout << wstr << endl;

			memset(szBuf, 0, sizeof(char) * MAX_BUF);

			

		}
	}
 
	std::cout << strMessage << std::endl;

	fclose(fp);
}


void ReadUtf8()
{
	char szBuf[MAX_BUF];
	memset(szBuf, 0, sizeof(char) * MAX_BUF);
 
	std::string strMessage;
	
	FILE * fp = NULL;
	fp = fopen("utf8.txt", "rb");
	if (fp != NULL)
	{
		// UTF-8 file should offset 3 byte from start position.
		fseek(fp, sizeof(char) * 3, 0);
		while(fread(szBuf, sizeof(char), MAX_BUF, fp) > 0)
		{

			std::wstring wstr;
			StringConversion::Utf82Unicode(szBuf, strlen(szBuf), wstr);

			std::wcout.imbue(std::locale("chs"));
			wcout << wstr << endl;

			std::string str;
			StringConversion::wstring2string(wstr, str);
			strMessage += str;

			cout << str << endl;

			memset(szBuf, 0, sizeof(char) * MAX_BUF);
		}
	}
 
	std::cout << strMessage << std::endl;
 
	fclose(fp);
	
}


void ReadUnicode()
{
	wchar_t szBuf[MAX_BUF];  
    memset(szBuf, 0, sizeof(wchar_t) * MAX_BUF);  
  
    std::string strMessage;  
  
    FILE * fp = NULL;  
    fp = fopen("unicode.txt", "rb");  
    if (fp != NULL)  
    {  
        // Unicode file should offset wchar_t bits(2 byte) from start.  
        fseek(fp, sizeof(wchar_t), 0);  
        while(fread(szBuf, sizeof(wchar_t), MAX_BUF, fp) > 0)  
        {  
			std::string str;
            StringConversion::Unicode2Ansi(szBuf, wcslen(szBuf), str);  
            strMessage += str;  
			cout << str << endl;
            memset(szBuf, 0, sizeof(wchar_t) * MAX_BUF);  
        }  
    }  
  
    std::cout << strMessage << std::endl;  
  
    fclose(fp);  
}