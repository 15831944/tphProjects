#if !defined(AFX_CBUDDYTREE_H__6511A6EE_992D_487E_9D29_51BD0ECE39E0__INCLUDED_)
#define AFX_CBUDDYTREE_H__6511A6EE_992D_487E_9D29_51BD0ECE39E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 3DObjModelTree.h : header file
//
#include <Renderer/RenderEngine/3DNodeObject.h>

/////////////////////////////////////////////////////////////////////////////
// C3DObjModelEditTree window

class C3DNodeObject;

class C3DObjModelEditTree : public CTreeCtrl
{
// Construction
public:
	C3DObjModelEditTree();
	virtual		~C3DObjModelEditTree();

// Attributes
public:
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3DObjModelEditTree)
	//}}AFX_VIRTUAL


// Implementation
public:
	HTREEITEM	MoveChildItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter);

	BOOL SetBkImage(UINT nIDResource) ;
	BOOL SetBkImage(LPCTSTR lpszResourceName) ;

	void ReloadData(C3DNodeObject nodeObj);
	bool DeleteDataItem(C3DNodeObject nodeObj);
	void LoadData( C3DNodeObject nodeObj, HTREEITEM hParent );
	HTREEITEM RecursiveFind(C3DNodeObject nodeObj, HTREEITEM hParent = NULL);
	void AddNewItemToRoot( C3DNodeObject nodeObj );
	void SelectNodeObj( C3DNodeObject nodeObj );

	void DoAdd();
	void AddToSel( HTREEITEM hSelItem );
	void DoDel();
	void DoEdit();

	enum
	{
		UPDATE_SELECTED_ITEM_TYPE = (WM_USER + 10012),
		SELECT_NODE_ITEM,
		DELETE_NODE_ITEM,
		QUERY_IS_LOCKED,
	};
	enum SelectedItemType
	{
		InvalidItem = 0,
		RootItem,
		GroupItem,
		ComponentItem,
	};
	void UpdateSelcetedItemType();
	BOOL IsLocked();
protected:
	void SetDefaultCursor() ;
	BOOL IsDragAllowed(HTREEITEM hDragItem, HTREEITEM hDropItem);
	void CheckChildItems(HTREEITEM hParentItem, BOOL bVisible);
	// call this method when a new component is drop into the 3d view
	void UpdateNewComponent(const C3DNodeObject& nodeNew);

protected:
	//{{AFX_MSG(cBuddyTree)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	//}}AFX_MSG

	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnAddGroup();
	afx_msg void OnEditGroup();
	afx_msg void OnDelGroup();

	afx_msg void DelSel( HTREEITEM hSelItem );
	afx_msg void OnEditComp();

	afx_msg void EditSel( HTREEITEM hSelItem );
	afx_msg void OnDelComp();

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()

private:
	CPalette m_pal;
	CBitmap m_bitmap;
	int m_cxBitmap, m_cyBitmap ;

	COLORREF memDC_bgColor_bitmap;
	bool	 isImageTiled;

	//*********** CURSORS  *********************
	HCURSOR cursor_hand ;
	HCURSOR cursor_arr	;
	HCURSOR cursor_no	;
	bool	isCursorArrow;
	//******************************************


	//*************  DRAG & DROP ******************
	CImageList*	m_pDragImage;
	BOOL		m_bLDragging;
	HTREEITEM	m_hitemDrag;

	//*********************************************

	C3DNodeObject m_nodeObj;
	HTREEITEM m_hTrackPopupMenuItem;

	enum PopupMenuCmdID
	{
		ID_ADD_GROUP = 100001,
		ID_EDIT_GROUP,
		ID_DEL_GROUP,
// 		ID_ADD_COMPONENT,
		ID_EDIT_COMPONENT_NAME,
		ID_DEL_COMPONENT,
	};

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CBUDDYTREE_H__6511A6EE_992D_487E_9D29_51BD0ECE39E0__INCLUDED_)
