// AircraftFurningFrame.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "AircraftFurningFrame.h"

#include "AircraftFurning2DView.h"
#include "AircraftFurning3DView.h"
#include "AircraftFurningCommentView.h"
// CAircraftFurningFrame

IMPLEMENT_DYNCREATE(CAircraftFurningFrame, CMDIChildWnd)
static UINT indicators[] =
{
	ID_INDICATOR_DENSITYPARAMS,
		ID_INDICATOR_ANIMTIME,
};
CAircraftFurningFrame::CAircraftFurningFrame()
{
}

CAircraftFurningFrame::~CAircraftFurningFrame()
{
}


BEGIN_MESSAGE_MAP(CAircraftFurningFrame, CMDIChildWnd)
END_MESSAGE_MAP()

int CAircraftFurningFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_ANIMTIME, SBPS_NOBORDERS, 200);
	m_wndStatusBar.SetPaneInfo( 1, ID_INDICATOR_3DMOUSEMODE, SBPS_STRETCH | SBPS_NOBORDERS, 50 );


	return 0;
}

BOOL CAircraftFurningFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	//return m_wndSplitter1.Create(this, 2,2, CSize(10,10), pContext, WS_CHILD | WS_VISIBLE | SPLS_DYNAMIC_SPLIT);

	// Create the splitter window with two columns
	if (!m_wndSplitter1.CreateStatic(this, 2, 1))
	{
		TRACE0("Failed to create splitter window\n");
		return FALSE;
	}

	// Create the first nested splitter window to contain the ...

	if (!m_wndSplitter2.CreateStatic(&m_wndSplitter1, 1, 2, 
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		m_wndSplitter1.IdFromRowCol(1, 0)))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}


	if (!m_wndSplitter1.CreateView(0, 0, RUNTIME_CLASS(CAircraftFurning3DView),
		CSize(500, 500), pContext))
	{
		TRACE0("Failed to create COnBoardLayoutView\n");
		return FALSE;
	}

	if (!m_wndSplitter2.CreateView(0, 0, RUNTIME_CLASS(CAircraftFurning2DView),
		CSize(500, 500), pContext))
	{
		TRACE0("Failed to create CTreeView\n");
		return FALSE;
	}

	if (!m_wndSplitter2.CreateView(0, 1, RUNTIME_CLASS(CAircraftFurningCommentView),
		CSize(500, 500), pContext))
	{
		TRACE0("Failed to create CTreeView\n");
		return FALSE;
	}

	SetActiveView( (CView*) m_wndSplitter1.GetPane(0,0) );

	return TRUE;
}
// CAircraftFurningFrame message handlers
void CAircraftFurningFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CDocument* pDoc = GetActiveDocument();
	if(pDoc == NULL)
		return ;
	CView *pView = GetActiveView();
	if(pView == NULL)
		return ;

	//if(pView->IsKindOf(RUNTIME_CLASS(COnBoardLayoutView)))
	SetWindowText(pDoc->GetTitle() + "Aircraft Furnishing Construction View");

}

void CAircraftFurningFrame::OnDestroy()
{
	CMDIChildWnd::OnDestroy();
	// TODO: Add your message handler code here
	CWnd* pMF=AfxGetMainWnd();

}