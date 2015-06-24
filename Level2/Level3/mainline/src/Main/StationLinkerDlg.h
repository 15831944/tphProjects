#if !defined(AFX_STATIONLINKERDLG_H__DE64EFE4_5E0D_4B86_8E2D_6EEB7A42D20E__INCLUDED_)
#define AFX_STATIONLINKERDLG_H__DE64EFE4_5E0D_4B86_8E2D_6EEB7A42D20E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationLinkerDlg.h : header file
//
#include "..\engine\TrainTrafficGraph.h"
#include "TermPlanDoc.h"
/////////////////////////////////////////////////////////////////////////////
// CStationLinkerDlg dialog

class CStationLinkerDlg : public CDialog
{
// Construction
private:
	CTermPlanDoc* m_pDoc;
	TrainTrafficGraph m_tempTrafficeGraph;
	TrainTrafficGraph* m_pDocTrafficGraph;
	HTREEITEM m_treeRoot;
	HTREEITEM m_currentItem;
	typedef std::vector<HTREEITEM> TREEITEM;
	TREEITEM m_port1Vector;
	TREEITEM m_port2Vector;
	TREEITEM m_railWayVector;

	int m_iCurrentStationIndex;
	int m_iCurrentPortIndex;

	int m_iSecondStationIndex;
	int m_iSecondStationPort;
	void LoadTree();
	void BuildRailWay(HTREEITEM _parentItem,RAILWAY& railVector,int _stationIndex);
	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);

	void InitToolbar();
	CToolBar m_ToolBar;
	RailwayInfo* m_pSelectedRailWay;
	HTREEITEM m_hSelectedItem;
	int m_iSeletedType;
public:
	CStationLinkerDlg(TrainTrafficGraph* _pDocTrafficGraph,CTermPlanDoc* _pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStationLinkerDlg)
	enum { IDD = IDD_DIALOG_STATION_LINKER };
	CStatic	m_toolbarPosition;
	CTreeCtrl	m_tvStation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStationLinkerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStationLinkerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnStationlinkerAddlinker();
	afx_msg void OnStationlinkerPick();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchangedTreeRelation(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnStationLinkerNewRailway();
	afx_msg void OnStationLinkerDeleteRailway();
	afx_msg void OnStationLinkerZPos();
	//}}AFX_MSG
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONLINKERDLG_H__DE64EFE4_5E0D_4B86_8E2D_6EEB7A42D20E__INCLUDED_)
