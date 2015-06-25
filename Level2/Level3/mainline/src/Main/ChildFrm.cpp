// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "ChildFrm.h"

#include "3DCamera.h"
#include "3DView.h"
#include "ChildFrameSplit.h"
#include "DlgGetString.h"
#include "DlgLiveRecordParams.h"
#include "DlgRecordOptions.h"
#include "Floor2.h"
#include "FlowGeometryHelpers.h"
//#include "glh_genext.h"
#include "HyperCam.h"
#include "MainFrm.h"
#include "MSTexture.h"
#include "RunwayTextures.h"
#include "ShapesManager.h"
#include "TermPlan.h"
#include "TermPlanDoc.h"
#include <CommonData/Textures.h>
#include "../common/UnitsManager.h"
#include "../InputAirside/ARCUnitManager.h"
#include "common\ARCPath.h"
#include "engine\MovingSideWalk.h"
#include "inputs\SubFlowList.h"
#include "inputs\PipeDataSet.h"
#include "inputs\SubFlow.h"
#include "inputs\SinglePaxTypeFlow.h"
#include "Inputs\Structurelist.h"
#include "glrender\glTextureResource.h"
#include "glrender\glShapeResource.h"
#include ".\Airside3D.h"
#include ".\..\InputAirside\InputAirside.h"
#include "FilletTaxiway3D.h"
#include "Airport3D.h"
#include "MainBarUpdateArgs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT indicators[] =
{
	//ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_3DMOUSEMODE,
	ID_INDICATOR_GRIDSPACING,
	ID_INDICATOR_3DMOUSEPOS,
	ID_INDICATOR_ANIMTIME,
	ID_INDICATOR_ANIMATIONSPEED,
	ID_INDICATOR_FPS
};


/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, IRenderFrame)

BEGIN_MESSAGE_MAP(CChildFrame, IRenderFrame)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//ON_COMMAND(ID_ANIMATION_RECORD, OnAnimationRecord)
	//ON_UPDATE_COMMAND_UI(ID_ANIMATION_RECORD, OnUpdateAnimationRecord)
	//ON_COMMAND(ID_ANIMATION_RECSETTINGS, OnAnimationRecsettings)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_3DMOUSEMODE, OnUpdate3dMouseMode)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_3DMOUSEPOS, OnUpdate3dMousePos)
	ON_COMMAND(ID_VIEW_SAVENAMEDVIEW, OnViewSaveNamedView)
	ON_COMMAND(ID_VIEW_TOGGLE2D3D, OnViewToggle2D3D)
	ON_COMMAND(ID_WINDOW_SPLITHORIZONTAL, OnSplitHorizontal)
	ON_COMMAND(ID_WINDOW_SPLITVERTICAL, OnSplitVertical)
	ON_COMMAND(ID_WINDOW_UNSPLIT, OnUnSplit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame() :
	m_pHyperCam(NULL),
	m_bRecord(FALSE),
	m_vPaxShapeDLIDs(0),
	m_vResShapeDLIDs(0),
	m_vRwyTexIDs(0),
	m_nDefaultPaxShapeDLID(0),
	m_nOverlapPaxShapeDLID(0),
	//m_nDefaultACShapeDLID(0xFFFF),
	//m_nOverlapACShapeDLID(0xFFFF),
	m_nCompassDispListId(0),
	m_nLogoTexID(0),
	m_nMSTexId(0),
	m_nRampTexId(0),
	m_nFlowDLID(0),
	m_nSelectionDLID(0), //red sqaure
	m_nSphereDLID(0),
	m_nConeDLID(0),
	m_nDiskDLID(0),
	m_vWorkingCameras(0),
	m_bInfoActive(FALSE)
{
	m_vWorkingCameras.reserve(4);
	for(int i=0; i<4; i++) {
		m_vWorkingCameras.push_back(new C3DCamera(C3DCamera::perspective));
	}

	//m_nRecordTimerID = 10;
	//m_pMovieWriter = NULL;

	m_pTextureResource = new CTextureResource;
	m_pAirlineTextures = new CTextureResource;
	m_pShapeResource = new CglShapeResource;
	m_pAirsideShapeResource = new CglShapeResource;	
	m_pAirside3D = new CAirside3D;
	m_bInLoadingModel = FALSE;
	m_fLoadingPercent = 0.0f;

	m_pUserTexturePool = new CTextureResourcePool;
	
}

CChildFrame::~CChildFrame()
{
	if(m_pHyperCam != NULL) {
		delete m_pHyperCam;
		m_pHyperCam = NULL;
	}

	for(int i=0; i<4; i++) {
		delete m_vWorkingCameras[i];
	}
	m_vWorkingCameras.clear();

	m_vSelected.clear();

	//SAFE_DEL_VIEW_PBUFFER
	//SAFE_DEL_FLOOR_PBUFFER
	delete m_pTextureResource;
	delete m_pAirlineTextures;
	delete m_pShapeResource;
	delete m_pAirsideShapeResource;
	delete m_pAirside3D;
	delete m_pUserTexturePool;
	
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !IRenderFrame::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	IRenderFrame::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	IRenderFrame::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	return m_wndSplitter.Create(this, 2, 2, CSize(10,10), pContext, WS_CHILD | WS_VISIBLE | SPLS_DYNAMIC_SPLIT);
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (IRenderFrame::OnCreate(lpCreateStruct) == -1)
		return -1;

	MDICREATESTRUCT* lpmcs;
	lpmcs = (MDICREATESTRUCT*)lpCreateStruct->lpCreateParams;
	CCreateContext* pContext = (CCreateContext*)lpmcs->lParam;
	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetPaneInfo(3, ID_INDICATOR_ANIMTIME, SBPS_NOBORDERS, 110);
	m_wndStatusBar.SetPaneInfo( 0, ID_INDICATOR_3DMOUSEMODE, SBPS_STRETCH | SBPS_NOBORDERS, 0 );
	m_wndStatusBar.SetPaneInfo(4, ID_INDICATOR_ANIMATIONSPEED, SBPS_NOBORDERS, 110);

	wglMakeCurrent( GetPane(0,0)->GetClientDC()->GetSafeHdc(), GetPane(0,0)->GetHGLRC() );
	GenGlobalDLs();
	GenGlobalTextures();

	//	
	ASSERT(pContext->m_pCurrentDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pContext->m_pCurrentDoc;

	m_pTextureResource->SetResourcePath(((CTermPlanApp*) AfxGetApp())->GetTexturePath());
	m_pTextureResource->Load();
	m_pAirlineTextures->SetResourcePath(((CTermPlanApp*) AfxGetApp())->GetAirlineImgPath() );
	m_pAirlineTextures->Load();
	m_pShapeResource->SetResourcePath(((CTermPlanApp*) AfxGetApp())->GetShapeDataPath());
	m_pShapeResource->Load();
	m_pAirsideShapeResource->SetResourcePath(((CTermPlanApp*) AfxGetApp())->GetShapeDataPath() + _T("\\aircraft\\"));
	m_pAirsideShapeResource->Load(true);
	
	m_pAirside3D->Init(InputAirside::GetProjectID(pDoc->m_ProjInfo.name) ) ;




	//floors stuff: load map objects & create map texture
	//for each floor, if map & layers already loaded AND not processed then process
	//for(int i=0; i<static_cast<int>(pDoc->GetCurModeFloor().m_vFloors.size()); i++) {
	//	CFloor2* pFloor = pDoc->GetCurModeFloor().m_vFloors[i];

	//	pFloor->InvalidateVisibleRegions();

	//	if(pFloor != NULL && pFloor->IsCADLoaded() && pFloor->IsCADLayersLoaded() && !pFloor->IsCADProcessed()) {
	//		VERIFY(pFloor->ProcessCAD()); //process
	//	}
	//	if(pFloor->IsMapValid()) {
	//		pFloor->GenerateTextureMap();
	//	}
	//}
	for(int i=0; i<static_cast<int>(pDoc->GetFloorByMode(EnvMode_Terminal).m_vFloors.size()); i++) {
		CFloor2* pFloor = pDoc->GetFloorByMode(EnvMode_Terminal).m_vFloors[i];

		pFloor->InvalidateVisibleRegions();

		if(pFloor != NULL && pFloor->IsCADLoaded() && pFloor->IsCADLayersLoaded() && !pFloor->IsCADProcessed()) {
			VERIFY(pFloor->ProcessCAD()); //process
		}
		/*if(pFloor->IsMapValid()) {
			pFloor->GenerateTextureMap();
		}*/
	}

	for(int i=0; i<static_cast<int>(pDoc->GetFloorByMode(EnvMode_AirSide).m_vFloors.size()); i++) {
		CFloor2* pFloor = pDoc->GetFloorByMode(EnvMode_AirSide).m_vFloors[i];

		pFloor->InvalidateVisibleRegions();

		if(pFloor != NULL && pFloor->IsCADLoaded() && pFloor->IsCADLayersLoaded() && !pFloor->IsCADProcessed()) {
			VERIFY(pFloor->ProcessCAD()); //process
		}
		//if(pFloor->IsMapValid()) {
		//	pFloor->GenerateTextureMap();
		//}
	}

	for(int i=0; i<static_cast<int>(pDoc->GetFloorByMode(EnvMode_LandSide).m_vFloors.size()); i++) {
		CFloor2* pFloor = pDoc->GetFloorByMode(EnvMode_LandSide).m_vFloors[i];

		pFloor->InvalidateVisibleRegions();

		if(pFloor != NULL && pFloor->IsCADLoaded() && pFloor->IsCADLayersLoaded() && !pFloor->IsCADProcessed()) {
			VERIFY(pFloor->ProcessCAD()); //process
		}
		//if(pFloor->IsMapValid()) {
		//	pFloor->GenerateTextureMap();
		//}
	}
	return 0;
}

void CChildFrame::OnUpdate3dMousePos(CCmdUI *pCmdUI)
{
	C3DView* p3DView = GetActivePane();
	if(p3DView) { // if its valid
		CString s(_T(""));
		CUnitsManager* pUM = CUnitsManager::GetInstance();
		CTermPlanDoc* pDoc = GetDocument();
		if(pDoc && pDoc->m_systemMode == EnvMode_AirSide)
			s.Format("\tX: %5.2f %s | Y:%5.2f %s",\
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),p3DView->GetMousePos()[VX]),\
			CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),p3DView->GetMousePos()[VY]),\
			CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()));
		else
			s.Format("\tX: %5.2f %s | Y:%5.2f %s",\
			pUM->ConvertLength(p3DView->GetMousePos()[VX]),\
			pUM->GetLengthUnitString(pUM->GetLengthUnits()),\
			pUM->ConvertLength(p3DView->GetMousePos()[VY]),\
			pUM->GetLengthUnitString(pUM->GetLengthUnits()));

		pCmdUI->Enable(TRUE);
		pCmdUI->SetText( s );
	}
	else {
		pCmdUI->Enable(FALSE);
	}
}

