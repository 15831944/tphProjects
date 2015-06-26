/***************************************************************************************************/
#ifndef INC_DOTSCENEINTERFACE_PREREQ_H
#define INC_DOTSCENEINTERFACE_PREREQ_H
/***************************************************************************************************/
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DOTSCENEINTERFACE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DOTSCENEINTERFACE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
/***************************************************************************************************/
#define DOTSCENEINTERFACE_STATIC

#ifdef _MSC_VER
	#ifdef DOTSCENEINTERFACE_STATIC
		#define DOTSCENEINTERFACE_API
	#else
		#ifdef DOTSCENEINTERFACE_EXPORTS
			#define DOTSCENEINTERFACE_API __declspec(dllexport)
		#else
			#define DOTSCENEINTERFACE_API __declspec(dllimport)
		#endif
	#endif
#else // clever platforms do not need this shite ;)
	#define DOTSCENEINTERFACE_API
#endif
/***************************************************************************************************/
/***************************************************************************************************/
#endif // INC_DOTSCENEINTERFACE_PREREQ_H
/***************************************************************************************************/
