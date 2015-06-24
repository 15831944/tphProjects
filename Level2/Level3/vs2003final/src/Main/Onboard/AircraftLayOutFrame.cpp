// LayOutFrame.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "AircraftLayOutFrame.h"
#include "AircraftLayout3DView.h"
#include "AircraftLayoutTreeView.h"
#include <InputOnBoard/AircraftLayoutEditContext.h>
#include <common/UnitsManager.h>
#include <common/winMsg.h>
#include "../LayoutOverUpToolBar.h"
#include <CommonData/Fallback.h>
#include ".\aircraftlayoutframe.h"
#include "AircraftLayoutEditor.h"
#include "../ACFurnishingShapesManager.h"
#include "OnboardViewMsg.h"

#include "../DlgGetString.h"
#include "../DlgBookmarkedCamerasRender.h"
#include "../TermPlanDoc.h"

// CLayOutFrame
IMPLEMENT_DYNCREATE(CAircraftLayOutFrame, CMDIChildWnd)

static UINT indicators[] =
{
	//ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_3DMOUSEMODE,
	ID_INDICATOR_GRIDSPACING,
	ID_INDICATOR_3DMOUSEPOS,
	ID_INDICATOR_ANIMTIME,
	ID_INDICATOR_FPS
};

CAircraftLayOutFrame::CAircraftLayOutFrame()
{
	m_LayoutView = NULL;
}

CAircraftLayOutFrame::~CAircraftLayOutFrame()
{
	/*if(m_LayoutTool != NULL)
	{
		m_LayoutTool->DestroyWindow() ;
		delete m_LayoutTool ;
		m_LayoutTool = NULL ;
	}*/
	
}

BOOL CAircraftLayOutFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if (!m_wndSplitter1.CreateStatic(this, 1, 2))
	{
		TRACE0("Failed to create splitter window\n");
		return FALSE;
	}

	// Create the first nested splitter window to contain the ...

	if (!m_wndSplitter1.CreateView(0, 0, RUNTIME_CLASS(CAircraftLayoutTreeView),
		CSize(250, 500), pContext))
	{
		TRACE0("Failed to create COnBoardLayoutView\n");
		return FALSE;
	}

	if (!m_wndSplitter1.CreateView(0, 1, RUNTIME_CLASS(CAircraftLayout3DView),
		CSize(750, 500), pContext))
	{
		TRACE0("Failed to create CTreeView\n");
		return FALSE;
	}

	SetActiveView( (CView*) m_wndSplitter1.GetPane(0,1) );
  // CreateLayoutTool() ;
	m_LayoutView = (CAircraftLayout3DView*) m_wndSplitter1.GetPane(0,1) ;
	return TRUE;
}
void CAircraftLayOutFrame::CreateLayoutTool()
{
	/*if(m_LayoutTool == NULL)
	{
		m_LayoutTool = new  CLayoutOverUpToolBar((CTermPlanDoc*)GetActiveDocument() ,this) ;
		m_LayoutTool->Create(CLayoutOverUpToolBar::IDD,this) ;
	} 
	m_LayoutTool->GetWindowRect(&toolrect) ;*/
	CRect toolrect ;


	CRect viewRect ; 
	m_LayoutView->GetWindowRect(&viewRect) ;

	CRect newpos ;
	newpos.top = viewRect.top ;
	newpos.left = viewRect.right - toolrect.Width() ;
	newpos.right = viewRect.right ;
	newpos.bottom = viewRect.top + toolrect.Height() ;

	//m_LayoutTool->MoveWindow(newpos) ;
	//m_LayoutTool->ShowWindow(SW_SHOW) ;
}
void CAircraftLayOutFrame::OnUpdateFrameTitle(BOOL bAddToTitle) 
{
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!

	CDocument* pDocument = GetActiveDocument();
	if(bAddToTitle)
	{	
		CString strTitle = _T("Aircraft Model Layout");	
		this->SetWindowText(strTitle);
	}
}
BEGIN_MESSAGE_MAP(CAircraftLayOutFrame, CMDIChildWnd)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_AIR_SELCHANGED, ONAIRSelChanged)
	ON_MESSAGE(TP_TEMP_FALLBACK,OnDlgFallback)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_3DMOUSEMODE, OnUpdate3dMouseMode)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_3DMOUSEPOS, OnUpdate3dMousePos)
	ON_COMMAND(ID_CHANGETO_SEAT, OnChangetoSeat)
	ON_COMMAND(ID_CHANGETO_STRAGE, OnChangetoStrage)
	ON_COMMAND(ID_CHANGETO_GALLEY, OnChangetoGalley)
	ON_COMMAND(ID_CHANGETO_DOOR, OnChangetoDoor)
	ON_COMMAND(ID_CHANGETO_EMERGENCYEXIT, OnChangetoEmergencyexit)
	ON_COMMAND(ID_CHANGETO_STAIR, OnChangetoStair)
	ON_COMMAND(ID_CHANGETO_ESCALATOR, OnChangetoEscalator)
	ON_COMMAND(ID_CHANGETO_ELEVATOR, OnChangetoElevator)
	ON_COMMAND(ID_VIEW_SAVENAMEDVIEW, OnViewSaveNamedView)
	ON_COMMAND(ID_VIEW_MANAGESAVEDVIEWS, OnManageBMViewpoints)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NAMEDVIEWS1, OnUpdateViewNamedViews3D)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NAMEDVIEWS9, OnUpdateViewNamedViews2D)
	ON_COMMAND(ID_VIEW_3DTERMINALVIEW, OnView3dterminalview)
	ON_COMMAND(ID_VIEW_2DTERMINALVIEW, OnView2dterminalview)
	ON_COMMAND(ID_VIEW_OPENDEFAULT3D, OnView3DDefaultView)
	ON_COMMAND(ID_VIEW_OPENDEFAULT2D, OnView2DDefaultView)
	ON_COMMAND_EX_RANGE(ID_VIEW_NAMEDVIEWS1, ID_VIEW_NAMEDVIEWS8, OnViewNamedViews3D)
	ON_COMMAND_EX_RANGE(ID_VIEW_NAMEDVIEWS9, ID_VIEW_NAMEDVIEWS16, OnViewNamedViews2D)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

