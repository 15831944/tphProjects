#pragma once
#include "afxcmn.h"
#include "..\MFCExControl\CoolTree.h"
#include "..\MFCExControl\EditListBox.h"
#include "..\InputAirside\VehicleRoute.h"
#include "..\InputAirside\ALTObject.h"
#include "..\InputAirside\Intersections.h"
#include "..\MFCExControl\ARCTreeCtrl.h"
#include "../MFCExControl/XTResizeDialog.h"

// CDlgVehicleRoute dialog
class CDlgVehicleRoute : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgVehicleRoute)

public:
	CDlgVehicleRoute(int nProjID, CWnd* pParent = NULL);
	virtual ~CDlgVehicleRoute();

// Dialog Data
	enum { IDD = IDD_DIALOG_VEHICLEROUTES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
private:
	CEditListBox  m_wndListBoxRouteList;   
	CARCTreeCtrl   m_wndTreeRoute;	
	CToolBar	m_wndToolbarRouteList;
	CToolBar	m_wndToolbarRoute;

	int m_nProjID;
	CVehicleRouteList m_vehicleRouteList;
	std::vector<Intersections> m_vIntersections;
	std::vector<ALTObject> m_vStretch;		

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnNewRoute();
	afx_msg void OnDelRoute();
	afx_msg void OnNewStretchNode();
	afx_msg void OnDelStretchNode();
private:
	void InitToolBar();
	void UpdateToolBar();
	void SetListBoxRouteList();
	void LoadTree();
	void GetAllStretchAndIntersections();
	void TraverseOne(HTREEITEM hDataTreeItem,int nParentID,HTREEITEM& hTreeItem);
	void TraverseAll(HTREEITEM hDataTreeItem,int nParentID,HTREEITEM& hTreeItem);
public:
	afx_msg LRESULT OnListboxRouteListKillFocus( WPARAM wParam, LPARAM lParam );
	afx_msg void OnLbnSelchangeListboxRoutelist();
	afx_msg void OnTvnSelchangedTreeRoute(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
