// 95/10/06 - LC - Fix the MAKEPATH PROBLEM
//fileman.cpp
//Anthony Malizia
//June 23, 1995
// 95/09/06 - FZ - Modified for stricter file sharing.

#include <SYS\STAT.H>
#include <direct.h>
#include <iostream>
#include <strstream>
#include <string>
#include <cstdlib>
#include <io.h>

#include "replace.h"
#include "fileman.h"
#include "exception.h"
//#include "handler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Size of buffer that is created on the stack to copy file data.
//Adjust this parameter as needed.
#define FILECOPYBUFFERSIZE 1024

int    C_CopyDirectory (const char *source, char *destination)
{
	FileManager f;
    if (!f.MakePath (destination))
		return 0;
    return (f.CopyDirectory (source, destination,NULL));
}

int    C_DeleteDirectory (const char *path)
{
	FileManager f;
	return (f.DeleteDirectory (path));
}

int    C_MakePath (const char *path)
{
	FileManager f;
	return (f.MakePath (path));
}

int    C_CopyFile (const char *source, const char *destination)
{
	FileManager f;
	return (f.CopyFile (source, destination));
}

int    C_DeleteFile(const char* fileName)
{
	return FileManager::DeleteFile(fileName);
}

// Returns 1 if 'path' is a file, 0 otherwise.
int    C_IsFile (const char *p_path)
{
	FileManager f;
    return (f.IsFile (p_path));
}

int    C_IsDirectory (const char *p_path)
{
	FileManager f;
    return (f.IsDirectory (p_path));
}

//return a pointer to a stdfstream, if the stdfstream is opened sucessfully
stdfstream* C_OPEN_STREAM(const char* viewfile, FileStreamOpenMode mode)
{
	stdfstream* temp = NULL;
	switch(mode)
	{
	case FSTREAM_OVERWRITE:
		temp = new stdfstream(viewfile, stdios::binary|stdios::trunc |stdios::out);
		if (temp->bad())
		{
			delete temp;
			temp=NULL;
		}
		break;
	case FSTREAM_APPEND:
		temp = new stdfstream(viewfile, stdios::binary|stdios::ate |stdios::out);
		if (temp->bad())
		{
			delete temp;
			temp=NULL;
		}	
		break;
	case FSTREAM_READ:
		temp = new stdfstream(viewfile, stdios::binary|stdios::in);
		if (temp->bad())
		{
			delete temp;
			temp=NULL;
		}
		break;
	default:
		break;
	}
	return temp;
}

//delete the stream
void C_CLOSE_STREAM(stdfstream* filePtr)
{
	delete filePtr;
}


// returns 1 if 'path' is an accessible drive
int FileManager::IsDriveReady (const char *path)
{
	char temp[_MAX_PATH];
	 if (_fullpath (temp, path, _MAX_PATH) == NULL)
		  return 0;   //invalid path
	 return GetDiskSpace (temp) >= 0.0;
}

// Returns 1 if 'path' is a directory, 0 otherwise.
int FileManager::IsDirectory (const char *path)
{
	char temp[_MAX_PATH];
	temp[0] = 0;
	return IsDirectory(path, temp);
}



