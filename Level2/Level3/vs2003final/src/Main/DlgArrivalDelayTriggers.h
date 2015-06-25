#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "..\inputAirside\ArrivalDelayTriggers.h"
#include "..\MFCExControl\ListCtrlEx.h"
#include "..\MFCExControl\SimpleToolTipListBox.h"
#include "../MFCExControl/ARCTreeCtrlExWithColor.h"
// CDlgArrivalDelayTriggers dialog

class CDlgArrivalDelayTriggers : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgArrivalDelayTriggers)

public:
	CDlgArrivalDelayTriggers(int nProjID, Terminal* pTerminal, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgArrivalDelayTriggers();

// Dialog Data
	enum { IDD = IDD_DIALOG_AIRSIDE_ARRIVALDELAYTRIGGERS };

protected:
	enum ItemType
	{
		ItemType_FlightType = 0,
		ItemType_TimeRoot,
		ItemType_TimeItem,
		ItemType_RunwayRoot,
		ItemType_RunwayItem,
		ItemType_TriggerRoot,
		ItemType_EnrouteRoot,
		ItemType_EnrouteType,
		ItemType_RunwayExit,
		ItemType_QueueLength,
		ItemType_MinsPerAircraft,
	};

	class CTreeItemData
	{
	public:
		CTreeItemData(ItemType itemtype, AirsideArrivalDelayTrigger::CFlightTypeItem * pItem)
		{
			m_enumItemType = itemtype;
			m_FlightTypeItem = pItem;
			m_timeRange = NULL;
			m_runwayMark = NULL;
			m_pTriggerCondition = NULL;
		}
	public:
		//item type
		ItemType m_enumItemType;
		AirsideArrivalDelayTrigger::CFlightTypeItem *m_FlightTypeItem;
		AirsideArrivalDelayTrigger::CTimeRange* m_timeRange;
		AirsideArrivalDelayTrigger::CRunwayMark* m_runwayMark;
		AirsideArrivalDelayTrigger::CTriggerCondition *m_pTriggerCondition;

	};
	
protected:
	int m_nProjID;	
	Terminal *m_pTerminal;
	std::vector<ALTObject > m_vRunways;

	CAirsideArrivalDelayTrigger m_arrivalDelayTrigger;
	CToolBar m_wndToolBar;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	
	void InitToolbar();
	void GetRunway();
	void LoadArriDelayTriDatabase();

protected:
	void LoadFlightTypeItem(AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem);
	
	void LoadDateTime(AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem, HTREEITEM hRootItem);

	void LoadDateTimeItem(AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem, AirsideArrivalDelayTrigger::CTimeRange* timeRange, HTREEITEM hRootItem);

	void LoadLandingRunway(AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem, HTREEITEM hRootItem);

	void LoadLandingRunwayItem(AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem,AirsideArrivalDelayTrigger::CRunwayMark *pRunway, HTREEITEM hRootItem);

	void LoadTriggerCondition(AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem, HTREEITEM hRootItem);

	void LoadEnRouteCondition(AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem, HTREEITEM hRootItem);

	void LoadTriggerConditionItem(AirsideArrivalDelayTrigger::CFlightTypeItem *pFlightItem, AirsideArrivalDelayTrigger::CTriggerCondition* pCondition, HTREEITEM hRootItem);

	void UpdateToolbarState();

	void UpdateItemText(HTREEITEM hItemUpdate);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEditOperation();
	afx_msg void OnNewOperation();
	afx_msg void OnDelOperation();
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTvnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnTreeDoubleClick(WPARAM, LPARAM);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	CARCTreeCtrlExWithColor m_treeCtrl;

	
};
