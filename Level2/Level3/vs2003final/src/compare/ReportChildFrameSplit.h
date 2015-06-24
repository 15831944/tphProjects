#if !defined(AFX_REPORTCHILDFRAMESPLIT_H__8536A6E0_21CC_4F18_A0D5_D7814C830376__INCLUDED_)
#define AFX_REPORTCHILDFRAMESPLIT_H__8536A6E0_21CC_4F18_A0D5_D7814C830376__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportChildFrameSplit.h : header file
//
class CTermPlanDoc;

/////////////////////////////////////////////////////////////////////////////
// CReportChildFrameSplit frame
class CReportChildFrameSplit : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CReportChildFrameSplit)
protected:
	CReportChildFrameSplit();           // protected constructor used by dynamic creation

// Attributes
public:
	CSplitterWnd    m_wndSplitter1;		// Horizontal splitter
	CSplitterWnd    m_wndSplitter2;		// Vertical splitter

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
    virtual void OnUpdateFrameMenu (BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt);
	//{{AFX_VIRTUAL(CReportChildFrameSplit)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

	CTermPlanDoc* GetDocument() { return (CTermPlanDoc*) GetActiveDocument(); }

// Implementation
protected:
	virtual ~CReportChildFrameSplit();

	// Generated message map functions
	//{{AFX_MSG(CReportChildFrameSplit)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTCHILDFRAMESPLIT_H__8536A6E0_21CC_4F18_A0D5_D7814C830376__INCLUDED_)