/***************************************************************************\ 
* 
* TStrChr() 
* 
* A strchr() function which is ASCII or UNICODE. 
* 
* History: 
* 2/25/93 
*   Created. 
* 
\***************************************************************************/ 
PTCHAR TStrChr(const PTCHAR string, UINT c) 
{ 
#ifdef UNICODE 
	return wcschr((wchar_t*)string, (wchar_t)c); 
#else 
	PTCHAR  psz; 
	for (psz = string; *psz; psz = CharNext(psz)) 
	{ 
		if (*psz == (TCHAR)c) 
		{ 
			break; 
		} 
	} 
	return psz; 
#endif 
} 
/***************************************************************************\ 
* 
* IsDirectory() 
* 
*  Given an string in lpszFile, IsDirectory 
*    verifies whether or not the item is a directory, and if so, returns 
*    true, and places the full directory path in lpszFile(if lpszFile != NULL)
*
* 
*  input:   lpszDir     -   Holds current directory 
*           lpszFile    -   Holds item of dubious directoryness. 
* 
* 
\***************************************************************************/ 
int FileManager::IsDirectory(const char* lpszDir, LPTSTR lpszFile) 
{
	DWORD   dwAttrib; 
	LPTSTR  lpszHold; 
	TCHAR   szItem[_MAX_PATH * 2]; 

	// 
	// if it's '..', go up one directory 
	// 
	if( !lstrcmp(lpszFile, TEXT("..")) )
	{ 
		lstrcpy(lpszFile, lpszDir); 
        lpszHold = TStrChr(lpszFile, TEXT('\0')); 
		while( lpszHold > lpszFile )
		{ 
			if(*lpszHold == TEXT('\\'))
			{ 
				break; 
			} 
            lpszHold = CharPrev(lpszFile, lpszHold); 
		} 

        if(lpszHold <= lpszFile)
		{ 
            return(0); 
		} 
        else
		{ 
            if( TStrChr(lpszFile, TEXT('\\')) == lpszHold ) 
				lpszHold++; 
			*lpszHold = TEXT('\0'); 
			return(1); 
		} 
	} 

	// file under some file systems may have [] characters. 
    //   Prepend path, adding delimiting backslash unless we're in the root. 
    //   If the attribute check is successful, it's file, so leave. 
	// 
    // NOTE:  This is hack.  If there is file called '[foo]' and 
	//   Directory called 'foo', they will appear identical in the listbox. 
	//   Rather than check for this rare case, if it happens, Filer will 
    //   assume it is file first, as the directory may be changed from 
	//   the Directory Listbox. 
	// 
	lstrcpy(szItem, lpszDir); 
	lpszHold = TStrChr(szItem, TEXT('\0')); 

	lpszHold--; 
	if( *lpszHold != TEXT('\\') )
	{ 
		lpszHold++; 
		*lpszHold = TEXT('\\'); 
	} 
	lpszHold++; 

	lstrcpy(lpszHold, lpszFile); 
	dwAttrib = GetFileAttributes(szItem); 
	if( (dwAttrib == 0xFFFFFFFF) || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY) ) 
        return(0); 
	// 
    // OK, it's directory, and szItem now holds the fully qualified path. 
	// copy this to the filename buffer sent in, and return true. 
	// 
	CharUpper(szItem); 
	lstrcpy(lpszFile, szItem); 
	return(1); 
} 
//Returns 1 if 'path' is file, 0 otherwise.
int FileManager::IsFile (const char *path)
{
	CFileFind fileFind;
    return fileFind.FindFile(path, 0);
}

// returns number of free bytes on drive
// uses double to allow for drives larger than 4GB
double FileManager::GetDiskSpace (const char *drive)
{
	char temp[_MAX_PATH];
	strcpy(temp, drive);
	int len = strlen(temp);
	if (temp[len-1] != '\\')
	{
		temp[len] = '\\';
	}

	DWORD SectorsPerCluster, // address of sectors per cluster 
		BytesPerSector, // address of bytes per sector 
		NumberOfFreeClusters,// address of number of free clusters 
		TotalClusters;
	if (GetDiskFreeSpace(temp, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalClusters))
	{
		return SectorsPerCluster * BytesPerSector * NumberOfFreeClusters;
	}
	else
		return -1.0;
}

//Copy 'source' file to 'destination' file. Directory structure is assumed to
//exist. If 'destination' already exists, then the file will be replaced only if
//'overwrite' is 1. Returns 1 if sucessful, 0 otherwise.
int FileManager::CopyFile (const char *source, const char *destination,
    int overwrite)
{
	if (overwrite)
		return ::CopyFile(source, destination, FALSE);
	return ::CopyFile(source, destination, TRUE);
}

