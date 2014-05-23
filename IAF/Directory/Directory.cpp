#include "Directory.h"

#include <windows.h>
#include <tchar.h>
#include <iostream>
using namespace std;


namespace IAF
{

	void iDirectory::GetRootDirectory(std::wstring& strRootDirectory)
	{
		strRootDirectory = L"";
		TCHAR szTemp[BUFSIZE] = _T("");
		if(GetLogicalDriveStrings(BUFSIZE - 1, szTemp))
		{
			TCHAR *p = szTemp;
			while(*p)
			{
				strRootDirectory += p;
				p = p + _tcslen(p) + 1;
			}
		}
		return;
	}


}