#pragma once
#include "../MFCExControl/CoolTree.h"

class InputLandside;
class NonPaxVehicleAssignment;
class NonPaxVehicleStrategy;
class ParkingLotRoute;
class VehicleTimeEntry; 
class ALTObjectID;

// CDlgNonPassengerRelatedVehicleAssignment Dialog
class CDlgNonPassengerRelatedVehicleAssignment : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgNonPassengerRelatedVehicleAssignment)

	class TreeNodeData
	{
	public:
		enum TreeNodeType
		{
			Node_Root,
			Node_Strategy,
			Node_Destination,
			Node_Lot,
			Node_LotArrivalTime,
			Node_Daily,
			Node_TimeRange,
			Node_Vehicle,
			Node_VehicleName,
			Node_VehicleNumber,
			Node_ETA,			
			Node_Entry,
			Node_EntryName,
			Node_ProDist
		};

		TreeNodeData()
		{

		}
		~TreeNodeData()
		{

		}

		DWORD m_dwData;
		TreeNodeType m_emType;
	};
public:
	CDlgNonPassengerRelatedVehicleAssignment(InputLandside* pInLandside, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNonPassengerRelatedVehicleAssignment();

	// Dialog Data
	enum { IDD = IDD_DIALOG_LANDSIDEVEHICLEASSIGNMENT_NONPAX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	void OnInitToolbar();
	void UpdateToolBarState();
	void LoadTreeContent();

	void LoadSetStrategy(HTREEITEM hItem, NonPaxVehicleStrategy* pNonPaxVehicleStrategy);
	void LoadParkingLotRoute(HTREEITEM hItem, ParkingLotRoute* pParkingLotRoute);
	void ChangeTheTime(NonPaxVehicleStrategy* pNonPaxVehicleStrategy);
	void LoadVehicleTimeEntry(HTREEITEM hItem,  NonPaxVehicleStrategy* pNonPaxVehicleStrategy);
	void LoadVehicle(HTREEITEM hItem, VehicleTimeEntry* pTimeEntry);
	void LoadEntryOffSet(HTREEITEM hItem, NonPaxVehicleStrategy* pNonPaxVehicleStrategy);
	void LoadArrivalAndETAOffset(HTREEITEM hItem, NonPaxVehicleStrategy* pNonPaxVehicleStrategy);
	void DeleteTreeNodeData();
	bool IsTheLotItemExit(HTREEITEM hItem, ALTObjectID& objectID);
protected:
	virtual void OnOK();
	virtual void OnCancel();


	void SetModified(BOOL bModified = TRUE);
	afx_msg void OnAddObject();
	afx_msg void OnDeleteObject();
	afx_msg void OnEditObject();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnBtnSave();

protected:
	CCoolTree	   m_wndTreeCtrl;
	CToolBar       m_wndToolBar;

	InputLandside*	m_pInLandside;
	NonPaxVehicleAssignment* m_pNonPaxVehicleAssignment;
	std::vector<TreeNodeData*> m_vNodeData;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

};
