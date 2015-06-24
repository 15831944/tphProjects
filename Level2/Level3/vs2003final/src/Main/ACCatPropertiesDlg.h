#if !defined(AFX_ACCATPROPERTIESDLG_H__00F4739D_4145_11D4_930C_0080C8F982B1__INCLUDED_)
#define AFX_ACCATPROPERTIESDLG_H__00F4739D_4145_11D4_930C_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ACCatPropertiesDlg.h : header file
//

#include "..\MFCExControl\ListCtrlEx.h"

class CACCategory;
class CACTypesManager;

/////////////////////////////////////////////////////////////////////////////
// CDlgACCatProperties dialog

class CDlgACCatProperties : public CDialog
{
// Construction
public:
	CDlgACCatProperties(CACTypesManager* _pAcMan, CACCategory* pACC, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgACCatProperties)
	enum { IDD = IDD_ACCATPROPERTIES };
	CString	m_sName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgACCatProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CACCategory*		m_pACC;
	CACTypesManager*	m_pAcMan;
	CListCtrlEx			m_lstCondition;
	void EnableListCtrl(bool bEnable = true);
	void FillListCtrl();
	void InitListCtrl();

	// Generated message map functions
	//{{AFX_MSG(CDlgACCatProperties)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonDel();
	afx_msg void OnNewFilter();
	afx_msg void OnEditFilter();
	afx_msg void OnRemoveFilter();
	afx_msg LRESULT OnFieldCheck( WPARAM wParam, LPARAM lParam);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedScript();
	afx_msg void OnBnClickedNoScript();

private:
	CToolBar m_wndToolBar;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCATPROPERTIESDLG_H__00F4739D_4145_11D4_930C_0080C8F982B1__INCLUDED_)
