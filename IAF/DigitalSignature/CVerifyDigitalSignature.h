#ifndef _CVERIFYDIGITALSIGNATURE_H_11851929_8C5C_4D67_B055_A962188C03F6
#define _CVERIFYDIGITALSIGNATURE_H_11851929_8C5C_4D67_B055_A962188C03F6

#include <windows.h>

class CVerifyDigitalSignature
{
public:
	CVerifyDigitalSignature();
	~CVerifyDigitalSignature();

	static BOOL CheckFileTrust(LPCWSTR);
	static BOOL GetOwnerName(const TCHAR*, TCHAR*);
	static BOOL CheckOwnerName(const TCHAR*, const TCHAR*);
	static BOOL GetFileSign(const TCHAR*, TCHAR*, DWORD);

private:
	static int s_iCheckSign;
};


#endif