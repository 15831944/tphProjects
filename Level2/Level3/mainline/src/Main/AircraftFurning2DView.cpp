// AircraftFurning2DView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "AircraftFurning2DView.h"


// CAircraftFurning2DView

IMPLEMENT_DYNCREATE(CAircraftFurning2DView, CView)

CAircraftFurning2DView::CAircraftFurning2DView()
{
}

CAircraftFurning2DView::~CAircraftFurning2DView()
{
}

BEGIN_MESSAGE_MAP(CAircraftFurning2DView, CView)
END_MESSAGE_MAP()


// CAircraftFurning2DView drawing

void CAircraftFurning2DView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CAircraftFurning2DView diagnostics

#ifdef _DEBUG
void CAircraftFurning2DView::AssertValid() const
{
	CView::AssertValid();
}

void CAircraftFurning2DView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CAircraftFurning2DView message handlers
