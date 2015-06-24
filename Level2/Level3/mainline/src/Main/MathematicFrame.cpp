// MathematicFrame.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "MathematicFrame.h"
#include "MathematicView.h"
#include "ProcessFlowView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CMathematicFrame

IMPLEMENT_DYNCREATE(CMathematicFrame, CMDIChildWnd)

CMathematicFrame::CMathematicFrame()
{
}

CMathematicFrame::~CMathematicFrame()
{
}


BEGIN_MESSAGE_MAP(CMathematicFrame, CMDIChildWnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMathematicFrame message handlers
void CMathematicFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
    // Set...
	CDocument* pDoc = GetActiveDocument();
	if(pDoc == NULL)
		return ;

	CView *pView = GetActiveView();
	if(pView == NULL)
		return ;

	if(pView->IsKindOf(RUNTIME_CLASS(CMathematicView)))
	    SetWindowText(pDoc->GetTitle() + "        Modeling Sequence View");
	else if(pView->IsKindOf(RUNTIME_CLASS(CProcessFlowView)))
	    SetWindowText(pDoc->GetTitle() + "        Process Flow View");
}
