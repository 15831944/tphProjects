#if !defined(AFX_CARFLOWNEXTSTATION_H__099A9D70_3353_487B_B025_6D1486FEE13E__INCLUDED_)
#define AFX_CARFLOWNEXTSTATION_H__099A9D70_3353_487B_B025_6D1486FEE13E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CarFlowNextStation.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCarFlowNextStation dialog
#include "..\engine\TrainTrafficGraph.h"
class CCarFlowNextStation : public CDialog
{
// Construction
public:
	CCarFlowNextStation(TrainTrafficGraph* _pRailWayTraffic,IntegratedStation* _pStartStation,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCarFlowNextStation)
	enum { IDD = IDD_DIALOG_CARFLOW_NEXTSTATION };
	
	CTreeCtrl	m_treeNextStation;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCarFlowNextStation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCarFlowNextStation)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreeNextstationTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	TrainTrafficGraph* m_pRailWayTraffic;
	IntegratedStation* m_pStartStaion;
	std::vector<HTREEITEM> m_vStationItem;
	void LoadTree();
	bool m_bSelect;
	
//	CString GetRailWayName( RailwayInfo* _pRailWay, int _iFirstStationIndex );

public:
	IntegratedStation* m_pSelectedStation;
	RailwayInfo* m_pSelectedRailWay;
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CARFLOWNEXTSTATION_H__099A9D70_3353_487B_B025_6D1486FEE13E__INCLUDED_)
