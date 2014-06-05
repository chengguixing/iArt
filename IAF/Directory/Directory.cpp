#include "Directory.h"

#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <Shellapi.h>
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
	bool iDirectory::DeleteFileOrDirectory(std::string const& str)
	{
		char szFilePath[MAX_PATH] = {0};
		strcpy(szFilePath, str.c_str());
		strcat(szFilePath, "\0");
		SHFILEOPSTRUCTA FileOp = {0};
		FileOp.fFlags = FOF_ALLOWUNDO | //允许放回回收站
			FOF_NOCONFIRMATION; //不出现确认对话框
		FileOp.pFrom = szFilePath;
		FileOp.pTo = NULL; //一定要是NULL
		FileOp.wFunc = FO_DELETE; //删除操作
		return SHFileOperationA(&FileOp) == 0;
	}
}