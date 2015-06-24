#if !defined(AFX_EDITABLELISTBOX_H__24BA31FC_1440_414B_B272_11723F7BFB6D__INCLUDED_)
#define AFX_EDITABLELISTBOX_H__24BA31FC_1440_414B_B272_11723F7BFB6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditableListBox.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#define		WM_APP_LB_ITEM_EDITED			( WM_APP + 04100 )
		
#include "../MFCExControl/EditHelper.h"

/////////////////////////////////////////////////////////////////////////////
// CEditableListBox window

class CEditableListBox : public CListBox
{
// Construction
public:
	CEditableListBox();

// Attributes
protected:
	CEditHelper		m_ceEdit;
	int				m_iItemBeingEdited;

// Operations
protected:
	void	EditStarts	();
	void	EditEnds	( BOOL bCommitText = TRUE );

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditableListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditableListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditableListBox)
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnEditFinished(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITABLELISTBOX_H__24BA31FC_1440_414B_B272_11723F7BFB6D__INCLUDED_)
