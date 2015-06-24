#if !defined(AFX_FLIGHTSCHEDULEDLG_H__46782693_3A58_4EA8_8DDF_9D1DDEC579E3__INCLUDED_)
#define AFX_FLIGHTSCHEDULEDLG_H__46782693_3A58_4EA8_8DDF_9D1DDEC579E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlightScheduleDlg.h : header file
//

#include "inputs\flight.h"
#include "../MFCExControl/SortAndPrintListCtrl.h"
#include "inputs\AllPaxTypeFlow.h"
#include "inputs\PaxFlowConvertor.h"

/////////////////////////////////////////////////////////////////////////////
// CFlightScheduleDlg dialog

class CFlightScheduleDlg : public CDialog
{
// Construction
public:
	CWnd* m_pParent;
	void Print();
	CFlightScheduleDlg(CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFlightScheduleDlg)
	enum { IDD = IDD_DIALOG_FLIGHTSCHEDULE };
	CButton	m_btModifySchedule;
	CButton	m_butOK;
	CButton	m_butCancel;
	CButton	m_staticFrame;
	CButton	m_butSaveAs;
	CButton	m_butLoadFrom;
	CStatic	m_toolbarcontenter;
	CButton	m_btnSave;
	CSortAndPrintListCtrl m_listCtrlSchdList;
	//}}AFX_DATA
private:
	//use to Upadate Detail Gate Flow (only temp)
	CPaxFlowConvertor m_paxFlowConvertor;
	CAllPaxTypeFlow m_allFlightFlow;
	BOOL InitFlowConvetor();
	void UpdateFlow();
	int m_iPreSortByColumnIdx;
	///////////////////////////////////////
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlightScheduleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitToolbar();
	CToolBar m_ToolBar;
	CString GetProjPath();
	InputTerminal* GetInputTerminal();
	void LoadFlight( Flight* _pSelectFlight=NULL);
	void InitGridTitle();
	CMultiSortHeaderCtrl	m_ctlHeaderCtrl;
	// Generated message map functions
	//{{AFX_MSG(CFlightScheduleDlg)
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnColumnclickListSchedulelist(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	afx_msg void OnDblclkListSchedulelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickListSchedulelist(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnCancel();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonEdit();
	afx_msg void OnButtonSaveas();
	afx_msg void OnButtonLoadfrom();
	afx_msg void OnButtonPrint();
	afx_msg void OnButtonSave();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnButtonSplit();
	afx_msg void OnButtonModifySchdule();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void SetFlightInListCtrl( int _nIndex, const Flight& _flight );
	void SetFlightSelected( int _nIndex );
	int AppendOneFlightIntoListCtrl( const Flight* _pFlight ,int _nIndex);
	int GetSelIndex();
	// get multi selected items' indices
	// return the items' count
	int GetSelIndices(std::vector<int>& vecIndices);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLIGHTSCHEDULEDLG_H__46782693_3A58_4EA8_8DDF_9D1DDEC579E3__INCLUDED_)
