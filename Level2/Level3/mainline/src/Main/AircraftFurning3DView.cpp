// AircraftFurning3DView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "AircraftFurning3DView.h"


// CAircraftFurning3DView

IMPLEMENT_DYNCREATE(CAircraftFurning3DView, CView)

CAircraftFurning3DView::CAircraftFurning3DView()
{
}

CAircraftFurning3DView::~CAircraftFurning3DView()
{
}

BEGIN_MESSAGE_MAP(CAircraftFurning3DView, CView)
END_MESSAGE_MAP()


// CAircraftFurning3DView drawing

void CAircraftFurning3DView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CAircraftFurning3DView diagnostics

#ifdef _DEBUG
void CAircraftFurning3DView::AssertValid() const
{
	CView::AssertValid();
}

void CAircraftFurning3DView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CAircraftFurning3DView message handlers
