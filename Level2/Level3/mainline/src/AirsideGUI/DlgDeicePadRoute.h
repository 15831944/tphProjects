#pragma once
#include "afxcmn.h"

// CDlgDeicePadRoute dialog
extern enum RouteType;
class CPreferRouteList;
class CPreferRouteItem;
class CDeicePadItem;
class CRoutePath;
class CDlgDeicePadRoute : public CDialog
{
	DECLARE_DYNAMIC(CDlgDeicePadRoute)

public:
	CDlgDeicePadRoute(int nProjID,int nAirportID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDeicePadRoute();

// Dialog Data
	enum { IDD = IDD_DIALOG_DEICEROUTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	void OnInitToolbar();
	void OnInitTreeCtrl();
	void ModifySize(RouteType emType);
	void ShortSize();
	void PreferSize();
	void SetToolbarStatus();
	void SetComboBoxItem(CComboBox* pCB);
	int GetTreeCount(HTREEITEM hItem);
	HTREEITEM GetParentItem(HTREEITEM hItem,int lCount);
	int GetIndexByTree(HTREEITEM hItem);
	void SetTreeItemText(HTREEITEM hItem,CString strItemText);
	
public:
	int m_nProjID;
	int m_nAirportID;
	CToolBar m_wndStadToolbar;
	CToolBar m_wndRouteToolbar;
	RouteType m_emType;
	CCoolTree m_wndTreeCtrl;
	BOOL m_bDClick;

protected:
	CPreferRouteList* m_pRouteList;
	CPreferRouteItem* m_pCurRouteItem;
	CDeicePadItem* m_pCurDeiceItem;
	CRoutePath*	   m_pCurRoutePath;
public:
	afx_msg void OnTvnSelchangedTreeItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSave();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedRadioPath();
	afx_msg void OnBnClickedRadioRoute();
	afx_msg void OnNewStandAndRoutepathItem();
	afx_msg void OnDelStandAndRoutepathItem();
	afx_msg LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
