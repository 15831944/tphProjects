#if !defined(AFX_PROJECTSELECTBAR_H__B6BA59F3_62EF_408A_B780_38B39C48F27A__INCLUDED_)
#define AFX_PROJECTSELECTBAR_H__B6BA59F3_62EF_408A_B780_38B39C48F27A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjectSelectBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProjectSelectBar window

class CProjectSelectBar : public CToolBar
{
// Construction
public:
	//CProjectSelectBar();
	//virtual ~CProjectSelectBar();

// Attributes
public:

// Operations
public:
	CComboBox* GetComboBox() { return &m_wndProjSelect; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectSelectBar)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	CFont m_font;
	CComboBox m_wndProjSelect;
	//{{AFX_MSG(CProjectSelectBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	//afx_msg void OnSelectProject();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTSELECTBAR_H__B6BA59F3_62EF_408A_B780_38B39C48F27A__INCLUDED_)
