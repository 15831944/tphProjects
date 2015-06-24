#ifndef INPUTS_DLL_H
#define INPUTS_DLL_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef INPUTS_TRANSFER
	#if defined INPUTS_EXPORT
        #define INPUTS_TRANSFER  __declspec( dllexport ) 
	#elif defined INPUTS_IMPORT
        #define INPUTS_TRANSFER __declspec( dllimport ) 
	#else
        #define INPUTS_TRANSFER
	#endif
#endif

#endif
