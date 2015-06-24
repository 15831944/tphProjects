// LayOutFrame.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "LayOutFrame.h"
#include "LayoutView.h"
#include "OnBoard/AircraftLayoutView.h"
// CLayOutFrame
#include "LayoutOverUpToolBar.h"
#include "../common/WinMsg.h"
#include <Common/Fallback.h>
#include "TermPlanDoc.h"
#include "../InputOnBoard/CInputOnboard.h"
#include "../InputOnBoard/AircraftLayoutEditContext.h"
#include ".\layoutframe.h"
#include "../InputAirside/ARCUnitManager.h"
#include "../common/UnitsManager.h"
#include <InputOnBoard/AircaftLayOut.h>
#include <Common/OleDragDropManager.h>
#include "onboard/AircraftLayoutEditor.h"

IMPLEMENT_DYNCREATE(CLayOutFrame, CMDIChildWnd)


static UINT indicators[] =
{
	//ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_3DMOUSEMODE,
		ID_INDICATOR_GRIDSPACING,
		ID_INDICATOR_3DMOUSEPOS,
		ID_INDICATOR_ANIMTIME,
		ID_INDICATOR_FPS
};

CLayOutFrame::CLayOutFrame():m_LayoutTool(NULL)
{
}

CLayOutFrame::~CLayOutFrame()
{
	if(m_LayoutTool != NULL)
	{
		m_LayoutTool->DestroyWindow() ;
		delete m_LayoutTool ;
		m_LayoutTool = NULL ;
	}
	
}

BOOL CLayOutFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if (!m_wndSplitter1.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to create splitter window\n");
		return FALSE;
	}

	// Create the first nested splitter window to contain the ...

	if (!m_wndSplitter1.CreateView(0, 0, RUNTIME_CLASS(CLayoutView),
		CSize(250, 500), pContext))
	{
		TRACE0("Failed to create COnBoardLayoutView\n");
		return FALSE;
	}

	if (!m_wndSplitter1.CreateView(0, 1, RUNTIME_CLASS(CAircraftLayoutView),
		CSize(750, 500), pContext))
	{
		TRACE0("Failed to create CTreeView\n");
		return FALSE;
	}

	SetActiveView( (CView*) m_wndSplitter1.GetPane(0,1) );
  // CreateLayoutTool() ;
	m_LayoutView = (CAircraftLayoutView*) m_wndSplitter1.GetPane(0,1) ;
	return TRUE;
}
void CLayOutFrame::CreateLayoutTool()
{
	if(m_LayoutTool == NULL)
	{
		m_LayoutTool = new  CLayoutOverUpToolBar((CTermPlanDoc*)GetActiveDocument() ,this) ;
		m_LayoutTool->Create(CLayoutOverUpToolBar::IDD,this) ;
	} 
	CRect toolrect ;
	m_LayoutTool->GetWindowRect(&toolrect) ;


	CRect viewRect ; 
	m_LayoutView->GetWindowRect(&viewRect) ;

	CRect newpos ;
	newpos.top = viewRect.top ;
	newpos.left = viewRect.right - toolrect.Width() ;
	newpos.right = viewRect.right ;
	newpos.bottom = viewRect.top + toolrect.Height() ;

	m_LayoutTool->MoveWindow(newpos) ;
	m_LayoutTool->ShowWindow(SW_SHOW) ;
}
void CLayOutFrame::OnUpdateFrameTitle(BOOL bAddToTitle) 
{
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!

	CDocument* pDocument = GetActiveDocument();
	if(bAddToTitle)
	{
		TCHAR szText[256+_MAX_PATH];
		if (pDocument == NULL)
			lstrcpy(szText, m_strTitle);
		else
		{
	
			CAircaftLayOut* _layout =  ((CTermPlanDoc*)pDocument)->GetAircraftLayoutEditor()->GetEditLayOut() ;
			CString LayoutName  ;
			if(_layout != NULL)
				LayoutName = _layout->GetName() ;
			lstrcpy(szText, pDocument->GetTitle() + CString(_T("       Aircraft Model Layout : ")) + LayoutName);
		}
		this->SetWindowText(szText);
	}
}
BEGIN_MESSAGE_MAP(CLayOutFrame, CMDIChildWnd)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_AIR_SELCHANGED, ONAIRSelChanged)
	ON_MESSAGE(TP_TEMP_FALLBACK,OnDlgFallback)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_3DMOUSEMODE, OnUpdate3dMouseMode)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_3DMOUSEPOS, OnUpdate3dMousePos)
