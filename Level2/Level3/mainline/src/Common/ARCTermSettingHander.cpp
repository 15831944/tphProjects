#include "StdAfx.h"
#include ".\arctermsettinghander.h"
#include "INIFileHandle.h"
#include "ReadRegister.h"
#include "HeapObj.h"

CARCTermSettingHander::CARCTermSettingHander(void)
{
}

CARCTermSettingHander::~CARCTermSettingHander(void)
{
}
 CString CARCTermSettingHander::GetSettingINIFilePath() 
{
	CString m_Path ;

	CReadRegister regReader;//HEAPOBJ(CReadRegister, regReader, CReadRegister());
	//CReadRegister regReader ;
	if(!regReader.ReadRegisterPath(CString()))
		return  m_Path;
	m_Path.Append(regReader.m_Path.m_AppPath) ;
	m_Path.Append("\\") ;
	m_Path.Append("Arcterm Setting.ini") ;
	return m_Path ;
}
 UINT CARCTermSettingHander::GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
{
	CCfgData INIFileHandle ;
	if(!INIFileHandle.LoadCfgData(GetSettingINIFilePath()))
		return nDefault ;
	INIFileHandle.SetGroup(lpszSection) ;
	long _val = 0 ;
	if(lpszEntry == NULL)
		return nDefault ;
	if(!INIFileHandle.GetLongData(lpszEntry,_val))
		return nDefault ;
	else
		return _val ;
}
 CString CARCTermSettingHander::GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault) 
{
	if(lpszEntry == NULL )
		return CString() ;
	CCfgData INIFileHandle ;
	if(!INIFileHandle.LoadCfgData(GetSettingINIFilePath()))
		return lpszDefault ;
	INIFileHandle.SetGroup(lpszSection) ;
	CString _val ;
	if(lpszEntry == NULL)
		return lpszDefault ;
	if(!INIFileHandle.GetStrData(lpszEntry,_val))
		return lpszDefault ;
	else
		return _val ;
}
 BOOL CARCTermSettingHander::WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue) 
{
	CCfgData INIFileHandle ;
	INIFileHandle.LoadCfgData(GetSettingINIFilePath());
	INIFileHandle.SetGroup(lpszSection) ;
	TCHAR _val[1024] = {0} ;
	_itoa(nValue,_val,10) ;
	if(lpszEntry == NULL)
		return FALSE;
	if(!INIFileHandle.SetData(lpszEntry,_val))
		return FALSE ;
	else
	{
		INIFileHandle.SaveCfgData(GetSettingINIFilePath()) ;
		return TRUE ;
	}
}
 BOOL CARCTermSettingHander::WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue) 
{
	CCfgData INIFileHandle ;
	INIFileHandle.LoadCfgData(GetSettingINIFilePath());
	INIFileHandle.SetGroup(lpszSection) ;
	if(lpszEntry == NULL)
		return FALSE ;
	if(!INIFileHandle.SetData(lpszEntry,lpszValue))
		return FALSE ;
	else
	{
		INIFileHandle.SaveCfgData(GetSettingINIFilePath()) ;
		return TRUE ;
	}
}