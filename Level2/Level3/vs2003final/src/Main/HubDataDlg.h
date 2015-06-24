#if !defined(AFX_HUBDATADLG_H__D5AB9EB3_9EF2_465F_A7A0_FFB13A2B6795__INCLUDED_)
#define AFX_HUBDATADLG_H__D5AB9EB3_9EF2_465F_A7A0_FFB13A2B6795__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HubDataDlg.h : header file
//
#include "..\inputs\fltdist.h"
#include "..\inputs\pax_cnst.h"
#include "TreeCtrlEx.h"
/////////////////////////////////////////////////////////////////////////////
// CHubDataDlg dialog

#define TRANSIT_ITEMDATA 0
#define TRANSFER_ITEMDATA 1
#define TRANSFER_PERCENT_ITEMDATA 0
//#define TRANSFER_MIN_TIME 1
//#define TRANSFER_TIME_WINDOW 2
//#define TRANSFER_DEST 3
#define TRANSFER_DEST 1

#define TRANSFER_MIN_TIME 0
#define TRANSFER_TIME_WINDOW 1

#define CONTROL_ENABLE_ADD 1
#define CONTROL_ENABLE_MOD 2
#define CONTROL_ENABLE_DEL 4


class CHubDataDlg : public CDialog
{
// Construction
public:
	CHubDataDlg(CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHubDataDlg)
	enum { IDD = IDD_DIALOG_HUBDATA };
	CStatic	m_toolbarcontenter;
	CButton	m_btnOk;
	CButton	m_btnCancel;
	CButton	m_staFrame;
	CTreeCtrlEx	m_treeHubData;
	CButton	m_btnSave;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHubDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitToolbar();
	CToolBar m_ToolBar;
	void ReloadTransferFlightTree( HTREEITEM _hItem, FlightTypeDistribution* _pDist, int _nSelDBIdx );
	void SetProbabilityDistribution( ProbabilityDistribution* _pProbDist );
	void DeleteAllChildItems( HTREEITEM _hParentItem );


	// give HTREEITEM, and level, return treeitem of the paxtype
	HTREEITEM GetPaxTypeItem( HTREEITEM _hItem, int _nLevel );

	// add new pax type to the data. and automatic create the default value to the entyr.
	void AddPassengerTypeEntry();

	// delete the pax type in the hubbing database.
	void DeletePassengerEntry();

	// delete flight distrubition
	void DeleteFltDist();

	// get the level and the index on the level.
	// return 0 - 4
	void GetCurSelection( HTREEITEM _hTreeItem, int& _nLevel, int& _nIdx );


	// level 1, click on root: add pax
		// level 2, click on pax: delete pax
			// level 3, click on transit: modify
			// level 3, click on transfer : nothing.
				// level 4, click on percent, min, window: modify
				// level 4, click on dest: add
					// level 5, click on child of dest: edit, delete
	int GetControlEnableValue( HTREEITEM _hTreeItem );


	// create a label from csLabel + dist
	CString GetLabel( const CString& _csLabel, const ProbabilityDistribution* _pProb );

	// evenly distribute 100 into _nCount. make sure total is 100
	void GetEvenPercent( int _nCount, unsigned char *_pProb );

	// nDataType 0-Transit, 1-Transfer, 2-Min, 3-TimeWindow
//	void AddDataItem( int nDataType	 );
	void AddTransferDestEntry();
	void AddTransferDestFlight();

	// nDataType 0-Transit, 1-Transfer, 2-Min, 3-TimeWindow
	void DeleteDataItem( int _nDataType, int _nDBIdx );

	void SetPercent(int _nPercent);
	BOOL CheckPercent();

	
	HTREEITEM m_hRootItem;//save the root item
	HTREEITEM m_hRClickItem;//save the click item
	HTREEITEM m_hModifiedItem;//save the modify item

	void ReloadTree();
	CString GetProjPath();
	InputTerminal* GetInputTerminal();

	// Generated message map functions
	//{{AFX_MSG(CHubDataDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnHubbingdataModify();
	afx_msg void OnHubbingdataDel();
	afx_msg void OnHubbingdataAdd();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnToolbarbuttonadd();
	afx_msg void OnToolbarbuttondel();
	afx_msg void OnToolbarbuttonedit();
	afx_msg void OnSelchangedTreeHubdata(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HUBDATADLG_H__D5AB9EB3_9EF2_465F_A7A0_FFB13A2B6795__INCLUDED_)