void CAircraftLayOutFrame::OnDestroy()
{
	CMDIChildWnd::OnDestroy();
    // ((CTermPlanDoc*)GetActiveDocument())->GetInputOnboard()->GetAircraftLayOutEditContext()->SetEditLayOut(NULL) ;
	// TODO: Add your message handler code here

}

class CAircraftFurnishing;
LRESULT CAircraftLayOutFrame::ONAIRSelChanged( WPARAM wParam,LPARAM lParam )
{
	if(wParam != NULL)
	{
		CAircraftFurnishing* pSelShape = (CAircraftFurnishing*)(wParam);
		ASSERT(pSelShape != NULL);
		HANDLE hData = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(int));
		int* pShapeInfo = (int*) ::GlobalLock(hData);
		*pShapeInfo = (int)pSelShape;
		
		::GlobalUnlock(hData);

		COleDataSource ods;
		//UINT nFormat = ((CTermPlanApp*) AfxGetApp())->GetClipboardFormat();
		//ods.CacheGlobalData(nFormat, hData);

		// TRACE("Start Drop Effect\n");
		DROPEFFECT de = ods.DoDragDrop(DROPEFFECT_COPY);
	}
	return 0;

}
// CLayOutFrame message handlers
LRESULT CAircraftLayOutFrame::OnDlgFallback(WPARAM wParam, LPARAM lParam)
{
	FallbackReason reason = (FallbackReason) lParam;

	//find and activate the 3d view
	CDocument* pActiveDoc;

	if(!m_LayoutView)
		return FALSE;
	if (!(pActiveDoc = GetActiveDocument()))
		return FALSE;

	//decode the fallback data
	//FallbackData* pData = (FallbackData*) wParam;
	//m_LayoutView->m_hFallbackWnd = (HWND) pData->hSourceWnd;
	//m_LayoutView->m_lFallbackParam = lParam;

	//CAircraftLayoutView::EMouseState mousestate;

	//switch(reason)
	//{
	//case PICK_ONEPOINT:
	//	mousestate = CAircraftLayoutView::_getonepoint;
	//	break;
	//case PICK_TWOPOINTS:
	//	mousestate = CAircraftLayoutView::_gettwopoints;
	//	break;
	//case PICK_MANYPOINTS:
	//	mousestate = CAircraftLayoutView::_getmanypoints;
	//	break;
	//case PICK_ONERECT:
	//	mousestate = CAircraftLayoutView::_getonerect;
	//	break;
	//case PICK_GATE:
	//	mousestate=CAircraftLayoutView::_pickgate;
	//	break;
	//case PICK_PROC:
	//	mousestate=CAircraftLayoutView::_pickproc;
	//	break;
	//case SELECT_LANDSIDEPROC:
	//	mousestate = CAircraftLayoutView::_selectlandproc;
	//	break;
	//case PICK_STRETCH_POS:
	//	mousestate = CAircraftLayoutView::_pickstretchpos;
	//	break;
	//default:
	//	ASSERT(FALSE);
	//	return FALSE;
	//};

	//m_LayoutView->m_eMouseState = mousestate;
	//m_LayoutView->m_vMousePosList.clear();
	return TRUE;
}
int CAircraftLayOutFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
void CAircraftLayOutFrame::OnUpdate3dMousePos(CCmdUI *pCmdUI)
{

	CWnd* pWnd = m_wndSplitter1.GetActivePane(NULL, NULL);
	if(pWnd==NULL)
		return;

	if( pWnd->IsKindOf(RUNTIME_CLASS(CAircraftLayout3DView) ) )
	{
		CAircraftLayout3DView* pView = (CAircraftLayout3DView*)pWnd;
		ARCVector3 mousePt;

		CString s;
		if (pView->GetViewPortMousePos(mousePt))
		{
			CUnitsManager* pUM = CUnitsManager::GetInstance();
			s.Format("\tX: %5.2f | Y: %5.2f | Z: %5.2f %s",
				pUM->ConvertLength( mousePt[VX] ),
				pUM->ConvertLength( mousePt[VY] ),
				pUM->ConvertLength( mousePt[VZ] ),
				pUM->GetLengthUnitString(pUM->GetLengthUnits()));
		}

		pCmdUI->Enable(TRUE);
		pCmdUI->SetText( s );
	}
	else
		pCmdUI->Enable(FALSE);
}

