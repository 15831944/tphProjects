#include "StdAfx.h"
#include "resource.h"
#include "AirRouteDropWnd.h"
#include "termplan.h"
#include "TermPlanDoc.h"

#define PIPEDROPWND_WIDTH 220
#define PIPEDROPWND_HEIGHT 152
#define PIPEDROPWND_TOOLBAR_HEIGHT 32

static const UINT ID_AIRROUTEDROPWND = 30;

CAirRouteDropWnd::CAirRouteDropWnd()
{
}

CAirRouteDropWnd::~CAirRouteDropWnd()
{
}

BEGIN_MESSAGE_MAP(CAirRouteDropWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_COMMAND(ID_BUT_PIPESHOWALL, OnButPipeshowall)
	ON_COMMAND(ID_BUT_PIPESHOWNO, OnButPipeshowno)
	ON_LBN_SELCHANGE(ID_AIRROUTEDROPWND, OnListSelChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPipeDropWnd message handlers

int CAirRouteDropWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_toolBar.LoadToolBar(IDR_PIPESHOW_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	InitFont(80,"MS Sans Serif");
	if(!m_listBox.Create( LBS_NOTIFY|LBS_HASSTRINGS|WS_CHILD|WS_VISIBLE|WS_BORDER|WS_VSCROLL,
		CRect(0,0,0,0),this, ID_AIRROUTEDROPWND) )
	{
		return false;
	}
	m_listBox.SetFont(&m_Font,TRUE) ;

	return 0;
}

void CAirRouteDropWnd::ShowAirRouteWnd(CPoint& point)
{
	InitListBox();
	m_toolBar.MoveWindow(0,-2,PIPEDROPWND_WIDTH,PIPEDROPWND_TOOLBAR_HEIGHT);
	m_listBox.MoveWindow(2,PIPEDROPWND_TOOLBAR_HEIGHT-4,PIPEDROPWND_WIDTH-6,130);
	SetWindowPos(NULL,point.x,point.y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW );
}

void CAirRouteDropWnd::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);

	if(pNewWnd && pNewWnd->m_hWnd != m_listBox.m_hWnd)
		ShowWindow(SW_HIDE);
}

void CAirRouteDropWnd::InitListBox()
{
	m_listBox.ResetContent();

	//intersect nodes
	{
		int iIndex = m_listBox.InsertString(-1, _T("Taxiway Nodes") );
		m_listBox.SetItemData(iIndex,0);
		m_listBox.SetCheck(iIndex,GetTermPlanDoc()->m_bShowAirsideNodes);
	}
	//intersect nodes names
	{
		int iIndex = m_listBox.InsertString(-1, _T("Taxiway Nodes Names"));
		m_listBox.SetItemData(iIndex,1);
		m_listBox.SetCheck(iIndex,GetTermPlanDoc()->m_bShowAirsideNodeNames);		
	}

	/*std::vector<ns_AirsideInput::AirRoute>& airRouteList = GetTermPlanDoc()->GetAirsieInput().GetAirRoutes().GetAirRoutes();
	int nCount = (int)airRouteList.size();
	for (int i = 0; i < nCount; i++)
	{
		ns_AirsideInput::AirRoute& airRoute = airRouteList.at(i);

		int iIndex = m_listBox.InsertString(-1, airRoute.GetName());
		m_listBox.SetItemData(iIndex, i);
		m_listBox.SetCheck(iIndex, airRoute.getDispProperties().bDisplay[ARDP_DISP_SHAPE]);
	}*/
}

void CAirRouteDropWnd::InitFont(int nPointSize, LPCTSTR lpszFaceName)
{
	m_Font.DeleteObject(); 
	m_Font.CreatePointFont (nPointSize,lpszFaceName);
}

void CAirRouteDropWnd::OnButPipeshowall() 
{

	{
		BOOL bSel = (BOOL)m_listBox.GetCheck(0);
		GetTermPlanDoc()->m_bShowAirsideNodes = TRUE;
	}
	{
		BOOL bSel = (BOOL)m_listBox.GetCheck(1);
		GetTermPlanDoc()->m_bShowAirsideNodeNames = TRUE;
	}
	/*std::vector<ns_AirsideInput::AirRoute>& airRouteList = GetTermPlanDoc()->GetAirsieInput().GetAirRoutes().GetAirRoutes();
	int nCount = (int)airRouteList.size();
	for (int i = 0; i < nCount; i++)
	{
		ns_AirsideInput::AirRoute& airRoute = airRouteList.at(i);
		airRoute.getDispProperties().bDisplay[ARDP_DISP_SHAPE] = TRUE;
	}*/

	InitListBox();
	GetTermPlanDoc()->UpdateAllViews(NULL);
}

void CAirRouteDropWnd::OnButPipeshowno() 
{
	
	{
		BOOL bSel = (BOOL)m_listBox.GetCheck(0);
		GetTermPlanDoc()->m_bShowAirsideNodes = FALSE;
	}
	{
		BOOL bSel = (BOOL)m_listBox.GetCheck(1);
		GetTermPlanDoc()->m_bShowAirsideNodeNames = FALSE;
	}
	/*std::vector<ns_AirsideInput::AirRoute>& airRouteList = GetTermPlanDoc()->GetAirsieInput().GetAirRoutes().GetAirRoutes();
	int nCount = (int)airRouteList.size();
	for (int i = 0; i < nCount; i++)
	{
		ns_AirsideInput::AirRoute& airRoute = airRouteList.at(i);
		airRoute.getDispProperties().bDisplay[ARDP_DISP_SHAPE] = FALSE;
	}*/

	InitListBox();
	GetTermPlanDoc()->UpdateAllViews(NULL);
}

void CAirRouteDropWnd::OnListSelChanged()
{
	
	{
		BOOL bSel = (BOOL)m_listBox.GetCheck(0);
		GetTermPlanDoc()->m_bShowAirsideNodes = bSel;
	}
	{
		BOOL bSel = (BOOL)m_listBox.GetCheck(1);
		GetTermPlanDoc()->m_bShowAirsideNodeNames = bSel;
	}
	/*std::vector<ns_AirsideInput::AirRoute>& airRouteList = GetTermPlanDoc()->GetAirsieInput().GetAirRoutes().GetAirRoutes();
	int nCount = (int)airRouteList.size();
	for (int i = 0; i < nCount; i++)
	{
		BOOL bSel = (BOOL)m_listBox.GetCheck(i);
		ns_AirsideInput::AirRoute& airRoute = airRouteList.at(i);

		if( bSel != airRoute.getDispProperties().bDisplay[ARDP_DISP_SHAPE])
		{
			airRoute.getDispProperties().bDisplay[ARDP_DISP_SHAPE] = bSel;
		}
	}*/

	GetTermPlanDoc()->UpdateAllViews(NULL);
}

CTermPlanDoc* CAirRouteDropWnd::GetTermPlanDoc()
{
	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );

	return pDoc;
}
