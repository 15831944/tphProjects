#pragma once
#include "../MFCExControl/CoolTree.h"
#include "../InputAirside//FlightPlan.h"
#include "..\MFCExControl\SimpleToolTipListBox.h"
#include "AirsideObjectTreeCtrl.h"
#include "../MFCExControl/XTResizeDialog.h"

#include "TreeCtrlEx.h"

class ARWaypoint;
class CAirportDatabase;
class CDlgFlightPlan : public CXTResizeDialog
{
public:
	CDlgFlightPlan(int nProjID,CAirportDatabase* pAirportDB, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFlightPlan();

	class TreeNodeData
	{
	public:
		enum NodeType
		{
			State_Phase,
			Distance_Runway,
			Upper_Altitude,
			Lower_Altitude,
			Altitude_Sep,
			Lower_Speed,
			Upper_Speed
		};
		TreeNodeData()
		{

		};

		NodeType emType;
		DWORD wordData;
	};
// Dialog Data
	enum { IDD = IDD_DIALOG_AIRSIDE_FLIGHTPLAN };

protected:
	DECLARE_MESSAGE_MAP()
	CSimpleToolTipListBox m_lstFltType;
	CComboBox m_comboOperationType;
	CAirsideObjectTreeCtrl m_treeFltPlan;
	CToolBar m_wndFlightPlanToolBar;
	CToolBar m_wndAirRouteToolBar;
	HTREEITEM m_hRClickItem;
	ns_FlightPlan::FlightPlanBase* m_pCurFltPlan;
	int m_nProjID;
	ns_FlightPlan::FlightPlans m_flightPlans;

	AirRouteList m_airRouteList;

	CAirportDatabase * m_pAirportDB;

	std::vector<TreeNodeData*>m_vNodeList;

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();

	void SetOperationTypeContents();
	void LoadFlightPlans();
	HTREEITEM AddWayPointToTree(ns_FlightPlan::WayPointProperty* wayPointProp, HTREEITEM hInsertAfter);
	void AddRunwayMarkToTree(int nRunwayID, int nMarkIdx,HTREEITEM hInsertAfter);
	void AddGenericDataToTree(ns_FlightPlan::FlightPlanGeneric* pGenericProfile,ns_FlightPlan::FlightPlanGenericPhase* pLeafData);
	void DeleteWayPointFromTree(HTREEITEM hFixItem);
	void UpdateCurrentPlanContent();
	void InitializeAirRouteList();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewFlightPlan();
	afx_msg void OnDelFlightPlan();
	afx_msg void OnEditPlan();
	afx_msg void OnNewAirRoute();
	afx_msg void OnDelAirRoute();
	afx_msg void OnLbnSelChangeListFltType();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnFltplanModifyaltitude();
	afx_msg void OnFltplanModifyspeed();
	afx_msg void OnFltplanModifyPhase();
	afx_msg void OnFltModifyValue();
	afx_msg void OnSave();
	afx_msg void OnOperationTypeChanged();
	afx_msg LRESULT OnEndEditTreeValue(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEndEditComboBoxTreeValue(WPARAM wParam, LPARAM lParam);
};
