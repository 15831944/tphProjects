#ifndef __CThumbnailsListEx_H__
#define __CThumbnailsListEx_H__
#include <vector>
// CThumbnailsListEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CThumbnailsList window
#define UM_LISTCTRL_KILLFOCUS WM_USER+1
#define	THUMBNAIL_WIDTH		32
#define	THUMBNAIL_HEIGHT	32

class CThumbnailsListEx :
	public CListCtrl
{

	// Construction
public:
	CThumbnailsListEx();
	virtual ~CThumbnailsListEx();

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
	//{{AFX_VIRTUAL(CThumbnailsListEx)
	//}}AFX_VIRTUAL

	// Implementation
private:
	int m_nSelectedItem;
	int m_nPrevSeledIndex;
	int m_nMouseMoveItem;    // the item that the mouse moving on
	CImageList m_ImageListThumb;

	// Generated message map functions
protected:
	//{{AFX_MSG(CThumbnailsListEx)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __CThumbnailsListEx_H__