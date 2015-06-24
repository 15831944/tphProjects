#ifndef __ENGINE_STDAFX_H__
#define __ENGINE_STDAFX_H__

#pragma once

#define  _CRT_SECURE_NO_WARNINGS

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
#define _CRT_SECURE_NO_DEPRECATE
#define _SCL_SECURE_NO_WARNINGS


#include <valarray>
#include <afxwin.h>         // MFC core and standard components
#include <afxdlgs.h>		// for CFileDialog

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <gl/glew.h>
#include <gl/wglew.h>

#include <vector>
#include <list>
#include <map>

#pragma message("Engine -> Stdafx.h")

#include "enginedll.h"
#include <Inputs/InputsDll.h>
#endif  // include guard