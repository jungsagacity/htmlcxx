// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ
#include <afxmt.h>			//MFC�߳��¼�����֧��

#include <afxdisp.h>        // MFC �Զ�����
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
 * WM_SHOWERRORINFO ��Ϣ��Ӧ������wParam �������壺
 * 0	ϵͳ��Ϣ
 * 1	���������سɹ�
 * 2	����ʱ���ӷ�����ʧ��
 * 3	����ʱû���ҵ��������
 * 4	����ʱ���ӷ�����ʧ��
 * 5	����ʱû���ҵ��������
 * 6	���سɹ���������ҳ�ļ�ʧ��
 * 7	����ר��������
 * 8	����ר�����������
 * 9	���ӷ�����ʧ��
 * 10	û���ҵ��������
 * 11	��������
 */
#define WM_SHOWERRORINFO (WM_USER+121)

//#define WM_DISABLE_VISIT_TO_DB (WM_USER + 122)
//#define WM_ENABLE_VISIT_TO_DB (WM_USER + 123)
#define MAX_TABLE 100000000
#endif


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
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


