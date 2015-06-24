#if !defined(AFX_UNITBAR1_H__863821C6_4157_4E9B_AE0B_FDC0EE7BA8CB__INCLUDED_)
#define AFX_UNITBAR1_H__863821C6_4157_4E9B_AE0B_FDC0EE7BA8CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UnitBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUnitBar window
#include "ToolBar24X24.h"
#include "XPStyle/ComboBoxXP.h"

class CUnitBar1 : public CToolBar24X24
{
// Construction
public:
	CUnitBar1();

// Attributes
public:
	CComboBoxXP m_cbUint1;
	CComboBoxXP m_cbUint2;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnitBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	CComboBox* GetComboBoxWU();
	CComboBox* GetComboBoxLU();
	virtual ~CUnitBar1();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUnitBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg void OnSelectWUnits();
	afx_msg void OnSelectLUnits();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNITBAR1_H__863821C6_4157_4E9B_AE0B_FDC0EE7BA8CB__INCLUDED_)
