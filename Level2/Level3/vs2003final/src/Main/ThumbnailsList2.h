#pragma once



/////////////////////////////////////////////////////////////////////////////
// CThumbnailsList window
#include <vector>

#define UM_LISTCTRL_KILLFOCUS WM_USER+1
#define	THUMBNAIL_WIDTH		32
#define	THUMBNAIL_HEIGHT	32

class CThumbnailsList2 : public CListCtrl
{
// Construction
public:
	CThumbnailsList2();
	virtual ~CThumbnailsList2();

// Attributes
public:
	CWnd* m_pParent;

	// Operations
public:
	void DrawThumbnails(int _nSet,int nShapeRelationFlag = -1);
	void CreateThumnailImageList();
	int GetSelectedItem();
	int SetSelectedItem(int _n);
	int GetMouseMoveItem() const { return m_nMouseMoveItem; }
	void ClearPrevSeledIndex() { m_nPrevSeledIndex = -1; }
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThumbnailsList2)
	//}}AFX_VIRTUAL

// Implementation
private:
	int m_nSelectedItem;
	int m_nPrevSeledIndex;
	int m_nMouseMoveItem;    // the item that the mouse moving on
	CImageList m_ImageListThumb;

	// Generated message map functions
protected:
	//{{AFX_MSG(CThumbnailsList2)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

