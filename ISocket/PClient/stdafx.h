
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


#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")
#include <vector>
using namespace std;


#define		PAPER_BUFFER_SIZE	(1024 * 10)		//试题缓冲区1024*10
#define		ANSWER_BUFFER_SIZE	(100)			//答案缓冲区
#define		HEADELEN			(sizeof(HDR))	//包头长度
#define		BUFFER_SIZE			(128)			//接收数据缓冲区64
#define		PAPERHEADHEIGHT		90				//试卷头高度	
#define		ITEMLEFT			40				//试题左边距
#define		ITEMHEIGHT			50				//试题高度


//包类型
#define		PULSE				100				//心跳包	|客户端
#define		STATE				101				//学生状态	|
#define		STNAME				102				//学生姓名	|服务器
#define		PAPER				103				//试卷		|



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


