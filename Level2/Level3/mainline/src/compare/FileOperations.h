/** \file	FileOperations.h                   
			Project: FopDemo\n
			Project type: MFC App\n
			Author: Vinnichenko Alexey\n
			E-mail: subj@mail.ru\n
			Description: Declaration of CFileOperation class and CFileExeption class.
*/ 

//#include "resource.h"

#define PATH_ERROR			-1
#define PATH_NOT_FOUND		0
#define PATH_IS_FILE		1
#define PATH_IS_FOLDER		2
#pragma once

class CFExeption
{
public:
	CFExeption(DWORD dwErrCode);
	CFExeption(CString sErrText);
	CString GetErrorText() {return m_sError;}
	DWORD GetErrorCode() {return m_dwError;}

private:
	CString m_sError;
	DWORD m_dwError;
};

class CopyFileExDescription
{
public:
	void (CALLBACK* _ShowCopyInfo)(int, LPCTSTR) ;
	CString strSourceFile;
	CString strDestFile;

};

//*****************************************************************************************************

class CFileOperation
{
public:
	CFileOperation(); // constructor
	bool Delete(CString sPathName); // delete file or folder
	bool Copy(CString sSource, CString sDest,void (CALLBACK* _ShowCopyInfo)(int, LPCTSTR)); // copy file or folder
	bool Replace(CString sSource, CString sDest); // move file or folder
	bool Rename(CString sSource, CString sDest); // rename file or folder
	CString GetErrorString() {return m_sError;} // return error description
	DWORD GetErrorCode() {return m_dwError;} // return error code
	void ShowError() // show error message
		{MessageBox(NULL, m_sError, _T("Error"), MB_OK | MB_ICONERROR);}
	void SetAskIfReadOnly(bool bAsk = true) // sets behavior with readonly files(folders)
		{m_bAskIfReadOnly = bAsk;}
	bool IsAskIfReadOnly() // return current behavior with readonly files(folders)
		{return m_bAskIfReadOnly;}
	bool CanDelete(CString sPathName); // check attributes
	void SetOverwriteMode(bool bOverwrite = false) // sets overwrite mode on/off
		{m_bOverwriteMode = bOverwrite;}
	bool IsOverwriteMode() {return m_bOverwriteMode;} // return current overwrite mode
	int CheckPath(CString sPath);
	bool IsAborted() {return m_bAborted;}

	//add by porter
	//not copying "SimResult" folder
	bool NotCopyFolderOfSimResult(CString sSource, CString sDest,void (CALLBACK* _ShowCopyInfo)(int, LPCTSTR));

protected:
	void DoDelete(CString sPathName);
	void DoCopy(CString sSource, CString sDest,void (CALLBACK* _ShowCopyInfo)(int, LPCTSTR), bool bDelteAfterCopy = false);
	void DoFileCopy(CString sSourceFile, CString sDestFile, void (CALLBACK* _ShowCopyInfo)(int, LPCTSTR),bool bDelteAfterCopy = false);
	void DoFolderCopy(CString sSourceFolder, CString sDestFolder,void (CALLBACK* _ShowCopyInfo)(int, LPCTSTR), bool bDelteAfterCopy = false);
	void DoRename(CString sSource, CString sDest);
	bool IsFileExist(CString sPathName);
	void PreparePath(CString &sPath);
	void Initialize();
	void CheckSelfRecursion(CString sSource, CString sDest);
	bool CheckSelfCopy(CString sSource, CString sDest);
	CString ChangeFileName(CString sFileName);
	CString ParseFolderName(CString sPathName);

	//add by porter
	//not copying "SimResult" folder
	void NotDoCopyFolderOfSimResult(CString sSource, CString sDest,void (CALLBACK* _ShowCopyInfo)(int, LPCTSTR), bool bDelteAfterCopy = false);
	void NoCopyFolderOfSimResult(CString sSourceFolder, CString sDestFolder,void (CALLBACK* _ShowCopyInfo)(int, LPCTSTR), bool bDelteAfterCopy = false);

private:
	CString m_sError;
	DWORD m_dwError;
	bool m_bAskIfReadOnly;
	bool m_bOverwriteMode;
	bool m_bAborted;
	int m_iRecursionLimit;
};


//*****************************************************************************************************

