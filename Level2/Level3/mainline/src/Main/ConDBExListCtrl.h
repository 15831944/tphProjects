#if !defined(AFX_CONDBEXLISTCTRL_H__4B807B97_2B16_46A9_966B_E1613E71C736__INCLUDED_)
#define AFX_CONDBEXLISTCTRL_H__4B807B97_2B16_46A9_966B_E1613E71C736__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// ConDBExListCtrl.h : header file
//
#include "ConDBListCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CConDBExListCtrl window

class CConDBExListCtrl : public CConDBListCtrl
{
// Construction
public:
	CConDBExListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConDBExListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CConDBExListCtrl();
	//Reload data from the memory to the list control
	virtual void ReloadData( Constraint* _pSelCon );
	virtual bool AddEntry(enum CON_TYPE _enumConType, Constraint* _pNewCon);

	void InsertOtherCol(int bBeginIndex);
	// Generated message map functions
protected:
	//{{AFX_MSG(CConDBExListCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg LRESULT OnInplaceDateTime( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONDBEXLISTCTRL_H__4B807B97_2B16_46A9_966B_E1613E71C736__INCLUDED_)
