#if !defined(AFX_GLOBALDBMANDLG_H__0E9FCE02_72A1_43A2_9426_E4E0B474A593__INCLUDED_)
#define AFX_GLOBALDBMANDLG_H__0E9FCE02_72A1_43A2_9426_E4E0B474A593__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GlobalDBManDlg.h : header file
//

//include 
#include "resource.h"
#include "GlobalDBSheet.h"
#include "..\MFCExControl\listctrlex.h"

// declare
class CAirportDatabase;

/////////////////////////////////////////////////////////////////////////////
// CGlobalDBManDlg dialog

class CGlobalDBManDlg : public CPropertyPage
{
// Construction
public:
	CGlobalDBManDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGlobalDBManDlg)
	enum { IDD = IDD_DIALOG_GLOBALDBLIST };
	CButton	m_btnSave;
	CStatic	m_staticTitle;
	CListCtrlEx	m_listDBMan;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGlobalDBManDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGlobalDBManDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemchangedListGlobaldb(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListGlobaldb(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGlobaldbmanCopy();
	afx_msg void OnGlobaldbmanDelete();
	afx_msg void OnGlobaldbmanAc();
	afx_msg void OnGLOBALDBMANAirline();
	afx_msg void OnGlobaldbmanAirport();
	afx_msg void OnGlobaldbmanProbdist();
	afx_msg void OnDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnGlobaldbmanNew();
	afx_msg void OnGlobaldbmanEdit();
	//}}AFX_MSG
	afx_msg LRESULT OnInplaceEdit(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

private:
	CToolBar			m_ToolBar;
	PROJECTINFO_LIST*	m_pAllProjInfo;

private:
	void CopyAirportDBFile(LPCSTR sSourcesDBPath, LPCSTR sDestDBPath);
	BOOL RenameAirportDB(CAirportDatabase* pDB, LPCSTR szNewName);
	CString CreateNewAirportDBFolder(const CString& sDBName);
	BOOL CheckDBIsOpening(const CString& sDbName,CString& sReturnProjectName);

	//validate airportdatabase file if existed, 
	//and copy default file to supersede the loss file.
	//you'd better call it before airportdatabase::loadDatabase()
	void ValidateDBFile(const CString& sDestPath);
	void InitToolBar( void );
	void InitListCtrl( void );
	void EnableAllToolBarButton( BOOL _bEnable = TRUE );
	// pop folder selection, return true ok, return false cancel
	bool SelectDirectory(  const CString& _strTitle, CString& _strPath );
	
	// before delete DB, check foreign key
	bool CheckForeignKey( const CAirportDatabase* _pDelDB );

	void LoadDataToList( void );

public:
	void setAllProjectList( PROJECTINFO_LIST* _pAllProjectInfo ) { m_pAllProjInfo = _pAllProjectInfo; }
};

int CALLBACK BrowseCallbackProc( HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData );

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLOBALDBMANDLG_H__0E9FCE02_72A1_43A2_9426_E4E0B474A593__INCLUDED_)
