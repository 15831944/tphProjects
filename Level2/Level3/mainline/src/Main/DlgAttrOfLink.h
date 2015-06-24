#if !defined(AFX_DLGATTROFLINK_H__F8B0916B_D45B_4EB2_894B_7A745063840F__INCLUDED_)
#define AFX_DLGATTROFLINK_H__F8B0916B_D45B_4EB2_894B_7A745063840F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAttrOfLink.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAttrOfLink dialog

class CDlgAttrOfLink : public CDialog
{
// Construction
public:
	CDlgAttrOfLink(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgAttrOfLink)
	enum { IDD = ID_DIALOG_PFV_ATTROFLINK };
	CSpinButtonCtrl	m_spnDistance;
	CSpinButtonCtrl	m_spnRate;
	UINT	m_nRate;
	UINT	m_nDistance;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAttrOfLink)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAttrOfLink)
	virtual void OnOK();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpin4(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGATTROFLINK_H__F8B0916B_D45B_4EB2_894B_7A745063840F__INCLUDED_)
