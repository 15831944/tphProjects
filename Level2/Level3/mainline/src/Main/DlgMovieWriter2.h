#if !defined(AFX_DLGMOVIEWRITER2_H__79144CD9_B904_4122_BFEC_388A91B7AFEA__INCLUDED_)
#define AFX_DLGMOVIEWRITER2_H__79144CD9_B904_4122_BFEC_388A91B7AFEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMovieWriter.h : header file
//
class CTermPlanDoc;
class CMovieWriter;

/////////////////////////////////////////////////////////////////////////////
// CDlgMovieWriter2 dialog

class CDlgMovieWriter2 : public CDialog
{
// Construction
public:
	CDlgMovieWriter2(CTermPlanDoc* pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgMovieWriter2)
	enum { IDD = IDD_DIALOG_MOVIEWRITER };
	CProgressCtrl	m_progressMW;
	CString	m_sText;
	//}}AFX_DATA

	void RecordLive();
	void RecordMovie(int nMovieIdx);
	void RecordWalkthrough(int nWalkthroughIdx);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMovieWriter2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd* m_pParent;
	CTermPlanDoc* m_pDoc;
	BYTE* m_pBits;
	CMovieWriter* m_pWriter;
	BOOL m_bFirstPaint;
	BOOL m_bIsCancel;

	void PumpMessages();

	// Generated message map functions
	//{{AFX_MSG(CDlgMovieWriter2)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMOVIEWRITER2_H__79144CD9_B904_4122_BFEC_388A91B7AFEA__INCLUDED_)
