#include "StdAfx.h"
#include ".\tmpfilemanager.h"
#include <Shlwapi.h>

#include "Guid.h"
//
//CTmpFileManager::CTmpFileManager(void)
//{
//	//srand( (unsigned)time( NULL ) );
//}
//
//CTmpFileManager::~CTmpFileManager(void)
//{
//}

CTmpFileManager& CTmpFileManager::GetInstance()
{
	static CTmpFileManager sManager;
	return sManager;
}

CString CTmpFileManager::GetTmpFileName( LPCTSTR lpszExt /*= _T("tmp")*/ ) 
{

	while (true)
	{
		GUID guid;
		CString strFileName;
		VERIFY(CGuid::Create(guid) && CGuid::Convert(guid, strFileName));
		CString strFileFullName = GetTmpBaseFolder() + strFileName + _T(".") + lpszExt;

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = ::FindFirstFile(strFileFullName, &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) 
		{
			return strFileFullName;
		} 
		::FindClose(hFind);
	}

}

DWORD CTmpFileManager::DeleteDirectory1(LPCTSTR lpszDir)
{
	std::string     refcstrRootDirectory = lpszDir;
	HANDLE          hFile;                       // Handle to directory
	std::string     strFilePath;                 // Filepath
	std::string     strPattern;                  // Pattern
	WIN32_FIND_DATA FileInformation;             // File information


	strPattern = refcstrRootDirectory + "\\*.*";
	hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwErr = ERROR_SUCCESS;
		do
		{
			if(FileInformation.cFileName[0] != '.')
			{
				strFilePath.erase();
				strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

				if(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					// Delete subdirectory
					if (ERROR_SUCCESS != DeleteDirectory1(strFilePath.c_str()))
						break;
				}
				else
				{
					// Set file attributes
					// Delete file
					if(::SetFileAttributes(strFilePath.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE
						|| ::DeleteFile(strFilePath.c_str()) == FALSE)
						break;
				}
			}
		} while(::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		DWORD dwError = ::GetLastError();
		if(dwError != ERROR_NO_MORE_FILES)
			return dwError;
		else
		{
			// Set directory attributes
			// Delete directory
			if(::SetFileAttributes(refcstrRootDirectory.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE
				|| ::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
				return ::GetLastError();
		}
	}

	return ERROR_SUCCESS;
}


#include <shellapi.h>

bool CTmpFileManager::DeleteDirectory2(LPCTSTR lpszDir)
{
	int len = _tcslen(lpszDir);
	TCHAR *pszFrom = new TCHAR[len+2];
	_tcscpy(pszFrom, lpszDir);
	pszFrom[len] = 0;
	pszFrom[len+1] = 0;

	SHFILEOPSTRUCT fileop;
	fileop.hwnd   = NULL;    // no status display
	fileop.wFunc  = FO_DELETE;  // delete operation
	fileop.pFrom  = pszFrom;  // source file name as double null terminated string
	fileop.pTo    = NULL;    // no destination needed
	fileop.fFlags = FOF_NOCONFIRMATION|FOF_SILENT;  // do not prompt the user

	fileop.fAnyOperationsAborted = FALSE;
	fileop.lpszProgressTitle     = NULL;
	fileop.hNameMappings         = NULL;

	int ret = SHFileOperation(&fileop);
	delete [] pszFrom;  
	return (ret == 0);
}

void CTmpFileManager::DeleteBaseFolder()
{
	//CString m_strTmpBaseFolder=GetTmpBaseFolder();
	//int strLen = m_strTmpBaseFolder.GetLength();
	//if (strLen > 0)
	//{
	//	VERIFY(ERROR_SUCCESS == DeleteDirectory1(m_strTmpBaseFolder.GetAt(strLen - 1) == '\\' ? m_strTmpBaseFolder.Left(strLen - 1) : m_strTmpBaseFolder));
	//}
}

//void CTmpFileManager::SetBaseFolder( CString strBase )
//{
//	int nLen = strBase.GetLength();
//	if (nLen)
//	{
//		TCHAR cTail = strBase.GetAt(nLen - 1);
//		if (cTail != '\\' || cTail != '/')
//		{
//			strBase += '\\';
//		}
//		strBase += _T("temp\\");
//	}
//	
//	if (!PathIsDirectory(strBase)) 
//	{
//		if(!::CreateDirectory(strBase,NULL))
//		{
//			ASSERT(FALSE);
//		}
//	}
//	m_strTmpBaseFolder = strBase;
//}

CString CTmpFileManager::GetTmpDirectory() 
{
	while (true)
	{
		GUID guid;
		CString strFileName;
		VERIFY(CGuid::Create(guid) && CGuid::Convert(guid, strFileName));
		CString strFileFullName = GetTmpBaseFolder() + strFileName;
		
		if (!PathIsDirectory(strFileFullName)) 
		{
			if(::CreateDirectory(strFileFullName,NULL))
			{
				return strFileFullName + _T("\\");
			}
			else
			{
				DWORD errorcode = ::GetLastError();				
				ASSERT(FALSE);
				return _T(".\\");
			}
		} 		
	}
}

void CTmpFileManager::DeleteDirectoryFile( CString strFormat, CString dir )
{
	CString     refcstrRootDirectory = dir;
	HANDLE          hFile;                       // Handle to directory
	CString     strFilePath;                 // Filepath
	CString     strPattern;                  // Pattern
	WIN32_FIND_DATA FileInformation;             // File information


	strPattern = dir + "\\" + strFormat;
	hFile = ::FindFirstFile(strPattern, &FileInformation);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwErr = ERROR_SUCCESS;
		do
		{
			if(FileInformation.cFileName[0] != '.')
			{				
				strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;
				
				{
					// Set file attributes
					// Delete file
					if(::SetFileAttributes(strFilePath, FILE_ATTRIBUTE_NORMAL) == FALSE
						|| ::DeleteFile(strFilePath) == FALSE)
						break;
				}
			}
		} while(::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		
	}
	
}



CString CTmpFileManager::GetTmpBaseFolder() const
{
	TCHAR path[MAX_PATH];
	::GetTempPath(MAX_PATH,path);
	

	/*GetModuleFileName(NULL, path, MAX_PATH);
	*strrchr(path,'\\') = '\0';*/
	CString strTmpFolder = CString(path)+_T("\\");
	return strTmpFolder;
}