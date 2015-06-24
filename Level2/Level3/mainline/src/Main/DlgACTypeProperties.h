#include "afxwin.h"
#if !defined(AFX_DLGACTYPEPROPERTIES_H__263B12D8_F9A7_42FC_A785_F51C49CC9C37__INCLUDED_)
#define AFX_DLGACTYPEPROPERTIES_H__263B12D8_F9A7_42FC_A785_F51C49CC9C37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgACTypeProperties.h : header file
//


class CACType;
/////////////////////////////////////////////////////////////////////////////
// CDlgACTypeProperties dialog

class CDlgACTypeProperties : public CPropertyPage
{
// Construction
public:
	CDlgACTypeProperties( CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgACTypeProperties)
	enum { IDD = IDD_ACTYPEPROPERTIES };
	int		m_nCapacity;
	CString	m_sIATACode;
	CString	m_sDesc;
	double	m_dHeight;
	double	m_dLen;
	double	m_dMLW;
	double	m_dMTOW;
	double	m_dMZFW;
	double	m_dOEW;
	double	m_dSpan;
	double m_CabinWidth ;
    CString m_sICAOCode;
    CString m_sOtherCodes;
	CString m_sWVC;
	CString m_sCompany;
	CString m_sGroup;
	CString m_sWeightCategory;
	double  m_dMinTurn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgACTypeProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CACType* m_pACT;

	//intercept decimal from double
	double InterceptDecimal(double fValue, int nSaveNO = 2);
	// Generated message map functions
	//{{AFX_MSG(CDlgACTypeProperties)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void setACType(CACType* pACType);
protected:
	CStatic m_StaticName;
	CEdit m_EditName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGACTYPEPROPERTIES_H__263B12D8_F9A7_42FC_A785_F51C49CC9C37__INCLUDED_)
