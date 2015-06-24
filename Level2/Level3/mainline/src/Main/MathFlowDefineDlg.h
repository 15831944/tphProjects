#if !defined(AFX_MATHFLOWDEFINEDLG_H__91EAC854_1820_4A9F_88C7_89512E413F3A__INCLUDED_)
#define AFX_MATHFLOWDEFINEDLG_H__91EAC854_1820_4A9F_88C7_89512E413F3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MathFlowDefineDlg.h : header file
//
class CMathFlow;
class PassengerTypeList;

/////////////////////////////////////////////////////////////////////////////
// CMathFlowDefineDlg dialog

class CMathFlowDefineDlg : public CDialog
{
// Construction
public:
	CMathFlowDefineDlg(CMathFlow* pMathFlow, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CMathFlowDefineDlg)
	enum { IDD = IDD_DIALOG_MATH_FLOWDEFINE };
	CString	m_strFlowName;
	CString	m_strPaxType;
	//}}AFX_DATA

	void SetMathFlow(CMathFlow* pMathFlow) { m_pMathFlow = pMathFlow; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMathFlowDefineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMathFlowDefineDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBtnSelecttype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CMathFlow* m_pMathFlow;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MATHFLOWDEFINEDLG_H__91EAC854_1820_4A9F_88C7_89512E413F3A__INCLUDED_)
