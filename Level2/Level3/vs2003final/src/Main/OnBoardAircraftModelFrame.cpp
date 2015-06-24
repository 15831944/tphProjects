#include "StdAfx.h"
#include "../Main/resource.h"
#include ".\onboardaircraftmodelframe.h"
#include "Onboard/AircraftModelDatabaseView.h"
#include ".\MainBarUpdateArgs.h"
//#include "Onboard/OnBoardAircraftModelComman.h"
#include "TermPlanDoc.h"

static UINT indicators[] =
{
	ID_INDICATOR_DENSITYPARAMS,
	ID_INDICATOR_ANIMTIME,
};

IMPLEMENT_DYNCREATE(COnBoardAircraftModelFrame, CMDIChildWnd)

COnBoardAircraftModelFrame::COnBoardAircraftModelFrame()
:m_pAircraftModelEditView(NULL)
,m_pOnBoardAircraftCommandView(NULL)
{
}

COnBoardAircraftModelFrame::~COnBoardAircraftModelFrame()
{
}

int COnBoardAircraftModelFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

BOOL COnBoardAircraftModelFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	/*return m_wndSplitter1.Create(this, 2,2, CSize(10,10), pContext, WS_CHILD | WS_VISIBLE | SPLS_DYNAMIC_SPLIT);*/

	if (!m_wndSplitter1.CreateStatic(this, 2, 1))
	{
		TRACE0("Failed to create splitter window\n");
		return FALSE;
	}

	// Create the first nested splitter window to contain the ...


	if (!m_wndSplitter1.CreateView(0, 0, RUNTIME_CLASS(CAircraftModelEditView),
		CSize(500, 500), pContext))
	{
		TRACE0("Failed to create COnBoardLayoutView\n");
		return FALSE;
	}

	//if (!m_wndSplitter1.CreateView(1, 0, RUNTIME_CLASS(COnboardAircraftCommanView),
	//	CSize(500, 500), pContext))
	//{
	//	TRACE0("Failed to create CTreeView\n");
	//	return FALSE;
	//}

	SetActiveView( (CView*) m_wndSplitter1.GetPane(0,0) );
	m_pAircraftModelEditView = (CAircraftModelEditView*)m_wndSplitter1.GetPane(0,0);
	m_pOnBoardAircraftCommandView = (COnboardAircraftCommanView*)m_wndSplitter1.GetPane(1,0);

	return TRUE;
}

BEGIN_MESSAGE_MAP(COnBoardAircraftModelFrame, CMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()

END_MESSAGE_MAP()


// COnBoardLayoutFrame message handlers
void COnBoardAircraftModelFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CDocument* pDoc = GetActiveDocument();
	if(pDoc == NULL)
		return ;
	CView *pView = GetActiveView();
	if(pView == NULL)
		return ;

	if(pView->IsKindOf(RUNTIME_CLASS(CAircraftModelEditView)) )
	{
		CAircraftModelEditView* pEditView = (CAircraftModelEditView*)pView;
		pEditView->UpdateFrmTitile();
	}
}

void COnBoardAircraftModelFrame::OnDestroy()
{
	CMDIChildWnd::OnDestroy();

	// TODO: Add your message handler code here
	CWnd* pMF=AfxGetMainWnd();
	ToolBarUpdateParameters updatePara;
	updatePara.m_operatType = ToolBarUpdateParameters::CloseComponentEditView;
	updatePara.m_nCurMode = ((CTermPlanDoc*)GetActiveDocument())->GetCurrentMode();
	pMF->SendMessage(UM_SHOW_CONTROL_BAR,2,(LPARAM)&updatePara);
}

void COnBoardAircraftModelFrame::OnSize(UINT nType, int cx, int cy)
{
//	CMDIChildWnd::OnSize(nType,cx,cy);
	
	if (m_pAircraftModelEditView && m_pOnBoardAircraftCommandView)
	{
//		m_pAircraftModelEditView->MoveWindow(0,0,cx,cy*9/10);
//		m_pOnBoardAircraftCommandView->MoveWindow(0,cy*9/10,cx,cy/10);
		m_wndSplitter1.SetRowInfo(0,cy*9/10,0);
		m_wndSplitter1.SetRowInfo(1,cy/10,0);
	}
	CMDIChildWnd::OnSize(nType,cx,cy);
}


