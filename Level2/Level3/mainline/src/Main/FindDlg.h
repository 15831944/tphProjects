#if !defined(AFX_FINDDLG_H__B02B1F65_DFCB_4FBB_855B_02608D813E5C__INCLUDED_)
#define AFX_FINDDLG_H__B02B1F65_DFCB_4FBB_855B_02608D813E5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindDlg dialog
#include <vector>
#include <iostream>

class CFindDlg : public CDialog
{
// Construction
public:
	CFindDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindDlg)
	enum { IDD = IDD_DIALOG_FIND };
	CComboBox	m_ctrlComboBox;
	CString	m_strFind;
	BOOL	m_bMatchWholeWord;
	//}}AFX_DATA
	
	std::vector< CString > m_vStrBuff;
    BOOL m_bType ; //FALSE ; find word , TRUE; mark word
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CFindDlg)
	afx_msg void OnButtonMarkall();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDDLG_H__B02B1F65_DFCB_4FBB_855B_02608D813E5C__INCLUDED_)