void CAircraftLayOutFrame::OnUpdate3dMouseMode(CCmdUI* pCmdUI)
{

	CWnd* pWnd = m_wndSplitter1.GetActivePane(NULL, NULL);
	if(pWnd==NULL)
		return;
	if(pWnd->IsKindOf(RUNTIME_CLASS(CAircraftLayout3DView)) == FALSE)
		return;

	
	CAircraftLayout3DView* pAircraft3DView = (CAircraftLayout3DView*) pWnd;
	if(pAircraft3DView) 
	{ // if its valid
		CString s(_T(""));
		pCmdUI->Enable(TRUE);
		//switch(pAircraft3DView->m_eMouseState)
		//{
		//case CAircraftLayoutView::_floormanip:
		//	s = _T(" Floor Manipulation Mode");
		//	break;
		//case CAircraftLayoutView::_default:
		//	s = _T(" Navigation Mode");
		//	break;
		//case CAircraftLayoutView::_placeproc:
		//	s = _T(" Place Element");
		//	break;
		//case CAircraftLayoutView::_scaleproc:
		//	s = _T(" Scale Element");
		//	break;
		//case CAircraftLayoutView::_scaleprocX:
		//	s = _T(" Scale Element X");
		//	break;
		//case CAircraftLayoutView::_scaleprocY:
		//	s = _T(" Scale Element Y");
		//	break;
		//case CAircraftLayoutView::_scaleprocZ:
		//	s = _T(" Scale Element Z");
		//	break;
		//case CAircraftLayoutView::_rotateproc:
		//	s = _T(" Rotate Element");
		//	break;
		//case CAircraftLayoutView::_rotateairsideobject:
		//	s = _T(" Rotate On Board Object");
		//	break;
		//case CAircraftLayoutView::_getmirrorline:
		//	s = _T(" Draw a line to reflect across");
		//	break;
		//case CAircraftLayoutView::_getarrayline:
		//	s = _T(" Draw a line to array along");
		//	break;
		//case CAircraftLayoutView::_getonepoint:
		//	s = _T(" Select a coordinate");
		//	break;
		//case CAircraftLayoutView::_gettwopoints:
		//	s = _T(" Select two points");
		//	break;
		//case CAircraftLayoutView::_getmanypoints:
		//	s = _T(" Select points, RC when done");
		//	break;
		//case CAircraftLayoutView::_placemarker:
		//	s = _T(" Place alignment marker");
		//	break;
		//case CAircraftLayoutView::_tumble:
		//	s = _T(" Rotate view");
		//	break;
		//case CAircraftLayoutView::_dolly:
		//	s = _T(" Zoom view");
		//	break;
		//case CAircraftLayoutView::_pan:
		//	s = _T(" Pan view");
		//	break;
		//case CAircraftLayoutView::_distancemeasure:
		//	s = _T(" Distance Measurement");
		//}
		pCmdUI->SetText( s );
	}
	else
		pCmdUI->Enable(FALSE);
}
//CTermPlanDoc* CAircraftLayOutFrame::GetDocument() 
//{
//	CDocument* pDoc = GetActiveDocument();
//	ASSERT(GetActiveDocument()->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
//	return (CTermPlanDoc*) GetActiveDocument();
//}