//Copy contents of 'source' directory to 'destination' directory. Destination
//directory is assumed to exist, but sub-directories are created as needed.
//Returns 1 if sucessful, 0 otherwise.
int FileManager::CopyDirectory (const char *source, const char *destination,void (CALLBACK * _ShowCopyProgress)(LPCTSTR))
{
	char p1[_MAX_PATH];
	char p2[_MAX_PATH];
	if (_fullpath (p1, source, _MAX_PATH) == NULL)
		return 0;   //invalid source path
	int len1 = strlen (p1);
	if (len1 && p1[len1 - 1] != '\\')
	{
        p1[len1] = '\\';        //add trailing '\\'
        p1[++len1] = '\0';   //end of string
    }
    if (_fullpath (p2, destination, _MAX_PATH) == NULL)
        return 0;   //invalid destination path
    int len2 = strlen (p2);
	if (len2 && p2[len2 - 1] != '\\')
    {
		p2[len2] = '\\';     //add trailing '\\'
        p2[++len2] = '\0';   //end of string
    }
    if (_strnicmp (p1, p2, len1) == 0)
        return 0;   //destination path is same or is sub-directory of source
	if (!IsDirectory (p1))
        return 0;   //source path is not directory
    if (!IsDirectory (p2))
                  return 0;   //destination path is not directory
    strcat (p1, "*.*");

	CFileFind finder;
    if (!finder.FindFile(p1,0))
        return 1;   //nothing to do (not an error)
	BOOL bContinue = TRUE;
	while(bContinue)
	{
		bContinue = finder.FindNextFile();
		if (finder.IsDots())
            continue;   //skip '.' and '..' directories
		CString ff_name = finder.GetFileName();
		strcpy (p1 + len1, ff_name);
        strcpy (p2 + len2, ff_name);
        if (finder.IsDirectory())
        {
			if (!IsDirectory (p2) && _mkdir (p2) != 0)
                return 0;   //could not create sub-directory
            if (!CopyDirectory (p1, p2,_ShowCopyProgress))   //recursive call
                return 0;   //could not copy sub-directory
        }
		else
		{
			if (_ShowCopyProgress != NULL)
			{
				(*_ShowCopyProgress)(p1);
			}
			if (!CopyFile (p1, p2))
            return 0;   //could not copy file
		}
    } 
	return 1;   //success
}

//Creates the directory tree 'path'.
//Returns 1 if successful, 0 otherwise.
int FileManager::MakePath (const char *path)
{
	if (IsDirectory (path))
        return 1;   //already exists
	char strPath[_MAX_PATH];
	strcpy(strPath, path);
	int count = strlen(strPath);
	int i=0;
	for ( i = count-1; i > -1; i--)
	{
		if (strPath[i] == '\\')
		{
			strPath[i] = 0;
			if (i != count - 1)
			{
				if (MakePath(strPath) )
				{
					return !_mkdir(path);
				}
				else
				{
					return false;
				}
			}
		}
	}
	if (i == 0)
	{
	    return !_mkdir(path);
	}
	return false;
}

//Compares the modification times of two files.  Assumes that both files exist.
//Return -1 if 'first' is older than 'second', 0 if both modification times are
//the same, and 1 if 'first' is newer than 'second'.
int FileManager::IsNewerFile (const char *first, const char *second)
{
	if (!IsFile (first) || !IsFile (second))
        return 0;   //error, should throw an exception instead
    struct stat s1, s2;
    if (stat (first, &s1) != 0)
        return -1;  //error, should throw an exception instead
     if (stat (second, &s2) != 0)
        return 1;   //error, should throw an exception instead
    if (s1.st_mtime == s2.st_mtime)
        return 0;   //same modification times
	if (s1.st_mtime < s2.st_mtime)
        return -1;  //'first' is older than 'second'
	return 1;   //'first' must be newer than 'second'
}

//Returns the size of the file specified by 'path', or -1 on error.
long FileManager::FileSize (const char *path)
{
    if (!IsFile (path))
        return -1;  //error
    struct stat s;
    if (stat (path, &s) != 0)
        return -1;  //error
    return s.st_size;
}

//Deletes file specified by 'path'. Returns 1 on success, 0 otherwise.
int FileManager::DeleteFile (const char *path)
{
// 	CFileFind fileFind;
//     if (fileFind.FindFile(path, 0))
// 	{
// 		BOOL bContinue = TRUE;
// 		while(bContinue)
// 		{
// 			bContinue = fileFind.FindNextFile();
// 	        if (fileFind.IsDots())
// 				continue;   //skip '.' and '..' directories
// 			ff_name = fileFind.GetFilePath();
// 			
// 			if(fileFind.IsReadOnly()) {
// 				_chmod(ff_name, _S_IREAD | _S_IWRITE);
// 			}
// 			
// 			if(remove(ff_name)) //remove return -1 if fail
// 				return 0;   //could not delete file
// 		}
//     } 
	return 1;
}

