#if !defined(AFX_MATHPROCESSDEFINE_H__D63A1321_F100_4B35_B2C8_BB2ED121065F__INCLUDED_)
#define AFX_MATHPROCESSDEFINE_H__D63A1321_F100_4B35_B2C8_BB2ED121065F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MathProcessDefine.h : header file
//
#include <vector>
#include "MathProcess.h"


/////////////////////////////////////////////////////////////////////////////
// CMathProcessDefine dialog

class CMathProcessDefine : public CDialog
{
// Construction
public:
	void SetMathProcess(const CMathProcess& proc);
	CMathProcessDefine(CWnd* pParent = NULL);   // standard constructor

	const CMathProcess& GetMathProcess() const
	{
		return m_mathProcess;
	}
	void SetInTerminal(Terminal* pTerminal)
	{
		m_pInTerm = pTerminal;
	}
	void SetProcessVectorPtr(const std::vector<CMathProcess>* pProcessVector)
	{
		if(pProcessVector)
			m_pProcessVector = pProcessVector;
	}

// Dialog Data
	//{{AFX_DATA(CMathProcessDefine)
	enum { IDD = IDD_DIALOG_MATH_PROCESS_DEFINE };
	CButton	m_chkCapacity;
	CComboBox	m_cmbProcDist;
	CSpinButtonCtrl	m_spnCapacity;
	CSpinButtonCtrl	m_spnProcCount;
	CComboBox	m_cmbQConfig;
	CComboBox	m_cmbQDiscipline;
	CString	m_strName;
	int		m_nProcCount;
	int		m_nCapacity;
//	ProbabilityDistribution* m_pProbDist;
	CString		m_strProcDist;
	BOOL	m_bCapacity;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMathProcessDefine)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitProbDistComboBox();
	Terminal* m_pInTerm;
	BOOL CheckData();
	CMathProcess m_mathProcess;
	void InitComboBox();
	const std::vector<CMathProcess>* m_pProcessVector;
	// Generated message map functions
	//{{AFX_MSG(CMathProcessDefine)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDeltaposSpinProcCount(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinCapacity(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeProcDist();
	afx_msg void OnCheckCapacity();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	bool CheckProcDistData();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MATHPROCESSDEFINE_H__D63A1321_F100_4B35_B2C8_BB2ED121065F__INCLUDED_)
