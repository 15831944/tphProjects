#pragma once
// DlgVisibleRegions.h : header file
//
#include <common/BaseFloor.h>

//class CSmartEdit;
/////////////////////////////////////////////////////////////////////////////
// CDlgVisibleRegions dialog
class CDeck;
class CDlgVisibleRegions : public CDialog
{
// Construction
public:
	CDlgVisibleRegions(CBaseFloor* pFloor,BOOL bIsVR, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVisibleRegions() {} ;
// Dialog Data
	//{{AFX_DATA(CDlgVisibleRegions)
	enum { IDD = IDD_DIALOG_VISIBLEREGIONS };
	CStatic	m_staticToolbar;
	CTreeCtrl	m_treeVisRegions;
	BOOL	m_bEnableVR;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgVisibleRegions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolBar  m_toolBar;
	CBaseFloor* m_pFloor;
	HTREEITEM m_hRClickItem;
	HTREEITEM m_hEditItem;
	//CSmartEdit* m_pSmartEdit;
	BOOL m_bIsVR;
	std::vector<CVisibleRegion*> m_VisibleRegions;
	std::vector<CVisibleRegion*> m_InVisibleRegions;

	void InitToolBar();
	void LoadRegionData();
	void DeleteRegionTree();
	void ShowDialog(CWnd* parentWnd);
	void HideDialog(CWnd* parentWnd);
	void EditItem(HTREEITEM hItem);
	void UpdateToolbar();
	HTREEITEM InsertItem(CVisibleRegion* pVR);

	// Generated message map functions
	//{{AFX_MSG(CDlgVisibleRegions)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	virtual afx_msg void OnPickFromMap()=0;
	afx_msg void OnSelchangedVisRegions(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnCheckEnableVR();
	//}}AFX_MSG
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	afx_msg LONG OnEndEdit( WPARAM p_wParam, LPARAM p_lParam );
	void SaveData() ;
	DECLARE_MESSAGE_MAP()
};

class CTermPlanDoc;
class CFloor2;
//teminal
class CDlgFloorVisibleRegions : public CDlgVisibleRegions
{
public:
	CDlgFloorVisibleRegions(CBaseFloor* pFloor,BOOL bIsVR,CTermPlanDoc* pDoc, CWnd* pParent = NULL);
	afx_msg void OnPickFromMap() ;
	CTermPlanDoc* mpDoc;

	
};
//onboard 
class CDlgDeckVisibleRegions : public CDlgVisibleRegions
{
public:
	CDlgDeckVisibleRegions(CBaseFloor* pFloor,BOOL bIsVR, CTermPlanDoc* pDoc, CWnd* pParent = NULL) ;
	~CDlgDeckVisibleRegions() {};
protected:
	void OnOK() ;
	void SaveDataToDB() ;
	CTermPlanDoc* mpDoc;
	afx_msg void OnPickFromMap() ;
	DECLARE_MESSAGE_MAP()
};
