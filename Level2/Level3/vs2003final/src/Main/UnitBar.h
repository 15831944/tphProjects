#if !defined(AFX_UNITBAR_H__863821C6_4157_4E9B_AE0B_FDC0EE7BA8CB__INCLUDED_)
#define AFX_UNITBAR_H__863821C6_4157_4E9B_AE0B_FDC0EE7BA8CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UnitBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUnitBar window
#include "ToolBar24X24.h"
#include "XPStyle/ComboBoxXP.h"
#include "afxtempl.h"
class CTermPlanDoc;
class CUnitBar : public CToolBar24X24
{
// Construction
public:
	CUnitBar();

// Attributes
public: 
	CArray<CComboBoxXP*,CComboBoxXP*> m_arUnitComboBox;

// Operations
protected:
	void AddArrowButtonAtTail(void);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnitBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	CComboBox* GetComboBoxWU();
	CComboBox* GetComboBoxLU();
	CComboBox* GetComboBoxTU();
	CComboBox* GetComboBoxVU();
	CComboBox* GetComboBoxAU();
	void UpdateUnitBar(CTermPlanDoc* pDoc);
	virtual ~CUnitBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUnitBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg void OnSelectWUnits();
	afx_msg void OnSelectLUnits();
	afx_msg void OnSelectTUnits();
	afx_msg void OnSelectVUnits();
	afx_msg void OnSelectAUnits();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTbnDropDown(NMHDR *pNMHDR, LRESULT *pResult);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNITBAR_H__863821C6_4157_4E9B_AE0B_FDC0EE7BA8CB__INCLUDED_)
