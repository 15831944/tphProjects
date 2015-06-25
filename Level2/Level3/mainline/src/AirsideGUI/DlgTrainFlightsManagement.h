#pragma once
#include "afxwin.h"
#include "../MFCExControl/ARCTreeCtrl.h"
#include "InputAirside/trainingflightcontainer.h"
#include "InputAirside/CTrainFlightsManage.h"
#include "../MFCExControl/XTResizeDialog.h"


// DlgTrainFlightsManagement dialog
class  CTakeOffPosition;
class CDlgTrainFlightsManagement : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgTrainFlightsManagement)

public:
	CDlgTrainFlightsManagement(CAirportDatabase* pAirportDB,int nProjID, CWnd* pParent);   // standard constructor
	virtual ~CDlgTrainFlightsManagement();

// Dialog Data
	enum { IDD = IDD_DIALOG_TRAINFLIGHTSMANAGEMENT };
protected:
	enum TY_NODE 
	{
		TY_TREENODE_FLIGHT,
		TY_TREENODE_TIMERANGE,
		TY_TREENODE_TAKEOFFPOS,
		TY_TREENODE_NULL
	};
	enum TY_PRIORITYTREE_NODE 
	{
		TY_TRAIN_TAKEOFFSEQUENCE_ITEM,
		TY_TRAIN_TAKEOFF_ITEM,
		TY_TRAIN_FLIGHT_ITEM,
		TY_TRAIN_TIMERANGE_ITEM,
		TY_TRAIN_POSITION_ITEM,
		TY_TRAIN_TRAIN_ITEM,
		TY_TRAIN_APPROACH_ITEM,
		TY_TRAIN_APPROACHSEQUENCE_ITEM
	};
	
protected:
	int m_nProjID;
	CCoolTree m_wndTreeCtrl;
	HTREEITEM m_Takeoff;
	HTREEITEM m_hTakeOffItem;
	HTREEITEM m_TrueRandoom;
	HTREEITEM m_hApproachitem;
	CTrainingFlightsManage m_Data;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg	void OnContextMenu(CWnd* pWnd, CPoint point);

	afx_msg void OnAddTimeRange();
	afx_msg void OnEditTimeRange();
	afx_msg void OnDeleteTimeRange();

	afx_msg void OnAddTakeoffPosition();
	afx_msg void OnEditTakeoffPosition();
	afx_msg void OnDeleteTakeoffPosition();
	
	afx_msg void OnSave();
	afx_msg void OnOk();
	afx_msg void OnCancel();
protected:
	void AddFlightTreeItem(CTrainingFlightTypeManage* _flightItem,CString str);
	void AddTimeRangeTreeItem(CTrainingTimeRangeManage* _TimeRange , HTREEITEM _FltItem);
	void EditTimeRangeTreeItem(CTrainingTimeRangeManage* _TimeRange,HTREEITEM _TimeItem);
	CString FormatTimeRangeString(ElapsedTime& _startTime ,ElapsedTime& _endTime);
	TY_NODE GetTreeItemNodeType(HTREEITEM _item);
protected:	
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	void AddTakeOffPositionItem(CTrainingTakeOffPosition* _positionStr,std::vector<RunwayExit> _runwayExit,HTREEITEM _PositionsItem);
	CString FormatTakeOffItemText(std::vector<RunwayExit> _runwayExit);
	void InitFlightTree();

private:
	CAirportDatabase* m_pAirportDB;
};
