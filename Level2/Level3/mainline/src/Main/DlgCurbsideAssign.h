#pragma once
//#include "../Landside/CurbsideDetail.h"
//#include "../Landside/CurbSideFlight.h"
#include "../Landside/CurbSideFlightList.h"
//#include "../Landside/CurbsideTime.h"
#include "../MFCExControl/ListCtrlEx.h"

#include "afxcmn.h"
#include "TermPlanDoc.h"

// CDlgCurbsideAssign dialog

static const UINT ID_NEW_FLTTYPE = 10;
static const UINT ID_DEL_FLTTYPE = 11;
static const UINT ID_EDIT_FLTTYPE = 12;
static const UINT ID_NEW_RUNWAY = 20;
static const UINT ID_DEL_RUNWAY = 21;
static const UINT ID_EDIT_RUNWAY = 22;
class CDlgCurbsideAssign : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgCurbsideAssign)

public:
	enum TreeNodeType
	{
		TREENODE_FLTTYPE = 0,
		TREENODE_TIMERANGE
	};
	CDlgCurbsideAssign(CWnd* pParent = NULL);   // standard constructor
    CDlgCurbsideAssign(CTermPlanDoc *tmpDoc,CWnd*pParent=NULL);
	virtual ~CDlgCurbsideAssign();
	afx_msg void OnTvnSelChangedFlight(NMHDR*pNMHDR,LRESULT *pResult);
// 	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	virtual BOOL OnInitDialog();
	virtual void OnSelChangedFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnListItemEndLabelEdit(int nItem, int nSubItem);
	void InitToolBar();
	void InitTree();
	void UpdateTreeToolBar();
	void UpdateListToolBar();
// 	bool IsvalidTimeRange(CString strTimeRange1,CString strTimeRange2);
// 	afx_msg void OnNewTimeRange();
// 	afx_msg void OnDelTimeRange();
// 	afx_msg void OnEditTimeRange();
	afx_msg void OnNewCurbSide();
	afx_msg void OnDelCurbSide();
	afx_msg void OnEditCurbSide();
	afx_msg void OnNewPaxType();
	afx_msg void OnDelPaxType();
	afx_msg void OnEditPaxType();
	afx_msg void OnSave();
	afx_msg void OnOk();
	afx_msg void OnCancel();
	afx_msg void OnLvnItemChangedCurbSideList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);
//     BOOL getTreeItemByType(TreeNodeType nodeType,HTREEITEM &hItem ); 
	void InsertTreeItemorder(CCurbSideFlight *tmpFligType);
// Dialog Data
	enum { IDD = IDD_DIALOG_CURBSIDEASSIGN1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CListCtrlEx m_listCtrl;
	CTreeCtrl m_treeCtrl;
// 	CCurbSideFlightList m_flightList;
// 	CCurbSidePaxList m_paxList;
	CToolBar m_treeToolBar;
	CToolBar m_listToolBar;
// 	HTREEITEM m_hTimeRangeTreeItem;
	CTermPlanDoc *pDoc;
	InputLandside *m_pLandSide;
};
