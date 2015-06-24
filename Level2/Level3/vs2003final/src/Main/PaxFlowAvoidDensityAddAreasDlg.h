#if !defined(AFX_PAXFLOWAVOIDDENSITYADDAREASDLG_H__ED325AAE_7514_4D97_895A_7181B7156A34__INCLUDED_)
#define AFX_PAXFLOWAVOIDDENSITYADDAREASDLG_H__ED325AAE_7514_4D97_895A_7181B7156A34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaxFlowAvoidDensityAddAreasDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPaxFlowAvoidDensityAddAreasDlg dialog
#include "..\inputs\AreasPortals.h"

class CPaxFlowAvoidDensityAddAreasDlg : public CDialog
{
// Construction
public:
	CPaxFlowAvoidDensityAddAreasDlg(CAreaList* vAreas,CWnd* pParent = NULL);   // standard constructor
	CAreaList* m_vAreas;
	CStringArray m_strArraySeledAreas;
// Dialog Data
	//{{AFX_DATA(CPaxFlowAvoidDensityAddAreasDlg)
	enum { IDD = IDD_PAXFLOW_AVOID_DENSITY_ADD_AREA };
	CListBox	m_listBoxArea;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaxFlowAvoidDensityAddAreasDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPaxFlowAvoidDensityAddAreasDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAXFLOWAVOIDDENSITYADDAREASDLG_H__ED325AAE_7514_4D97_895A_7181B7156A34__INCLUDED_)
