// OnBoardLayoutView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "OnBoardLayoutView.h"
#include ".\onboardlayoutview.h"


// COnBoardLayoutView

IMPLEMENT_DYNCREATE(COnBoardLayoutView, CView)

COnBoardLayoutView::COnBoardLayoutView()
{
}

COnBoardLayoutView::~COnBoardLayoutView()
{
}

BEGIN_MESSAGE_MAP(COnBoardLayoutView, CView)
END_MESSAGE_MAP()


// COnBoardLayoutView drawing

void COnBoardLayoutView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// COnBoardLayoutView diagnostics

#ifdef _DEBUG
void COnBoardLayoutView::AssertValid() const
{
	CView::AssertValid();
}

void COnBoardLayoutView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// COnBoardLayoutView message handlers

void COnBoardLayoutView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
}