//Deletes directory specified by 'path', as well as all sub-directories.
//Returns 1 on success, 0 otherwise.
int FileManager::DeleteDirectory (const char *path)
{
    char dir[_MAX_PATH];
    if (_fullpath (dir, path, _MAX_PATH) == NULL)
        return 0;   //invalid path
	int len = strlen (dir);
    if (len && dir[len - 1] != '\\')
    {
        dir[len] = '\\';     //add trailing '\\'
        dir[++len] = '\0';   //end of string
    }
    if (!IsDirectory (dir))
        return 0;   //path is not directory
	
	//find files and subdir, delete them
    char files[_MAX_PATH];	
	strcpy(files, dir);
    strcat (files, "*.*");

	CString ff_name;
	CFileFind fileFind;
	bool ErrorOccurred = false;
	int dirFound = true; 
    while (dirFound && fileFind.FindFile(files, 0))
	{
		BOOL bContinue = TRUE;
		dirFound = false;
		while(bContinue)
		{
			bContinue = fileFind.FindNextFile();
	        if (fileFind.IsDots())
				continue;   //skip '.' and '..' directories
			ff_name = fileFind.GetFilePath();

			if (fileFind.IsDirectory())
		    {
				dirFound = true;
				bContinue = false;
				fileFind.Close();
				if (!DeleteDirectory (ff_name))    //recursive call
				    ErrorOccurred = true;   //could not delete sub-directory
			}
	        else 
			{
				if (!DeleteFile (ff_name))
					ErrorOccurred = true;   //could not delete file
			}
		}
    } 
	fileFind.Close();
    if (len - 1 >= 0)
        dir[len - 1] = '\0';
	int ret = _rmdir (dir) ;
    if (ret != 0 || ErrorOccurred)
		return 0;   //could not remove directory
    return 1;   //success
}

