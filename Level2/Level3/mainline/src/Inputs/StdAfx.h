#ifndef __INPUTS_STDAFX_H__
#define __INPUTS_STDAFX_H__

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
#define _CRT_SECURE_NO_WARNINGS
#include <afx.h>
#include <afxcmn.h>

#include <gl/glew.h>
#include <gl/wglew.h>

#include <vector>
#include <list>
#include <map>

#include "InputsDll.h"
#pragma message("Inputs -> Stdafx.h")


#endif  // include guard