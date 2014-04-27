#include "stdafx.h"
#include "HookFunction.h"



HookFunction::pInit HookFunction::m_Init = NULL;
HookFunction::pTerm HookFunction::m_Term = NULL;
HookFunction::pGetLastInputTime HookFunction::m_GetLastInputTime = NULL;


BOOL HookFunction::Init()
{
	HINSTANCE hInst = LoadLibrary(_T("SetWindowsHookExDll.dll"));
	if (NULL == hInst)
	{
		return FALSE;
	}
	
	m_Init = (pInit)GetProcAddress(hInst, "Init");
	m_Term = (pTerm)GetProcAddress(hInst, "Term");
	m_GetLastInputTime = (pGetLastInputTime)GetProcAddress(hInst, "GetLastInputTime");


	if (NULL == m_Init ||
		NULL == m_Term ||
		NULL == m_GetLastInputTime)
	{
		return FALSE;
	}

	m_Init();
	return TRUE;
}

void HookFunction::Term()
{
	m_GetLastInputTime();
}

DWORD HookFunction::GetLastInputTime()
{
	return m_GetLastInputTime();
}