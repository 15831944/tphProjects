#ifndef REPORTS_DLL_H
#define REPORTS_DLL_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef REPORTS_TRANSFER
	#if defined REPORTS_EXPORT
        #define REPORTS_TRANSFER  __declspec( dllexport ) 
	#elif defined REPORTS_IMPORT
        #define REPORTS_TRANSFER __declspec( dllimport ) 
	#else
        #define REPORTS_TRANSFER
	#endif
#endif

#endif
