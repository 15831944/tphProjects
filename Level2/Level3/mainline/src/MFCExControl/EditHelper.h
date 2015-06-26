#if !defined(AFX_EDITHELPER_H__546F7AFD_CFEC_416A_8896_B689665E3E86__INCLUDED_)
#define AFX_EDITHELPER_H__546F7AFD_CFEC_416A_8896_B689665E3E86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MFCExControlAPI.h"
// EditHelper.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#define		WM_APP_ED_EDIT_FINISHED			( WM_APP + 04101 )		

/////////////////////////////////////////////////////////////////////////////
// CEditHelper window

class MFCEXCONTROL_API CEditHelper : public CEdit
{
// Construction
public:
	CEditHelper();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditHelper)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditHelper();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditHelper)
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITHELPER_H__546F7AFD_CFEC_416A_8896_B689665E3E86__INCLUDED_)
