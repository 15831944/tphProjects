#pragma once
#include "afxcmn.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "../Landside/ODFlightTypeList.h"
#include "TermPlanDoc.h"
#include "../Landside/InputLandside.h"
// CDlgODAssign dialog

class CDlgODAssign : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgODAssign)

public:
	enum TreeNodeType
	{
		TREENODE_FLTTYPE = 0,
		TREENODE_TIMERANGE
	};
	CDlgODAssign(CWnd* pParent = NULL);   // standard constructor
	CDlgODAssign(CTermPlanDoc *tmpDoc,CWnd*pParent=NULL);
	virtual ~CDlgODAssign();
	void InitTree();
	void InitOrigin();
	void InitDest();
	afx_msg void OnNewFltType();
	afx_msg void OnEditFltType();
	afx_msg void OnDelFltType();
	afx_msg void OnNewTimeRange();
	afx_msg void OnEditTimeRange();
	afx_msg void OnDelTimeRange();
	afx_msg void OnNewOriginOD();
	afx_msg void OnEditOriginOD();
	afx_msg void OnDelOriginOD();
	afx_msg void OnNewDestOD();
	afx_msg void OnEditDestOD();
	afx_msg void OnDelDestOD();
	virtual BOOL OnInitDialog();
	bool IsvalidTimeRange(CString strTimeRange1,CString strTimeRange2);
	void InitToolBar();
	BOOL getTreeItemByType(TreeNodeType nodeType,HTREEITEM &hItem );
	void OnContextMenu(CWnd* pWnd, CPoint pos);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBtnSave();
	void UpdateTreeBar();
	void UpdateOriginBar();
	void UpdateDestBar();
	void OnListItemEndLabelEdit(int nItem, int nSubItem,enumODAssignmentType tmpType);

// Dialog Data
	enum { IDD = IDD_DIALOG_ODASSIGNMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_treeCtrl;
	CListCtrlEx m_originCtrl;
	CListCtrlEx m_destCtrl;
	CToolBar m_treeToolBar;
	CToolBar m_OriginToolBar;
	CToolBar m_destToolBar;
	CODFlightTypeList m_flightTypeList;
	CTermPlanDoc *pDoc;
	HTREEITEM m_hTimeRangeTreeItem;
	afx_msg void OnTvnSelchangedTreeOdassignment(NMHDR *pNMHDR, LRESULT *pResult);
	InputLandside *m_pLandSide;
	afx_msg void OnLvnItemchangedListOdoriginassign(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListOdoriginassign(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListOddestassign(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListOddestassign(NMHDR *pNMHDR, LRESULT *pResult);
};
