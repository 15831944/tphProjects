#if !defined(AFX_CHILDFRAMEDENSITYANIM_H__2899FBFD_0757_4BAA_9A91_1706B0B9DB7F__INCLUDED_)
#define AFX_CHILDFRAMEDENSITYANIM_H__2899FBFD_0757_4BAA_9A91_1706B0B9DB7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChildFrameDensityAnim.h : header file
//

class CDensityGLView;
/////////////////////////////////////////////////////////////////////////////
// CChildFrameDensityAnim frame
#include "XPStyle/StatusBarXP.h"

class CChildFrameDensityAnim : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrameDensityAnim)
protected:
	CChildFrameDensityAnim();           // protected constructor used by dynamic creation

// Attributes
public:
	CStatusBarXP  m_wndStatusBar;

// Operations
protected:
	CDensityGLView* GetDensityAnimView();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrameDensityAnim)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
      virtual void OnUpdateFrameMenu (BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CChildFrameDensityAnim();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CChildFrameDensityAnim)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRAMEDENSITYANIM_H__2899FBFD_0757_4BAA_9A91_1706B0B9DB7F__INCLUDED_)