void CChildFrame::OnUpdate3dMouseMode(CCmdUI* pCmdUI)
{
	C3DView* p3DView = GetActivePane();
	if(p3DView) { // if its valid
		CString s(_T(""));
		pCmdUI->Enable(TRUE);
		switch(p3DView->m_eMouseState)
		{
		case NS3DViewCommon::_floormanip:
			s = _T(" Floor Manipulation Mode");
			break;
		case NS3DViewCommon::_default:
			s = _T(" Navigation Mode");
			break;
		case NS3DViewCommon::_placeproc:
			s = _T(" Place Processor");
			break;
		case NS3DViewCommon::_scaleproc:
			s = _T(" Scale Processor");
			break;
		case NS3DViewCommon::_scaleprocX:
			s = _T(" Scale Processor X");
			break;
		case NS3DViewCommon::_scaleprocY:
			s = _T(" Scale Processor Y");
			break;
		case NS3DViewCommon::_scaleprocZ:
			s = _T(" Scale Processor Z");
			break;
		case NS3DViewCommon::_rotateproc:
			s = _T(" Rotate Processor");
			break;
		case NS3DViewCommon::_rotateairsideobject:
			s = _T(" Rotate Airside Object");
			break;
		case NS3DViewCommon::_getmirrorline:
			s = _T(" Draw a line to reflect across");
			break;
		case NS3DViewCommon::_getarrayline:
			s = _T(" Draw a line to array along");
			break;
		case NS3DViewCommon::_getonepoint:
			s = _T(" Select a coordinate");
			break;
		case NS3DViewCommon::_gettwopoints:
			s = _T(" Select two points");
			break;
		case NS3DViewCommon::_getmanypoints:
			s = _T(" Select points, RC when done");
			break;
		case NS3DViewCommon::_placemarker:
			s = _T(" Place alignment marker");
			break;
		case NS3DViewCommon::_tumble:
			s = _T(" Rotate view");
			break;
		case NS3DViewCommon::_dolly:
			s = _T(" Zoom view");
			break;
		case NS3DViewCommon::_pan:
			s = _T(" Pan view");
			break;
		case NS3DViewCommon::_distancemeasure:
			s = _T(" Distance Measurement");
		}
		pCmdUI->SetText( s );
	}
	else
		pCmdUI->Enable(FALSE);
}

void CChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	IRenderFrame::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}

void CChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!
	
	CView* pView = GetActiveView();
	ASSERT(pView->IsKindOf(RUNTIME_CLASS(C3DView)));
	C3DView* p3DView = (C3DView*) pView;
	CString sViewName;
	if(p3DView->GetCamera()->GetProjectionType() == C3DCamera::perspective)
		//sViewName.Format("        %s [3D View]",p3DView->GetCamera()->GetName());
		sViewName=_T("       3D View");
	else
		//sViewName.Format("        %s [2D View]",p3DView->GetCamera()->GetName());
		sViewName=_T("       2D View");

	CDocument* pDocument = GetActiveDocument();
	if(bAddToTitle)
	{
		TCHAR szText[256+_MAX_PATH];
		if (pDocument == NULL)
			lstrcpy(szText, m_strTitle);
		else
			lstrcpy(szText, pDocument->GetTitle() + sViewName);
		this->SetWindowText(szText);
	}
}
/*
void CChildFrame::OnAnimationRecord() 
{
	if(!m_bRecord) { //start recording
		if(m_pHyperCam == NULL) {
			m_pHyperCam = new IHyperCam;
			if(!m_pHyperCam->CreateDispatch("HyperCam.Application")) {
				AfxMessageBox("Error: Could not initialize recording module.");
				delete m_pHyperCam;
				m_pHyperCam = NULL;
				return;
			}
		}
		//set options
		CRect r;
		this->GetClientRect(&r);
		this->ClientToScreen(&r);
		m_pHyperCam->SetStartX(r.left);
		m_pHyperCam->SetStartY(r.top);
		m_pHyperCam->SetWidth(r.Width());
		m_pHyperCam->SetHeight(r.Height());
		CString sFileName = GetActivePane()->GetDocument()->m_ProjInfo.path + "\\" + GetActivePane()->GetDocument()->m_ProjInfo.name + ".avi";
		m_pHyperCam->SetFileName(sFileName);
		m_pHyperCam->StartRec();

		m_bRecord = TRUE;
	}
	else {
		//stop recording
		m_pHyperCam->StopRec();
		m_bRecord = FALSE;
	}
}

void CChildFrame::OnUpdateAnimationRecord(CCmdUI* pCmdUI) 
{
	if(!m_bRecord)
		pCmdUI->SetText("Start Recording");
	else
		pCmdUI->SetText("Stop Recording");

	pCmdUI->Enable(TRUE);
	
}

void CChildFrame::OnAnimationRecsettings() 
{
	if(m_pHyperCam == NULL) {
		m_pHyperCam = new IHyperCam;
		if(!m_pHyperCam->CreateDispatch("HyperCam.Application")) {
			AfxMessageBox("Error: Could not initialize recording module.");
			delete m_pHyperCam;
			m_pHyperCam = NULL;
			return;
		}
	}
	CDlgRecordOptions dlg(m_pHyperCam);
	dlg.DoModal();
}
*/
void CChildFrame::SetPaneInfo(CViewDesc& vd)
{
	int dummy;
	vd.paneinfo.nNumRows = m_wndSplitter.GetRowCount();
	vd.paneinfo.nNumCols = m_wndSplitter.GetColumnCount();
	m_wndSplitter.GetRowInfo(0, vd.paneinfo.nHeightRow0, dummy);
	m_wndSplitter.GetColumnInfo(0, vd.paneinfo.nWidthCol0, dummy);
}

void CChildFrame::SaveWorkingViews(CViewDesc* pvd)
{
	SetPaneInfo(*pvd);
	//pvd->panecameras.reserve(pvd->paneinfo.nNumRows*pvd->paneinfo.nNumCols);
	pvd->panecameras.reserve(4);
	for(int r=0; r<2; r++) {
		for(int c=0; c<2; c++) {
			if(pvd->panecameras[c+2*r] != NULL)
				*(pvd->panecameras[c+2*r]) = *(m_vWorkingCameras[c+2*r]);
			else
				pvd->panecameras[c+2*r] = new C3DCamera(*(m_vWorkingCameras[c+2*r]));
		}
	}
}

void CChildFrame::LoadWorkingViews(CViewDesc* pvd)
{
	for(int r=0; r<2; r++) {
		for(int c=0; c<2; c++) {
			*(m_vWorkingCameras[c+2*r]) = *(pvd->panecameras[c+2*r]);
		}
	}
	if(pvd->paneinfo.nNumRows > 1)
		SplitRow(pvd->paneinfo.nHeightRow0);
	else
		UnSplitRow();
	if(pvd->paneinfo.nNumCols > 1)
		SplitColumn(pvd->paneinfo.nWidthCol0);
	else
		UnSplitColumn();
}

void CChildFrame::OnDestroy() 
{
	IRenderFrame::OnDestroy();

	for (int j = 0; j <static_cast<int>(GetDocument()->GetStructurelistByMode(EnvMode_Terminal).getStructureNum()); j++)
	{
		CStructure* pstructure = GetDocument()->GetStructurelistByMode(EnvMode_Terminal).getStructureAt(j);
		pstructure->SetDisplayDirtflag(true);
	}
	for (int k = 0; k <static_cast<int>(GetDocument()->GetStructurelistByMode(EnvMode_AirSide).getStructureNum()); k++)
	{
		CStructure* pstructure = GetDocument()->GetStructurelistByMode(EnvMode_AirSide).getStructureAt(k);
		pstructure->SetDisplayDirtflag(true);
	}
	//mark floor textures as invalid
	for(int i=0; i<static_cast<int>(GetDocument()->GetCurModeFloor().m_vFloors.size()); i++) {
		GetDocument()->GetCurModeFloor().m_vFloors[i]->InvalidateTexture();
	}
	
	StorePanInfo();
/*	GetDocument()->GetCamerasByMode(EnvMode_Terminal).saveDataSet(GetDocument()->m_ProjInfo.path,FALSE);
	GetDocument()->GetCamerasByMode(EnvMode_AirSide).saveDataSet(GetDocument()->m_ProjInfo.path,FALSE);
	std::vector<CCameras *> vCamara;
	GetDocument()->GetAllCameras(vCamara);

	for (int ii= 0 ;ii < static_cast<int>(vCamara.size()) ;ii++)
	{
		((CCameras *)vCamara[ii])->saveDataSet(GetDocument()->m_ProjInfo.path,FALSE);
	}
*/
//
//	if(GetWorkingCamera(0,0)->GetProjectionType() == C3DCamera::perspective) {//current working view type is 3D
//		pvd = &(GetDocument()->GetCamerasByMode(EnvMode_Terminal).m_userView3D);
//	}
//	else {
//		pvd = &(GetDocument()->GetCamerasByMode(EnvMode_Terminal).m_userView2D);
//	}
//	//copy cameras from working views to user view
//	SaveWorkingViews(pvd);
//	GetDocument()->GetCamerasByMode(EnvMode_Terminal).saveDataSet(GetDocument()->m_ProjInfo.path,FALSE);


	CWnd* pMF=AfxGetMainWnd();
	ToolBarUpdateParameters updatePara;
	updatePara.m_operatType = ToolBarUpdateParameters::Close3DViewFrm;
	updatePara.m_nCurMode = GetDocument()->GetCurrentMode();
	pMF->SendMessage(UM_SHOW_CONTROL_BAR,2,(LPARAM)&updatePara);
	
}

