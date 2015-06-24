#if !defined(AFX_IMPORTINTOOLDPROJECTDLG_H__492B98E7_6478_4FEF_96F3_7D44EE44AA3E__INCLUDED_)
#define AFX_IMPORTINTOOLDPROJECTDLG_H__492B98E7_6478_4FEF_96F3_7D44EE44AA3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportIntoOldProjectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImportIntoOldProjectDlg dialog
#include "common\ProjectManager.h"

class CImportIntoOldProjectDlg : public CDialog
{
	static	int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
// Construction
public:
	CImportIntoOldProjectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImportIntoOldProjectDlg)
	enum { IDD = IDD_DIALOG_IMPORT_INTO_OLD_PROJECT };
	CEdit	m_editSrcZip;
	CListCtrl	m_listExistProject;
	CString	m_strSourceFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportIntoOldProjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImportIntoOldProjectDlg)
	afx_msg void OnButtonBrowse();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnGetdispinfoListExistProject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListExistProject(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	long m_lPrjectDB;
	int m_ZipFileVersion ;
private:
	bool DoImport();
	bool DoExtractFiles(CString _strPath,PROJECTINFO* pi);
	bool CreateTempZipFile(CString& strTempExtractPath);
	void DeleteTempZipFile(const CString& strTempFilePath);
	bool DoExtractInputFiles(const CString& strPath,PROJECTINFO* pi,const CString& strTempFilePath,CString& strOldProjName);
	bool DoExtractEconomicFiles(const CString& strProjectName,const CString& strPath,const CString& strTempFilePath);
	// pop folder selection, return true ok, return false cancel
	bool SelectDirectory(  const CString& _strTitle, CString& _strPath );

	CString getTempPath( const CString& _strDefault = "c:\\" );
	bool readIniFileData( const CString& _strTempExtractPath, CString& _strInputZip, CString& _strDBZip,BOOL& bNotForceDB ,PROJECTINFO* pi);
	bool unzipFiles( const CString& _strZip, const CString& _strPath );
	bool unzipInputFiles( const CString& _strInputZip, const CString& _strPath );
	bool unzipDBFiles( const CString& _strDBZip,bool bNotForceDB,const CString& _strTempExtractPath,PROJECTINFO* pi,const CString& DesstrDirectory);
	void unzipAirportFiles(const CString OldProjName , const CString& _strPath);
	void unzipAirsideExportFiles(const CString oldProjName,const CString& _strPath);
	void unzipInputLandsideFile(const CString oldProjName,const CString& _strPath);
	CString GetDBName(const CString& _strTempExtractPath) ;
	void UnZipImportDBFiles(const CString& _strTempExtractPath , const CString OldProjName,const CString& _strPath) ;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTINTOOLDPROJECTDLG_H__492B98E7_6478_4FEF_96F3_7D44EE44AA3E__INCLUDED_)
