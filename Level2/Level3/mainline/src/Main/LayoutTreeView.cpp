// LayoutTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "LayoutTreeView.h"


// CLayoutTreeView

IMPLEMENT_DYNCREATE(CLayoutTreeView, CTreeView)

CLayoutTreeView::CLayoutTreeView()
{
}

CLayoutTreeView::~CLayoutTreeView()
{
}

BEGIN_MESSAGE_MAP(CLayoutTreeView, CTreeView)
END_MESSAGE_MAP()


// CLayoutTreeView diagnostics

#ifdef _DEBUG
void CLayoutTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLayoutTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG


// CLayoutTreeView message handlers
