
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持



#pragma warning(disable:4786) 
//该程序定义
#define SERVERPORT		5557
#define THREAD_SLEEP	500

#include <WINSOCK2.H>
#include <list>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

//目录分隔符
#define	SEPERATOR	'|'

//数据包类型
#define ROOT		'R'			//根目录
#define DIRC		'D'			//文件目录
#define FSIZ		'S'			//文件大小
#define FDAT		'F'			//文件数据
#define HEADLEN		(sizeof(hdr))//包头的长度

#define NORMAL		0			//正常接收数据
#define EXCEPT		1			//接收数据异常
#define FINISH		2			//数据接收完毕

//系统发送和接收数据大小
#define BUF_TIMES	10			//设置系统的发送和接收数据缓冲区为默认的10倍，在xp32下为 8k*10
#define BUF_SIZE	(1024 * 60)	//缓冲区的大小

#define TIMER		1			//定时器标识符

//包头该结构在win32xp下为12byte
typedef struct _hdr
{
	char			type;	//类型		
	unsigned short	len;	//数据包长度(头+包体)
	unsigned long	fpos;	//文件开始位置
	unsigned long	flen;	//文件长度
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