DWORD CALLBACK CopyProgress(
							LARGE_INTEGER TotalFileSize,          // total file size, in bytes
							LARGE_INTEGER TotalBytesTransferred,  // total number of bytes transferred
							LARGE_INTEGER StreamSize,              // total number of bytes for this stream
							LARGE_INTEGER StreamBytesTransferred, // total number of bytes transferred for this stream
							DWORD dwStreamNumber,                  // the current stream
							DWORD dwCallbackReason,                // reason for callback
							HANDLE hSourceFile,                    // handle to the source file
							HANDLE hDestinationFile,               // handle to the destination file
							LPVOID lpData                          // passed by CopyFileEx
							)
{
	// TRACE("%08X:%08X of %08X:%08X copied.\n",TotalBytesTransferred.HighPart,TotalBytesTransferred.LowPart,TotalFileSize.HighPart,TotalFileSize.LowPart);
	
	CopyFileParam* param = (CopyFileParam *)lpData;  
	IProgressDialog *pProgressDialog = (IProgressDialog*)param->pProgressDialog;
	if (pProgressDialog->HasUserCancelled())
	{
		param->bCanceled = true;
		pProgressDialog->StopProgressDialog();
		pProgressDialog->Release();
		return PROGRESS_CANCEL;
	}
	
	int nPercent = static_cast<int>(((TotalBytesTransferred.QuadPart*1.0)  /(TotalFileSize.QuadPart* 1.0)) *100.0);
	pProgressDialog->SetProgress(nPercent,100);
	
	//copy file message
	CString strCopyState;
	strCopyState.Format("Total %d files, %d files left",param->nTotalFileCount ,param->nTotalFileCount - param->nCurCopyFile);
	pProgressDialog->SetLine(2,strCopyState.AllocSysString(),FALSE,NULL);
	return PROGRESS_CONTINUE;
}
//int FileManager::CopyProjFilesWithProgress(const char *source, const char *destination,CopyFileParam *pParam )
//{
//	::CoInitialize(NULL);
//
//	IProgressDialog *pProgressDialog;
//
//	CoCreateInstance( CLSID_ProgressDialog,
//		NULL,
//		CLSCTX_ALL,
//		IID_IProgressDialog,
//		(LPVOID*)&pProgressDialog);
//	pProgressDialog->SetTitle(L"Copy files...");
//	pProgressDialog->SetCancelMsg(L"Cancelling...",NULL);
//
//	pProgressDialog->StartProgressDialog(NULL, NULL, PROGDLG_NOMINIMIZE, NULL);
//	
//	CopyFileParam copyParam;
//	copyParam.pProgressDialog = pProgressDialog;
//	//now the project has 1144 files, if the file count changed,the value must be changed
//	copyParam.nTotalFileCount = 1144;
//	copyParam.nCurCopyFile = 0;
//	
//	CopyDirectoryWithProgress(source,destination,&copyParam);
//	//copy folder with progress
//	pProgressDialog->StopProgressDialog();
//
//	pProgressDialog->Release();
//
//	CoUninitialize();
//
//	return 0;
//}
int FileManager::CopyDirectoryWithProgress(const char *source, const char *destination,CopyFileParam* pParam)
{
	char p1[_MAX_PATH];
	char p2[_MAX_PATH];
	if (_fullpath (p1, source, _MAX_PATH) == NULL)
		return 0;   //invalid source path
	int len1 = strlen (p1);
	if (len1 && p1[len1 - 1] != '\\')
	{
		p1[len1] = '\\';        //add trailing '\\'
		p1[++len1] = '\0';   //end of string
	}
	if (_fullpath (p2, destination, _MAX_PATH) == NULL)
		return 0;   //invalid destination path
	int len2 = strlen (p2);
	if (len2 && p2[len2 - 1] != '\\')
	{
		p2[len2] = '\\';     //add trailing '\\'
		p2[++len2] = '\0';   //end of string
	}
	if (_strnicmp (p1, p2, len1) == 0)
		return 0;   //destination path is same or is sub-directory of source
	if (!IsDirectory (p1))
		return 0;   //source path is not directory
	if (!IsDirectory (p2))
		return 0;   //destination path is not directory
	strcat (p1, "*.*");

	CFileFind finder;
	if (!finder.FindFile(p1,0))
		return 1;   //nothing to do (not an error)
	BOOL bContinue = TRUE;
	while(bContinue)
	{
		bContinue = finder.FindNextFile();
		if (finder.IsDots())
			continue;   //skip '.' and '..' directories
		CString ff_name = finder.GetFileName();
		strcpy (p1 + len1, ff_name);
		strcpy (p2 + len2, ff_name);
		if (finder.IsDirectory())
		{
			if (!IsDirectory (p2) && _mkdir (p2) != 0)
				return 0;   //could not create sub-directory
			if (!CopyDirectoryWithProgress (p1, p2,pParam))   //recursive call
				return 0;   //could not copy sub-directory
		}
		else
		{
			int nCancel = FALSE;
			if(!CopyFileEx(p1,p2,(LPPROGRESS_ROUTINE)CopyProgress,(LPVOID)pParam,&nCancel,COPY_FILE_FAIL_IF_EXISTS))
			{
				if (pParam->bCanceled == true)
				{
					FileManager::DeleteDirectory(pParam->strDestDir);
				}
				printf("CopyFileEx() failed.\n");
				return 0;
			}
			pParam->nCurCopyFile+=1;
		}
	} 
	return 1;   //success
}

int  FileManager::GetFileCountInDir(const char *source, int& nCount)
{
	char p1[_MAX_PATH];
	if (_fullpath (p1, source, _MAX_PATH) == NULL)
		return 0;   //invalid source path
	int len1 = strlen (p1);
	if (len1 && p1[len1 - 1] != '\\')
	{
		p1[len1] = '\\';        //add trailing '\\'
		p1[++len1] = '\0';   //end of string
	}
	strcat (p1, "*.*");
	CFileFind finder;
	if (!finder.FindFile(p1,0))
		return 1;   //nothing to do (not an error)
	BOOL bContinue = TRUE;
	while(bContinue)
	{
		bContinue = finder.FindNextFile();
		if (finder.IsDots())
			continue;   //skip '.' and '..' directories
		CString ff_name = finder.GetFileName();
		strcpy (p1 + len1, ff_name);
		if (finder.IsDirectory())
		{

			if (!GetFileCountInDir (p1, nCount) )  //recursive call
				return 0;   //could not copy sub-directory
		}
		else
		{
			nCount += 1;
		}
	}
	return 1;
}