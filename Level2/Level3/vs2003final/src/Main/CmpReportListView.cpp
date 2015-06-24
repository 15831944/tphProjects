// CmpReportListView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "CmpReportListView.h"


// CCmpReportListView

IMPLEMENT_DYNCREATE(CCmpReportListView, CView)

CCmpReportListView::CCmpReportListView()
{
}

CCmpReportListView::~CCmpReportListView()
{
}

BEGIN_MESSAGE_MAP(CCmpReportListView, CView)
END_MESSAGE_MAP()


// CCmpReportListView drawing

void CCmpReportListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CCmpReportListView diagnostics

#ifdef _DEBUG
void CCmpReportListView::AssertValid() const
{
	CView::AssertValid();
}

void CCmpReportListView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CCmpReportListView message handlers
