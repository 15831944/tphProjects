#if !defined(AFX_PAXFLOWBYPROCDLG_H__EAE93145_79D8_4197_911A_74286ACBFBE0__INCLUDED_)
#define AFX_PAXFLOWBYPROCDLG_H__EAE93145_79D8_4197_911A_74286ACBFBE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxFlowByProcDlg.h : header file
//
#include "inputs\paxflow.h"
#include "PrintableTreeCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CPaxFlowByProcDlg dialog

class CPaxFlowByProcDlg : public CDialog
{
// Construction
public:
	CPaxFlowByProcDlg(CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPaxFlowByProcDlg)
	enum { IDD = IDD_DIALOG_PAXFLOWBYPROC };
	CButton	m_btnCancel;
	CButton	m_btnOk;
	CButton	m_btnNew;
	CButton	m_BarFrame;
	CStatic	m_toolbarcontenter;
	CButton	m_btnSave;
	CPrintableTreeCtrl	m_treeFlow;
	CButton	m_btnDel;
	CListBox	m_listProc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxFlowByProcDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolBar m_ToolBar;
	HTREEITEM m_hRClickItem;
	HTREEITEM m_hModifiedItem;

	void InitToolbar();

	void SetPercent(int _nPercent);

	// general function.
	InputTerminal* GetInputTerminal();
	CString GetProjPath();

	// load processor entry from paxflow database.
	void ReloadProcList();

	// base on the cur selection load the tree.
	void ReloadTree();

	// create string.
	CString GetLabel( const ProcessorID& _procId, int _nPercent );


	void CheckToolBarEnable();


	bool All100Percent();

	bool All100Percent( bool _bStationFlowPaxDB );

	void GetEvenPercent( int _nCount, double *_pProb );

	// input: selection on the list
	// output: database
	CMobileElemConstraintDatabase* GetDBBaseOnSelection( int _nSelIdx );



	// Generated message map functions
	//{{AFX_MSG(CPaxFlowByProcDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListProcessor();
	afx_msg void OnBtnNew();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBtnSave();
	afx_msg void OnBtnDelete();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonEdit();
	afx_msg void OnButtonPrint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaxflowbyprocAdddestprocessor();
	afx_msg void OnPaxflowbyprocAddpassengertype();
	afx_msg void OnPaxflowbyprocDeletepassengertype();
	afx_msg void OnPaxflowbyprocDeleteprocessor();
	afx_msg void OnPaxflowbyprocModifypercent();
	afx_msg void OnSelchangedTreeFlow(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXFLOWBYPROCDLG_H__EAE93145_79D8_4197_911A_74286ACBFBE0__INCLUDED_)
