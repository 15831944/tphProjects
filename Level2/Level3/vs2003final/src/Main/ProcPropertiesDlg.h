#if !defined(AFX_PROCPROPERTIESDLG_H__A6440A59_63E9_11D4_933B_0080C8F982B1__INCLUDED_)
#define AFX_PROCPROPERTIESDLG_H__A6440A59_63E9_11D4_933B_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcPropertiesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgProcProperties dialog
#include "engine\Process.h"

class CTermPlanDoc;

class CDlgProcProperties : public CDialog
{
// Construction
public:
	CDlgProcProperties(Processor* pProc, CTermPlanDoc* pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgProcProperties)
	enum { IDD = IDD_PROCPROPERTIES2 };
	CComboBox	m_cmbType;
	int		m_iQueueType;
	BOOL	m_bUseQueue;
	BOOL	m_bUseServLoc;
	CString	m_sInC;
	BOOL	m_bUseInC;
	BOOL	m_bUseOutC;
	CString	m_sOutC;
	CString	m_sQueue;
	CString	m_sServLoc;
	CString	m_sFloorName;
	CString	m_sProcName;
	//}}AFX_DATA

protected:
	Processor* m_pProcessor;
	Path m_inC;
	Path m_outC;
	Path m_queue;
	Path m_servLoc;

	CTermPlanDoc* m_pDoc;
	CWnd* m_pParent;
	
	Path* m_pPathToSet;
	CString* m_pStrToSet;

private:
	void SetMappedVars();
	void EnableTypeDepControls();
	void EnableServLocControls(BOOL bEnable = TRUE);
	void EnableInCControls(BOOL bEnable = TRUE);
	void EnableOutCControls(BOOL bEnable = TRUE);
	void EnableQueueControls(BOOL bEnable = TRUE);
	void EnableIsQFixedControls(BOOL bEnable = TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProcProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	int DoFakeModal();
protected:
	BOOL UpdateProcData();
	virtual int DoModal(); // do not allow this to be called by others

	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);

	// Generated message map functions
	//{{AFX_MSG(CDlgProcProperties)
	afx_msg void OnChkUseServLoc();
	afx_msg void OnSelChangeCmbType();
	virtual void OnOK();
	afx_msg void OnPFMServLoc();
	afx_msg void OnPFMInC();
	afx_msg void OnPFMOutC();
	afx_msg void OnPFMQueue();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCPROPERTIESDLG_H__A6440A59_63E9_11D4_933B_0080C8F982B1__INCLUDED_)
