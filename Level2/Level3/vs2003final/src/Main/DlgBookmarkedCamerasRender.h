#pragma once
// DlgBookmarkedCameras.h : header file
//

#include <Common/3DViewCameras.h>

/////////////////////////////////////////////////////////////////////////////
// CDlgBookmarkedCamerasRender dialog

class CDlgBookmarkedCamerasRender : public CDialog
{
// Construction
public:
	CDlgBookmarkedCamerasRender(const C3DViewCameras& cams, bool bAllowAdd, CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_DIALOG_BOOKMARKEDCAMERAS };
	C3DViewCameras m_cams;

// Implementation
protected:
	CStatic	m_staticToolbar;
	CListCtrl	m_lstBMCameras;
	CToolBar  m_toolBar;

	bool m_bAllowAdd;
	bool m_bLabelEditing;

	void LoadBMCameraData();
	void InitToolBar();
	void DeleteListItem(int _idx);
	void UpdateToolbar();

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolbarbuttonDEL();
	afx_msg void OnToolbarbuttonEDIT();
	afx_msg void OnToolbarbuttonADD();
	afx_msg void OnBeginLabelEditListBMCameras(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEditListBMCameras(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetDispInfoListBMCameras(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedListBmcameras(NMHDR *pNMHDR, LRESULT *pResult);
};
