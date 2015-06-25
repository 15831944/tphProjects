#pragma once
#include "NodeViewDbClickHandler.h"

#include "MFCExControl/ARCTreeCtrl.h"
#include "TreeCtrlRunwayExit.h"
#include "../InputAirside/Runway.h"
#include "../MFCExControl/XTResizeDialog.h"

class FlightTypeRunwayExitStrategyItem;
class RunwayExitStandStrategyItem;
class RunwayExitStrategies;
class InputAirside;
class CAirportDatabase;


class CDlgLandingRunwayExit : public CXTResizeDialog
{
public:
	enum typedddd
	{
		ID_NEW_FLIGHTTYPE=1000001,
       
		ID_MODIFY_FLIGHTTYPE,
		ID_CUT_FLIGHTTYPE,
		
		ID_NEW_STANDGROUP,
		ID_MODIFY_STANDGROUP,
		ID_CUT_STANDGROUP,

		ID_NEW_TIMERANGE,
		ID_MODIFY_TIMERANGE,
		ID_CUT_TIMERANGE,
	};
	enum FltTypeTreeNodeType
	{
		TREENODE_FLTTYPE = 0,
		TREENODE_STAND,
		TREENODE_TIMERANGE
	};

	enum RunwayTreeNodeType
	{
		TREENODE_PROB_RUNWAY = 0,
		TREENODE_PROB_MARKING,
		TREENODE_PROB_BACKTRACK,

		TREENODE_PRIO_RUNWAY,
		TREENODE_PRIO_MARKING,
		TREENODE_PRIO_BACKTRACK,
	};
	

public:
	CDlgLandingRunwayExit(int nProjID, PFuncSelectFlightType pSelectFlightType, InputAirside* pInputAirside, CWnd* pParent = NULL);
	virtual ~CDlgLandingRunwayExit();
    void ShowTips(const CString & tmpTips);
	PFuncSelectFlightType	m_pSelectFlightType;

//	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

protected:
	void ReadData();
	void ResetFltTimeTreeCtrlContent();

	void AddOneExitStrategyToTree( FlightTypeRunwayExitStrategyItem* pStrategyItem, COOLTREE_NODE_INFO &cni );

	void AddOneStandStrategyItemToTree( RunwayExitStandStrategyItem* pStandStrategyItem, COOLTREE_NODE_INFO & cni, HTREEITEM hFltTypeTreeItem );
	void AddOneTimeRangeToTree( TimeRangeRunwayExitStrategyItem* pTimeRangeItem, COOLTREE_NODE_INFO &cni, HTREEITEM hStandStrategyTreeItem );
	void ResetExitStrategyContents();
	void ResetExitStrategyContents( TimeRangeRunwayExitStrategyItem* pTimeRangeItem );

	BOOL GetFltTypeTreeItemByType(FltTypeTreeNodeType nodeType, HTREEITEM& hTreeItem);
	BOOL GetRunwayTreeItemByType(RunwayTreeNodeType nodeType, HTREEITEM& hTreeItem);

	void UpdateState();
	void AddRunwayExitItemToTree(RunwayExitStrategyPercentItem *pRunwayMarkExitItem);
	void AddRunwayExitPrioritiesItemToTree(RunwayExitStrategyPriorityItem* pRunwayMarkPrioritiesItem);


	virtual void DoDataExchange(CDataExchange* pDX); 
	virtual BOOL OnInitDialog();

	void InitExitStrategyComboBox();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL CheckExitPercent();

	DECLARE_MESSAGE_MAP()

protected:
	CComboBox			m_wndCmbExitStrategy;
	CARCTreeCtrl		m_wndTreeFltTime;
	CToolBar			m_wndFltToolbar;
	CToolBar			m_wndRunwayToolbar;
	CTreeCtrlRunwayExit	m_wndTreeExitStrategy;

	CButton m_radioProbBased;   // Probability Based
	CButton m_radioPrioBased;   // Priority Based
	

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMsgSave();
	afx_msg void OnMsgNewFltType();
	afx_msg void OnMsgDelFltType();
	afx_msg void OnMsgEditFltType();
	afx_msg void OnMsgNewRunway();
	

	afx_msg void OnMsgDelRunway();
	afx_msg void OnMsgEditRunway();
	afx_msg void OnMsgExitStrategyChange();

	afx_msg void OnMsgAddStandFamily();
	afx_msg void OnMsgEditStandFamily();
	afx_msg void OnMsgDelStandFamily();

	afx_msg void OnMsgAddTimeRange();
	afx_msg void OnMsgEditTimeRange();
	afx_msg void OnMsgDelTimeRange();

	afx_msg void OnBnClickedProbabilityBased();
	afx_msg void OnBnClickedPriorityBased();

	afx_msg void OnMsgAddExitPriority();
	afx_msg void OnMsgDelExitPriority();

	afx_msg void OnMsgBacktrack();
	afx_msg void OnMsgNoBacktrack();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnSelChangedFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	BOOL IsManagedExitTimeRangeTreeItem();

	void GetRunwayExitName(RunwayExit *pRunwayExit, RunwayExitList *pRunwayExitList, CString& strRunwayExitName);

	void AddNewRunwayMarkExitItem( TimeRangeRunwayExitStrategyItem * pTimeRangeItem, int nRunwayID, int nRunwayMarkIndex );
	void AddNewRunwayMarkExitPriorities( TimeRangeRunwayExitStrategyItem * pTimeRangeItem, int nRunwayID, int nRunwayMarkIndex );

private:
	int m_nProjID;
	RunwayExitStrategies* m_pStrategies;
	HTREEITEM m_hTimeRangeTreeItem;
	CAirportDatabase* m_pAirportDB;
	InputAirside m_inputAirside;
	CToolTipCtrl m_Tips;
public:
//	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
protected:
	
	void OnSize(UINT nType, int cx, int cy) ;

		
};