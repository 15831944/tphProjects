#if !defined(AFX_COLORTREECTRL_H__30E1C50D_FD9B_4961_8F2D_6A0C5C050520__INCLUDED_)
#define AFX_COLORTREECTRL_H__30E1C50D_FD9B_4961_8F2D_6A0C5C050520__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorTreeCtrl.h : header file
//
#define DEFCOLOR RGB(53,151,53)
#include "TreeCtrlEx.h"
/////////////////////////////////////////////////////////////////////////////
// CColorTreeCtrl window

class CColorTreeCtrl : public CTreeCtrlEx
{
// Construction
public:
	CColorTreeCtrl();

// Attributes
public:
private:
	COLORREF m_DefColor;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	COLORREF GetTreeDefColor();
	COLORREF GetItemColor(HTREEITEM hItem);
	BOOL GetItemFont(HTREEITEM hItem, LOGFONT * plogfont);
	BOOL GetItemBold(HTREEITEM hItem);

	void SetTreeDefColor( COLORREF color );
	void SetItemColor(HTREEITEM hItem, COLORREF color = -1);
	void SetItemFont(HTREEITEM hItem, LOGFONT& logfont);
	void SetItemBold(HTREEITEM hItem, BOOL bBold);

	BOOL DeleteItem( HTREEITEM hItem );
	BOOL DeleteAllItems( );
	virtual ~CColorTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorTreeCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG
private:
	struct Color_Font
	{
		COLORREF color;
		LOGFONT  logfont;
	};
	CMap< void*, void*, Color_Font, Color_Font& > m_mapColorFont ;
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORTREECTRL_H__30E1C50D_FD9B_4961_8F2D_6A0C5C050520__INCLUDED_)
