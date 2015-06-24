#ifndef __REPORTS_STDAFX_H__
#define __REPORTS_STDAFX_H__

#pragma once


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN	// Exclude rarely-used items from Windows headers.
#endif

#ifndef WINVER
#define WINVER 0x0500			// Windows 2000 or later
#endif

#ifndef _WIN32_WINNT		
#define _WIN32_WINNT 0x0500		// Windows 2000 or later
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0501		// Internet Explorer 5.01 or later.
#endif		


#include <afxwin.h>         // MFC core and standard components
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <gl/glew.h>
#include <gl/wglew.h>

#include <vector>
#include <list>
#include <map>

#include <Inputs/InputsDll.h>

#endif  // include guard