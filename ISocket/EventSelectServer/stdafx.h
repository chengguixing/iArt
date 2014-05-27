
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��
#include <atlconv.h>


//�ó�������
//header
#include <WINSOCK2.H>
#include <afxmt.h>
#pragma comment(lib, "ws2_32.lib")

#define SERVMESSAGE 200
#define USERINFOR 201

#define WM_USER_ACCEPT (WM_USER + 200)
#define WM_USER_READ (WM_USER + 201)
#define WM_USER_CLOSE (WM_USER + 202)

#define		SERVERPORT			5559						//�������˿�
#define MAX_NUM_EVENTS WSA_MAXIMUM_WAIT_EVENTS
#define SERVER_WAIT_TIMEOUT 100

#define CLIENTPACKETLEN (sizeof(CLIENTPACKET))
#define		PACKETHDRLEN		(sizeof(PACKETHDR))			//��ͷ����


//��������ͻ��˷������ݰ�ͷ
typedef struct _packethdr
{
	u_short	type;	//����
	u_short	len;	//���ݰ�����(����)
}PACKETHDR, *PPACKETHDR;


typedef struct _threadparam
{
	HWND hServHwnd;
	BOOL* bRunning;
	WORD* pTotalEvent;
	WSAEVENT* pArrEvent;
	SOCKET* pArrSocket;
	HANDLE hEventExit;
}THREADPARAM, *PTHREADPARAM;










#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


