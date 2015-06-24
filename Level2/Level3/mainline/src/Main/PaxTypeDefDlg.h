#if !defined(AFX_PAXTYPEDEFDLG_H__ECF8F5F0_9AA3_46B6_B301_DB359DB73923__INCLUDED_)
#define AFX_PAXTYPEDEFDLG_H__ECF8F5F0_9AA3_46B6_B301_DB359DB73923__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxTypeDefDlg.h : header file
//
#include "..\inputs\in_term.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CPaxTypeDefDlg dialog

class CPaxTypeDefDlg : public CDialog
{
// Construction
public:
	CPaxTypeDefDlg(CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPaxTypeDefDlg)
	enum { IDD = IDD_DIALOG_PAXTYPEDEF };
	CStatic	m_toolbarcontenter;
	CButton	m_btnSave;
	CTreeCtrl	m_treePax;
	CComboBox	m_comboLevel;
	CListBox	m_listboxName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxTypeDefDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitToolbar();
	CToolBar m_ToolBar;
	bool m_bStructureChanged;
	void InsertPaxItem( HTREEITEM _hParent, int _nIdxMinLevel );
	void DeleteAllChildren(HTREEITEM hItem);
	CString GetProjPath();
	InputTerminal* GetInputTerminal();
	void ReloadLevelCombo();
	void ReloadTree();
	void ReloadName( int _nIdxLevel );
	void EditItem( int _nIdx );

	int m_nSelIdx;
	// Generated message map functions
	//{{AFX_MSG(CPaxTypeDefDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListPaxname();
	afx_msg LONG OnEndEdit( WPARAM p_wParam, LPARAM p_lParam );
	afx_msg void OnSelchangeComboLevel();
	afx_msg void OnDropdownComboLevel();
	afx_msg void OnSelchangeListPaxname();
	afx_msg void OnButtonSave();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnPeoplemoverNew();
	afx_msg void OnPeoplemoverDelete();
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CEdit m_EditLevelName;
public:
	afx_msg void OnEnKillfocusEditName();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXTYPEDEFDLG_H__ECF8F5F0_9AA3_46B6_B301_DB359DB73923__INCLUDED_)
