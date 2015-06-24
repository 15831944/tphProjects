#if !defined(AFX_STATIONLINKERPOPDLG_H__9054F070_65E4_403E_A918_0E32B52F0117__INCLUDED_)
#define AFX_STATIONLINKERPOPDLG_H__9054F070_65E4_403E_A918_0E32B52F0117__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StationLinkerPopDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStationLinkerPopDlg dialog
#include "..\engine\TrainTrafficGraph.h"
class CStationLinkerPopDlg : public CDialog
{
private:
	TrainTrafficGraph* m_pTempTrafficeGraph;
	
	int m_iFirstStationIndex;
	int m_iFirstStationPort;
	

	bool m_bHaveSelect;
	void LoadTree();
	typedef std::vector<HTREEITEM> TREEITEM;
	TREEITEM m_portVector;

	HTREEITEM m_currentItem;
// Construction
public:
	//void InitDlg(TrainTrafficGraph* _pDocTrafficGraph );
	int m_iSelectedStationIndex;
	int m_iSelectedStationPort;
	CStationLinkerPopDlg(int _firstStationIndex, int _firstStationPort, TrainTrafficGraph* _pGraph,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStationLinkerPopDlg)
	enum { IDD = IDD_DIALOG_STATIONLINKER_POPDLG };
	CTreeCtrl	m_tree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStationLinkerPopDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStationLinkerPopDlg)
	virtual BOOL OnInitDialog();
	///afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//afx_msg void OnStationlinkerPopdlgSelect();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONLINKERPOPDLG_H__9054F070_65E4_403E_A918_0E32B52F0117__INCLUDED_)
