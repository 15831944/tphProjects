// CmpReportGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "CmpReportGraphView.h"


// CCmpReportGraphView

IMPLEMENT_DYNCREATE(CCmpReportGraphView, CView)

CCmpReportGraphView::CCmpReportGraphView()
{
}

CCmpReportGraphView::~CCmpReportGraphView()
{
}

BEGIN_MESSAGE_MAP(CCmpReportGraphView, CView)
END_MESSAGE_MAP()


// CCmpReportGraphView drawing

void CCmpReportGraphView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CCmpReportGraphView diagnostics

#ifdef _DEBUG
void CCmpReportGraphView::AssertValid() const
{
	CView::AssertValid();
}

void CCmpReportGraphView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CCmpReportGraphView message handlers
