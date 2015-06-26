#pragma once


class CCmpReportChildFrameSplit : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CCmpReportChildFrameSplit)
protected:
	CCmpReportChildFrameSplit();           // protected constructor used by dynamic creation

public:
	CSplitterWnd    m_wndSplitter1;		// Horizontal splitter
	CSplitterWnd    m_wndSplitter2;		// Vertical splitter

public:

	// ClassWizard generated virtual function overrides
    virtual void OnUpdateFrameMenu (BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	//{{AFX_VIRTUAL(CReportChildFrameSplit)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

	CDocument* GetDocument() { return (CDocument*) GetActiveDocument(); }

protected:
	virtual ~CCmpReportChildFrameSplit();

	//{{AFX_MSG(CReportChildFrameSplit)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


