#include "my.h"

extern CMyWinApp theApp;

CWinApp* AfxGetApp()
{
	return theApp.m_pCurrentWinApp;
}