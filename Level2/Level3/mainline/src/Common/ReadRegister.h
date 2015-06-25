#pragma once
 #include <windows.h>
#define HKEY_LOCAL_MACHINE          (( HKEY ) (ULONG_PTR)((LONG)0x80000002) )
#define CLICENSE_REGISTRY_PATH_ARCTERM_KEY          "SOFTWARE\\Aviation Research Corporation\\ARCTerm\\Paths"
#define CLICENSE_REGISTRY_PATH_ARCTERM_VALUE        "AppPath"

// ARCCapture...
#define CLICENSE_REGISTRY_PATH_ARCCAPTURE_KEY       "SOFTWARE\\Aviation Research Corporation\\ARCCapture\\Paths"
#define CLICENSE_REGISTRY_PATH_ARCCAPTURE_VALUE     "AppPath"

// License registry locations...

// ARCTerm...
#define CLICENSE_REGISTRY_LICENSE_ARCTERM_KEY       "SOFTWARE\\Aviation Research Corporation\\ARCTerm\\WindowPos"
#define CLICENSE_REGISTRY_LICENSE_ARCTERM_VALUE     "fonts"

// ARCCapture...
#define CLICENSE_REGISTRY_LICENSE_ARCCAPTURE_KEY    "SOFTWARE\\Aviation Research Corporation\\ARCCapture\\WindowPos"
#define CLICENSE_REGISTRY_LICENSE_ARCCAPTURE_VALUE  "fonts"

#include "INIFileHandle.h"
class License
{
public:
	BYTE m_font[2048] ;
	TCHAR m_height[2048] ;
	TCHAR m_left[2048] ;
	BYTE m_setting[2048] ;
	TCHAR m_top[2048] ;
	TCHAR m_width[2048] ;
	DWORD m_fontsize ;
	DWORD msettingsize ;
	License()
	{
		m_fontsize = 0 ;
        msettingsize = 0 ;
	    memset(m_font,0,sizeof(TCHAR)*2048) ;
		memset(m_height,0,sizeof(TCHAR)*2048) ;
		memset(m_left,0,sizeof(TCHAR)*2048) ;
		memset(m_setting,0,sizeof(TCHAR)*2048) ;
		memset(m_top,0,sizeof(TCHAR)*2048) ;
		memset(m_width,0,sizeof(TCHAR)*2048) ;
	}
};
class DBData 
{
public:
	TCHAR m_name[2048] ;
	TCHAR m_PW[2048] ;
	TCHAR m_server[2048] ;
	TCHAR m_username[2048] ;
	TCHAR m_version[2048] ;
	DBData()
	{
		memset(m_name,0,sizeof(TCHAR)*2048) ;
		memset(m_PW,0,sizeof(TCHAR)*2048) ;
		memset(m_server,0,sizeof(TCHAR)*2048) ;
		memset(m_username,0,sizeof(TCHAR)*2048) ;
		memset(m_version,0,sizeof(TCHAR)*2048) ;
	}
};
class PathData
{
public:
	TCHAR m_AppPath[2048] ;
	PathData()
	{
	memset(m_AppPath,0,sizeof(TCHAR)*2048) ;
	}
};
class ResourceData
{
public:
   TCHAR m_Version[2048] ;
   ResourceData()
   {
	   memset(m_Version,0,sizeof(TCHAR)*2048) ;
   }
};
class UnitData
{
public:
	TCHAR m_leng[2048] ;
	TCHAR m_weight[2048] ;
	UnitData()
	{
memset(m_leng,0,sizeof(TCHAR)*2048) ;
memset(m_weight,0,sizeof(TCHAR)*2048) ;
	}
};
class CReadRegister
{
public:
	CReadRegister(void) ;
	~CReadRegister(void);
public:
	BOOL ReadRegister(CString& _errorMessage) ;
	BOOL CreateINIFile() ;
public:
	BOOL ReadRegisterDB(CString& _errorMes) ;
	BOOL ReadRegisterPath(CString& _errorMes) ;
	BOOL ReadRegisterResourse(CString& _errorMes) ;
    BOOL ReadRegisterUnit(CString& _errorMes) ;
    BOOL ReadRegisterLicense(CString& _errorMes) ;


public:
    License m_license ;
	DBData m_DB ;
	PathData m_Path ;
	ResourceData m_Resouce ;
	UnitData m_Unit;
};