END_MESSAGE_MAP()

void CLayOutFrame::OnDestroy()
{
	CMDIChildWnd::OnDestroy();
     ((CTermPlanDoc*)GetActiveDocument())->GetInputOnboard()->GetAircraftLayOutEditContext()->SetEditLayOut(NULL) ;
	// TODO: Add your message handler code here

}
class CAircraftFurnishing;
LRESULT CLayOutFrame::ONAIRSelChanged( WPARAM wParam,LPARAM lParam )
{
	if(wParam != NULL)
	{
		CAircraftFurnishing* pSelShape = (CAircraftFurnishing*)(wParam);
		ASSERT(pSelShape != NULL);

		OleDragDropManager::GetInstance().SetDragDropData(DragDropDataItem::Type_AircraftFurnishing, &pSelShape, sizeof(CAircraftFurnishing*));
	}
	return 0;

}
// CLayOutFrame message handlers
LRESULT CLayOutFrame::OnDlgFallback(WPARAM wParam, LPARAM lParam)
{
	FallbackReason reason = (FallbackReason) lParam;

	//find and activate the 3d view
	CDocument* pActiveDoc;

	if(!m_LayoutView)
		return FALSE;
	if (!(pActiveDoc = GetActiveDocument()))
		return FALSE;

	//decode the fallback data
	FallbackData* pData = (FallbackData*) wParam;
	m_LayoutView->m_hFallbackWnd = (HWND) pData->hSourceWnd;
	m_LayoutView->m_lFallbackParam = lParam;

	CAircraftLayoutView::EMouseState mousestate;

	switch(reason)
	{
	case PICK_ONEPOINT:
		mousestate = CAircraftLayoutView::_getonepoint;
		break;
	case PICK_TWOPOINTS:
		mousestate = CAircraftLayoutView::_gettwopoints;
		break;
	case PICK_MANYPOINTS:
		mousestate = CAircraftLayoutView::_getmanypoints;
		break;
	case PICK_ONERECT:
		mousestate = CAircraftLayoutView::_getonerect;
		break;
	case PICK_GATE:
		mousestate=CAircraftLayoutView::_pickgate;
		break;
	case PICK_PROC:
		mousestate=CAircraftLayoutView::_pickproc;
		break;
	case SELECT_LANDSIDEPROC:
		mousestate = CAircraftLayoutView::_selectlandproc;
		break;
	case PICK_STRETCH_POS:
		mousestate = CAircraftLayoutView::_pickstretchpos;
		break;
	default:
		ASSERT(FALSE);
		return FALSE;
	};

	m_LayoutView->m_eMouseState = mousestate;
	m_LayoutView->m_vMousePosList.clear();
	return TRUE;
}
int CLayOutFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetPaneInfo(3, ID_INDICATOR_ANIMTIME, SBPS_NOBORDERS, 110);
	m_wndStatusBar.SetPaneInfo( 0, ID_INDICATOR_3DMOUSEMODE, SBPS_STRETCH | SBPS_NOBORDERS, 0 );
	return 0;
}
void CLayOutFrame::OnUpdate3dMousePos(CCmdUI *pCmdUI)
{

	CWnd* pWnd = m_wndSplitter1.GetActivePane(NULL, NULL);
	if(pWnd==NULL)
		return;
	if(pWnd->IsKindOf(RUNTIME_CLASS(CAircraftLayoutView)) == FALSE)
		return;

	CAircraftLayoutView* pAircraft3DView = (CAircraftLayoutView*) pWnd;


	if(pAircraft3DView) { // if its valid
		CString s(_T(""));
		CUnitsManager* pUM = CUnitsManager::GetInstance();
		CTermPlanDoc* pDoc = (CTermPlanDoc *)GetDocument();
		//if(pDoc && pDoc->m_systemMode == EnvMode_AirSide)
		//{
		//	s.Format("\tX: %5.2f %s | Y:%5.2f %s",\
		//	CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),pAircraft3DView->GetMousePos()[VX]),\
		//	CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
		//	CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),pAircraft3DView->GetMousePos()[VY]),\
		//	CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()));
		//}
		//else
		{
			s.Format("\tX: %5.2f %s | Y:%5.2f %s",\
			pUM->ConvertLength(pAircraft3DView->GetMousePos()[VX]),\
			pUM->GetLengthUnitString(pUM->GetLengthUnits()),\
			pUM->ConvertLength(pAircraft3DView->GetMousePos()[VY]),\
			pUM->GetLengthUnitString(pUM->GetLengthUnits()));
		}
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText( s );
	}
	else {
		pCmdUI->Enable(FALSE);
	}
}