void CAircraftLayOutFrame::OnChangetoSeat()
{
	// TODO: Add your command handler code here
	if(!m_LayoutView)
		return;
	m_LayoutView->GetEditor()->OnChangeUndefinedToSeat(m_LayoutView);
}

void CAircraftLayOutFrame::OnChangetoStrage()
{
	// TODO: Add your command handler code here
	if(!m_LayoutView)
		return;
	m_LayoutView->GetEditor()->OnChangeUndefinedToStorage(m_LayoutView);

}

void CAircraftLayOutFrame::OnChangetoGalley()
{
	// TODO: Add your command handler code here
	if(!m_LayoutView)
		return;
	m_LayoutView->GetEditor()->OnChangeUndefinedToGalley(m_LayoutView);

}

void CAircraftLayOutFrame::OnChangetoDoor()
{
	// TODO: Add your command handler code here
	if(!m_LayoutView)
		return;
	m_LayoutView->GetEditor()->OnChangeUndefinedToDoor(m_LayoutView);

}

void CAircraftLayOutFrame::OnChangetoEmergencyexit()
{
	// TODO: Add your command handler code here
	if(!m_LayoutView)
		return;
	m_LayoutView->GetEditor()->OnChangeUndefinedToEmergencyExit(m_LayoutView);

}

void CAircraftLayOutFrame::OnChangetoStair()
{
	// TODO: Add your command handler code here
	if(!m_LayoutView)
		return;
	m_LayoutView->GetEditor()->OnChangeUndefinedToStair(m_LayoutView);

}

void CAircraftLayOutFrame::OnChangetoEscalator()
{
	// TODO: Add your command handler code here
	if(!m_LayoutView)
		return;
	m_LayoutView->GetEditor()->OnChangeUndefinedToEscaltor(m_LayoutView);

}

void CAircraftLayOutFrame::OnChangetoElevator()
{
	// TODO: Add your command handler code here
	if(!m_LayoutView)
		return;
	m_LayoutView->GetEditor()->OnChangeUndefinedToElevator(m_LayoutView);

}

void CAircraftLayOutFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default
	if (SC_CLOSE == nID)
	{
		// close toolbar
		ACFURNISHINGSHAPESMANAGER.ShowShapesBar(FALSE);
		m_LayoutView->SaveCamerasSettings();
		CDocument* pDoc = GetActiveDocument();
		if (pDoc)
		{
			pDoc->UpdateAllViews(NULL, VM_ONBOARD_LAYOUT_EDITFINISH, (CObject*)m_LayoutView->GetEditor()->GetEditLayOut());
		}
	}

	CMDIChildWnd::OnSysCommand(nID, lParam);
}

void CAircraftLayOutFrame::OnViewSaveNamedView()
{
	CDlgGetString dlg(_T("Enter a name for the new view"));
	if (dlg.DoModal() == IDOK)
	{
		m_LayoutView->AddNewNamedView(dlg.m_sValue);
		m_LayoutView->SaveCamerasSettings();
	}
}
void CAircraftLayOutFrame::OnManageBMViewpoints()
{
	//save cameras data
	C3DViewCameras& cams = m_LayoutView->Get3DViewCameras();
	CDlgBookmarkedCamerasRender dlg(cams, true);
	if(dlg.DoModal() == IDOK)
	{
		cams.m_savedCameras = dlg.m_cams.m_savedCameras;
		m_LayoutView->SaveCamerasSettings();
	}
}

