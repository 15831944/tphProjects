#if !defined(AFX_CAPITALBDDECISIONDLG_H__D8B6F419_C155_4753_9737_63CCFD975921__INCLUDED_)
#define AFX_CAPITALBDDECISIONDLG_H__D8B6F419_C155_4753_9737_63CCFD975921__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CapitalBDDecisionDlg.h : header file
//

#include "economic\EconomicManager.h"

/////////////////////////////////////////////////////////////////////////////
// CCapitalBDDecisionDlg dialog

class CCapitalBDDecisionDlg : public CDialog
{
// Construction
public:
	CCapitalBDDecisionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCapitalBDDecisionDlg)
	enum { IDD = IDD_DIALOG_CAPTBDDECISIONS };
	CEdit	m_editDiscountRate;
	CButton	m_btnSave;
	CSpinButtonCtrl	m_spinProjLife;
	CSpinButtonCtrl	m_spinDiscRate;
	int		m_nDiscountRate;
	int		m_nProjectLife;
	int		m_nDiscOptions;
	CEdit m_editProjectLife;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCapitalBDDecisionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void LoadDatabase();
	CEconomicManager* GetEconomicManager();
	CString GetProjPath();

	// Generated message map functions
	//{{AFX_MSG(CCapitalBDDecisionDlg)
	afx_msg void OnDeltaposSpinDiscrate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinLifeproj(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnRadioDiscountrate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPITALBDDECISIONDLG_H__D8B6F419_C155_4753_9737_63CCFD975921__INCLUDED_)
