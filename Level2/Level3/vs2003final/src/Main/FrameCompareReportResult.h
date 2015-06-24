#if !defined(AFX_FRAMECOMPAREREPORTRESULT_H__ACDC77DA_B5FD_4E0A_8C42_C5F069CE022F__INCLUDED_)
#define AFX_FRAMECOMPAREREPORTRESULT_H__ACDC77DA_B5FD_4E0A_8C42_C5F069CE022F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameCompareReportResult.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrameCompareReportResult frame

class CFrameCompareReportResult : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CFrameCompareReportResult)
protected:
	CFrameCompareReportResult();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameCompareReportResult)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual ~CFrameCompareReportResult();

	// Generated message map functions
	//{{AFX_MSG(CFrameCompareReportResult)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMECOMPAREREPORTRESULT_H__ACDC77DA_B5FD_4E0A_8C42_C5F069CE022F__INCLUDED_)
