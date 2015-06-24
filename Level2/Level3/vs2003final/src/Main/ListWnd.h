#pragma once

// ListWnd.h : header file
//
#include "ThumbnailsList2.h"


#define LISTWND_WIDTH             410
#define LISTWND_HEIGHT            360
#define LISTTYPEWND_WIDTH         90


/////////////////////////////////////////////////////////////////////////////
// CListWnd window
class CListWnd : public CWnd
{
	// Construction
public:
	CListWnd();
	virtual ~CListWnd();

	// Attributes
public:

	// Operations
public:
	void ShouldHideParent(BOOL bHide = TRUE);
	// display a static bitmap at the right boundary of the m_listCtrlItems
	//      pass in the CThumbnailsList2 pointer to identify which the control is
	void DisplayPreviewCtrl(CThumbnailsList2* pThumbnailList/* add by Benny */);
	void CompleteSelect(CThumbnailsList2* pThumbnailList/* add by Benny */);
	
	CWnd* GetParentWnd();
	CWnd* SetParentWnd(CWnd* _pNewParent);

	void SetShapeItem(int nShapeItem);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListWnd)
	//}}AFX_VIRTUAL

	// Implementation
private:
	void UpdateList(int nSeledIndex);

	CWnd* m_pParent;

	BOOL CreateListItems();
	BOOL CreateListSets();
	BOOL CreatePreviewCtrl();

	CThumbnailsList2 m_listCtrlItems;
	CThumbnailsList2 m_listCtrlSets;
	CStatic	m_staticPreviewCtrl;

	BOOL	m_bHideParent;
	
	int m_nShapeItem;
	// Generated message map functions
protected:
	//{{AFX_MSG(CListWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClickListItems(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListSets(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	afx_msg LRESULT OnListCtrlKillFocus(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};


