
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


#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")
#include <vector>
using namespace std;


#define		PAPER_BUFFER_SIZE	(1024 * 10)		//���⻺����1024*10
#define		ANSWER_BUFFER_SIZE	(100)			//�𰸻�����
#define		HEADELEN			(sizeof(HDR))	//��ͷ����
#define		BUFFER_SIZE			(128)			//�������ݻ�����64
#define		PAPERHEADHEIGHT		90				//�Ծ�ͷ�߶�	
#define		ITEMLEFT			40				//������߾�
#define		ITEMHEIGHT			50				//����߶�


//������
#define		PULSE				100				//������	|�ͻ���
#define		STATE				101				//ѧ��״̬	|
#define		STNAME				102				//ѧ������	|������
#define		PAPER				103				//�Ծ�		|



#define SERVERPORT 6000
#define SNLEN 4


typedef struct _header
{
	u_short ustype;
	u_short usLen;
}HDR, *PHDER;










#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


