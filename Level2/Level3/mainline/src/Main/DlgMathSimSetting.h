#if !defined(AFX_DLGMATHSIMSETTING_H__36223298_61F7_4AA7_863D_DE78C16D8D0E__INCLUDED_)
#define AFX_DLGMATHSIMSETTING_H__36223298_61F7_4AA7_863D_DE78C16D8D0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMathSimSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMathSimSetting dialog

class CDlgMathSimSetting : public CDialog
{
// Construction
public:
	CDlgMathSimSetting(CWnd* pParent = NULL);   // standard constructor
	void set_Interval(int nInterval) { m_nInterval = nInterval; }
	int get_Interval() const { return m_nInterval; }
// Dialog Data
	//{{AFX_DATA(CDlgMathSimSetting)
	enum { IDD = IDD_DIALOG_MATHSIMSETTING };
	CDateTimeCtrl	m_dtInterval;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMathSimSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMathSimSetting)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int m_nInterval; // seconds;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMATHSIMSETTING_H__36223298_61F7_4AA7_863D_DE78C16D8D0E__INCLUDED_)
