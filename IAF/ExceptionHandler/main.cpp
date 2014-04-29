/*
	author   : izhuxin
	date     : 2014/04/29
	function : simulate exceptions
	reference: http://www.codeproject.com/Articles/207464/Exception-Handling-in-Visual-Cplusplus
*/


#include <stdlib.h>
#include <iostream>
#include <windows.h>
#include "CrashHandler.h"
#include <float.h>
using namespace std;

#define BIG_NUMBER 0x1fffffff
#pragma warning(disable: 4717)

int RecurseAlloc();

void sigfpe_test();

class CDerived;
class CBase
{
public:
	CBase(CDerived* derived) : m_pDerived(derived)
	{
		NULL;
	}
	~CBase();
	virtual void function(void) = 0;

	CDerived* m_pDerived;
};


#pragma warning(disable: 4355)
class CDerived : public CBase
{
public:
	CDerived() : CBase(this)	//C4355
	{
		NULL;
	}
	virtual void function(void)
	{
		;
	}
};
CBase::~CBase()
{
	m_pDerived->function();
}


int main(void) 
{ 
	iCrashHandler ch;
	ch.SetProcessExceptionHandlers();
	ch.SetThreadExceptionHandlers();

	cout << "Choose an exception type:" << endl;
	cout << "0 ---> SEH exception" << endl;
	cout << "1 ---> terminate" << endl;
	cout << "2 ---> unexcepted" << endl;
	cout << "3 ---> pure virtual method call" << endl;
	cout << "4 ---> invalid parameter" << endl;
	cout << "5 ---> new operator fault" << endl;
	cout << "6 ---> SIGABRT" << endl;
	cout << "7 ---> SIGFPE" << endl;
	cout << "8 ---> SIGILL" << endl;
	cout << "9 ---> SIGINT" << endl;
	cout << "10---> SIGSEGV" << endl;
	cout << "11---> SIGTERM" << endl;
	cout << "12---> RaiseException" << endl;
	cout << "13--->throw C++ typed exception" << endl;
	cout << "Your choice > " << endl;

	int ExceptionType = 0;
	cin >> ExceptionType;

	switch (ExceptionType)
	{
	case 0:
		{
			int* p = 0;
#pragma warning(disable: 6011)
			*p = 0;
#pragma warning(default: 6011)
		}
		break;
	case 1:
		{
			terminate();
		}
		break;
	case 2:
		{
			unexpected();
		}
		break;
	case 3:
		{
			CDerived derived;
		}
		break;
	case 4:
		{
			char* formatString;
			formatString = NULL;
#pragma warning(disable : 6387)
			printf(formatString);
#pragma warning(default: 6387)
		}
		break;
	case 5:
		{
			RecurseAlloc();
		}
		break;
	case 6:
		{
			abort();
		}
		break;
	case 7:
		{
			sigfpe_test();
		}
		break;
	case 8:
		{
			raise(SIGILL);
		}
		break;
	case 9:
		{
			raise(SIGINT);
		}
		break;
	case 10:
		{
			raise(SIGSEGV);
		}
		break;
	case 11:
		{
			raise(SIGTERM);
		}
		break;
	case 12:
		{
			RaiseException(123, EXCEPTION_NONCONTINUABLE, 0, NULL);
		}
		break;
	case 13:
		{
			throw 13;
		}
		break;
	default:
		{
			printf("Unknown exception type specified.");
		}
	}

	system("pause");
	return 0;
} 

int RecurseAlloc()
{
	int* pi = new int[BIG_NUMBER];
	pi;
	RecurseAlloc();
	return 0;
}


void sigfpe_test()
{
	_clearfp();
	unsigned int cw;
	_controlfp_s(&cw, 0, 0);
	cw &= ~(EM_OVERFLOW | EM_UNDERFLOW | EM_ZERODIVIDE | EM_DENORMAL | EM_INVALID);
	unsigned int cwOriginal;
	_controlfp_s(&cwOriginal, cw, MCW_EM);

	float a = 1;
	float b = 0;
	float c = a / b;

	c;
}