void CChildFrame::OnUpdateFrameMenu (BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt)
{
    IRenderFrame::OnUpdateFrameMenu (bActivate, pActivateWnd, hMenuAlt);
	if(bActivate)
		GetDocument()->UpdateTrackersMenu();
}

void CChildFrame::GenPaxShapeDLIDs(CPaxDispProps* pPDP)
{
	wglMakeCurrent( GetPane(0,0)->GetClientDC()->GetSafeHdc(), GetPane(0,0)->GetHGLRC() );
	if(glIsList(m_nDefaultPaxShapeDLID))
		glDeleteLists(m_nDefaultPaxShapeDLID,1);
	if(glIsList(m_nOverlapPaxShapeDLID))
		glDeleteLists(m_nOverlapPaxShapeDLID,1);
	for(int i=0; i<static_cast<int>(m_vPaxShapeDLIDs.size()); i++) {
		if(glIsList(m_vPaxShapeDLIDs[i]))
			glDeleteLists(m_vPaxShapeDLIDs[i],1);
	}
	m_vPaxShapeDLIDs.clear();
	//m_nDefaultPaxShapeDLID = CPaxShapes::GeneratePaxDL(pPDP->GetDefaultDispProp()->GetShape().item, pPDP->GetDefaultDispProp()->GetColor());
	//m_nOverlapPaxShapeDLID = CPaxShapes::GeneratePaxDL(pPDP->GetOverLapDispProp()->GetShape().item, pPDP->GetOverLapDispProp()->GetColor());
	m_nDefaultPaxShapeDLID = SHAPESMANAGER->GenerateShapeDisplayList( 
			pPDP->GetDefaultDispProp()->GetShape().first, 
			pPDP->GetDefaultDispProp()->GetShape().second, 
			pPDP->GetDefaultDispProp()->GetColor() );

	m_nOverlapPaxShapeDLID = SHAPESMANAGER->GenerateShapeDisplayList( 
		pPDP->GetOverLapDispProp()->GetShape().first, 
		pPDP->GetOverLapDispProp()->GetShape().second, 
		pPDP->GetOverLapDispProp()->GetColor() );

	for(int i=0; i<pPDP->GetCount(); i++) {
		m_vPaxShapeDLIDs.push_back(
			SHAPESMANAGER->GenerateShapeDisplayList(
							pPDP->GetDispPropItem(i)->GetShape().first,
							pPDP->GetDispPropItem(i)->GetShape().second, 
							pPDP->GetDispPropItem(i)->GetColor() )   );
	}
	//wglMakeCurrent(NULL,NULL);
}

void CChildFrame::GenResShapeDLIDs(CResourceDispProps* pRDP)
{
	wglMakeCurrent( GetPane(0,0)->GetClientDC()->GetSafeHdc(), GetPane(0,0)->GetHGLRC() );
	for(int i=0; i<static_cast<int>(m_vResShapeDLIDs.size()); i++)
	{
		if(glIsList(m_vResShapeDLIDs[i]))
			glDeleteLists(m_vResShapeDLIDs[i],1);
	}

	m_vResShapeDLIDs.clear();

	for(int i=0; i<pRDP->GetCount(); i++)
	{
		//modify by porter 2006-12-07
		//control display of the resource 
		if ( pRDP->GetDispPropItem(i)->IsVisible() )
		{
			m_vResShapeDLIDs.push_back(
				SHAPESMANAGER->GenerateShapeDisplayList(
								pRDP->GetDispPropItem(i)->GetShape().first,
								pRDP->GetDispPropItem(i)->GetShape().second, 
								pRDP->GetDispPropItem(i)->GetColor() )   );
		}
		else 
		{
			m_vResShapeDLIDs.push_back(0);
		}
	}

}

void CChildFrame::GenAircraftShapeDLIDs(CAircraftDispProps* pADP)
{
	wglMakeCurrent( GetPane(0,0)->GetClientDC()->GetSafeHdc(), GetPane(0,0)->GetHGLRC() );

	int nACSet = SHAPESMANAGER->FindShapeIndexByName(_T("Aircraft"));
	ASSERT(nACSet != -1);
	if(nACSet==-1){
		AfxMessageBox(_T("Error Find Aircraft Shapes,try update resource packages!"));
		return;
	}
	
	if(glIsList(m_nDefaultACShapeDLID))
		glDeleteLists(m_nDefaultACShapeDLID,1);
	if(glIsList(m_nOverlapACShapeDLID))
		glDeleteLists(m_nOverlapACShapeDLID,1);
	for(int i=0; i<static_cast<int>(m_vAircraftShapeDLIDs.size()); i++) {
		if(glIsList(m_vAircraftShapeDLIDs[i]))
			glDeleteLists(m_vAircraftShapeDLIDs[i],1);
	}
	m_vAircraftShapeDLIDs.clear();

	m_nDefaultACShapeDLID = SHAPESMANAGER->GenerateShapeDisplayList( 
		nACSet, 
		0, 
		pADP->GetDefaultDispProp()->GetColor() );

	m_nOverlapACShapeDLID = SHAPESMANAGER->GenerateShapeDisplayList( 
		nACSet, 
		0, 
		pADP->GetOverLapDispProp()->GetColor() );

	for(int i=0; i<pADP->GetCount(); i++) {
		m_vAircraftShapeDLIDs.push_back(
			SHAPESMANAGER->GenerateShapeDisplayList(
				pADP->GetDispPropItem(i)->GetShape().first,
				pADP->GetDispPropItem(i)->GetShape().second, 
				pADP->GetDispPropItem(i)->GetColor() ) );
	}
	

	for(int i=0; i<static_cast<int>(m_vAllAircraftShapeDLIDs.size()); i++) {
		if(glIsList(m_vAllAircraftShapeDLIDs[i]))
			glDeleteLists(m_vAllAircraftShapeDLIDs[i],1);
	}
	m_vAllAircraftShapeDLIDs.clear();
	
	int nACCount = SHAPESMANAGER->GetShapeCount(nACSet);

	for(int i=0; i<nACCount; i++) {
		m_vAllAircraftShapeDLIDs.push_back(
			SHAPESMANAGER->GenerateShapeDisplayList( nACSet, i, RGB(0,0,0) ) );
	}

	//loading ac shapes
	m_bInLoadingModel = TRUE;
	CglShapeResource::ShapeMap& shapes = m_pAirsideShapeResource->getShapeMap();
	int nCout = shapes.size();
	CProgressBar progresbar("Loading Shapes ... ",100, nCout);
	int iCurrent = 0;
	for( CglShapeResource::ShapeMap::iterator itr = shapes.begin();itr!=shapes.end(); itr++)
	{
		(*itr).second->display();
		iCurrent++;
		m_fLoadingPercent = float(iCurrent)/nCout;
		progresbar.StepIt();
	}
	m_bInLoadingModel = FALSE;
	m_fLoadingPercent = 0.0f;

}

