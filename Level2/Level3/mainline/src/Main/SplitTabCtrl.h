#if !defined(AFX_SPLITTABCTRL_H__F6F2F674_255A_4457_B981_7736776DAAF1__INCLUDED_)
#define AFX_SPLITTABCTRL_H__F6F2F674_255A_4457_B981_7736776DAAF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplitTabCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplitTabCtrl window

class CSplitTabCtrl : public CTabCtrl
{
// Construction
public:
	CSplitTabCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitTabCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSplitTabCtrl();
	// Generated message map functions
protected:
	//{{AFX_MSG(CSplitTabCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLITTABCTRL_H__F6F2F674_255A_4457_B981_7736776DAAF1__INCLUDED_)
