// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef WINVER
#define WINVER 0x0500			// Windows 2000 or later
#endif

#ifndef _WIN32_WINNT		
#define _WIN32_WINNT 0x0500		// Windows 2000 or later
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0501		// Internet Explorer 5.01 or later.
#endif							
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:

// TODO: reference additional headers your program requires here
#include <afxdisp.h>        // MFC Automation classes
#include <afxwin.h>         // MFC core and standard components


#pragma warning(disable:4244)
#pragma warning(disable:4305)
#pragma warning(disable:4193)
#include "AfxHeader.h"
#include <Ogre.h>
#include "ogreutil.h"
#define RENDERENGINE_EXPORTS
#include "RenderEngineAPI.h"