void CChildFrame::GenGlobalTextures()
{
	glGenTextures(1, &m_nMSTexId);
	glBindTexture(GL_TEXTURE_2D, m_nMSTexId);
	
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 64, 64, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) mstexture);


	glGenTextures(1, &m_nRampTexId);
	glBindTexture(GL_TEXTURE_2D, m_nRampTexId);
	
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 64, 64, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) ramptexture);


	//gen rumway textures
	static GLuint data_buffer[512*512*4];

	//clear any existing rwy textures
	for(int i=0; i<static_cast<int>(m_vRwyTexIDs.size()); i++)
		glDeleteTextures(1, &m_vRwyTexIDs[i]);
		//glDeleteLists(m_vRwyTexIDs[i], 1);
	m_vRwyTexIDs.clear();
	//gen rwy textures
	int nRwyCount = RunwayTextures::GetNumTextures();
	m_vRwyTexIDs.reserve(nRwyCount);
	for(int i=0; i<nRwyCount; i++) {
		int w, h, c;
		GLuint nId = 0;
		if(RunwayTextures::GetTexture((RunwayTextures::TextureEnum)i, data_buffer, w, h, c)) {
			glGenTextures(1, &nId);
			glBindTexture(GL_TEXTURE_2D, nId);
			// TRACE("Generating texture id %d..", nId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (i>1)?GL_CLAMP:GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (i>1)?GL_CLAMP:GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			
			//glTexImage2D(GL_TEXTURE_2D,0,(c==4)?GL_RGBA:GL_RGB,w,h,0,(c==4)?GL_RGBA:GL_RGB,GL_UNSIGNED_BYTE,(GLvoid*) data_buffer);
			gluBuild2DMipmaps(GL_TEXTURE_2D, c, w, h, (c==4)?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) data_buffer);
			m_vRwyTexIDs.push_back(nId);
		}

	}

	//ASSERT(m_vRwyTexIDs.size() == nRwyCount);
	
	//Generate all textures need; add by ben 2005-8.22
	for(int i=0; i<static_cast<int>(m_vAllTexIDs.size()); i++)
	{
		glDeleteTextures(1, &m_vAllTexIDs[i]);
		//glDeleteLists(m_vAllTexIDs[i], 1);
	}
	m_vAllTexIDs.clear();
	//gen rwy textures
	int nTextureCount = Textures::GetNumTextures();
	m_vAllTexIDs.reserve(nTextureCount);
	for(int i=0; i<nTextureCount; i++) {
		int w, h, c;
		GLuint nId = 0;
		char* pTextureBuf = Textures::GetTexture((Textures::TextureEnum)i, ((CTermPlanApp*)AfxGetApp())->GetTexturePath(), w, h, c);
		if(pTextureBuf) {
			glGenTextures(1, &nId);
			glBindTexture(GL_TEXTURE_2D, nId);
			// TRACE("Generating texture id %d..", nId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			//glTexImage2D(GL_TEXTURE_2D,0,(c==4)?GL_RGBA:GL_RGB,w,h,0,(c==4)?GL_RGBA:GL_RGB,GL_UNSIGNED_BYTE,(GLvoid*) data_buffer);
			gluBuild2DMipmaps(GL_TEXTURE_2D, c, w, h, (c==4)?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, pTextureBuf);
			Textures::ReleaseTextureBuffer(pTextureBuf);
			m_vAllTexIDs.push_back(nId);
		}
		else{m_vAllTexIDs.push_back(0);}
		}
	}
	


void CChildFrame::GenGlobalDLs()
{
	m_nCompassDispListId = glGenLists(1);
	GLUquadricObj* pQuadObj = gluNewQuadric();
	gluQuadricDrawStyle(pQuadObj, GLU_FILL);
	gluQuadricNormals(pQuadObj, GLU_SMOOTH);
	glNewList(m_nCompassDispListId, GL_COMPILE);
	// altitude
	glPushMatrix();
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0, 0.0, -1.0);
	gluCylinder(pQuadObj, 0.05, 0.05, 2.0, 5, 5);
	gluSphere(pQuadObj, 0.05, 10, 10);
	glTranslatef(0.0, 0.0, 2.0);
	gluDisk(pQuadObj, 0.05, 0.09, 10, 5);
	gluCylinder(pQuadObj, 0.15, 0.0, 0.5, 5, 5);
	glPopMatrix();

	// North-South
	glPushMatrix();
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glTranslatef(0.0, 0.0, -1.0);
	gluCylinder(pQuadObj, 0.05, 0.05, 2.0, 5, 5);
	gluSphere(pQuadObj, 0.05, 10, 10);
	glTranslatef(0.0, 0.0, 2.0);
	gluDisk(pQuadObj, 0.05, 0.09, 10, 5);
	gluCylinder(pQuadObj, 0.15, 0.0, 0.5, 5, 5);
	glPopMatrix();

	// East-West
	glPushMatrix();
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glRotatef(90, 0.0, 1.0, 0.0);
	glTranslatef(0.0, 0.0, -1.0);
	gluCylinder(pQuadObj, 0.05, 0.05, 2.0, 5, 5);
	gluSphere(pQuadObj, 0.05, 10, 10);
	glTranslatef(0.0, 0.0, 2.0);
	gluDisk(pQuadObj, 0.05, 0.09, 10, 5);
	gluCylinder(pQuadObj, 0.15, 0.0, 0.5, 5, 5);
	glPopMatrix();

	glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
	gluSphere(pQuadObj, 0.25, 5, 5);
	glEndList();

	//red-quare
	m_nSelectionDLID = glGenLists(1);
	glNewList(m_nSelectionDLID, GL_COMPILE);
	//glLineWidth(3.0);
	glBegin(GL_LINE_LOOP);
	glVertex3f(-5.0,-5.0,0.0);
	glVertex3f(5.0,-5.0,0.0);
	glVertex3f(5.0,5.0,0.0);
	glVertex3f(-5.0,5.0,0.0);
	glEnd();
	//glLineWidth(1.0);
	glEndList();

	//sphere
	m_nSphereDLID = glGenLists(1);
	glNewList(m_nSphereDLID, GL_COMPILE);
	gluSphere(pQuadObj, 1.0, 16, 16);
	glEndList();
	
	//cone
	m_nConeDLID = glGenLists(1);
	glNewList(m_nConeDLID, GL_COMPILE);
	gluCylinder(pQuadObj, 0.0, 1.0, 1.0, 10, 10);
	glEndList();

	//disk
	m_nDiskDLID = glGenLists(1);
	glNewList(m_nDiskDLID,GL_COMPILE);
	gluDisk(pQuadObj, 0.0, 1.0, 16, 1);
	glEndList();

	gluDeleteQuadric(pQuadObj);
}

