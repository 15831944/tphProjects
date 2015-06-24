#if !defined(AFX_COMPARAREPORTDLG_H__7309354C_BC28_4389_A94E_280A4D03EDAE__INCLUDED_)
#define AFX_COMPARAREPORTDLG_H__7309354C_BC28_4389_A94E_280A4D03EDAE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComparaReportDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComparaReportDlg dialog

class CComparaReportDlg : public CDialog
{
// Construction
private:
	CToolBar m_ToolBar;
public:

	void InitListEx();

	void InitToolBar();
	CComparaReportDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CComparaReportDlg)
	enum { IDD = IDD_DIALOG_COMPARATIVEREPORT };
	CListCtrlEx	m_ListModels;
	CStatic	m_StaticToolBarReports;
	CStatic	m_StaticToolBarModels;
	CTreeCtrl	m_TreeCtlReport;
	CString	m_sDiscription;
	CString	m_sName;
	CString	m_sReport;
	CString	m_sStatus;
	CString	m_sModel;
	int		m_nProcessor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComparaReportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementatio
		
protected:

	// Generated message map functions
	//{{AFX_MSG(CComparaReportDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPARAREPORTDLG_H__7309354C_BC28_4389_A94E_280A4D03EDAE__INCLUDED_)
