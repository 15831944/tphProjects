#pragma once
#include "..\compare\CmpReportCtrl.h"

// CCompareReportView view

class CCompareReportView : public CView
{
	DECLARE_DYNCREATE(CCompareReportView)

protected:
	CCompareReportView();
	virtual ~CCompareReportView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CCmpReportCtrl* GetReportWnd() { return &m_wndCmpReport; }

protected:
	CCmpReportCtrl m_wndCmpReport;

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnReportGraphType(UINT nID);
	afx_msg void OnUpdateReportGraphType(CCmdUI* pCmdUI);
};