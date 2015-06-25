#pragma once
#include "NodeViewDbClickHandler.h"
#include "VehiclePoolsAndDeploymentCoolTree.h"
#include "..\MFCExControl\EditListBox.h"
#include "..\InputAirside\VehiclePoolsAndDeployment.h"
#include "../MFCExControl/XTResizeDialog.h"

// CDlgVehiclePoolsAndDeployment dialog
class InputAirside;
class CDlgVehiclePoolsAndDeployment : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgVehiclePoolsAndDeployment)

public:
	CDlgVehiclePoolsAndDeployment(int nProjID, PSelectFlightType pSelectFlightType, InputAirside* pInputAirside, PSelectProbDistEntry pSelectProbDistEntry, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVehiclePoolsAndDeployment();

// Dialog Data
	enum { IDD = IDD_DIALOG_VEHICLEPOOLSANDDEPLOYMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CListBox m_wndListBoxVehicleType;
	CEditListBox  m_wndListBoxPools;    
	CVehiclePoolsAndDeploymentCoolTree m_wndTreeAttributes;	
	CToolBar	m_wndToolbarPools;
	CToolBar	m_wndToolbarAttributes;
	CVehiclePoolsAndDeployment m_vehiclePoolsAndDeployment;
	int m_nProjID;
	PSelectFlightType	m_pSelectFlightType;
	InputAirside* m_pInputAirside;
	PSelectProbDistEntry    m_pSelectProbDistEntry;
	HTREEITEM m_hTreeItemLocation;
	HTREEITEM m_hTreeItemQuantity;
	HTREEITEM m_hTreeItemMintrunaroundtime;
	HTREEITEM m_hTreeItemDeployment;
	std::vector<ALTObject> m_vParkingLot;
	int m_nDefaultStandFamilyID;

	enumVehicleBaseType m_eSelVehicleType;
protected:
	virtual void OnOK();
	afx_msg void OnNewPool();
	afx_msg void OnDelPool();
	afx_msg void OnNewAttribute();
	afx_msg void OnDelAttribute();
	afx_msg void OnEditAttribute();
	void LoadTree();
	void LoadFollowMeCarTree(CVehiclePool* pItem);
	void LoadTowTruckTree(CVehiclePool* pVehiclePoolItem);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButtonSave();
	virtual BOOL OnInitDialog();
private:
	void InitToolBar();
	void UpdateToolBar();
	void SetListBoxVehicleType();
	CVehicleTypePools* GetCurSelVehicleTypePools();
	CVehiclePool* GetCurSelVehiclePool();
	bool CheckInputData();
public:
	afx_msg void OnLbnSelchangeListboxVehicletype();
	afx_msg void OnLbnSelchangeListboxPools();
	afx_msg void OnTvnSelchangedTreeAttributes(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnListboxPoolsKillFocus( WPARAM wParam, LPARAM lParam );
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
