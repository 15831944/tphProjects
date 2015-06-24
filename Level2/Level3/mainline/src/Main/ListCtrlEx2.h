#if !defined(AFX_LISTCTRLEX2_H__620A301D_A9C1_4031_8D9C_CE999BA9FD7A__INCLUDED_)
#define AFX_LISTCTRLEX2_H__620A301D_A9C1_4031_8D9C_CE999BA9FD7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlEx2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx2 window

class CListCtrlEx2 : public CListCtrl
{
// Construction
public:
	CListCtrlEx2();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlEx2)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListCtrlEx2();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlEx2)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLEX2_H__620A301D_A9C1_4031_8D9C_CE999BA9FD7A__INCLUDED_)
