#if !defined(AFX_SORTABLEHEADERCTRL_H__0A345281_0458_11D2_9AED_2F8C0624A000__INCLUDED_)
#define AFX_SORTABLEHEADERCTRL_H__0A345281_0458_11D2_9AED_2F8C0624A000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "MySpin.h"
// SortableHeaderCtrl.h : header file
//
#define MAX_COLUMNS	     300
#define MULTI_COLUMN_SORT  1
#define SINGLE_COLUMN_SORT 0

enum EDataType { dtNULL, dtINT, dtSTRING, dtDATETIME, dtDEC, dtTIME };



/////////////////////////////////////////////////////////////////////////////
// CSortableHeaderCtrl window

class MFCEXCONTROL_API CSortableHeaderCtrl : public CHeaderCtrl
{
// Construction
protected:
 	DECLARE_DYNCREATE(CSortableHeaderCtrl)

public:
	CSortableHeaderCtrl();
	int     m_nSortCol;
    BOOL    m_bSortAsc;
	CBitmap m_bmpArrowUp;
	CBitmap m_bmpArrowDown;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSortableHeaderCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual		~CSortableHeaderCtrl();
	bool			GetItemSortState(int iItem);
	const int	GetLastColumn() const;
	void			LoadSortList();
	void			RemoveAllSortImages();
	void			RemoveSortImage( int iItem );
   void SetDataType(int iItem, EDataType Type);
	void			SetItemSortState(int iItem, bool bSortState);
	void			SetRegString( CString p_csRegSect, CString p_csRegEntry );
	void			SetWithFilter( bool p_bWithFilter=true );
	void			SaveSortList();
	virtual void SortColumn( int p_nSubItem, bool p_bSortingMultipleColumns = false );
	void			ResetSortState(){ m_lColumnSortStates = 0; }
	void			ResetAll(void);
	// Generated message map functions
protected:
	int			m_aCombinedSortedColumns[MAX_COLUMNS];
   EDataType	m_arrColType[MAX_COLUMNS];
	bool			m_bWithFilter;
	CString		m_csRegSect;
	CString		m_csRegEntry;
   UINT			m_lColumnSortStates;
	int			m_nLastColumn;


	static int CALLBACK Compare(LPARAM p_lParam1, LPARAM p_lParam2, LPARAM p_lParamThis);

	virtual int	CompareItem( LPARAM p_lParam1, LPARAM p_lParam2 );
	int			FindItemInCombinedSortedList( int p_nItem );
	int			GetNumCombinedSortedColumns();
	int			HitText( CPoint p_point );
	void			MoveItemInCombinedSortedListToEnd( int p_nItem );
	bool			NotInCombinedSortedColumnList( int p_nItem );
	int			SetSortImage( int nCol, BOOL bAsc );
	void		InitSortableHeaderCtrl(void);
	//{{AFX_MSG(CSortableHeaderCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SORTABLEHEADERCTRL_H__0A345281_0458_11D2_9AED_2F8C0624A000__INCLUDED_)
