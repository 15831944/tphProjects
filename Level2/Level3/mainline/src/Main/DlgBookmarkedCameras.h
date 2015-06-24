#if !defined(AFX_DLGBOOKMARKEDCAMERAS_H__EC8E6EE0_E5AB_4E05_8657_B1F421E0B442__INCLUDED_)
#define AFX_DLGBOOKMARKEDCAMERAS_H__EC8E6EE0_E5AB_4E05_8657_B1F421E0B442__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBookmarkedCameras.h : header file
//

class CTermPlanDoc;
/////////////////////////////////////////////////////////////////////////////
// CDlgBookmarkedCameras dialog

class CDlgBookmarkedCameras : public CDialog
{
// Construction
public:
	CDlgBookmarkedCameras(CTermPlanDoc* pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBookmarkedCameras)
	enum { IDD = IDD_DIALOG_BOOKMARKEDCAMERAS };
	CStatic	m_staticToolbar;
	CListCtrl	m_lstBMCameras;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBookmarkedCameras)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTermPlanDoc* m_pDoc;
	CToolBar  m_toolBar;

	void LoadBMCameraData();
	void InitToolBar();
	void DeleteListItem(int _idx);

	// Generated message map functions
	//{{AFX_MSG(CDlgBookmarkedCameras)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetDispInfoListBMCameras(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEndLabelEditListBMCameras(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListBMCameras(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnToolbarbuttonDEL();
	afx_msg void OnToolbarbuttonEDIT();
	afx_msg void OnToolbarbuttonADD();
	afx_msg void OnBeginLabelEditListBMCameras(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBOOKMARKEDCAMERAS_H__EC8E6EE0_E5AB_4E05_8657_B1F421E0B442__INCLUDED_)
