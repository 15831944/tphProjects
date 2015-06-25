#pragma once
#include "afxcmn.h"
#include "MFCExControl/EditListBox.h"
#include "..\InputAirside\TowingRoute.h"
#include "..\InputAirside\Taxiway.h"
#include "..\MFCExControl\ARCTreeCtrl.h"
#include "../MFCExControl/XTResizeDialog.h"


// CDlgTowingRoute dialog
class CAirportGroundNetwork;

class CDlgTowingRoute : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgTowingRoute)

public:
	CDlgTowingRoute(int nProjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTowingRoute();

// Dialog Data
	enum { IDD = IDD_DIALOG_TOWINGROUTE };
	typedef struct
	{
		int nOterObjID;
		int nIndex;
	}combdata;

	typedef struct
	{
		Taxiway* pTaxiway;
		int nIntersecIndex;
	}RouteItemData;
	std::vector< RouteItemData* > m_vectRouteNode;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	//CListCtrl m_wndListCtrl;
//	CCoolTree m_wndTree;
	CEditListBox  m_wndListBox;   
	CARCTreeCtrl m_wndTree;
	CToolBar	m_wndRoutebar;
	CToolBar	m_wndTaxiwayNodeToolbar;

	int m_nProjID;
	CTowingRouteList m_towingRouteList;
	std::vector<Taxiway> m_vTaxiway;
	int m_nDefaultStandFamilyID;
	std::vector<combdata> m_vCombdata;
	CTowingRoute* m_pCurRoute;

	//
	Taxiway* GetTaxiwayByID(int nID);

	CAirportGroundNetwork* m_pAltNetwork;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	afx_msg void OnBnClickedButtonSave();

	afx_msg void OnNewRoute();
	afx_msg void OnDelRoute();
	afx_msg void OnEditRoute();

	afx_msg void OnNewTaxiwayNode();
	afx_msg void OnDelTaxiwayNode();
private:
	void InitToolBar();
	void InitListBox();
	void UpdateToolBar();
	void GetAllTaxiway();
	void LoadTree();
	void TraverseOne(HTREEITEM hDataTreeItem,int nParentID,HTREEITEM& hTreeItem);
	void TraverseAll(HTREEITEM hDataTreeItem,int nParentID,HTREEITEM& hTreeItem);
public:
	afx_msg void OnLvnItemchangedListTowingroute();
	afx_msg void OnTvnSelchangedTreeTowingroute(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg LRESULT OnMessageModifyRouteName( WPARAM wParam, LPARAM lParam );
};
