#ifndef __LISTWNDEX_H__
#define __LISTWNDEX_H__

// ListWnd.h : header file
//
#include "ThumbnailsListEx.h"


#define LISTWND_WIDTH             410
#define LISTWND_HEIGHT            360
#define LISTTYPEWND_WIDTH         90
/////////////////////////////////////////////////////////////////////////////
// CListWndEX window
class CListWndEX :
	public CWnd
{
public:
	CListWndEX();
	virtual ~CListWndEX();

	// Attributes
public:

	// Operations
public:
	void ShouldHideParent(BOOL bHide = TRUE);
	// display a static bitmap at the right boundary of the m_listCtrlItems
	//      pass in the CThumbnailsListEx pointer to identify which the control is
	void DisplayPreviewCtrl(CThumbnailsListEx* pThumbnailList/* add by Benny */);
	void CompleteSelect(CThumbnailsListEx* pThumbnailList/* add by Benny */);

	CWnd* GetParentWnd();
	CWnd* SetParentWnd(CWnd* _pNewParent);

	void SetShapeItem(int nShapeItem);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListWndEX)
	//}}AFX_VIRTUAL

	// Implementation
private:
	void UpdateList(int nSeledIndex);

	CWnd* m_pParent;

	BOOL CreateListItems();
	BOOL CreateListSets();
	BOOL CreatePreviewCtrl();

	CThumbnailsListEx m_listCtrlItems;
	CThumbnailsListEx m_listCtrlSets;
	CStatic	m_staticPreviewCtrl;

	BOOL	m_bHideParent;

	int m_nShapeItem;
	// Generated message map functions
protected:
	//{{AFX_MSG(CListWndEX)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClickListItems(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListSets(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	afx_msg LRESULT OnListCtrlKillFocus(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
#endif // __LISTWNDEX_H__