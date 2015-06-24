#if !defined(AFX_INPUTREPDLG_H__870096F3_564A_437A_85F8_74D51BB80904__INCLUDED_)
#define AFX_INPUTREPDLG_H__870096F3_564A_437A_85F8_74D51BB80904__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputRepDlg.h : header file
//

#include "TermPlanDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CInputRepDlg dialog

class CInputRepDlg : public CDialog
{
// Construction
public:
	CInputRepDlg(CTermPlanDoc* m_pDoc , CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputRepDlg)
	enum { IDD = IDD_DIALOG_INPUTSUMMARY };
	CListCtrl	m_listRep;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputRepDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	//}}AFX_VIRTUAL

// Implementation
protected:
	CTermPlanDoc* m_pDoc;

protected:
	void LoadDatabase();
	CString GetLevelString( CString _str, int _nLevel );
	CString GetLevelString( double _dVal, int _nLevel );
	void LoadProc(int &nNextIdx);
	void LoadPax(int &nNextIdx);
	void LoadBaggage(int &nNextIdx);
	void LoadFlight(int &nNextIdx);
	void LoadLayout(int &nNextIdx);

	InputTerminal* GetInputTerminal();

	// Generated message map functions
	//{{AFX_MSG(CInputRepDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTREPDLG_H__870096F3_564A_437A_85F8_74D51BB80904__INCLUDED_)
