#include "CVerifyDigitalSignature.h"
#include <windows.h>
#include <WinTrust.h>
#include <Shlwapi.h>
#include <SoftPub.h>
#include <tchar.h>

#pragma comment(lib, "WinTrust.lib")
#pragma comment(lib, "Shlwapi.lib")

extern "C" 
{
	typedef HANDLE HCATADMIN;
	typedef HANDLE HCATINFO;

	typedef struct CATALOG_INFO_ 
	{
		DWORD cbStruct;
		WCHAR wszCatalogFile[MAX_PATH];
	} CATALOG_INFO;

	BOOL WINAPI IsCatalogFile( HANDLE hFile, WCHAR* pwszFileName );

	BOOL WINAPI CryptCATAdminAcquireContext( HCATADMIN* phCatAdmin, const GUID* pgSubsystem, DWORD dwFlags );


	BOOL WINAPI CryptCATAdminCalcHashFromFileHandle( HANDLE hFile, DWORD* pcbHash, BYTE* pbHash, DWORD dwFlags );

	HCATINFO WINAPI CryptCATAdminEnumCatalogFromHash( HCATADMIN hCatAdmin, BYTE* pbHash, DWORD cbHash, DWORD dwFlags, HCATINFO* phPrevCatInfo );

	BOOL WINAPI CryptCATAdminReleaseCatalogContext( HCATADMIN hCatAdmin, HCATINFO hCatInfo, DWORD dwFlags );

	BOOL WINAPI CryptCATAdminReleaseContext( HCATADMIN hCatAdmin, DWORD dwFlags );

	BOOL WINAPI CryptCATCatalogInfoFromContext( HCATINFO hCatInfo, CATALOG_INFO* psCatInfo, DWORD dwFlags );
};



extern HINSTANCE s_hInstance;

int CVerifyDigitalSignature::s_iCheckSign = -1;

CVerifyDigitalSignature::CVerifyDigitalSignature()
{
	NULL;
}

CVerifyDigitalSignature::~CVerifyDigitalSignature()
{
	NULL;
}

