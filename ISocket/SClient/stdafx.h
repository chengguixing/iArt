
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



#pragma warning(disable:4786) 
//�ó�����
#define SERVERPORT		5557
#define THREAD_SLEEP	500

#include <WINSOCK2.H>
#include <list>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

//Ŀ¼�ָ���
#define	SEPERATOR	'|'

//���ݰ�����
#define ROOT		'R'			//��Ŀ¼
#define DIRC		'D'			//�ļ�Ŀ¼
#define FSIZ		'S'			//�ļ���С
#define FDAT		'F'			//�ļ�����
#define HEADLEN		(sizeof(hdr))//��ͷ�ĳ���

#define NORMAL		0			//������������
#define EXCEPT		1			//���������쳣
#define FINISH		2			//���ݽ������

//ϵͳ���ͺͽ������ݴ�С
#define BUF_TIMES	10			//����ϵͳ�ķ��ͺͽ������ݻ�����ΪĬ�ϵ�10������xp32��Ϊ 8k*10
#define BUF_SIZE	(1024 * 60)	//�������Ĵ�С

#define TIMER		1			//��ʱ����ʶ��

//��ͷ�ýṹ��win32xp��Ϊ12byte
typedef struct _hdr
{
	char			type;	//����		
	unsigned short	len;	//���ݰ�����(ͷ+����)
	unsigned long	fpos;	//�ļ���ʼλ��
	unsigned long	flen;	//�ļ�����
}hdr, *phdr;










#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


