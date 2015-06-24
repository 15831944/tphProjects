#ifndef ENGINE_DLL_H
#define ENGINE_DLL_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef ENGINE_TRANSFER
	#if defined ENGINE_EXPORT
        #define ENGINE_TRANSFER  __declspec( dllexport ) 
	#elif defined ENGINE_IMPORT
        #define ENGINE_TRANSFER __declspec( dllimport ) 
	#else
        #define ENGINE_TRANSFER
	#endif
#endif

#endif
