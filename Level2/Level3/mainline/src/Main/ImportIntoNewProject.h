#if !defined(AFX_IMPORTINTONEWPROJECT_H__D9EDA179_1385_42EE_AC78_436731120526__INCLUDED_)
#define AFX_IMPORTINTONEWPROJECT_H__D9EDA179_1385_42EE_AC78_436731120526__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportIntoNewProject.h : header file
//
#include "common\ProjectManager.h"
/////////////////////////////////////////////////////////////////////////////
// CImportIntoNewProject dialog

class CImportIntoNewProject : public CDialog
{
// Construction
public:
	CImportIntoNewProject(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImportIntoNewProject)
	enum { IDD = IDD_DIALOG_IMPORT_INTO_NEW_PROJECT };
	CEdit	m_controlProjectName;
	CString	m_strProjectName;
	CString	m_strFileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportIntoNewProject)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImportIntoNewProject)
	afx_msg void OnButtonBrowse();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool DoExtractFiles(CString _strPath);
	void DoCreateNewProject();

	bool CreateTempZipFile(CString& strTempExtractPath);
	void DeleteTempZipFile(const CString& strTempFilePath);
	bool DoExtractInputFiles(const CString& strPath,const CString& strTempFilePath,CString& strOldProjName);
	bool DoExtractEconomicFiles(const CString& strProjectName,const CString& strPath,const CString& strTempFilePath);

	// pop folder selection, return true ok, return false cancel
	bool SelectDirectory(  const CString& _strTitle, CString& _strPath );	

	CString getTempPath( const CString& _strDefault = "c:\\" );

	bool unzipFiles( const CString& _strZip, const CString& _strPath );

	bool readIniFileData( const CString& _strTempExtractPath, CString& _strInputZip, CString& _strDBZip,
		CString& _strMathematicZip, CString& _strMathResultZip,
		BOOL& bNotForceDB);

	bool unzipInputFiles( const CString& _strInputZip, const CString& _strPath );

	bool unzipDBFiles( const CString& _strDBZip,bool bNotForceDB ,const CString& _strTempExtractPath,const CString& DesstrDirectory);
	
	bool unzipAirportFiles(const CString OldProjName , const CString& _strPath);

	bool unzipAirsideExportFiles(const CString& oldProjName, const CString& _strPath);
	bool unzipInputLandsideFile(const CString oldProjName,const CString& _strPath);
	bool unzipInputUserBarsFile(const CString oldProjName,const CString& _strPath);
private:
	PROJECTINFO m_projInfo;
	int m_ZipFileVersion ;

	CString GetDBName(const CString& _strTempExtractPath) ;
	void UnZipImportDBFiles(const CString& _strTempExtractPath , const CString OldProjName,const CString& _strPath) ;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTINTONEWPROJECT_H__D9EDA179_1385_42EE_AC78_436731120526__INCLUDED_)
