#ifndef __RESULTS_STDAFX_H__
#define __RESULTS_STDAFX_H__

#pragma once


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN	// Exclude rarely-used items from Windows headers.
#endif

#define  _CRT_SECURE_NO_WARNINGS

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


#include <afx.h>

#include <Inputs/InputsDll.h>

#endif  // include guard