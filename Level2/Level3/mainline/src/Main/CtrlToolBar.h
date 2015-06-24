#if !defined(AFX_CTRLTOOLBAR_H__2786C703_D336_4170_9BB0_769AC9CB392C__INCLUDED_)
#define AFX_CTRLTOOLBAR_H__2786C703_D336_4170_9BB0_769AC9CB392C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CtrlToolBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCtrlToolBar window

class CCtrlToolBar : public CToolBar
{
// Construction
public:
	CCtrlToolBar();

// Attributes
public:

private:
	CWnd* m_pParentWnd;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCtrlToolBar)
	virtual LRESULT DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam );	
	//}}AFX_VIRTUAL
	BOOL Create( CWnd* pParentWnd, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP, UINT nID = AFX_IDW_TOOLBAR );	
	BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP, CRect rcBorders = CRect(0, 0, 0, 0), UINT nID = AFX_IDW_TOOLBAR);	
// Implementation
public:
	virtual ~CCtrlToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCtrlToolBar)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTRLTOOLBAR_H__2786C703_D336_4170_9BB0_769AC9CB392C__INCLUDED_)
