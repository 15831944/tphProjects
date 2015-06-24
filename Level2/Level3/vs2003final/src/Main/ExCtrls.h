#ifndef _EXCTRLS_H_CE016911_F4B8_11D5_B18D_00D0B7555BD3_
#define _EXCTRLS_H_CE016911_F4B8_11D5_B18D_00D0B7555BD3_

//#ifdef _EXCTRLS_DLL_
//	#define EXCTRLSDDLEXP _declspec(dllexport)
//#else
//	#define EXCTRLSDDLEXP _declspec(dllimport)
//#endif

#include <afxtempl.h>

class  CTreeCtrlEx : public CTreeCtrl
{
	typedef CMap< HTREEITEM, HTREEITEM, CWnd*, CWnd* > mapItemToCB;

	//
	// Class for attached combo boxes
	// Internal use only
	//
	class CTreeComboBox : public CComboBox
	{
	private:
		CString			strCurrText;
	public:
		CTreeComboBox(int _nID)
		{
			nID=_nID;
		}
		int nID;
		//{{AFX_MSG(CTreeComboBox)
		afx_msg void	OnCloseUp();
		afx_msg void	OnKillFocus();
		afx_msg void	OnEditUpdate();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};

public:
	CTreeCtrlEx();
	virtual ~CTreeCtrlEx();

// Public Interface
public:
	void SetItemText2(HTREEITEM hItem, LPCTSTR lpszItem);
	void SetItemText1(HTREEITEM hItem,LPCTSTR lpszItem);

	//
	// Set Dropped height for item's combo box
	//
	// Arguments:	iNewHeight - number of pixels of dropped combo box height
	//
	void			SetComboDroppedHeight( int iNewHeight );

	//
	// Set minimum and maximux width for combo box
	//
	// Arguments:	iNewMinWidth - minimum preferred number of pixels for combo box width
	//								(width of combo box is always restricted by control's rectangle)
	//				iNewMaxWidth - maximum number of pixels for combo box width
	//
	void			SetComboMinMaxWidths( UINT iNewMinWidth = 150, UINT iNewMaxWidth = UINT_MAX );

	//
	// Insert item into control with attached dropdown list (combo box)
	//
	// Arguments:	lpszItem, hParent, hInsertAfter - as in InsertItem(...) function in CTreeCtrl
	//				bEditableCombo - determines the type of combo box. TRUE - editable, FALSE - drop down list
	//
	// Return value: Same as in InsertItem(...) function in CTreeCtrl
	//
	HTREEITEM		InsertItemWithComboBox(LPCTSTR lpszItem, BOOL bEditableCombo, HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);

	//
	// Remove specified item's combo box
	//
	// Arguments:	htiRemoveFrom - tree item, from which combo box need to be removed
	//
	void			RemoveComboBox( HTREEITEM htiRemoveFrom );
	//
	// Remove all combo boxes from control
	//
	// Arguments:	none
	//
	void			RemoveAllComboBoxes();
	
	//
	// Insert a string into item's combo box
	//
	// Arguments:	htiInsertTo - descriptor of tree item in which dropped list will be inserted a new entry
	//				lpszItem, iIndex - as in InsertString(...) function of CComboBox.
	//
	// Return value: Same as in InsertString(...) function of CComboBox
	//
	int				InsertComboBoxString( HTREEITEM htiInsertTo, LPCTSTR lpszItem, int iIndex = -1 );

	//
	// Next two functions overriden from parent class to prevent deleting tree item 
	// without deleting attached combo box
	//
	// Arguments:	same as in same function in parent class
	//
	//
	// Return value: Same as in parent class.
	//
	BOOL			DeleteItem( HTREEITEM hItem );
	BOOL			DeleteAllItems();

private:
	UINT			m_iComboDroppedHeight;	// Height of dropped list of item's combo box
	UINT			m_iComboMinWidth;		// Minimum width of item's combo box
	UINT			m_iComboMaxWidth;		// Maximum width of item's combo box
	mapItemToCB		m_mapExControls;		// Table for attached combo boxes 
	mapItemToCB		m_mapExControls2;
private:
	//
	// For internal use only
	// Show attached combo box if there is one.
	//
	// Arguments:	hti - descriptor of tree item in which dropped list will be shown
	//
	// Return value: true if success, false otherwise
	//
	bool			ShowComboBox( HTREEITEM hti );

	//{{AFX_VIRTUAL(CTreeCtrlEx)
protected:
	virtual void	PreSubclassWindow();
	//}}AFX_VIRTUAL

private:
	//{{AFX_MSG(CTreeCtrlEx)
	afx_msg void	OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

////
////	All members and functions are the same as in CTreeCtrlEx, except item decriptor type.
////	In CTreeCtrlEx - HTREEITEM, In CListCtrlEx - int
////	And also there are no function for item insertion with specifed index.
////	All items will be inserted at end of list.
////
//class  CListCtrlEx : public CListCtrl
//{
//	typedef CMap< int, int, CWnd*, CWnd* > mapItemToCB;
//
//	class CListComboBox : public CComboBox
//	{
//	private:
//		CString			strCurrText;
//
//		//{{AFX_MSG(CListComboBox)
//		afx_msg void	OnCloseUp();
//		afx_msg void	OnKillFocus();
//		afx_msg void	OnEditUpdate();
//		//}}AFX_MSG
//		DECLARE_MESSAGE_MAP()
//	};
//
//public:
//	CListCtrlEx();
//	virtual ~CListCtrlEx();
//
//public:
//	void			SetComboDroppedHeight( int iNewHeight );
//	void			SetComboMinMaxWidths( UINT iNewMinWidth = 150, UINT iNewMaxWidth = UINT_MAX );
//	int				InsertItemWithComboBox( LPCTSTR lpszItem, BOOL bEditableCombo );
//	void			RemoveComboBox( int nItemRemoveFrom );
//	void			RemoveAllComboBoxes();
//	int				InsertComboBoxString( int nItemInsertTo, LPCTSTR lpszItem, int iIndex = -1 );
//	BOOL			DeleteItem( int nItem );
//	BOOL			DeleteAllItems();
//
//private:
//	UINT			m_iComboDroppedHeight;
//	UINT			m_iComboMinWidth;
//	UINT			m_iComboMaxWidth;
//	mapItemToCB		m_mapExControls;
//
//private:
//	bool ShowComboBox( int nItem );
//
//	//{{AFX_VIRTUAL(CListCtrlEx)
//	protected:
//	virtual void	PreSubclassWindow();
//	//}}AFX_VIRTUAL
//
//private:
//	//{{AFX_MSG(CListCtrlEx)
//	afx_msg void	OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void	OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
//	//}}AFX_MSG
//	DECLARE_MESSAGE_MAP()
//};

#endif // _EXCTRLS_H_CE016911_F4B8_11D5_B18D_00D0B7555BD3_
