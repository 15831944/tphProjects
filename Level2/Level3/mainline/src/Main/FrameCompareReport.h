#if !defined(AFX_FRAMECOMPAREREPORT_H__504EB850_E6AE_4EBF_9B65_0287A011C65C__INCLUDED_)
#define AFX_FRAMECOMPAREREPORT_H__504EB850_E6AE_4EBF_9B65_0287A011C65C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameCompareReport.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFrameCompareReport frame

class CFrameCompareReport : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CFrameCompareReport)
protected:
	CFrameCompareReport();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameCompareReport)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual ~CFrameCompareReport();

	// Generated message map functions
	//{{AFX_MSG(CFrameCompareReport)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMECOMPAREREPORT_H__504EB850_E6AE_4EBF_9B65_0287A011C65C__INCLUDED_)
