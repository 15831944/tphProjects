// OnBoardLayoutPartView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "OnBoardLayoutPartView.h"
#include ".\onboardlayoutpartview.h"
#include "ViewMsg.h"

// COnBoardLayoutPartView

IMPLEMENT_DYNCREATE(COnBoardLayoutPartView, COnBoardLayoutView)

COnBoardLayoutPartView::COnBoardLayoutPartView()
{
}

COnBoardLayoutPartView::~COnBoardLayoutPartView()
{
}

BEGIN_MESSAGE_MAP(COnBoardLayoutPartView, COnBoardLayoutView)
END_MESSAGE_MAP()


// COnBoardLayoutPartView drawing

void COnBoardLayoutPartView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();

	// TODO: add draw code here
}


// COnBoardLayoutPartView diagnostics

#ifdef _DEBUG
void COnBoardLayoutPartView::AssertValid() const
{
	COnBoardLayoutView::AssertValid();
}

void COnBoardLayoutPartView::Dump(CDumpContext& dc) const
{
	COnBoardLayoutView::Dump(dc);
}
#endif //_DEBUG


// COnBoardLayoutPartView message handlers

void COnBoardLayoutPartView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if(this == pSender)
		return;

	if(lHint == VM_ONBOARD_LAYOUT_DEFAULT)
	{
		;
	}
}

