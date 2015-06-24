#pragma once

//#include "..\AirsideReport\AirsideReportCtrl.h"
// CAirsideReportView view

class CAirsideReportView : public CView
{
	DECLARE_DYNCREATE(CAirsideReportView)

protected:
	CAirsideReportView();           // protected constructor used by dynamic creation
	virtual ~CAirsideReportView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

//	CAirsideReportCtrl* GetReportWnd() { return &m_wndARCReport;	}

protected:
	//CAirsideReportCtrl m_wndARCReport;

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnReportGraphType(UINT nID);
	afx_msg void OnUpdateReportGraphType(CCmdUI* pCmdUI);
};