void CAircraftLayOutFrame::OnUpdateViewNamedViews2D(CCmdUI* pCmdUI) 
{
	CTermPlanDoc::LoadSavedRenderCamerasIntoCmdUI(pCmdUI, m_LayoutView->Get3DViewCameras(), CCameraData::parallel);
}

void CAircraftLayOutFrame::OnUpdateViewNamedViews3D(CCmdUI* pCmdUI) 
{
	CTermPlanDoc::LoadSavedRenderCamerasIntoCmdUI(pCmdUI, m_LayoutView->Get3DViewCameras(), CCameraData::perspective);
}
BOOL CAircraftLayOutFrame::OnViewNamedViews2D(UINT nID) 
{
	C3DViewCameras& cams = m_LayoutView->Get3DViewCameras();
	cams.m_b3DMode = false;
	cams.m_userCamera3D = CTermPlanDoc::GetRenderCameraDescByIndex(cams, nID - ID_VIEW_NAMEDVIEWS9, CCameraData::parallel);
	m_LayoutView->LoadCamerasSettings();
	return TRUE;
}

BOOL CAircraftLayOutFrame::OnViewNamedViews3D(UINT nID) 
{
	C3DViewCameras& cams = m_LayoutView->Get3DViewCameras();
	cams.m_b3DMode = true;
	cams.m_userCamera3D = CTermPlanDoc::GetRenderCameraDescByIndex(cams, nID - ID_VIEW_NAMEDVIEWS1, CCameraData::perspective);
	m_LayoutView->LoadCamerasSettings();
	return TRUE;
}



void CAircraftLayOutFrame::OnView2DDefaultView() //opens or activates default 3d view (single pane)
{
	C3DViewCameras& cams = m_LayoutView->Get3DViewCameras();
	cams.m_b3DMode = false;
	cams.m_userCamera2D.InitCameraData(CCameraData::parallel);
	m_LayoutView->LoadCamerasSettings();
}

void CAircraftLayOutFrame::OnView3DDefaultView() //opens or activates default 3d view (single pane)
{
	C3DViewCameras& cams = m_LayoutView->Get3DViewCameras();
	cams.m_b3DMode = true;
	cams.m_userCamera3D.InitCameraData(CCameraData::perspective);
	m_LayoutView->LoadCamerasSettings();
}

void CAircraftLayOutFrame::OnView2dterminalview() //opens 2D user view (working view)
{
	C3DViewCameras& cams = m_LayoutView->Get3DViewCameras();
	cams.m_b3DMode = false;
	m_LayoutView->ReadCamerasSettings();
	m_LayoutView->LoadCamerasSettings();
}

void CAircraftLayOutFrame::OnView3dterminalview() //opens or activates working 3d view
{
	C3DViewCameras& cams = m_LayoutView->Get3DViewCameras();
	cams.m_b3DMode = true;
	m_LayoutView->ReadCamerasSettings();
	m_LayoutView->LoadCamerasSettings();
}

BOOL CAircraftLayOutFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	// !!!
	// Here we should pre-filter the command message, avoid to be handled by CTermplanDoc
	BOOL bHandled = FALSE;
	if (CN_COMMAND == nCode && NULL == pExtra)
	{
		// command
		if (ID_VIEW_MANAGESAVEDVIEWS == nID)
		{
			OnManageBMViewpoints();
			bHandled = TRUE;
		}
	}
	else if (CN_UPDATE_COMMAND_UI == nCode && pExtra)
	{
		// update command UI
		CCmdUI* pCmdUI = (CCmdUI*)pExtra;
		if (ID_VIEW_NAMEDVIEWS1 == nID)
		{
			OnUpdateViewNamedViews3D(pCmdUI);
			bHandled = TRUE;
		}
		else if (ID_VIEW_NAMEDVIEWS9 == nID)
		{
			OnUpdateViewNamedViews2D(pCmdUI);
			bHandled = TRUE;
		}
	}

	if (bHandled)
		return TRUE;

	return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
