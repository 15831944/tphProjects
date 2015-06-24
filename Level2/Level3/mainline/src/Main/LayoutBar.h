#if !defined(AFX_LAYOUTBAR_H__0D32A560_4954_4DAC_9274_35648BF1ED56__INCLUDED_)
#define AFX_LAYOUTBAR_H__0D32A560_4954_4DAC_9274_35648BF1ED56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayoutBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLayoutBar window
#include "ToolBar24X24.h"
class CLayoutBar : public CToolBar24X24
{
// Construction
public:
	CLayoutBar();

// Attributes
public:
	CComboBox* GetActiveFloorCB() { return &m_cbActiveFloor; }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayoutBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLayoutBar();

protected:
	CComboBoxXP m_cbActiveFloor;

	// Generated message map functions
protected:
	//{{AFX_MSG(CLayoutBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelChangeActiveFloorComboBox();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYOUTBAR_H__0D32A560_4954_4DAC_9274_35648BF1ED56__INCLUDED_)
