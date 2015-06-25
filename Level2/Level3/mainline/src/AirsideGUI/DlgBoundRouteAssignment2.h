#pragma once
#include "MFCExControl/ListCtrlEx.h"
#include "..\MFCExControl\ARCTreeCtrl.h"
#include "..\MFCExControl\SplitterControl.h"
#include <MFCExControl/XTResizeDialogEx.h>
#include "NodeViewDbClickHandler.h"
#include "afxwin.h"

class RunwayExit;
class CBoundRouteAssignment;
class CTimeRangeRouteAssignment;
class CIn_OutBoundRoute;
class CRouteItem;
class CAirportDatabase;
class CRoutePriority;


// CDlgBoundRouteAssignment2 dialog

class CDlgBoundRouteAssignment2 : public CXTResizeDialogEx
{
	DECLARE_DYNAMIC(CDlgBoundRouteAssignment2)
	class CTreeData
	{
	public:
		enum TreeItemType
		{
			Non_Type,
			Priority_Item,
			Start_Item,
			Route_Item,
			Count_Item,
			Time_Item,
			Preceding_Item,
			Recapture_Item
		};
	public:
		CTreeData()
			:m_pData(NULL)
			,m_emType(Non_Type)
		{

		}
		DWORD m_pData;
		TreeItemType m_emType;
	};

public:
	CDlgBoundRouteAssignment2(int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb, LPCTSTR lpszTitle, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgBoundRouteAssignment2();

// Dialog Data
	enum { IDD = IDD_DIALOG_BOUNDROUTEASSIGN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	void UpdateSplitterRange();

	void SetAllControlsResize();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCmdNewOrgDst();
	afx_msg void OnCmdDelOrgDst();
	afx_msg void OnCmdEditOrgDst();

	afx_msg void OnCmdNewRoute();
	afx_msg void OnCmdDelRoute();
	afx_msg void OnCmdEditRoute();
	afx_msg void OnCmdMoveNextPriority();
	afx_msg void OnCmdMovePreviousPriority();

	afx_msg void OnBnClickedButtonSave();

	afx_msg void OnLvnItemchangedOrgDst(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnTvnSelchangedTreeRoute(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnCmdNewFlightTime();
	afx_msg void OnCmdDelFlightTime();
	afx_msg void OnCmdEditFlightTime();

	afx_msg void OnAddTimeRange();
	afx_msg void OnEditTimeRange();
	afx_msg void OnDelTimeRange();

	afx_msg	void OnContextMenu(CWnd* pWnd, CPoint point);

	afx_msg void OnTvnSelchangedTreeFlighttypeTime(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	afx_msg void OnBnClickedRadioShortestpath();
	afx_msg void OnBnClickedRadioPreferredroute();
	afx_msg void OnSize(UINT nType, int cx, int cy);


protected:
	virtual void NewOrgDst();
	virtual void EditOrgDst();

	virtual void InitListCtrl(void);
	virtual void SetOrgDstListContent(void);

	void InitToolBar(void);
	void InitInboundRouteAssignmentState(void);
	void InitPrioirtyItemContent(void);
	void InitPrioirtyItemContent(CRoutePriority*pPriority,HTREEITEM hPriorityItem);
	void InitChangeConditionContent();
	void InitChangeConditionItemContent(HTREEITEM hItem);
	void DeletePriorityItem(HTREEITEM hItem);

	void SetTreeContent(void);

	void DeleteChildItemInTree(HTREEITEM hItem);
	void SetChildItemContent(HTREEITEM hParentItem, CRouteItem *pOutboundRouteItem);

	void UpdateRouteChoice(CIn_OutBoundRoute* pCurBoundRoute);

	void UpdateRouteChoiceBtns();
	void UpdateOrgDstToolBar();
	void UpdateRouteToolBar();
	void DoResize(int delta,UINT nIDSplitter);

	void InitFlightTimeTree() ;

protected:
	CButton m_btnShortestPath;
	CButton m_btnPreferredRoute;
	CToolBar     m_ToolBarFlightType;
	CToolBar     m_toolbarOrgDst;
	CToolBar     m_ToolBarRoute;

	CTreeCtrl    m_wndTreeFltTime;
	CARCTreeCtrl m_TreeCtrlRoute;
	CListCtrlEx	 m_listOrgDst;

	CSplitterControl m_wndSplitterVer;
	CSplitterControl m_wndSplitterHor;

	CBoundRouteAssignment     *m_pBoundRouteAssignment;
	CIn_OutBoundRoute         *m_pCurBoundRoute;
	CTimeRangeRouteAssignment *m_pCurTimeAssignment;

	PFuncSelectFlightType m_pSelectFlightType;
	CAirportDatabase*     m_pAirportDB;
	int                   m_nProjID;
	CString               m_strTitle;

};

/////////////////////////InboundRouteAssignmentDlg///////////////
// class InboundRouteAssignmentDlg : public CDlgBoundRouteAssignment2
// {
// public:
// 	InboundRouteAssignmentDlg(int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb,LPCTSTR lpszTitle,  CWnd* pParent = NULL);
// 	virtual ~InboundRouteAssignmentDlg();
// 
// protected:
// 	virtual void NewOrgDst();
// 	virtual void EditOrgDst();
// 	void InitListCtrl(void);
// 	void SetOrgDstListContent(void);
// };
// 
/////////////////////////OutboudRouteAssignmentDlg/////////////////////
class CDlgOutboundRouteAssignment2 : public CDlgBoundRouteAssignment2
{
public:
	CDlgOutboundRouteAssignment2(int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb,LPCTSTR lpszTitle,  CWnd* pParent = NULL);
	virtual ~CDlgOutboundRouteAssignment2();

protected:
	virtual void NewOrgDst();
	virtual void EditOrgDst();
	void InitListCtrl(void);
	void SetOrgDstListContent(void);
};