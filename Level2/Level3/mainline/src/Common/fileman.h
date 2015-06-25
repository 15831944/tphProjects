//fileman.h
//Anthony Malizia
//June 23, 1995

#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <cstdio>
#include <fstream>
#include <share.h>
#include "commondll.h"
#include "replace.h"
#include <ShlObj.h>
// Franz Zemen 07-24-97 Include windows.h and remove UNICODE DEFINE BECAUSE IT CONFLICTS WITH DeleteFile
// changing it to DeleteFileA.  Make sure all windows header files, wherever used
// appear prior to fileman.h.

//typedef void (CALLBACK *ShowCopyProgress)(LPCTSTR strPath);

#define FM_NON_CRITICAL_FILE    0
#define FM_CRITICAL_FILE        1

enum
{
    FM_REPORTFUNC_NULL,
    FM_REPORTFUNC_WIDGETS_ERROR,
    FM_REPORTFUNC_WIDGETS_NOTIFY,
    FM_REPORTFUNC_TEXT_ERROR,
    FM_REPORTFUNC_TEXT_NOTIFY
};

extern "C" {

// 95/09/06 - FZ - Overloading OpenSharedFile to pass the reporting function.
// 95/09/13 - LF - maintained to allow existing calls to be redirected
//COMMON_TRANSFER FILE * C_OpenSharedFileReport (int p_reportFlag, const char * p_fileName, char *p_mode);

// 95/09/13 - LF - Replacement for above shared open "C" function,
// existing calls will only replaced when share flag needs to be set
//COMMON_TRANSFER FILE *  C_OpenSharedFile (const char *p_fileName, char *p_mode, int p_shareMode);

// 95/09/13 - LF - Used for accessing shared databases
// if the user gives up trying to access the file, return value == NULL
// this is the only one of the OpenFile functions that can return NULL
//COMMON_TRANSFER FILE *  C_OpenNonCriticalFile (const char *p_fileName, const char *p_mode, int p_shareMode);


// -LC-
//COMMON_TRANSFER FILE * C_OpenLocalFile (const char *p_fileName, const char *p_mode, int p_shareMode);

// 95/09/06 - FZ/LC - Unlock and close a file in shared mode.
// 95/09/13 - LF - wrapper for fclose, no unlock required
//COMMON_TRANSFER int C_CloseSharedFile (FILE *p_file);

//Copy directory and subdirectories from 'source' to 'destination'.
COMMON_TRANSFER int C_CopyDirectory (const char *source, const char *destination);

//Delete all files and subdirectories as well as 'path' itself.
COMMON_TRANSFER int C_DeleteDirectory (const char *path);

//Creates the directory tree 'path'.
//Returns 1 if successful, 0 otherwise.
COMMON_TRANSFER int C_MakePath (const char *path);

//Copy 'source' file to 'destination' file. Directory structure is assumed to
//exist. If 'destination' already exists, then the file will be replaced only
//if 'overwrite' is 1. Returns 1 if sucessful, 0 otherwise.
COMMON_TRANSFER int C_CopyFile (const char *source,const  char *destination);

COMMON_TRANSFER int C_DeleteFile(const char* fileName);

// Returns 1 if 'path' is a file, 0 otherwise.
COMMON_TRANSFER int C_IsFile (const char *p_path);

COMMON_TRANSFER int C_IsDirectory (const char *p_path);

// function used to query user whether or not to try to open the file again
// must be defined externally to allow for text / graphic user I/O
COMMON_TRANSFER int  LockedFileRetryFunction (const char *p_filename);

enum FileStreamOpenMode{	FSTREAM_OVERWRITE = 0, FSTREAM_APPEND, FSTREAM_READ};
//return a pointer to a fstream, if the fstream is opened sucessfully
COMMON_TRANSFER stdfstream* C_OPEN_STREAM(const char* viewfile, FileStreamOpenMode mode);
//delete the stream
COMMON_TRANSFER void C_CLOSE_STREAM(stdfstream* filePtr);

}
class CopyFileParam 
{
public:
	IProgressDialog *pProgressDialog;
	int nTotalFileCount;
	int nCurCopyFile;
	CString strDestDir;
	bool bCanceled;

