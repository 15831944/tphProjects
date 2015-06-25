#pragma once

class CARCTermSettingHander
{
public:
	CARCTermSettingHander(void);
	~CARCTermSettingHander(void);
public:
	static CString GetSettingINIFilePath() ;
    static UINT GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault) ;
	static CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault) ;
	static BOOL WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue) ;
	static BOOL WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue) ;
};
