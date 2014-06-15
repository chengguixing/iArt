#include <afx.h>
#include <stdio.h>

int main(void)
{
	int lo, hi;
	CString str;
	CStdioFile fFibo;

	fFibo.Open(TEXT("FIBO.DAT"), CFile::modeWrite | CFile::modeCreate | CFile::typeText);
	str.Format(TEXT("%s\n"), TEXT("Fibonacci sequencee, less then 100 : "));
	_tprintf(TEXT("%s"), (LPCTSTR)str);
	fFibo.WriteString(str);

	lo = hi = 1;
	str.Format(TEXT("%d\n"), lo);
	_tprintf(TEXT("%s"), (LPCTSTR)str);
	fFibo.WriteString(str);

	while (hi < 100)
	{
		str.Format(TEXT("%d\n"), hi);
		_tprintf(TEXT("%s"), (LPCTSTR)str);
		fFibo.WriteString(str);
		hi = lo + hi;
		lo = hi - lo;
	}

	fFibo.Close();

	system("pause");
	return 0;
}