/*
GLuint CChildFrame::AircraftShapeDLID( LPCTSTR _sACType ) const
{
	if( _tcslen(_sACType) < 2 )
		return -1;

	if( '7' == _sACType[0] ) {
		if( '4' == _sACType[1] )
			return m_vAllAircraftShapeDLIDs[0]; //747
		else if( '3' == _sACType[1] )
			return m_vAllAircraftShapeDLIDs[1]; //737
		else if( '7' == _sACType[1] )
			return m_vAllAircraftShapeDLIDs[2]; //777
	}

	if( '3' == _sACType[0] ) {
		if( '2' == _sACType[1] )
			return m_vAllAircraftShapeDLIDs[3]; //320
		else if( '3' == _sACType[1] )
			return m_vAllAircraftShapeDLIDs[4]; //330
		else if( '4' == _sACType[1] )
			return m_vAllAircraftShapeDLIDs[5]; //340
	}

	return m_vAllAircraftShapeDLIDs[0];
}
*/


void CChildFrame::UpdateFlowGeometry(const CSinglePaxTypeFlow* _pSPTF,const CSinglePaxTypeFlow* _pSPTFForGateAssaign)
{
	//make context current
	if(!::wglMakeCurrent( GetPane(0,0)->GetClientDC()->GetSafeHdc(), GetPane(0,0)->GetHGLRC() ))
		ASSERT(FALSE);
	if(glIsList(m_nFlowDLID)) {
		glDeleteLists(m_nFlowDLID,1);
	}
	if(_pSPTF==NULL)
		return;

	const CSinglePaxTypeFlow* pFlowToDraw = NULL;
	m_nFlowDLID = glGenLists(1);
	CTermPlanDoc* pDoc = GetDocument();
	int nNumFloors = pDoc->GetCurModeFloor().m_vFloors.size(); 
	double dAlt[4];
	BOOL bOn[4];
	for(int i=0; i<nNumFloors; i++)
	{
		dAlt[i] = pDoc->GetCurModeFloor().m_vFloors[i]->Altitude();
		bOn[i] = pDoc->GetCurModeFloor().m_vFloors[i]->IsVisible();
	}
	glNewList(m_nFlowDLID, GL_COMPILE);
	
	for( int iFlow=0; iFlow<2; ++iFlow ) {
		if( iFlow == 0) {
			pFlowToDraw = _pSPTF;
			glColor4ub(60,60,90,64);
		}
		else {
			pFlowToDraw = _pSPTFForGateAssaign;
			glColor4ub(200,60,90,64);
		}
		if( pFlowToDraw == NULL ) {
			continue;
		}
		int c = pFlowToDraw->GetFlowPairCount();
		for(int i=0; i<c; i++) {
			//for each source processor (could be a processor group)
			const CProcessorDestinationList* pFP = NULL;
			pFP = &(pFlowToDraw->GetFlowPairAt(i));
			if( pDoc->GetTerminal().m_pSubFlowList->IfProcessUnitExist( pFP->GetProcID().GetIDString() ) ) {
				CSubFlow* pFlow = pDoc->GetTerminal().m_pSubFlowList->GetProcessUnit( pFP->GetProcID().GetIDString() );
				ASSERT( pFlow != NULL );
				//*** draw process internal flow
				pFlow->DrawProcessInternalFlow( bOn , dAlt );
				int iDestCount = pFP->GetDestCount();
				for( int iDest=0; iDest<iDestCount; ++iDest ) {
					CFlowDestination dpai = pFP->GetDestProcAt( iDest );
					if( pDoc->GetTerminal().m_pSubFlowList->IfProcessUnitExist( dpai.GetProcID().GetIDString() ) ) {
						//*** draw process to process 
						CSubFlow* pSecondFlow = pDoc->GetTerminal().m_pSubFlowList->GetProcessUnit( dpai.GetProcID().GetIDString() );
						ASSERT( pSecondFlow );
						DrawProcessToProcess ( pFlow, pSecondFlow, dAlt, bOn );
					}
					else { //*** draw process to normal procssor
						DrawProcessToProcessor( pFlow, dpai.GetProcID(), dAlt, bOn,  dpai.GetOneToOneFlag() ? true : false );					
					}
				}
				continue;
			}
			GroupIndex gidx = pDoc->GetTerminal().procList->getGroupIndex(pFP->GetProcID());
			if(gidx.start == -1 || gidx.end == -1)
				continue;
			for(int j=gidx.start; j<=gidx.end; j++) {
				if(j==START_PROCESSOR_INDEX || j==TO_GATE_PROCESS_INDEX
					|| j==FROM_GATE_PROCESS_INDEX || j == BAGGAGE_DEVICE_PROCEOR_INDEX )
					continue;
				//for each processor in group
				Processor* procSource = pDoc->GetTerminal().procList->getProcessor(j);
				int nIndexInFloor=static_cast<int>(procSource->getFloor()/SCALE_FACTOR);
				if(!bOn[nIndexInFloor]) continue;
				ASSERT(procSource != NULL);
				//create geometry between this processor and every destination processors in flow
				int nDestCount = pFP->GetDestCount();
				for(int k=0; k<nDestCount; k++) {
					//for each destination (could be a group)
					CFlowDestination dpai = pFP->GetDestProcAt(k);
					if( pDoc->GetTerminal().m_pSubFlowList->IfProcessUnitExist( dpai.GetProcID().GetIDString() ) )	{
						CSubFlow* pFlow = pDoc->GetTerminal().m_pSubFlowList->GetProcessUnit( dpai.GetProcID().GetIDString() );
						ASSERT( pFlow != NULL);
						//*** draw nomal processor to process
						DrawProcessorToProcess( procSource, pFlow , dAlt , bOn, dpai.GetOneToOneFlag() ? true : false);
						//***draw process internal flow
						pFlow->DrawProcessInternalFlow( bOn , dAlt );
						continue;
					}
					
					GroupIndex gidxDest = pDoc->GetTerminal().procList->getGroupIndex(dpai.GetProcID());
					if(gidxDest.start == -1 || gidxDest.end == -1 || 
						gidxDest.start==TO_GATE_PROCESS_INDEX || 
						gidxDest.start==FROM_GATE_PROCESS_INDEX ||
						gidxDest.start==BAGGAGE_DEVICE_PROCEOR_INDEX ||
						gidxDest.end==TO_GATE_PROCESS_INDEX || 
						gidxDest.end==FROM_GATE_PROCESS_INDEX ||
						gidxDest.end==BAGGAGE_DEVICE_PROCEOR_INDEX )						
						continue;
					for(int l = gidxDest.start; l<=gidxDest.end; l++) {
						if(l==END_PROCESSOR_INDEX )
							continue;
						//for each proc in dest group
						Processor* procDest = pDoc->GetTerminal().procList->getProcessor(l);
						// **** modified by tutu in 2003-2-21
						Point ptStart = procSource->serviceLocationPath()->getPoint( 0 );
						Point ptEnd = procDest->serviceLocationPath()->getPoint( 0 );
						Point ptFlowStart;
						Point ptFlowEnd;
						POLLYGONVECTOR vPipeCoveredRegion;
						if( dpai.GetTypeOfUsingPipe() == 1 ) {// using pipe system
							GetDocument()->GetTerminal().m_pPipeDataSet->AutomaicGetPipeFlowCoveredRegion( ptStart,ptEnd,vPipeCoveredRegion, ptFlowStart, ptFlowEnd );
							int iQUADCount = vPipeCoveredRegion.size();
							DistanceUnit dZ = dAlt[(int) ptStart.getZ () /100];
							for( int i=0; i<iQUADCount ; ++i ) {
								//vCoveredArea[i].
								glBegin(GL_QUADS);
								glVertex3d( vPipeCoveredRegion[i].getPoint(0).getX(),vPipeCoveredRegion[i].getPoint(0).getY(),dZ );
								glVertex3d( vPipeCoveredRegion[i].getPoint(1).getX(),vPipeCoveredRegion[i].getPoint(1).getY(),dZ );
								glVertex3d( vPipeCoveredRegion[i].getPoint(2).getX(),vPipeCoveredRegion[i].getPoint(2).getY(),dZ );
								glVertex3d( vPipeCoveredRegion[i].getPoint(3).getX(),vPipeCoveredRegion[i].getPoint(3).getY(),dZ );
								glEnd();
							}
							if( iQUADCount > 0 ) {
								CreateServLocToPointGeometry( procSource, dAlt[(int)(procSource->getFloor()/SCALE_FACTOR)], ARCVector2(ptFlowStart) , dAlt[(int)(ptFlowStart.getZ()/SCALE_FACTOR)] );
								CreateServLocToPointGeometry( procDest, dAlt[(int) (procDest->getFloor()/SCALE_FACTOR)], ARCVector2(ptFlowEnd) , dAlt[(int)(ptFlowEnd.getZ()/SCALE_FACTOR)] );
							}
							else {
								//CreateServLocToPointGeometry( procSource, ptEnd , dAlt );
								CreateSourceToDestGeometry(procSource,procDest,dAlt);
							}
						}
						else if( dpai.GetTypeOfUsingPipe() == 2 ) {// useing user selected pipes
							GetDocument()->GetTerminal().m_pPipeDataSet->GetPipeListFlowCoveredRegion( ptStart,ptEnd,dpai.GetPipeVector(), vPipeCoveredRegion, ptFlowStart, ptFlowEnd );
							int iQUADCount = vPipeCoveredRegion.size();
							DistanceUnit dZ = dAlt[(int) ptEnd.getZ () /100];
							for( int i=0; i<iQUADCount ; ++i ) {
								//vCoveredArea[i].
								glBegin(GL_QUADS);
								glVertex3d( vPipeCoveredRegion[i].getPoint(0).getX(),vPipeCoveredRegion[i].getPoint(0).getY(),dZ );
								glVertex3d( vPipeCoveredRegion[i].getPoint(1).getX(),vPipeCoveredRegion[i].getPoint(1).getY(),dZ );
								glVertex3d( vPipeCoveredRegion[i].getPoint(2).getX(),vPipeCoveredRegion[i].getPoint(2).getY(),dZ );
								glVertex3d( vPipeCoveredRegion[i].getPoint(3).getX(),vPipeCoveredRegion[i].getPoint(3).getY(),dZ );
								glEnd();
							}
							if( iQUADCount > 0 ) {
								CreateServLocToPointGeometry( procSource, dAlt[(int)(procSource->getFloor()/SCALE_FACTOR)], ARCVector2(ptFlowStart) , dAlt[(int)(ptFlowStart.getZ()/SCALE_FACTOR)] );
								CreateServLocToPointGeometry( procDest, dAlt[(int)(procDest->getFloor()/SCALE_FACTOR)], ARCVector2(ptFlowEnd) , dAlt[(int)(ptFlowEnd.getZ()/SCALE_FACTOR)] );
							}
							else {
								//CreateServLocToPointGeometry( procSource, ptEnd , dAlt );
								CreateSourceToDestGeometry(procSource,procDest,dAlt);
							}
						}
						else {
							if( dpai.GetOneToOneFlag() )
							{
								if( procSource->getID()->GetLeafName() != procDest->getID()->GetLeafName() )
								{
									continue;
								}
							}
							//create geometry between procSource and procDest						
							CreateSourceToDestGeometry(procSource,procDest,dAlt);
						}
						// **** modified by tutu in 2003-2-21
					}
				}
			}
		}
	}
	glEndList();
	pDoc->UpdateAllViews(NULL);
}

