#if !defined(AFX_DLGRECORDOPTIONS_H__B849065B_2EE6_4B4F_8EC3_8208C50C5F21__INCLUDED_)
#define AFX_DLGRECORDOPTIONS_H__B849065B_2EE6_4B4F_8EC3_8208C50C5F21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRecordOptions.h : header file
//

class IHyperCam;
/////////////////////////////////////////////////////////////////////////////
// CDlgRecordOptions dialog

class CDlgRecordOptions : public CDialog
{
// Construction
public:
	CDlgRecordOptions(IHyperCam* pHyperCam, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRecordOptions)
	enum { IDD = IDD_DIALOG_RECORDOPTIONS };
	CComboBox	m_cmbCompressor;
	CSpinButtonCtrl	m_spinQuality;
	int		m_nCompressorIdx;
	int		m_nFpsPB;
	int		m_nQuality;
	int		m_nFpsRec;
	BOOL	m_bRecMouse;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRecordOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	IHyperCam* m_pHyperCam;

	// Generated message map functions
	//{{AFX_MSG(CDlgRecordOptions)
	afx_msg void OnButtonCompressorConfig();
	afx_msg void OnDeltaposSpinQuality(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnKillfocusEditPlaybackrate();
	afx_msg void OnKillfocusEditRecordrate();
	afx_msg void OnKillfocusEditQuality();
	afx_msg void OnCheckRecmouse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRECORDOPTIONS_H__B849065B_2EE6_4B4F_8EC3_8208C50C5F21__INCLUDED_)
