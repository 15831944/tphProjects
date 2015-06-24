#if !defined(AFX_CONWITHIDDBLISTCTRL_H__3B2481BD_73DC_4349_B857_B3D415069D4F__INCLUDED_)
#define AFX_CONWITHIDDBLISTCTRL_H__3B2481BD_73DC_4349_B857_B3D415069D4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConWithIDDBListCtrl.h : header file
//
#include "condblistctrl.h"
/////////////////////////////////////////////////////////////////////////////
// CConWithIDDBListCtrl window

class CConWithIDDBListCtrl : public CConDBListCtrl
{
// Construction
public:
	CConWithIDDBListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConWithIDDBListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CConWithIDDBListCtrl();
	//Reload data from the memory to the list control
	virtual void ReloadData( Constraint* _pSelCon );
	virtual bool AddEntry(enum CON_TYPE _enumConType, Constraint* _pNewCon);

	void SetProjectID(int nPrjID);
	int m_nPrjID;
	// Generated message map functions
protected:
	//{{AFX_MSG(CConWithIDDBListCtrl)
	afx_msg BOOL OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONWITHIDDBLISTCTRL_H__3B2481BD_73DC_4349_B857_B3D415069D4F__INCLUDED_)
