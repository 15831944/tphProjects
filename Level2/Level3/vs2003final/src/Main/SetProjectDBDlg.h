#if !defined(AFX_SETPROJECTDBDLG_H__B2D23ACA_5B9A_4707_AE87_978EC7D55355__INCLUDED_)
#define AFX_SETPROJECTDBDLG_H__B2D23ACA_5B9A_4707_AE87_978EC7D55355__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetProjectDBDlg.h : header file
//
//include
#include "..\MFCExControl\listctrlex.h"
#include "GlobalDBSheet.h"

// declare

/////////////////////////////////////////////////////////////////////////////
// CSetProjectDBDlg dialog

class CSetProjectDBDlg : public CPropertyPage
{
// Construction
public:
	CSetProjectDBDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetProjectDBDlg)
	enum { IDD = IDD_DIALOG_SETDBFORPRJ };
	CButton	m_btnSave;
	CStatic	m_staticTitle;
	CListCtrlEx	m_listProject;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetProjectDBDlg)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetProjectDBDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnDblclkListProject(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg LRESULT OnInplaceCombox( WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	CToolBar m_ToolBar;
	PROJECTINFO_LIST*	m_pAllProjInfo;

private:
	void InitToolBar( void );
	void InitListCtrl( void );
	void LoadDataToList( void );
	BOOL UpdateAirportDBBNameFOrDataBase(PROJECTINFO* _proInfo) ;

public:
	void setAllProjectList( PROJECTINFO_LIST* _pAllProjectInfo ) { m_pAllProjInfo = _pAllProjectInfo; }
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETPROJECTDBDLG_H__B2D23ACA_5B9A_4707_AE87_978EC7D55355__INCLUDED_)
