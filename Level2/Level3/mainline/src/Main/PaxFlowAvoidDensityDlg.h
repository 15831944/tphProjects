#if !defined(AFX_PAXFLOWAVOIDDENSITYDLG_H__232BCEB2_3F31_4040_927F_7FD81CC0BB90__INCLUDED_)
#define AFX_PAXFLOWAVOIDDENSITYDLG_H__232BCEB2_3F31_4040_927F_7FD81CC0BB90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxFlowAvoidDensityDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPaxFlowAvoidDensityDlg dialog
#include "Inputs\FlowDestination.h"
#include "PaxFlowAvoidDensityAddAreasDlg.h"
#include "TermPlanDoc.h"

class CPaxFlowAvoidDensityDlg : public CDialog
{
// Construction
public:
	BOOL CheckIsExist(const CString& name);
	CTermPlanDoc* m_pDoc;
	CPaxFlowAvoidDensityDlg(CTermPlanDoc* pDoc,CAreaList* vAreas,CFlowDestination* arcInfo,CWnd* pParent = NULL);   // standard constructor
	CFlowDestination* m_arcInfo;
	CAreaList* m_vAreas;
	
// Dialog Data
	//{{AFX_DATA(CPaxFlowAvoidDensityDlg)
	enum { IDD = IDD_PAXFLOW_AVOID_DENSITY };
	CListBox	m_listBoxArea;
	CEdit	m_editLimit;
	float	m_fLimit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxFlowAvoidDensityDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPaxFlowAvoidDensityDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonNewArea();
	afx_msg void OnButtonAddArea();
	afx_msg void OnButtonDeleteArea();
	virtual void OnOK();
	//}}AFX_MSG
	afx_msg LRESULT OnDlgAreaPortalShowOrHide(WPARAM wParam,LPARAM lParam);
		
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXFLOWAVOIDDENSITYDLG_H__232BCEB2_3F31_4040_927F_7FD81CC0BB90__INCLUDED_)
