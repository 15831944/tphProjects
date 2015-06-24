#pragma once
#include "../Landside/ParkingLotFlightTypeList.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "../MFCExControl/SplitterControl.h"
#include "afxcmn.h"
#include "TermPlanDoc.h"
#include "../Landside/InputLandside.h"
// CDlgParkinglotAssign dialog

class CDlgParkinglotAssign : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgParkinglotAssign)

public:
	CDlgParkinglotAssign(CWnd* pParent = NULL);   // standard constructor
	CDlgParkinglotAssign(CTermPlanDoc *tmpDoc,CWnd *pParent=NULL);
	virtual ~CDlgParkinglotAssign();
	enum TreeNodeType
	{
		TREENODE_FLTTYPE = 0,
		TREENODE_TIMERANGE
	};
	afx_msg void OnTvnSelChangedFlight(NMHDR*pNMHDR,LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	virtual BOOL OnInitDialog();
	virtual void OnSelChangedFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnListItemEndLabelEdit(int nItem, int nSubItem);
	void InitToolBar();
	void InitTree();
	void UpdateTreeToolBar();
	void UpdateListToolBar();
	bool IsvalidTimeRange(CString strTimeRange1,CString strTimeRange2);
	afx_msg void OnNewTimeRange();
	afx_msg void OnDelTimeRange();
	afx_msg void OnEditTimeRange();
	afx_msg void OnNewParkinglot();
	afx_msg void OnDelParkinglot();
	afx_msg void OnEditParkinglot();
	afx_msg void OnNewFlightType();
	afx_msg void OnDelFlightType();
	afx_msg void OnEditFlightType();
	afx_msg void OnSave();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnLvnItemChangedParkinglotList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);
	BOOL getTreeItemByType(TreeNodeType nodeType,HTREEITEM &hItem ); 
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnSizeEx(int cx,int cy);
	void DoResize(int delta,UINT nIDSplitter);
	void InsertTreeItemorder(CParkingFligthType *tmpFligType);
	void EditTreeItemOrder(CParkingFligthType *tmpFlightType);
	
// Dialog Data
	enum { IDD = IDD_DIALOG_PARKINGLOTASSIGN };
	void ResizeRightWnd(CSize size,CSize sizeLast);
	void ResizeLeftWnd(CSize size,CSize sizeLast);
	void ResizeToHideRightWnd();
	void ShowRightWnd(BOOL bShow);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CTreeCtrl m_treeCtrl;
	CListCtrlEx m_listCtrl;
	CToolBar m_treeToolBar;
	CToolBar m_listToolBar;
	CButton m_btnSave;
	CButton m_btnOk;
	CButton m_btnCancel;
	CStatic m_frameFlight;
	CStatic m_frameDetail;
	HTREEITEM m_hTimeRangeTreeItem;
	CTermPlanDoc *pDoc;
	CParkingLotFlightTypeList m_flightList;
	CSplitterControl m_wndSplitter1;
	CSplitterControl m_wndSplitter2;
	CSplitterControl m_wndSplitter3;
	CSplitterControl m_wndSplitter4;
	CSplitterControl m_wndSplitter5;
	CArray <CWnd*,CWnd*> m_listRightWnd;
	CArray<CWnd*,CWnd*>m_listLeftBtn;
	CSize m_sizeLastWnd;
	BOOL m_bExpand;
	BOOL m_onInit;
	int m_cx;
	int m_cy;
	InputLandside *m_pLandSide;
};
