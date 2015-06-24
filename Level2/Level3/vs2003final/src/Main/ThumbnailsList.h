#ifndef __THUMBNAILSLIST_H__
#define __THUMBNAILSLIST_H__
//

/////////////////////////////////////////////////////////////////////////////
// CThumbnailsList window
#include <vector>
#define UM_LISTCTRL_KILLFOCUS WM_USER+1


#define	LW_THUMBNAIL_WIDTH			30
#define	LW_THUMBNAIL_HEIGHT			30


class CThumbnailsList : public CListCtrl
{
// Construction
public:
	CThumbnailsList(BOOL bPreview=FALSE);
	virtual ~CThumbnailsList();

	void CreateThumnailImageList();

	void DrawThumbnails();
	BOOL GetImageFileNames(CString strSection);

	void SetImageDir(CString str);
	void SetLabelString(const char* szLabel[]);
	void ClearPrevSeledIndex() { m_nPrevSeledIndex = -1; }


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThumbnailsList)
	//}}AFX_VIRTUAL


	// Generated message map functions
protected:
	//{{AFX_MSG(CThumbnailsList)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Attributes
public:
	CWnd* m_pParent;
	CImageList m_ImageListThumb;
	int m_nSelectedItem;CString m_strImageDir;
	std::vector<CString>	m_VectorImageNames;		// vector holding the image names
	std::vector<CString> m_VectorItemDataLeft;
	std::vector<CString> m_VectorItemDataRight;
private:
	char**				 m_szShapeLabel;	
	BOOL m_bPreview;
	int m_nPrevSeledIndex;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __THUMBNAILSLIST_H__
