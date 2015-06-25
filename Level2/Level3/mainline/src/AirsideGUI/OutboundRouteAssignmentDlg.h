#pragma once
#include "MFCExControl/ListCtrlEx.h"
#include "..\MFCExControl\ARCTreeCtrl.h"
#include "..\MFCExControl\SplitterControl.h"
#include "NodeViewDbClickHandler.h"

class RunwayExit;
class CBoundRouteAssignment;
class CTimeRangeRouteAssignment;
class CIn_OutBoundRoute;
class CRouteItem;
class CAirportDatabase;
class CRoutePriority;


// CBoundRouteAssignmentDlg dialog

class CBoundRouteAssignmentDlg : public CDialog
{
	DECLARE_DYNAMIC(CBoundRouteAssignmentDlg)
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
	CBoundRouteAssignmentDlg(int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb, LPCTSTR lpszTitle, CWnd* pParent = NULL);   // standard constructor
	virtual ~CBoundRouteAssignmentDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_OUTBOUNDROUTEASSIGN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCmdNewRunwayExitAndStandFamily();
	afx_msg void OnCmdDelRunwayExitAndStandFamily();
	afx_msg void OnCmdEditRunwayExitAndStandFamily();

	afx_msg void OnCmdNewRoute();
	afx_msg void OnCmdDelRoute();
	afx_msg void OnCmdEditRoute();
	afx_msg void OnCmdMoveNextPriority();
	afx_msg void OnCmdMovePreviousPriority();

	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedOk();

	afx_msg void OnLvnItemchangedListRunwayexitandstandfamily(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeRoute(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnCmdNewFlightTime();
	afx_msg void OnCmdDelFlightTime();
	afx_msg void OnCmdEditFlightTime();

	afx_msg void OnAddTimeRange();
	afx_msg void OnEditTimeRange();
	afx_msg void OnDelTimeRange();

	afx_msg	void OnContextMenu(CWnd* pWnd, CPoint point);

	afx_msg void OnTvnSelchangedTreeFlighttypeTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboRoutechoice();
	afx_msg LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	void InitFlightTimeTree() ;


protected:
	virtual void InitComboBox();

	virtual void NewOriginAndDestination();
	virtual void EditOriginAndDestination();

	virtual void InitListCtrl(void);
	virtual void SetListContent(void);

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

	void SetCtrlState(BOOL bState);

	void UpdateRunwayExitAndStandToolBar();
	void UpdateRouteToolBar();
	void DoResize(int delta,UINT nIDSplitter);


protected:
	int                        m_nProjID;
	CToolBar    m_ToolBarRunwayExitAndStandFamily;
	CToolBar    m_ToolBarRoute;
	CToolBar    m_ToolBarFlightType;
	CStatic		m_staticFlightType;
	CStatic		m_staticRouteAssign;
	CStatic     m_static;
	CButton		m_btnSave;
	CButton		m_btOk;
	CButton     m_btCancel;

	CARCTreeCtrl   m_TreeCtrlRoute;
	CListCtrlEx	m_ListCtrlExitPositionAndStandFamily;
	CSplitterControl m_wndSplitter1;

	CBoundRouteAssignment   *m_pBoundRouteAssignment;
	CIn_OutBoundRoute       *m_pCurBoundRoute;
	CTimeRangeRouteAssignment       *m_pCurTimeAssignment;

	CComboBox m_cmbRouteType;
	CTreeCtrl m_wndTreeFltTime;
	PFuncSelectFlightType  m_pSelectFlightType;
	CAirportDatabase* m_pAirportDB;
	CString m_strTitle;
};

/////////////////////////InboundRouteAssignmentDlg///////////////
class InboundRouteAssignmentDlg : public CBoundRouteAssignmentDlg
{
public:
	InboundRouteAssignmentDlg(int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb,LPCTSTR lpszTitle,  CWnd* pParent = NULL);
	virtual ~InboundRouteAssignmentDlg();

protected:
	virtual void NewOriginAndDestination();
	virtual void EditOriginAndDestination();
	void InitListCtrl(void);
	void SetListContent(void);
};

/////////////////////////OutboudRouteAssignmentDlg/////////////////////
class OutboundRouteAssignmentDlg : public CBoundRouteAssignmentDlg
{
public:
	OutboundRouteAssignmentDlg(int nProjID,PFuncSelectFlightType pSelectFlightType,CAirportDatabase *pAirPortdb,LPCTSTR lpszTitle,  CWnd* pParent = NULL);
	virtual ~OutboundRouteAssignmentDlg();

protected:
	virtual void NewOriginAndDestination();
	virtual void EditOriginAndDestination();
	void InitListCtrl(void);
	void SetListContent(void);
    void InitComboBox();
};