#ifndef RESULTS_DLL_H
#define RESULTS_DLL_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef RESULTS_TRANSFER
	#if defined RESULTS_EXPORT
        #define RESULTS_TRANSFER  __declspec( dllexport ) 
	#elif defined RESULTS_IMPORT
        #define RESULTS_TRANSFER __declspec( dllimport ) 
	#else
        #define RESULTS_TRANSFER
	#endif
#endif

#endif
