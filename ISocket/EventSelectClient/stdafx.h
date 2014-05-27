
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





//�ó�������
//////////////////////////////////////////////////////////////////////////
#include <afxcview.h>
#include "winsock2.h"
#include "mmsystem.h"
#include <afxmt.h>
#pragma	comment(lib, "ws2_32.lib.")

//��������ͻ���֮�䷢�͵����ݰ�ͷ
typedef struct _packethdr
{
	u_short		type;	//����
	u_short		len;	//���ݰ�����(����)
}PACKETHDR, *PPACKETHDR;

//�ͻ���֮�䷢�����ݰ�
typedef struct _clientpacket
{
	int		type;	//����		
	u_short	xPos;	//ˮƽλ��
	u_short	yPos;	//��ֱλ��
}CLIENTPACKET, *PCLIENTPACKET;

//�̺߳�������
typedef struct _threadparam
{
	HWND	hClientHwnd;	//�����ھ��
	BOOL	*pRunning;		//�߳�����״̬
	WORD	*pTotalEvent;	//�¼���������
	WSAEVENT*pArrEvent;		//�¼���������
	SOCKET	*pArrSocket;	//�׽��־������
	HANDLE	hEventExit;		//�߳��˳��¼����
}THREADPARAM, *PTHREADPARAM;


#define		USERWIEWWIDTH		150							//�û���Ϣ��ͼ���
#define		SERVERPORT			5559						//�������˿�
#define		MAX_NUM_EVENTS		WSA_MAXIMUM_WAIT_EVENTS		//����¼���������
#define		CLIENT_WAIT_TIMEOUT	100							//�߳�˯��ʱ��

#define		CLIENTPACKETLEN		(sizeof(CLIENTPACKET))		//���ݰ�����
#define		PACKETHDRLEN		(sizeof(PACKETHDR))			//��ͷ����

#define		DIVISIONS			15							//��������
#define		CELLSIZE			40							//���ӿ��


//������Ӧ��
#define		ACCEPT				101							//����
#define		DECLINE				100							//�ܾ�
//�����¿�����ܾ�
#define		NEWGAME				160							//�¿���
#define		NONEWGAME			170							//�ܾ��¿���
//�˳���Ϸ
#define		DISCONNECT			99							//�Ͽ�����
//����
#define		SURRENDER			150							//Ͷ��
//ʤ�����
#define		COORDS				102							//��ͨ��һ����
#define		UWIN				110							//�Է�ʤ
#define		IWIN				120							//�ҷ�ʤ(��ʱ)
#define		ULOSE				130							//�Է���
#define		EVEN				140							//ƽ��




//�Զ�����Ϣ
#define		WM_USER_CLIENTACCEPT	(WM_USER + 300)//FD_ACCEPT
#define		WM_USER_CLIENTREAD		(WM_USER + 301)//FD_READ
#define		WM_USER_CLIENTCLOSE		(WM_USER + 302)//FD_CLOSE

//��������ͻ���֮����������
#define		SERVMESSAGE				200							//��������Ϣ
#define		USERINFOR				201							//�û���Ϣ










#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


