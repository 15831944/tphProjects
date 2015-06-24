#pragma once

#include "MFCExControl/CoolTree.h"
#include "Landside/LandsideParkingSpotConstraints.h"

// CDlgParkingSpotSpec dialog
class LandsideFacilityLayoutObjectList;
class ALTObjectID;
class CDlgParkingSpotSpec : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgParkingSpotSpec)
	class TreeNodeData
	{
	public:
		enum TreeNodeType
		{
			SpotGroup_Node,
			TimeRange_Node,
			VehicleType_Node,
			ParkingLot_Node,
			ParkingLotLevel_Node,
			ParkingLotRow_Node,
			StartPos_Node,
			EndPos_Node
		};

		TreeNodeData()
			:m_emType(SpotGroup_Node)
		{

		}
		~TreeNodeData()
		{

		}

		DWORD m_dwData;
		TreeNodeType m_emType;
	};

public:
	CDlgParkingSpotSpec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgParkingSpotSpec();

// Dialog Data
	enum { IDD = IDD_DIALOG_PARKINGSPOTSPEC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnCmdNewItem();
	afx_msg void OnCmdDeleteItem();
	afx_msg void OnAddParkingSpot();
	afx_msg void OnAddVehicleType();
	afx_msg void OnDeleteParkingSpot();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnBtnSave();
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void OnInitToolbar();
	void UpdateToolBarState();
	void LoadTreeContent();

	//*******************************************************************
	//**Tree item operation
	void InsertSpotGroupItem(HTREEITEM hItem, ParkingSpotGroup* pParkingSpotGroup);
	void InsertSpotTimeRangeItem(HTREEITEM hItem, ParkingSpotTimeRange* pParkingSpotTimeRange);
	void InsertSpotVehicleTypeItem(HTREEITEM hItem, ParkingSpotVehicleType* pParkingSpotVehicleType);
	void InsertSpotPropertyItem(HTREEITEM hItem, ParkingSpotProperty* pParkingSpotProperty);
	void InsertSpotLevelItem(HTREEITEM hItem, const ALTObjectID& altID,ParkingSpotLevel* pParkingSpotLevel);
	void InsertSpotRowItem(HTREEITEM hItem, ParkingSpotRow* pParkingSpotRow);

private:
	void DeleteTreeNodeData(TreeNodeData* pNodeData);
	void ClearTreeNodeData();
	LandsideFacilityLayoutObjectList* GetLandsideLayoutObjectList();
private:
	CToolBar  m_wndToolbar;
	CCoolTree m_wndTreeCtrl;

	LandsideParkingSpotConstraints m_landsideParkingSpotCon;

	std::vector<TreeNodeData*> m_vNodeData;
};