	CopyFileParam()
	{
		pProgressDialog = NULL;
		nTotalFileCount = 0;
		nCurCopyFile  = 0;
		strDestDir = _T("");
		bCanceled = FALSE;
	}
	~CopyFileParam (){}

};
class COMMON_TRANSFER FileManager
{
public:
	FileManager (void) {;}

    // returns 1 if 'path' is an accessible drive
	// path should have the format "X:", plus optional path and filename
	// WARNING: floppy drive tests from a windows dos box fail
	static int IsDriveReady (const char *path);

	// Returns 1 if 'path' is a directory, 0 otherwise.
	// if path does not have a trailing '\\' IsDir will assume the path
	// contains a filename and will trim the last segment from the dir
	static int IsDirectory (const char *path);
	static int IsDirectory(const char* lpszDir, LPTSTR lpszFile);

	// Returns 1 if 'path' is a file, 0 otherwise.
	static int IsFile (const char *path);

	// returns number of free bytes on drive
	// drive should have the format "X:", plus optional path and filename
	// uses double to allow for drives larger than 4GB
	// returns -1.0 if drive not accessible
	static double GetDiskSpace (const char *drive);

	//Copy 'source' file to 'destination' file. Directory structure is assumed to
	//exist. If 'destination' already exists, then the file will be replaced only
	//if 'overwrite' is 1. Returns 1 if sucessful, 0 otherwise.
	static int CopyFile (const char *source, const char *destination, int overwrite = 1);

	//Copy contents of 'source' directory to 'destination' directory. Destination
	//directory is assumed to exist, but sub-directories are created as needed.
	//Returns 1 if sucessful, 0 otherwise.
	static int CopyDirectory (const char *source, const char *destination ,void (CALLBACK * _ShowCopyProgress)(LPCTSTR));

	//Creates the directory tree 'path'.
	//Returns 1 if successful, 0 otherwise.
	static int MakePath (const char *path);

	//Compares the modification times of two files.  Assumes that both files
	//exist. Returns -1 if 'first' is older than 'second', 0 if both modification
	//times are the same, and 1 if 'first' is newer than 'second'.
	static int IsNewerFile (const char *first, const char *second);

	//Returns the size of the file specified by 'path', or -1 on error.
	static long FileSize (const char *path);

	//Deletes file specified by 'path'. Returns 1 on success, 0 otherwise.
	static int DeleteFile(const char *path);

	//Deletes directory specified by 'path', as well as all sub-directories.
	//Returns 1 on success, 0 otherwise.
	static int DeleteDirectory (const char *path);


	// 95/09/13 - LF - wrapper for _fsopen that adds error checking
	// NOTE: read operations are only allowed to block unshared write ops
	// use fsstream to bypass this restriction
	//	static FILE *OpenSharedFile (const char *p_path, const char *p_mode,
	//		int p_shareMode = SH_DENYRW);

	// 95/09/20 - FZ - Open non critical file, try block exception will not exit program.
	//static FILE * OpenNonCriticalFile (const char *p_fileName, const char *p_mode, int p_shareMode);

	//static FILE * OpenLocalFile (const char *p_fileName, const char *p_mode, int p_shareMode);

	// 95/09/13 - LF - wrapper for fclose
	//static int CloseSharedFile (FILE *p_file);
	// 96/03/05 - FZ - Gets the number of files which exist according the the wild name.
    // the name portion of the wild name (not the extension) may contain wild cards.
	static int GetNumberOfWildFiles (const char *p_wildName);
	// 95/03/03 - FZ - Gets the i'th wild file it finds in a path whose filename name
	// portion contains wildcards.  retruns 1 if filename is good, 0 if error.
    // The index should be zero based.
	static int GetIndexedWildFileName (const int p_index, const char *p_wildName, char *p_name);




//	int CopyProjFilesWithProgress(const char *source, const char *destination,CopyFileParam *pParam );
	
	int CopyDirectoryWithProgress(const char *source, const char *destination,CopyFileParam* pParam);
	
	static int GetFileCountInDir(const char *source, int& nCount);
};


#endif // FILE_MANAGER_H
