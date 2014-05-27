
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





//该程序声明
//////////////////////////////////////////////////////////////////////////
#include <afxcview.h>
#include "winsock2.h"
#include "mmsystem.h"
#include <afxmt.h>
#pragma	comment(lib, "ws2_32.lib.")

//服务器与客户端之间发送的数据包头
typedef struct _packethdr
{
	u_short		type;	//类型
	u_short		len;	//数据包长度(包体)
}PACKETHDR, *PPACKETHDR;

//客户端之间发送数据包
typedef struct _clientpacket
{
	int		type;	//类型		
	u_short	xPos;	//水平位置
	u_short	yPos;	//垂直位置
}CLIENTPACKET, *PCLIENTPACKET;

//线程函数参数
typedef struct _threadparam
{
	HWND	hClientHwnd;	//主窗口句柄
	BOOL	*pRunning;		//线程运行状态
	WORD	*pTotalEvent;	//事件对象数量
	WSAEVENT*pArrEvent;		//事件对象数组
	SOCKET	*pArrSocket;	//套接字句柄数组
	HANDLE	hEventExit;		//线程退出事件句柄
}THREADPARAM, *PTHREADPARAM;


#define		USERWIEWWIDTH		150							//用户信息视图宽度
#define		SERVERPORT			5559						//服务器端口
#define		MAX_NUM_EVENTS		WSA_MAXIMUM_WAIT_EVENTS		//最大事件对象数量
#define		CLIENT_WAIT_TIMEOUT	100							//线程睡眠时间

#define		CLIENTPACKETLEN		(sizeof(CLIENTPACKET))		//数据包长度
#define		PACKETHDRLEN		(sizeof(PACKETHDR))			//包头长度

#define		DIVISIONS			15							//格子数量
#define		CELLSIZE			40							//格子宽度


//请求与应答
#define		ACCEPT				101							//接受
#define		DECLINE				100							//拒绝
//请求新开局与拒绝
#define		NEWGAME				160							//新开局
#define		NONEWGAME			170							//拒绝新开局
//退出游戏
#define		DISCONNECT			99							//断开连接
//认输
#define		SURRENDER			150							//投降
//胜负结果
#define		COORDS				102							//普通的一步棋
#define		UWIN				110							//对方胜
#define		IWIN				120							//我方胜(暂时)
#define		ULOSE				130							//对方负
#define		EVEN				140							//平局




//自定义消息
#define		WM_USER_CLIENTACCEPT	(WM_USER + 300)//FD_ACCEPT
#define		WM_USER_CLIENTREAD		(WM_USER + 301)//FD_READ
#define		WM_USER_CLIENTCLOSE		(WM_USER + 302)//FD_CLOSE

//服务器与客户端之间数据类型
#define		SERVMESSAGE				200							//服务器消息
#define		USERINFOR				201							//用户信息










#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


