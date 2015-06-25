#pragma once
#include "DlgRunwayAssignment.h"
#include "..\MFCExControl\ARCTreeCtrl.h"
#include "..\MFCExControl\SplitterControl.h"
#include "..\InputAirside\TakeoffStandRunwayAssignItem.h"
#include "..\MFCExControl\ToolbarWithDropMenu.h"

class TakeoffRunwayAssignmentStrategies;
class CTakeoffFlightTypeRunwayAssignment;
class CTakeOffTimeRangeRunwayAssignItem;
class CManageAssignPriorityData;
class CTakeOffPriorityRunwayItem;
class CTakeOffPosition;

class CDlgTakeoffRunwayAssign : public CDialog
{
public:
	enum { IDD = IDD_TAKEOFFRUNWAYASSIGN };
public:
	CDlgTakeoffRunwayAssign(int nProjID, PSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb,int nAirPortID,CWnd* pParent = NULL);
	virtual ~CDlgTakeoffRunwayAssign();

protected:
	enum TY_NODE 
	{
		TY_TREENODE_STAND,
		TY_TREENODE_FLIGHT,
		TY_TREENODE_TIMERANGE
	};
	enum TY_PRIORITYTREE_NODE 
	{
		TY_PRIORITY,
		//TY_TAKEOFFRWY,
		TY_CONDITION,
		TY_TAKEOFF_POSITION,
		TY_TAKEOFF_POSITION_ITEM,
		TY_DECISIONPOINT,
		TY_DECISIONPOINT_DEPSTAND,
		TY_DECISIONPOINT_INTERSECTION,
		TY_DECISIONPOINT_INTERSECTION_ITEM,
// 		TY_DECISION_DEPSTAND,
// 		TY_DECISION_INTERSECTION,
		TY_BACKTRACK,
		TY_NUMBER,
		TY_TIME,
		TY_TAKEOFF,
		TY_TRAIL
	};

	int m_ProID ;
	PSelectFlightType m_FlightType ;
	CAirportDatabase* m_AirportDB ;

	CTakeoffStandRunwayAssign m_Data ;
	CComboBox	m_wndCmbAssignStrategy;
	CTreeCtrl	m_wndTreeFltTime;
	CARCTreeCtrl   m_wndListPriority;
	CToolbarWithDropMenu	m_wndFltToolbar;
	CToolbarWithDropMenu	m_wndRunwayToolbar;
	CStatic		m_staticFlightType;
	CStatic		m_staticRwyAssign;
	HTREEITEM	m_hTimeRangeTreeItem ;

	CSplitterControl m_wndSplitter1;

	int m_nAirPortID;

//	CDlgSelectTakeoffPosition *m_dlgTakeoffPos ;

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() ;
	int OnCreate(LPCREATESTRUCT lpCreateStruct) ;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg	void OnContextMenu(CWnd* pWnd, CPoint point);

	afx_msg void OnAddStand() ;
	afx_msg void OnEditStand() ;
	afx_msg void OnDeleteStand() ;

	afx_msg void OnAddFlight() ;
	afx_msg void OnDeleteFlight() ;
	afx_msg void OnEditFlight() ;

	afx_msg void OnAddTimeRange() ;
	afx_msg void OnEditTimeRange() ;
	afx_msg void OnDeleteTimeRange() ;

	afx_msg void OnAddPriorityRunway() ;
	afx_msg void OnEditPriorityRunway() ;
	afx_msg void OnDeleteRunway();
	
	
	afx_msg void OnAddTakeoffPosition() ;
	afx_msg void OnEditTakeoffPos();
	afx_msg void OnDeleteTakeoffPosition() ;

	afx_msg void OnAddIntersection() ;
	afx_msg void OnDelIntersection() ;	

protected:
	void InitStandFlightTree() ;
	void InitPriorityTree(CTakeOffTimeRangeRunwayAssignItem* _PTimeRange) ;
	
	//--c:don't need any more--
	void AddStandTreeItem(CTakeoffStandRunwayAssignItem* _standItem ) ;
	void EditStandTreeItem(CTakeoffStandRunwayAssignItem* _standItem,HTREEITEM _HStandItem) ;
	//--c:don't need any more--

	void AddFlightTreeItem(CTakeoffFlightTypeRunwayAssignment* _fltItem , HTREEITEM _standItem) ;
	void EditFlightTreeItem(CTakeoffFlightTypeRunwayAssignment* _fltItem,HTREEITEM _FltItem) ;

	void AddTimeRangeTreeItem(CTakeOffTimeRangeRunwayAssignItem* _TimeRange , HTREEITEM _FltItem) ;
	void EditTimeRangeTreeItem(CTakeOffTimeRangeRunwayAssignItem* _TimeRange,HTREEITEM _TimeItem) ;

	void InitComboBox() ;	
	TY_NODE GetTreeItemNodeType(HTREEITEM _item) ;

	void AddPriorityItem(CManageAssignPriorityData* _PriorityData, int nPriority) ;

	void AddTakeOffPositionItem(CTakeOffPosition* _positionStr,HTREEITEM _PositionsItem) ;
	void EditTakeOffPositionItem(CTakeOffPosition* _positionStr,HTREEITEM _takeoffPositionItem) ;

protected:
	void OnAddLeftToolBar() ;
	void OnEditLeftToolBar() ;
	void OnDeleteLeftToolBar() ;

	void OnAddRightToolBar() ;
	void OnDeleteRightToolBar() ;
	void OnEditRightToolBar() ;

	void OnDeletePriorityRunway() ;

	void EditTakeoffPositionPercent(CTakeOffPriorityRunwayItem* pRwyItem, HTREEITEM hRwyItem) ;


	CString FormatTimeRangeString(ElapsedTime& _startTime ,ElapsedTime& _endTime);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;

	void DoResize(int delta,UINT nIDSplitter);

protected:
	void OnAssignmentStrategyChange();
	CString FormatNumberOfAircraft(int _number);
	CString FormatLastAircratTime(int _time);
	CString FormatTakeOffItemText(CTakeOffPosition* _str);
	//double GetTakeOffPositionPercent(HTREEITEM _RunwayItem);
	BOOL CheckThePercentvaild(HTREEITEM _TakeoffPosition ,double _floatval);


protected:
	void OnSave();
	void OnOK() ;
	void OnCancel() ;
	void EnableRightCtrlView(BOOL _isOrNo);
	void OnTvnSelchangedFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult);
	void InitRightCtrlData(CTakeOffTimeRangeRunwayAssignItem* _TimeRangeItem);
	void DoDataExchange(CDataExchange* pDX);
	void OnTvnSelchangedPriorityTree(NMHDR *pNMHDR, LRESULT *pResult);
};