BOOL CVerifyDigitalSignature::CheckFileTrust(LPCWSTR lpFileName)
{
	if ( s_iCheckSign == -1 )
	{
		TCHAR szFileName[MAX_PATH];
		::GetModuleFileName(s_hInstance, szFileName, MAX_PATH);

		TCHAR szIniPath[MAX_PATH];
		::PathCombine(szIniPath, szFileName, L"..\\test.ini");

		s_iCheckSign = ::GetPrivateProfileInt(L"check", L"VerifyDigitalSignature", 1, szIniPath);
	}

	if ( s_iCheckSign == 0 )
	{
		return TRUE;
	}

	BOOL bRet = FALSE;
	WINTRUST_DATA wd = { 0 };
	WINTRUST_FILE_INFO wfi = { 0 };
	WINTRUST_CATALOG_INFO wci = { 0 };
	CATALOG_INFO ci = { 0 };

	HCATADMIN hCatAdmin = NULL;
	if ( !CryptCATAdminAcquireContext( &hCatAdmin, NULL, 0 ) )
	{
		return FALSE;
	}

	HANDLE hFile = CreateFileW( lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		CryptCATAdminReleaseContext( hCatAdmin, 0 );
		return FALSE;
	}

	DWORD dwCnt = 100;
	BYTE byHash[100] = {0};
	CryptCATAdminCalcHashFromFileHandle( hFile, &dwCnt, byHash, 0 );
	CloseHandle( hFile );

	LPWSTR pszMemberTag = new WCHAR[dwCnt * 2 + 1];
	for ( DWORD dw = 0; dw < dwCnt; ++dw )
	{
		wsprintfW( &pszMemberTag[dw * 2], L"%02X", byHash[dw] );
	}

	HCATINFO hCatInfo = CryptCATAdminEnumCatalogFromHash( hCatAdmin, byHash, dwCnt, 0, NULL );
	if ( NULL == hCatInfo )
	{
		wfi.cbStruct       = sizeof( WINTRUST_FILE_INFO );
		wfi.pcwszFilePath  = lpFileName;
		wfi.hFile          = NULL;
		wfi.pgKnownSubject = NULL;

		wd.cbStruct            = sizeof( WINTRUST_DATA );
		wd.dwUnionChoice       = WTD_CHOICE_FILE;
		wd.pFile               = &wfi;
		wd.dwUIChoice          = WTD_UI_NONE;
		wd.fdwRevocationChecks = WTD_REVOKE_NONE;
		wd.dwStateAction       = WTD_STATEACTION_IGNORE;
		wd.dwProvFlags         = WTD_SAFER_FLAG;
		wd.hWVTStateData       = NULL;
		wd.pwszURLReference    = NULL;
	}
	else
	{
		CryptCATCatalogInfoFromContext( hCatInfo, &ci, 0 );
		wci.cbStruct             = sizeof( WINTRUST_CATALOG_INFO );
		wci.pcwszCatalogFilePath = ci.wszCatalogFile;
		wci.pcwszMemberFilePath  = lpFileName;
		wci.pcwszMemberTag       = pszMemberTag;

		wd.cbStruct            = sizeof( WINTRUST_DATA );
		wd.dwUnionChoice       = WTD_CHOICE_CATALOG;
		wd.pCatalog            = &wci;
		wd.dwUIChoice          = WTD_UI_NONE;
		wd.fdwRevocationChecks = WTD_STATEACTION_VERIFY;
		wd.dwProvFlags         = 0;
		wd.hWVTStateData       = NULL;
		wd.pwszURLReference    = NULL;
	}
	GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;
	HRESULT hr  = S_OK;
	try
	{	// 系统库有崩溃
		hr = WinVerifyTrust( NULL, &action, &wd );
	}
	catch (...)
	{
	}

	bRet        = SUCCEEDED( hr );

	if ( NULL != hCatInfo )
	{
		CryptCATAdminReleaseContext( hCatInfo, 0 );
	}
	CryptCATAdminReleaseCatalogContext( hCatAdmin, hCatInfo, 0 );

	CryptCATAdminReleaseContext( hCatAdmin, 0 );
	delete[] pszMemberTag;
	return bRet;
}

	
BOOL CVerifyDigitalSignature::GetOwnerName(const TCHAR* fileName, TCHAR* ownerBuff)
{
	if (fileName == NULL || ownerBuff == NULL)
	{
		return FALSE;
	}
	if (!PathFileExists(fileName))
	{
		return FALSE;
	}
	HCERTSTORE hStore = NULL;
	HCRYPTMSG hMsg = NULL; 
	PCCERT_CONTEXT pCertContext = NULL;
	CERT_INFO CertInfo;
	BOOL fResult;   
	DWORD dwEncoding, dwContentType, dwFormatType;
	PCMSG_SIGNER_INFO pSignerInfo = NULL;
	DWORD dwSignerInfo;
	fResult = CryptQueryObject(CERT_QUERY_OBJECT_FILE,
		fileName,
		CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
		CERT_QUERY_FORMAT_FLAG_BINARY,
		0,
		&dwEncoding,
		&dwContentType,
		&dwFormatType,
		&hStore,
		&hMsg,
		NULL);
	if (!fResult)
	{
		return FALSE;
	}

	// Get signer information size.
	fResult = CryptMsgGetParam(hMsg, 
		CMSG_SIGNER_INFO_PARAM, 
		0, 
		NULL, 
		&dwSignerInfo);
	if (!fResult)
	{
		if (hStore != NULL)
		{
			CertCloseStore(hStore, 0);
		}

		if (hMsg != NULL) 
		{
			CryptMsgClose(hMsg);
		}
		return FALSE;
	}

	// Allocate memory for signer information.
	pSignerInfo = (PCMSG_SIGNER_INFO)new char[dwSignerInfo];
	if (pSignerInfo == NULL)
	{
		if (hStore != NULL)
		{
			CertCloseStore(hStore, 0);
		}
		if (hMsg != NULL) 
		{
			CryptMsgClose(hMsg);
		}
		return FALSE;
	}

	// Get Signer Information.
	fResult = CryptMsgGetParam(hMsg, 
		CMSG_SIGNER_INFO_PARAM, 
		0, 
		(PVOID)pSignerInfo, 
		&dwSignerInfo);
	if (!fResult)
	{
		if (hStore != NULL)
		{
			CertCloseStore(hStore, 0);
		}
		if (hMsg != NULL) 
		{
			CryptMsgClose(hMsg);
		}
		delete [] pSignerInfo;
		return FALSE;
	}

	CertInfo.Issuer = pSignerInfo->Issuer;
	CertInfo.SerialNumber = pSignerInfo->SerialNumber;

	pCertContext = CertFindCertificateInStore(hStore,
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		0,
		CERT_FIND_SUBJECT_CERT,
		(PVOID)&CertInfo,
		NULL);
	if (!pCertContext)
	{
		if (hStore != NULL)
		{
			CertCloseStore(hStore, 0);
		}
		if (hMsg != NULL) 
		{
			CryptMsgClose(hMsg);
		}
		delete [] pSignerInfo;    
		return FALSE;
	}

	DWORD dwData = CertGetNameString(pCertContext, 
		CERT_NAME_SIMPLE_DISPLAY_TYPE, 
		0,
		NULL,
		NULL,
		0);
	if (!dwData)
	{
		if (hStore != NULL)
		{
			CertCloseStore(hStore, 0);
		}
		if (hMsg != NULL) 
		{
			CryptMsgClose(hMsg);
		}
		delete [] pSignerInfo;    
		return FALSE;
	}

	LPTSTR szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
	if (!szName)
	{
		if (hStore != NULL)
		{
			CertCloseStore(hStore, 0);
		}
		if (hMsg != NULL) 
		{
			CryptMsgClose(hMsg);
		}
		delete [] pSignerInfo;    
		return FALSE;
	}

	if (!(CertGetNameString(pCertContext, 
		CERT_NAME_SIMPLE_DISPLAY_TYPE,
		0,
		NULL,
		szName,
		dwData)))
	{
		if (hStore != NULL)
		{
			CertCloseStore(hStore, 0);
		}
		if (hMsg != NULL) 
		{
			CryptMsgClose(hMsg);
		}
		delete [] pSignerInfo;    
		return FALSE;
	}

	_tcscpy(ownerBuff, szName);
	LocalFree(szName);
	szName = NULL;

	if (hStore != NULL)
	{
		CertCloseStore(hStore, 0);
	}
	if (hMsg != NULL) 
	{
		CryptMsgClose(hMsg);
	}
	delete [] pSignerInfo;
	return TRUE;
}