C3DView* CChildFrame::GetPane(int row, int col)
{
	CWnd* pWnd = m_wndSplitter.GetPane(row, col);
	ASSERT(pWnd!=NULL);
	ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(C3DView)));
	return (C3DView*) pWnd;
}

C3DView* CChildFrame::GetActivePane(int* _pRow, int* _pCol)
{
	CWnd* pWnd = m_wndSplitter.GetActivePane(_pRow, _pCol);
	ASSERT(pWnd!=NULL);
	ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(C3DView)));
	return (C3DView*) pWnd;
}

CTermPlanDoc* CChildFrame::GetDocument() 
{
	CDocument* pDoc = GetActiveDocument();
	ASSERT(GetActiveDocument()->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	return (CTermPlanDoc*) GetActiveDocument();
}

void CChildFrame::OnViewSaveNamedView()
{
	CDlgGetString dlg(IDS_DIALOG_VIEWNAME);
	if(dlg.DoModal() == IDOK) {
		GetDocument()->SaveNamedView(dlg.m_sValue);	
	}
}


void CChildFrame::OnViewToggle2D3D()
{
	C3DView* pActiveView = GetActivePane();
	C3DCamera* pActiveCam = pActiveView->GetCamera();
	if(pActiveCam->GetProjectionType() == C3DCamera::perspective) {
		pActiveCam->Make2D();
	}
	else {
		pActiveCam->Make3D();
	}
	pActiveCam->AutoAdjustCameraFocusDistance(GetDocument()->GetActiveFloorAltitude());
	OnUpdateFrameTitle(TRUE);
	pActiveView->Invalidate(FALSE);
}

BOOL CChildFrame::SplitRow()
{
	if(m_wndSplitter.GetRowCount() > 1)
		return FALSE;
	CRect r;
	GetClientRect(&r);
	return m_wndSplitter.SplitRow(r.Height()/2);
}

BOOL CChildFrame::SplitColumn()
{
	if(m_wndSplitter.GetColumnCount() > 1)
		return FALSE;
	CRect r;
	GetClientRect(&r);
	return m_wndSplitter.SplitColumn(r.Width()/2);
}

BOOL CChildFrame::SplitRow(int _cyBefore)
{
	if(m_wndSplitter.GetRowCount() > 1)
		return FALSE;
	return m_wndSplitter.SplitRow(_cyBefore);
}

BOOL CChildFrame::SplitColumn(int _cxBefore)
{
	if(m_wndSplitter.GetColumnCount() > 1)
		return FALSE;
	return m_wndSplitter.SplitColumn(_cxBefore);
}

void CChildFrame::OnSplitHorizontal()
{
	SplitRow();
}

void CChildFrame::OnSplitVertical()
{
	SplitColumn();
}

void CChildFrame::OnUnSplit()
{
	if(m_wndSplitter.GetColumnCount() > 1)
		m_wndSplitter.DeleteColumn(1);
	if(m_wndSplitter.GetRowCount() > 1)
		m_wndSplitter.DeleteRow(1);
}

BOOL CChildFrame::UnSplitRow()
{
	if(m_wndSplitter.GetRowCount() > 1) {
		m_wndSplitter.DeleteRow(1);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CChildFrame::UnSplitColumn()
{
	if(m_wndSplitter.GetColumnCount() > 1) {
		m_wndSplitter.DeleteColumn(1);
		return TRUE;
	}
	else
		return FALSE;
}

void CChildFrame::OnTimer(UINT nIDEvent) 
{
	/*
	if(nIDEvent == m_nRecordTimerID) {
		C3DView* p3DView = GetPane(0,0);
		//p3DView->GetSnapshot();

		//ASSERT(m_pMovieWriter);
		//m_pMovieWriter->WriteFrame(m_nRecordFrameIdx, p3DView->GetDC(), m_rcRecord);
		m_nRecordFrameIdx++;
	}
	*/
	
	IRenderFrame::OnTimer(nIDEvent);
}

GLuint CChildFrame::LogoTexID() const
{
	if(glIsTexture(m_nLogoTexID))
		return m_nLogoTexID;
	else {
		//get logo bitmap
		static const DWORD dwWidth = 160;
		static const DWORD dwHeight = 18;
		static const DWORD dwByteCount = dwWidth*dwHeight*4;
		GLubyte logobits[dwByteCount];

		CBitmap bm;
		VERIFY(bm.LoadBitmap(IDB_BITMAP_LOGOARC));
		bm.GetBitmapBits(dwByteCount, (LPVOID) logobits);
	
		glGenTextures(1, &m_nLogoTexID);
		glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_nLogoTexID);

		//set texture filtering params
		glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		glTexImage2D( GL_TEXTURE_RECTANGLE_NV, 0, 3, dwWidth, dwHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*) logobits);

		return m_nLogoTexID;
	}
}

void CChildFrame::UnSelectAll()
{
	for(SelectableList::iterator itr= m_vSelected.begin();itr!= m_vSelected.end();itr++){
		(*itr)->Selected(false);
	}
	m_vSelected.clear();	
}

void CChildFrame::Select( Selectable * pSel )
{
	
	ASSERT(pSel);
	if(pSel)
	{
		
		if(pSel->GetSelectType() == Selectable::FILLET_TAXIWAY)
		{
			CFilletTaxiway3D * pFilet3D = (CFilletTaxiway3D*)pSel;
			GetAirside3D()->GetActiveAirport()->m_vFilletTaxiways.SetInEditFillet(pFilet3D->m_filetTaxway.GetID());
		}
		
		
		m_vSelected.insert(pSel);

		pSel->Selected(true);

	}

	
}

void CChildFrame::SelectObject( int objID )
{
	
}

void CChildFrame::UnSelectAllExceptObject()
{
	SelectableList vSelectObject;
	for(SelectableList::iterator itr= m_vSelected.begin();itr!= m_vSelected.end();itr++){
		Selectable * pSel = (*itr).get();
		if(pSel->GetSelectType() == Selectable::ALT_OBJECT)
		{
			vSelectObject.push_back(pSel);
		}
		else
		{
			pSel->Selected(false);
		}
	}
	m_vSelected = vSelectObject;
}

void CChildFrame::StorePanInfo()
{
	//store pane info (num rows, num cols, sizes)
	CViewDesc* pvd;

	if(GetWorkingCamera(0,0)->GetProjectionType() == C3DCamera::perspective) {//current working view type is 3D
		pvd = &(GetDocument()->GetCurModeCameras().m_userView3D);
	}
	else {
		pvd = &(GetDocument()->GetCurModeCameras().m_userView2D);
	}
	//copy cameras from working views to user view
	SaveWorkingViews(pvd);

	GetDocument()->GetCurModeCameras().saveDataSet(GetDocument()->m_ProjInfo.path,FALSE);
}