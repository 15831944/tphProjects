#pragma once

class CTmpFileManager
{
protected:
	//CTmpFileManager(void);
	//~CTmpFileManager(void);

private:
	//CString m_strTmpBaseFolder;
public:
	CString GetTmpBaseFolder() const;
	//void SetBaseFolder( CString strBase );
	CString GetTmpFileName(LPCTSTR lpszExt = _T("tmp"));
	void DeleteBaseFolder() ;
	CString GetTmpDirectory();

	static CTmpFileManager& GetInstance();

	// two static methods to delete directory
	static DWORD DeleteDirectory1(LPCTSTR lpszDir);
	static bool DeleteDirectory2(LPCTSTR lpszDir);

	static void DeleteDirectoryFile(CString strregex, CString dir);
};
