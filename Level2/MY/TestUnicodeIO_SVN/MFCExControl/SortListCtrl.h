/*******************************************

             没事儿，尽管用。
                          --栾义明
comment:
       貌似没事，实则有事 。 
*********************************************/
#ifndef SORTLISTCTRL_H
#define SORTLISTCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef SORTHEADERCTRL_H
	#include "SortHeaderCtrl.h"
#endif	// SORTHEADERCTRL_H

#ifdef _DEBUG
	#define ASSERT_VALID_STRING( str ) ASSERT( !IsBadStringPtr( str, 0xfffff ) )
#else	//	_DEBUG
	#define ASSERT_VALID_STRING( str ) ( (void)0 )
#endif	//	_DEBUG

struct ItemData
{
public:
	ItemData() : arrpsz( NULL ), dwData( NULL ),crText ( NULL ),crBak (NULL) {}
	
	LPTSTR* arrpsz;
	DWORD dwData;
	//color
	COLORREF * crText;
	COLORREF * crBak;
	int      nACTermUser;	
	
private:
	// ban copying.
	ItemData( const ItemData& );
	ItemData& operator=( const ItemData& );
};


class MFCEXCONTROL_API CSortListCtrl : public CListCtrl
{
// Construction
public:
	CSortListCtrl();

// Attributes
public:

// Operations
public:
	BOOL SetHeadings( UINT uiStringID );
	BOOL SetHeadings( const CString& strHeadings );

	int AddItem( int nACTuser, LPCTSTR pszText, ... );
	int AddItem( int nACTuser,CStringArray& strArray);
	BOOL DeleteItem( int iItem );
	BOOL DeleteAllItems();
	void LoadColumnInfo();
	void SaveColumnInfo();
	BOOL SetItemText( int nItem, int nSubItem, LPCTSTR lpszText );
	void Sort( int iColumn, BOOL bAscending );
	void Sort();	
	BOOL SetItemData(int nItem, DWORD dwData);
	DWORD GetItemData(int nItem) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortListCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetItemColor(int nItem, int nSubItem, COLORREF clrText, COLORREF clrBkgnd);
	void DrawText(int nItem, int nSubItem, CDC *pDC, COLORREF crText, COLORREF crBkgnd, CRect &rect);
	BOOL GetSubItemRect(int nItem, int nSubItem, int nArea, CRect& rect);
	int GetColumns();
	int AddItemColor(LPCTSTR pszText,COLORREF crText,COLORREF crBak);
	virtual ~CSortListCtrl();
	int GetColumnCount(){ return m_iNumColumns ;}
	void AddColumnCount(){ m_iNumColumns++ ;}

	// Generated message map functions
protected:
	void UpdateSubItem(int nItem, int nSubItem);
	static int CALLBACK CompareFunction( LPARAM lParam1, LPARAM lParam2, LPARAM lParamData );
	void FreeItemMemory( const int iItem );
	BOOL CSortListCtrl::SetArray(int nACTuser, int iItem, LPTSTR* arrpsz ,COLORREF * clrText,COLORREF * clrBak);
	LPTSTR* CSortListCtrl::GetTextArray( int iItem ) const;
	

	CSortHeaderCtrl m_ctlHeader;

	int m_iNumColumns;
	int m_iSortColumn;
	BOOL m_bSortAscending;

	//{{AFX_MSG(CSortListCtrl)
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnSysColorChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	COLORREF crWindowText,crWindow,crHighLight,crHighLightText;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // SORTLISTCTRL_H
