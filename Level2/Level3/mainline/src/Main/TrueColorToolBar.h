
#pragma once

// TrueColorToolBar.h : header file

#include <afxpriv.h>

/////////////////////////////////////////////////////////////////////////////
// CTrueColorToolBar window

class CTrueColorToolBar : public CToolBar
{
// Construction
public:
	CTrueColorToolBar();

// Attributes
private:

	
// Operations
public:
	BOOL LoadTrueColorToolBar(int nBtnWidth,
							  UINT uToolBar,
							  UINT uToolBarHot,
							  UINT uToolBarDisabled = 0);

private:
	BOOL SetTrueColorToolBar(UINT uToolBarType,
		                     UINT uToolBar,
						     int nBtnWidth);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrueColorToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTrueColorToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTrueColorToolBar)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	DECLARE_MESSAGE_MAP()
};

