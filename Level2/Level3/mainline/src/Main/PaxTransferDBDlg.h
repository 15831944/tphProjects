#if !defined(AFX_PAXTRANSFERDBDLG_H__A61B26FC_B560_45D2_8761_0D43E9B7DB21__INCLUDED_)
#define AFX_PAXTRANSFERDBDLG_H__A61B26FC_B560_45D2_8761_0D43E9B7DB21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxTransferDBDlg.h : header file
//
#include "inputs\con_db.h"
#include "..\inputs\in_term.h"


/////////////////////////////////////////////////////////////////////////////
// CPaxTransferDBDlg dialog

class CPaxTransferDBDlg : public CDialog
{
// Construction
public:
	CPaxTransferDBDlg( ConstraintEntry* _pEntry, CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPaxTransferDBDlg)
	enum { IDD = IDD_DIALOG_PAXTRANSFERDB };
	CButton	m_btnSet;
	CStatic	m_staticPercentage;
	CSpinButtonCtrl	m_spinPercentage;
	CEdit	m_editPercentage;
	CButton	m_btnDel;
	CListCtrl m_listctrlFltDist;
	CString	m_csPaxType;
	UINT	m_nPercentage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxTransferDBDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SelectFltDist( int _nIdx );
	void ReloadFltDist( int _nIdx );
	ConstraintEntry* m_pEntry;

	// Generated message map functions
	//{{AFX_MSG(CPaxTransferDBDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDelete();
	afx_msg void OnClickListFltdistr(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinPercentage(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonSet();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXTRANSFERDBDLG_H__A61B26FC_B560_45D2_8761_0D43E9B7DB21__INCLUDED_)
