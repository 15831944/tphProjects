#pragma once
#include "DlgLandsideStrategy.h"
#include "Landside/LandsideParkingLotStrategy.h"

class CDlgParkingLotStrategy : public CDlgLandsideStrategy
{
	DECLARE_DYNAMIC(CDlgParkingLotStrategy)
public:
	CDlgParkingLotStrategy(CWnd* pParent = NULL);
	~CDlgParkingLotStrategy(void);

	virtual void SetTitleHeard();
	virtual void SetTreeContent();
	virtual void ReadData();
	virtual void SaveData();

	virtual void OnAddTreeItem();
	virtual void OnDeleteTreeItem();
	virtual void OnDisplayMenu(TreeNodeData* pNodeData,CPoint point);
	virtual void UpdateToolbar(HTREEITEM hItem);
	virtual void UpdateDeleteOperationToolbar();
	virtual void HandleTreeMessage(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void HandLDoubleClickMessage(WPARAM wParam, LPARAM lParam);

private:
	HTREEITEM InsertVehicleItem(HTREEITEM hItem, LandsideParkingLotVehicleStrategy* pVehicle);
	HTREEITEM InsertParkingLotItem(HTREEITEM hItem, LandsideParkingLotSettingData* pParkingLot);
	HTREEITEM InsertPriorityItem(HTREEITEM hItem, int idx,LandsideParkingLotVehicleSinglePriority* pSinglePriority);
	HTREEITEM InsertPrioritySpecifiedItem(HTREEITEM hItem, int idx,LandsideParkingLotVehicleSinglePriority* pSinglePriority);
	HTREEITEM InsertFirstPriorityItem(HTREEITEM hItem, LandsideParkingLotVehicleSinglePriority* pSinglePriority);

private:
	LandsideParkingLotStrategy m_parkingLotContainer;
};
