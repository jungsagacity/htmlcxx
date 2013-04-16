// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展
#include <afxmt.h>			//MFC线程事件对象支持

#include <afxdisp.h>        // MFC 自动化类
#include <afxdb.h> 

/////////////////////////////
#include <map>
#include <string>
#include<shlwapi.h>
using namespace std;
#include <afxmt.h>			//MFC support for Thread Event
#include "include\libpq-fe.h"

#ifndef USER_MESSAGE
#define USER_MESSAGE
#define WM_ERROR_MES (WM_USER+100)
#define WM_SHOWTASK (WM_USER+101)
#define WM_SHOWHTML (WM_USER+102)
#define WM_UPDATEIPCTREE (WM_USER+103)
#define WM_FIN_CLEARUP (WM_USER+105)
#define WM_UPDATEPROGRESS (WM_USER+106)
#define WM_FINISHED (WM_USER+107)
#define WM_FIN_UPDATEKEY (WM_USER+108)
#define WM_FIN_UPDATEINDEX (WM_USER+109)
#define WM_FIN_UPDATESUCESS (WM_USER+110)
#define WM_FIN_CREATEINDEX (WM_USER+111)
#define WM_INDEX_ERROR_MES (WM_USER+112)
#define WM_SHOWNAMEPROGRESS (WM_USER+113)
#define WM_HAND_ADD (WM_USER+114)
#define WM_BC_ERROR_MSG (WM_USER+115)
#define WM_READOVER (WM_USER+116)
#define WM_KEYSET_STATUS (WM_USER+117)
#define WM_AFTERSHOWNAME (WM_USER+118)
#define WM_AFTEREXPORT (WM_USER+119)
#define WM_ADDNEWTAB (WM_USER+120)

/* 
 * WM_SHOWERRORINFO 消息响应函数的wParam 参数意义：
 * 0	系统信息
 * 1	检索或下载成功
 * 2	检索时连接服务器失败
 * 3	检索时没有找到相关内容
 * 4	下载时连接服务器失败
 * 5	下载时没有找到相关内容
 * 6	下载成功但保存网页文件失败
 * 7	该条专利待下载
 * 8	该条专利已完成下载
 * 9	连接服务器失败
 * 10	没有找到相关内容
 * 11	正在下载
 */
#define WM_SHOWERRORINFO (WM_USER+121)

//#define WM_DISABLE_VISIT_TO_DB (WM_USER + 122)
//#define WM_ENABLE_VISIT_TO_DB (WM_USER + 123)
#define MAX_TABLE 100000000
#endif


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT




#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


