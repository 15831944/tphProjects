#if !defined(AFX_DLGFLOORTHICKNESS_H__4160A0C9_0E5E_40AF_8883_95FB5689332F__INCLUDED_)
#define AFX_DLGFLOORTHICKNESS_H__4160A0C9_0E5E_40AF_8883_95FB5689332F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFloorThickness.h : header file
//
#include "TermPlanDoc.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgFloorThickness dialog

class CDlgFloorThickness : public CDialog
{
// Construction
public:
	CDlgFloorThickness(double _thickness,CTermPlanDoc* pDoc ,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFloorThickness)
	enum { IDD = IDD_DIALOG_FLOORTHICKNESS };
	CSpinButtonCtrl	m_spinThickness;
	CString	m_sUnits;
	//}}AFX_DATA
	CTermPlanDoc* m_pDoc;
	double GetThickness() { return m_dThickness; }


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFloorThickness)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	double m_dThickness;


// Implementation
protected:

	void UpdateTextField();

	// Generated message map functions
	//{{AFX_MSG(CDlgFloorThickness)
	afx_msg void OnKillfocusEditThickness();
	afx_msg void OnDeltaposSpinThickness(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFLOORTHICKNESS_H__4160A0C9_0E5E_40AF_8883_95FB5689332F__INCLUDED_)