BOOL CVerifyDigitalSignature::CheckOwnerName(const TCHAR* patchPath, const TCHAR* patchOwnerName)
{
	if (patchPath == NULL || patchOwnerName == NULL)
	{
		return FALSE;
	}
	TCHAR tchOwnerBuffer[MAX_PATH] = {0};
	BOOL bRet = GetOwnerName(patchPath, tchOwnerBuffer);
	if (!bRet)
	{
		return FALSE;
	}

	if (_tcsicmp(patchOwnerName, tchOwnerBuffer) == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



BOOL CVerifyDigitalSignature::GetFileSign(const TCHAR* filename, TCHAR* buff, DWORD len)
{
	if(filename == NULL || buff == NULL || len <= 0)
	{
		//XL_WARN(_T("Invalid param"));
		return FALSE;
	}
	//XL_DEBUG(_T("buffer size : %d, file : %s"), len, filename);
	if(!PathFileExists(filename))
	{
		//XL_WARN(_T("File not exist"));
		return FALSE;
	}
	HCERTSTORE hStore = NULL;
	HCRYPTMSG hMsg = NULL; 
	PCCERT_CONTEXT pCertContext = NULL;
	BOOL fResult;   
	DWORD dwEncoding, dwContentType, dwFormatType;
	PCMSG_SIGNER_INFO pSignerInfo = NULL;
	DWORD dwSignerInfo;
	fResult = CryptQueryObject(CERT_QUERY_OBJECT_FILE,
		filename,
		CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
		CERT_QUERY_FORMAT_FLAG_BINARY,
		0,
		&dwEncoding,
		&dwContentType,
		&dwFormatType,
		&hStore,
		&hMsg,
		NULL);
	if (!fResult)
	{
		//XL_WARN(_T("CryptQueryObject failed, error code : %d"), GetLastError());
		return FALSE;
	}

	// Get signer information size.
	fResult = CryptMsgGetParam(hMsg, 
		CMSG_SIGNER_INFO_PARAM, 
		0, 
		NULL, 
		&dwSignerInfo);
	if (!fResult)
	{
		//XL_WARN(_T("CryptMsgGetParam failed, error code : %d"), GetLastError());
		if (hStore != NULL)
		{
			CertCloseStore(hStore, 0);
		}

		if (hMsg != NULL) 
		{
			CryptMsgClose(hMsg);
		}
		return FALSE;
	}

	// Allocate memory for signer information.
	pSignerInfo = (PCMSG_SIGNER_INFO)new char[dwSignerInfo];
	if (pSignerInfo == NULL)
	{
		//XL_WARN(_T("allocate memory for Signer Info Failed"));
		if (hStore != NULL)
		{
			CertCloseStore(hStore, 0);
		}
		if (hMsg != NULL) 
		{
			CryptMsgClose(hMsg);
		}
		return FALSE;
	}

	// Get Signer Information.
	// 又是系统库崩溃
	try
	{
		fResult = CryptMsgGetParam(hMsg, 
			CMSG_SIGNER_INFO_PARAM, 
			0, 
			(PVOID)pSignerInfo, 
			&dwSignerInfo);
	}
	catch (...)
	{
	}

	if (!fResult)
	{
		//XL_WARN(_T("CryptMsgGetParam failed, error code : %d"), GetLastError());
		if (hStore != NULL)
		{
			CertCloseStore(hStore, 0);
		}
		if (hMsg != NULL) 
		{
			CryptMsgClose(hMsg);
		}
		delete [] pSignerInfo;
		return FALSE;
	}

	DWORD dwData = pSignerInfo->SerialNumber.cbData;
	if (len < (dwData * 2 + 1))
	{
		//XL_WARN(_T("Buffer too small")); 
		if (hStore != NULL)
		{
			CertCloseStore(hStore, 0);
		}
		if (hMsg != NULL) 
		{
			CryptMsgClose(hMsg);
		}
		delete [] pSignerInfo;    
		return FALSE;
	}

	TCHAR hex_data_map[] = _T("0123456789abcdef");
	DWORD n;
	for (/*DWORD */n = 0; n < dwData; n++)
	{
		BYTE byte = pSignerInfo->SerialNumber.pbData[dwData - (n + 1)];
		buff[2 * n] = hex_data_map[(byte >> 4)];
		buff[2 * n + 1] = hex_data_map[(byte & 0x0f)];
	}
	buff[2 * n] = TCHAR('\0');
	if (hStore != NULL)
	{
		CertCloseStore(hStore, 0);
	}
	if (hMsg != NULL) 
	{
		CryptMsgClose(hMsg);
	}
	delete [] pSignerInfo;
	return TRUE;
}