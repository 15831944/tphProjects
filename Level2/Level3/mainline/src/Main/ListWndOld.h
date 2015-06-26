#if !defined(AFX_LISTWND_H__E37470B1_1372_4197_98B2_AC679FDA6D78__INCLUDED_)
#define AFX_LISTWND_H__E37470B1_1372_4197_98B2_AC679FDA6D78__INCLUDED_

#include "ThumbnailsList.h"	// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListWndOld window
#define LISTWND_WIDTH_OLD                  400
#define LISTWND_HEIGHT_OLD                 350
#define LISTTYPEWND_WIDTH_OLD              80

class CListWndOld : public CWnd
{
// Construction
public:
	CListWndOld();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListWndOld)
	//}}AFX_VIRTUAL

// Implementation
public:
	void CompleteSelect();
	void UpdateList(int nSeledIndex);
	CWnd* m_pParent;
	BOOL CreateListType();
	void DisplayPreviewCtrl(int nSeledIndex);
	BOOL CreatePreviewCtrl();
	BOOL CreateList();

	void ShouldHideParent(BOOL bHide = TRUE);
	void SetImageDir(const CString& str);
	//void SetLabelString(const char* szLabel[]);

	CThumbnailsList m_listCtrl;
	CThumbnailsList m_listCtrlType;
	
	CStatic	m_staticPreviewCtrl;
	virtual ~CListWndOld();

protected:
	BOOL	m_bHideParent;

	// Generated message map functions
protected:
	//{{AFX_MSG(CListWndOld)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClickListThumb(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblClickListThumb(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListType(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblClickListType(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	afx_msg LRESULT OnListCtrlKillFocus(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTWND_H__E37470B1_1372_4197_98B2_AC679FDA6D78__INCLUDED_)
