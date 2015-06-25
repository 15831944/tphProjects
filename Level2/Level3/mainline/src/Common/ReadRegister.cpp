#include "StdAfx.h"
#include ".\readregister.h"

CReadRegister::CReadRegister(void)
{
}

CReadRegister::~CReadRegister(void)
{
}
BOOL CReadRegister::ReadRegisterLicense(CString& _errorMes)
{
	HKEY hKey ; 
	BOOL res = true;
	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, CLICENSE_REGISTRY_LICENSE_ARCTERM_KEY, 0, KEY_READ, 
		&hKey))
	{
		if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, CLICENSE_REGISTRY_LICENSE_ARCCAPTURE_KEY, 0, KEY_READ, 
			&hKey))
		{
			_errorMes.Append(_T("Not Install the Application !")) ;
			return FALSE ;
		}
	}
	DWORD dwLicenseSize = 0 ;
	DWORD dwType ;
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, CLICENSE_REGISTRY_LICENSE_ARCTERM_VALUE, NULL, &dwType, 
		NULL, 
		&(m_license.m_fontsize)))
	{
		_errorMes.Append(_T("Read font error!")) ;
		res =  FALSE ;
	}
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, CLICENSE_REGISTRY_LICENSE_ARCTERM_VALUE, NULL, &dwType, 
		(BYTE *) m_license.m_font, 
		&m_license.m_fontsize))
	{
		_errorMes.Append(_T("Read font error!")) ;
		res =  FALSE ;
	}


	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "height", NULL, &dwType, 
		NULL, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read height error!")) ;
		res = FALSE ;
	}
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "height", NULL, &dwType, 
		(BYTE *) m_license.m_height, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read height error!")) ;
		res = FALSE ;
	}


	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "left", NULL, &dwType, 
		NULL, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read left error!")) ;
		res = FALSE ;
	}
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "left", NULL, &dwType, 
		(BYTE *) m_license.m_left, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read left error!")) ;
		res = FALSE ;
	}


	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "setting", NULL, &dwType, 
		NULL, 
		&(m_license.msettingsize)))
	{
		_errorMes.Append(_T("Read setting error!")) ;
		res = FALSE ;
	}
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "setting", NULL, &dwType, 
		(BYTE *) m_license.m_setting, 
		&(m_license.msettingsize)))
	{
		_errorMes.Append(_T("Read setting error!")) ;
		res = FALSE ;
	}


	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "top", NULL, &dwType, 
		NULL, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read top error!")) ;
		res = FALSE ;
	}
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "top", NULL, &dwType, 
		(BYTE *) m_license.m_top, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read top error!")) ;
		res = FALSE ;
	}

	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "width", NULL, &dwType, 
		NULL, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read width error!")) ;
		res = FALSE ;
	}

	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "width", NULL, &dwType, 
		(BYTE *) m_license.m_width, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read width error!")) ;
		res = FALSE ;
	}
	RegCloseKey(hKey);
	return res ;
}
BOOL CReadRegister::ReadRegisterDB(CString& _errorMes)
{
	HKEY hKey ; 
	BOOL res = TRUE ;
	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Aviation Research Corporation\\ARCTerm\\DataBase", 0, KEY_READ, 
		&hKey))
	{
		if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Aviation Research Corporation\\ARCCapture\\DataBase", 0, KEY_READ, 
			&hKey))
		{
			_errorMes.Append(_T("Have not Installed the Arcterm Application !\r\n")) ;
			return FALSE ;
		}
	}
	DWORD dwLicenseSize = 0 ;
	DWORD dwType ;
	int code = RegQueryValueEx(hKey, "DataBase", NULL, &dwType, 
		NULL, 
		&dwLicenseSize) ;
	if(ERROR_SUCCESS != code)
	{
		_errorMes.Append(_T("Read DB name error!\r\n")) ;
		res =  FALSE ;
	}
	 code = RegQueryValueEx(hKey, "DataBase", NULL, &dwType, 
		(BYTE *) m_DB.m_name, 
		&dwLicenseSize) ;
	if(ERROR_SUCCESS != code)
	{
		_errorMes.Append(_T("Read DB name error!\r\n")) ;
		res =  FALSE ;
	}

	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "Password", NULL, &dwType, 
		NULL, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read DB password error!\r\n")) ;
		res =  FALSE ;
	}
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "Password", NULL, &dwType, 
		(BYTE *) m_DB.m_PW, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read DB password error!\r\n")) ;
		res =  FALSE ;
	}

	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "Server", NULL, &dwType, 
		NULL, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read DB server error!\r\n")) ;
		res =  FALSE ;
	}
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "Server", NULL, &dwType, 
		(BYTE *) m_DB.m_server, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read DB server error!\r\n")) ;
		res =  FALSE ;
	}

	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "UserName", NULL, &dwType, 
		NULL, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read DB username error!\r\n")) ;
		res =  FALSE ;
	}
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "UserName", NULL, &dwType, 
		(BYTE *) m_DB.m_username, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read DB username error!\r\n")) ;
		res =  FALSE ;
	}

	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "Version", NULL, &dwType, 
		NULL, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read DB version error!\r\n")) ;
		res =  FALSE ;
	}
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "Version", NULL, &dwType, 
		(BYTE *) m_DB.m_version, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read DB version error!\r\n")) ;
		res =  FALSE ;
	}
	RegCloseKey(hKey);
	return res ;
}
BOOL CReadRegister::ReadRegisterPath(CString& _errorMes)
{
	HKEY hKey ; 
	BOOL res = TRUE;
	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Aviation Research Corporation\\ARCTerm\\Paths", 0, KEY_READ, 
		&hKey))
	{
		if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Aviation Research Corporation\\ARCCapture\\Paths", 0, KEY_READ, 
			&hKey))
		{
			_errorMes.Append(_T("Not Install the Application !\r\n")) ;
			return FALSE ;
		}
	}
	DWORD dwLicenseSize = 0 ;
	DWORD dwType ;
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "AppPath", NULL, &dwType, 
		NULL, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read path error!\r\n")) ;
		res =  FALSE ;
	}
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "AppPath", NULL, &dwType, 
		(BYTE *) m_Path.m_AppPath, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read path error!\r\n")) ;
		res =  FALSE ;
	}
	RegCloseKey(hKey);
	return res ;
}
BOOL CReadRegister::ReadRegisterResourse(CString& _errorMes)
{
	HKEY hKey ; 
	BOOL res = TRUE ;
	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Aviation Research Corporation\\ARCTerm\\Resource", 0, KEY_READ, 
		&hKey))
	{
		if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Aviation Research Corporation\\ARCCapture\\Resource", 0, KEY_READ, 
			&hKey))
		{
			_errorMes.Append(_T("Not Install the Application !\r\n")) ;
			return FALSE ;
		}
	}
	DWORD dwLicenseSize = 0 ;
	DWORD dwType ;
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "Version", NULL, &dwType, 
		NULL, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read resource version  error!\r\n")) ;
		res =  FALSE ;
	}
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "Version", NULL, &dwType, 
		(BYTE *) m_Resouce.m_Version, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read resource version  error!\r\n")) ;
		res =  FALSE ;
	}
	RegCloseKey(hKey);
	return res ;
}
BOOL CReadRegister::ReadRegisterUnit(CString& _errorMes)
{
	HKEY hKey ; 
	BOOL res = true ;
	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Aviation Research Corporation\\ARCTerm\\UnitsManager", 0, KEY_READ, 
		&hKey))
	{
		if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Aviation Research Corporation\\ARCCapture\\UnitsManager", 0, KEY_READ, 
			&hKey))
		{
			_errorMes.Append(_T("Not Install the Application !\r\n")) ;
			return FALSE ;
		}
	}
	DWORD dwLicenseSize = 0 ;
	DWORD dwType ;
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "LengthUnits", NULL, &dwType, 
		NULL, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read lenght  error!\r\n")) ;
		res =  FALSE ;
	}
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "LengthUnits", NULL, &dwType, 
		(BYTE *) m_Unit.m_leng, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read lenght  error!\r\n")) ;
		res =  FALSE ;
	}

	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "WeightUnits", NULL, &dwType, 
		NULL, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read weight  error!\r\n")) ;
		res =  FALSE ;
	}
	if(ERROR_SUCCESS != RegQueryValueEx(hKey, "WeightUnits", NULL, &dwType, 
		(BYTE *) m_Unit.m_weight, 
		&dwLicenseSize))
	{
		_errorMes.Append(_T("Read weight  error!\r\n")) ;
		res =  FALSE ;
	}
	RegCloseKey(hKey);
	return res ;
}
BOOL CReadRegister::CreateINIFile() 
{
	BOOL res = FALSE ;
	CINIFileForDBHandle DbHandle ;
	DbHandle.SetDataBaseName(m_DB.m_name) ;
	DbHandle.SetDBPassword(m_DB.m_PW) ;
	DbHandle.SetDBServer(m_DB.m_server) ;
	DbHandle.SetDBUserName(m_DB.m_username) ;
	DbHandle.SetDBVersion(m_DB.m_version) ;
	res =   DbHandle.SaveData(m_Path.m_AppPath) ;

	CINIFileForLicenseHandle dblic ;
	dblic.LoadFile(m_Path.m_AppPath) ;
	dblic.SetLicenseFONT(m_license.m_font,m_license.m_fontsize) ;
	dblic.SetLicenseHeight(m_license.m_height) ;
	dblic.SetLicenseLeft(m_license.m_left) ;
	dblic.SetLicenseSetting(m_license.m_setting,m_license.msettingsize) ;
	dblic.SetLicenseTop(m_license.m_top) ;
	dblic.SetLicenseWidth(m_license.m_width) ;
	dblic.SaveData(m_Path.m_AppPath) ;

	CINIFileForPathsHandle dbpath ;
	dbpath.LoadFile(m_Path.m_AppPath) ;
	dbpath.SetPathVal(m_Path.m_AppPath) ;
	dbpath.SaveData(m_Path.m_AppPath) ;

	CINIFileForResourseHandle dbres ;
	dbres.LoadFile(m_Path.m_AppPath) ;
	dbres.SetResourseVersion(m_Resouce.m_Version) ;
	dbres.SaveData(m_Path.m_AppPath) ;

	CINIFileForUNITHandle dbUnit ;
	dbUnit.LoadFile(m_Path.m_AppPath) ;
	dbUnit.SetUnitLenght(m_Unit.m_leng) ;
	dbUnit.SetUnitWeight(m_Unit.m_weight) ;
	dbUnit.SaveData(m_Path.m_AppPath) ; 
	return res ;
}
BOOL CReadRegister::ReadRegister(CString& _errorMessage) 
{
	BOOL res = FALSE;
	res = ReadRegisterDB(_errorMessage ) ;
	res = ReadRegisterPath(_errorMessage ) ;
	res = ReadRegisterResourse(_errorMessage ) ;
	res = ReadRegisterUnit(_errorMessage ) ;
	res = ReadRegisterLicense(_errorMessage ) ;
	return res ;
}