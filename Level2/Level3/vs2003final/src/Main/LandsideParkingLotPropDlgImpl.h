#pragma once
#include "LayoutObjectPropDlgImpl.h"
#include "TreeCtrlItemInDlg.h"
#include "Landside/LandsideQueryData.h"
#include "CommonData/QueryData.h"

class ParkingLotTreeItemData;
class ParkLotTreeItemDataInst;
class LandsideParkingLot;
class ParkingDrivePipe;
class CFloor2;





class ParkingLotTreeItem;
class LandsideParkingLotLevel;
class ParkLotTreeItemData;
class LandParkingLotPropDlgImpl : public ILayoutObjectPropDlgImpl
{
public:
	class ParkingLotTreeItemInfo
	{
	public:
		PLItemType type;
		int itemIndex;

		void SetNewItemInfo(PLItemType _type, int _itemIndex)
		{
			type = _type;
			itemIndex = _itemIndex;
		}
	};

	LandParkingLotPropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg,QueryData* pData );
	~LandParkingLotPropDlgImpl();

	virtual CString getTitle();
	virtual void LoadTree();	
	virtual int getBitmapResourceID() { return IDB_BITMAP_LANDSIDE_PARKINGLOT; }
	virtual void OnPropDelete();

	virtual FallbackReason GetFallBackReason();	
	virtual void GetFallBackReasonData(FallbackData& data);

	virtual void OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);	
	virtual void DoFallBackFinished( WPARAM wParam, LPARAM lParam );
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnSelChangedPropTree(HTREEITEM hTreeItem);
	virtual BOOL OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	bool OnOK( CString& errorMsg );

	LandsideParkingLot* getParkingLot();

	void SelectParkingLotItem(HTREEITEM hItem,int nLevel,emLandsideItemType parkLotItemType,int nIndex);

	virtual bool ChangeFocus()const;
	virtual bool NeedChangeVerticalProfile();

	virtual void HandleChangeVerticalProfile();
	ParkingDrivePipe* GetParkingDrivePipe();
	bool GetCurrentLevel(int& nCurrentLevel);

protected:
	ParkingLotTreeItemData* GetTreeItemData(HTREEITEM hitem);

	ParkLotTreeItemDataInst* mDataInst;

	ParkingLotTreeItemInfo m_newExpandItem; 

	QueryData	m_pData; //check whether parking space, parking entry...
	std::vector<HTREEITEM> m_vSelectedItem;

	bool m_bReloadTree;

	void OnLevelNameEdit(HTREEITEM hItem);
	void OnLevelHeightEdit(HTREEITEM hItem);
	void OnLevelThicknessEdit(HTREEITEM hItem);
	void OnSpotWidthEdit(HTREEITEM hItem);
	void OnSpotTypeCombo(HTREEITEM hItem);
	void OnSpotOpTypeCombo(HTREEITEM hItem);
	void OnSpotLengthEdit(HTREEITEM hItem);
	void OnSpotAngleEdit(HTREEITEM hItem);
	void OnSpotExitEntryPortEdit(HTREEITEM hItem);
	void AddLevelChildItems(ParkingLotTreeItem levelItem,const LandsideParkingLotLevel &parkingLevel,int nLevelIndex);
	void OnLevelAreaVisible(HTREEITEM hItem);
	void OnDrivePathLaneNum(HTREEITEM hItem);
	void OnDrivePathDir(HTREEITEM hItem);
	void OnDrivePathWidth(HTREEITEM hItem);

	CString GetFloorName(int idx)const;

};
