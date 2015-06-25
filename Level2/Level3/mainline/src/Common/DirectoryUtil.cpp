// DirectoryUtil.cpp: implementation of the CDirectoryUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DirectoryUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectoryUtil::CDirectoryUtil()
{

}

CDirectoryUtil::~CDirectoryUtil()
{

}

BOOL CDirectoryUtil::RemoveDirectoryR(LPCTSTR lpPathName)
{
//	CString tempPath=lpPathName;
	TCHAR oldDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, oldDir);
	if(!SetCurrentDirectory(lpPathName))
		return FALSE;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
	FindNextFile(hFind, &FindFileData);				// gets ".."
	while(GetLastError() != ERROR_NO_MORE_FILES) 
	{
		if(FindNextFile(hFind, &FindFileData)) 
		{
			if(FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) 
			{
				CString dirToKill = CString(lpPathName) + _T("\\") + FindFileData.cFileName;
				RemoveDirectoryR(dirToKill);
			}
		}
		else
			break;				
	}
	SetLastError(0);
	FindClose(hFind);
	SetCurrentDirectory(oldDir);
	EraseAllFilesFromDirectory(lpPathName);
//	RemoveDirectory(lpPathName);


	if(!RemoveDirectory(lpPathName))
	{
		LPVOID lpMsgBuf;
	FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
    FORMAT_MESSAGE_FROM_SYSTEM | 
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf,
    0,
    NULL 
	
);

MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

LocalFree( lpMsgBuf );

	}



	return TRUE;
}

BOOL CDirectoryUtil::EraseAllFilesFromDirectory(LPCTSTR lpPathName)
{
	TCHAR oldDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, oldDir);
	SetCurrentDirectory(lpPathName);
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
	FindNextFile(hFind, &FindFileData);				// gets ".."
	while(GetLastError() != ERROR_NO_MORE_FILES) {
		if(FindNextFile(hFind, &FindFileData)) {
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				//remove read-only flag if present
				SetFileAttributes(FindFileData.cFileName,FILE_ATTRIBUTE_NORMAL);
				//delete the file
				DeleteFile(FindFileData.cFileName);
			}
		}
		else
			break;				
	}
	FindClose(hFind);
	SetCurrentDirectory(oldDir);
	SetLastError(0);
	return TRUE;
}


/*
BOOL CDirectoryUtil::RemoveDir(LPCTSTR lpPathName)
 {
	BOOL Result;

	Result=PreRemoveDirectory(lpPathName)
      && RemoveDirectory(lpPathName);

	return Result;


 }




BOOL CDirectoryUtil::PreRemoveDirectory(CString DirName)
{
	LPTSTR lpBuffer;
	UINT uSize;
	CString fileName;
	HANDLE hHeap;
	BOOL result;
	HANDLE hFindFile;
	WIN32_FIND_DATA	FindFileData;
	uSize=(GetCurrentDirectory(0,NULL))*sizeof(TCHAR);
	hHeap=GetProcessHeap();
	lpBuffer=(LPSTR)HeapAlloc(hHeap,HEAP_ZERO_MEMORY,uSize);
	GetCurrentDirectory(uSize,lpBuffer);
	if (lpBuffer!=DirName) {
		SetCurrentDirectory(DirName);
	}
	hFindFile=FindFirstFile("*.*",&FindFileData);
	CString tFile;
	if (hFindFile!=INVALID_HANDLE_VALUE) {
		do {
			tFile=FindFileData.cFileName;
			if ((tFile==".")||(tFile=="..")) continue;

			if (FindFileData.dwFileAttributes==
              FILE_ATTRIBUTE_DIRECTORY)
			{
				if (DirName[DirName.GetLength()-1]!='\\')
					PreRemoveDirectory(DirName+'\\'+tFile);
				else
					PreRemoveDirectory(DirName+tFile);
				if (!RemoveDirectory(tFile))
					result=FALSE;
				else
					result=TRUE;
			}
			else if (SetFileAttributes(FindFileData.cFileName,FILE_ATTRIBUTE_NORMAL)&&!DeleteFile(tFile)) result=FALSE;
				else result=TRUE;
		}
		while (FindNextFile(hFindFile,&FindFileData));
		FindClose(hFindFile);
	}
	else {
		SetCurrentDirectory(lpBuffer);
		return FALSE;
	}
	SetCurrentDirectory(lpBuffer);
	return result;
}
*/