void CLayOutFrame::OnUpdate3dMouseMode(CCmdUI* pCmdUI)
{

	CWnd* pWnd = m_wndSplitter1.GetActivePane(NULL, NULL);
	if(pWnd==NULL)
		return;
	if(pWnd->IsKindOf(RUNTIME_CLASS(CAircraftLayoutView)) == FALSE)
		return;

	
	CAircraftLayoutView* pAircraft3DView = (CAircraftLayoutView*) pWnd;
	if(pAircraft3DView) 
	{ // if its valid
		CString s(_T(""));
		pCmdUI->Enable(TRUE);
		switch(pAircraft3DView->m_eMouseState)
		{
		case CAircraftLayoutView::_floormanip:
			s = _T(" Floor Manipulation Mode");
			break;
		case CAircraftLayoutView::_default:
			s = _T(" Navigation Mode");
			break;
		case CAircraftLayoutView::_placeproc:
			s = _T(" Place Element");
			break;
		case CAircraftLayoutView::_scaleproc:
			s = _T(" Scale Element");
			break;
		case CAircraftLayoutView::_scaleprocX:
			s = _T(" Scale Element X");
			break;
		case CAircraftLayoutView::_scaleprocY:
			s = _T(" Scale Element Y");
			break;
		case CAircraftLayoutView::_scaleprocZ:
			s = _T(" Scale Element Z");
			break;
		case CAircraftLayoutView::_rotateproc:
			s = _T(" Rotate Element");
			break;
		case CAircraftLayoutView::_rotateairsideobject:
			s = _T(" Rotate On Board Object");
			break;
		case CAircraftLayoutView::_getmirrorline:
			s = _T(" Draw a line to reflect across");
			break;
		case CAircraftLayoutView::_getarrayline:
			s = _T(" Draw a line to array along");
			break;
		case CAircraftLayoutView::_getonepoint:
			s = _T(" Select a coordinate");
			break;
		case CAircraftLayoutView::_gettwopoints:
			s = _T(" Select two points");
			break;
		case CAircraftLayoutView::_getmanypoints:
			s = _T(" Select points, RC when done");
			break;
		case CAircraftLayoutView::_placemarker:
			s = _T(" Place alignment marker");
			break;
		case CAircraftLayoutView::_tumble:
			s = _T(" Rotate view");
			break;
		case CAircraftLayoutView::_dolly:
			s = _T(" Zoom view");
			break;
		case CAircraftLayoutView::_pan:
			s = _T(" Pan view");
			break;
		case CAircraftLayoutView::_distancemeasure:
			s = _T(" Distance Measurement");
		}
		pCmdUI->SetText( s );
	}
	else
		pCmdUI->Enable(FALSE);
}
CTermPlanDoc* CLayOutFrame::GetDocument() 
{
	CDocument* pDoc = GetActiveDocument();
	ASSERT(GetActiveDocument()->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	return (CTermPlanDoc*) GetActiveDocument();
}
