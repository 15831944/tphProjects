#pragma once
#include "..\MFCExControl\ListCtrlEx.h"
#include "NodeViewDbClickHandler.h"
#include "../common/TimeRange.h"
//#include "AirlineRelatedBaseDlg.h"
#include "../MFCExControl/XTResizeDialog.h"


static const UINT ID_NEW_FLTTYPE = 10;
static const UINT ID_DEL_FLTTYPE = 11;
static const UINT ID_EDIT_FLTTYPE = 12;
static const UINT ID_NEW_RUNWAY = 20;
static const UINT ID_DEL_RUNWAY = 21;
static const UINT ID_EDIT_RUNWAY = 22;

class RunwayAssignmentStrategies;

class CDlgRunwayAssignment : public CXTResizeDialog
{
public:
	enum { IDD = IDD_TAKEOFFRUNWAYASSIGN };
	enum TreeNodeType
	{
		TREENODE_FLTTYPE = 0,
		TREENODE_TIMERANGE
	};

public:
	CDlgRunwayAssignment(int nProjID, PSelectFlightType pSelectFlightType, LPCTSTR lpszTitle, CWnd* pParent = NULL);
	virtual ~CDlgRunwayAssignment();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX); 
	virtual BOOL OnInitDialog();

	virtual void OnOK();
	virtual void OnCancel();
	virtual void OnSave();


	virtual void ReadData() = 0;



	virtual void InitTreeCtrl();
	void SetListCtrlContents();

	virtual void OnNewFltType(FlightConstraint& flightType);
	virtual void OnDelFltType();
	virtual void OnEditFltType();

	virtual void OnAddTimeRange();
	virtual void OnEditTimeRange();
	virtual void OnDelTimeRange();

	virtual void OnNewRunway();
	virtual void OnDelRunway();
	virtual void OnEditRunway();
	
	

	virtual void OnAssignmentStrategyChange();
	virtual void OnClickFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnSelChangedFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void ContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	virtual void OnListItemEndLabelEdit(int nItem, int nSubItem);
	virtual void OnListItemChanged(int nItem, int nSubItem);


	CString		m_strTitle;
	CComboBox	m_wndCmbAssignStrategy;
	CTreeCtrl	m_wndTreeFltTime;
	CListCtrlEx m_wndListTakeoffRunwayAssign;
	CToolBar	m_wndFltToolbar;
	CToolBar	m_wndRunwayToolbar;
	
	PSelectFlightType	m_pSelectFlightType;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMsgSave();
	afx_msg void OnMsgNewFltType();
	afx_msg void OnMsgDelFltType();
	afx_msg void OnMsgEditFltType();
	afx_msg void OnMsgNewRunway();
	afx_msg void OnMsgDelRunway();
	afx_msg void OnMsgEditRunway();
	afx_msg void OnMsgAssignmentStrategyChange();
	afx_msg void OnNMClickFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemChangedTakeoffList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnMsgAddTimeRange();
	afx_msg void OnMsgDelTimeRange();
	afx_msg void OnMsgEditTimeRange();

protected:
	int m_nProjID;
	RunwayAssignmentStrategies* m_pStrategies;
	HTREEITEM m_hTimeRangeTreeItem;

protected:
	BOOL GetTreeItemByType(TreeNodeType nodeType, HTREEITEM& hTreeItem);
	BOOL IsManagedAssignmentTimeRangeTreeItem();
	void UpdateState();
	bool IsvalidTimeRange(CString strTimeRange1,CString strTimeRange2);
};
