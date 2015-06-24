#pragma once
#include "Landside/LandsideCurbsideStrategy.h"
#include "MFCExControl/CoolTree.h"
#include "DlgLandsideStrategy.h"

class LandsideLayoutObjectList;
class InputLandside;
// CDlgCurbsideStrategy dialog

class CDlgCurbsideStrategy : public CDlgLandsideStrategy
{
	DECLARE_DYNAMIC(CDlgCurbsideStrategy)
public:
	CDlgCurbsideStrategy(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCurbsideStrategy();


protected:
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
	HTREEITEM InsertVehicleItem(HTREEITEM hItem, LandsideCurbsideVehicleStrategy* pVehicle);
	HTREEITEM InsertCurbsideItem(HTREEITEM hItem, LandsideCurbsideSettingData* pCurbside);
	HTREEITEM InsertPriorityItem(HTREEITEM hItem, int idx,LandsideCurbsideVehicleSinglePriority* pSinglePriority);
	HTREEITEM InsertPrioritySpecifiedItem(HTREEITEM hItem, int idx,LandsideCurbsideVehicleSinglePriority* pSinglePriority);
	HTREEITEM InsertFirstPriorityItem(HTREEITEM hItem, LandsideCurbsideVehicleSinglePriority* pSinglePriority);
private:
	LandsideCurbsideStrategy m_curbsideStrategy;
};
