#if !defined(AFX_CAMERABAR_H__5C241353_1D94_498D_AA19_D693517D069C__INCLUDED_)
#define AFX_CAMERABAR_H__5C241353_1D94_498D_AA19_D693517D069C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CameraBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCameraBar window
#include "ToolBar24X24.h"
class CCameraBar : public CToolBar24X24
{
// Construction
public:
	CCameraBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCameraBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCameraBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCameraBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAMERABAR_H__5C241353_1D94_498D_AA19_D693517D069C__INCLUDED_)
