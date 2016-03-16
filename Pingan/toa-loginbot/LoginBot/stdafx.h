
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

// Suppress conflict between stdint.h & safeint.h
// Ref: "https://connect.microsoft.com/VisualStudio/feedback/details/621653/including-stdint-after-intsafe-generates-warnings"
// 
#pragma warning(disable:4005)

#define _INTSAFE_H_INCLUDED_

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#define _WIN32_WINNT _WIN32_WINNT_WINXP
#define _WIN32_IE    _WIN32_IE_IE60SP2

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#include <afxhtml.h>            // MFC HTML view support

#include <afxsock.h>            // MFC socket extensions

#include <MsHtmdid.h>

#include <Wininet.h>

#define PSAPI_VERSION (1)
#include <psapi.h>
#pragma comment(lib, "Psapi.lib")

#include <TlHelp32.h>

#include <gdiplus.h>
using namespace Gdiplus;

#include <stdio.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <comdef.h>

#define USE_CURL

#ifdef USE_CURL
	#define CURL_STATICLIB
	#include <curl/curl.h>
#endif

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#include <sstream>
#include <fstream>
#include <streambuf>
#include <json/json.h>
#include <openssl/md5.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#ifdef USE_DETOURS
//define DETOURS_X86 & DETOURS_32BIT in project settings
#include <detours.h>
#endif

#include "statuscode.h"

// User Messages
#define WM_USER_INPUT_DONE              (WM_USER + 10001)
#define WM_USER_DELAY_RELEASE_LOCK      (WM_USER + 10002)

// Timer id
#define TIMER_ID_OVERTIME_CHECK         (10001)
#define TIMER_ID_UPDATE_GSET            (10002)

// Default Configuration
#define DEFAULT_TIMEOUT_SECONDS         (5 * 60)
