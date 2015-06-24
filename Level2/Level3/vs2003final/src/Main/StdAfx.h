// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//


#pragma once

#pragma warning( disable : 4786 )

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

#include <valarray>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxcview.h>		// for CTreeView
#include <afxole.h>			// for AfxOleInit (drag & drop)
#include <afxpriv.h>		// for CDockContext, AfxLoadString
#include <afxtempl.h>		// for CMap, CArray
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <gl/glew.h>
#include <gl/wglew.h>
#include "htmlhelp.h"

#include <afxdlgs.h>

#include <vector>
#include <list>
#include <map>

#define DefaultAirportDBFolder "Databases"
#define CustomerAirportDBFolder "ImportDB"

// Xtreme Toolkit library
#include <XTToolkitPro.h>
#include <afxdhtml.h>
//#include "../Database/DBElementCollection.h"
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

