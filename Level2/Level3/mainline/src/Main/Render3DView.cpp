// Render3DView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "Render3DView.h"
#include "Render3DFrame.h"
#include "TermPlanDoc.h"
#include "Floor2.h"
#include "Floors.h"

#include "ViewMsg.h"
#include <Common/OleDragDropManager.h>
#include <Common/RenderFloor.h>
#include <Common/CARCUnit.h>
#include <Common\UnitsManager.h>
#include <Common/AircraftAliasManager.h>
#include ".\render3dview.h"
#include <InputAirside/ARCUnitManager.h>
#include <InputAirside/ALTAirport.h>
#include <InputAirside/PanoramaViewData.h>

#include <Results/OutputAirside.h>
#include <Results/EventLogBufManager.h>
#include <Results/AirsideEventLogBuffer.h>

#include <Engine/Airside/CommonInSim.h>

#include "3DCamera.h"
#include "ShapesManager.h"
//#include <Ogre/OgreRay.h>
//#include <Ogre/OgreOverlay.h>
//#include <Ogre/OgreCamera.h>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include "Placement.h"

//#include <Ogre/OgreRenderTarget.h>
//#include <Renderer/RenderEngine/RotationSeries.h>
#include <Renderer/RenderEngine/Render3DContext.h>
#include <renderer/RenderEngine/RenderBridge3D.h>
//#include <renderer/RenderEngine/OgreUtil.h>
#include ".\AnimationManager.h"
#include <commonData/3DTextManager.h>
#include <engine/BridgeConnector.h>
#include ".\AirsideLevelList.h"
#include "DlgExport3DView.h"
#include "./ShapeRenderer.h"
#include <results\ElevatorLog.h>
#include <results\ResourceElementLog.h>
#include "Engine\OnboardFlightInSim.h"
#include "InputOnBoard\OnBoardAnalysisCondidates.h"
#include "InputOnBoard\CInputOnboard.h"
#include "ARCportLayoutEditor.h"
#include "landside/Landside.h"
#include "Landside/LandsideQueryData.h"
#include "LandsideEditContext.h"

#include "MainFrm.h"

#include "DlgWalkthroughRender.h"
#include "Renderer/RenderEngine/Render3DSceneTags.h"
#include "LandSide/ShapeList.h"

#include "Engine/OnboardFlightInSim.h"
#include "Engine/GroundInSim.h"
#include "Engine/OnboardDeckGroundInSim.h"
#include "Engine/OnboardCellGraph.h"
#include "InputAirside/Surface.h"
#include "InputAirside/contour.h"

#include "Landside/IntersectionLinkGroupLogEntry.h"
#include "Renderer/RenderEngine/LandIntersection3D.h"
#include "Common/LANDSIDE_BIN.h"
#include "common/WinMsg.h"

#include "Landside/LandsidePortals.h"
#include "DlgInputAltitude.h"
#include "Landside/LandsideParkingLot.h"
#include "Common/ViewMsgInfo.h"
#include "results/LandsideEventLogBufferManager.h"
#include "landside/LandsideVehicleLogEntry.h"
#include "landside/LandsideSimLogs.h"
#include "Renderer/RenderEngine/LandsideVehicle3D.h"
#include "Renderer/RenderEngine/RenderLandside3D.h"
// #include "Renderer/RenderEngine/LandsideTrafficLight3D.h"
#include "InputAirside/ALTAirportLayout.h"
#include "Common/ViewMsg.h"
#include <shobjidl.h>




static const double CORNEREPSION = 0.0035;


const UINT CRender3DView::SEL_FLIGHT_OFFSET = 20000;
const UINT CRender3DView::SEL_VEHICLE_OFFSET =40000;


#define XZVisibleFlag 16
#define YZVisibleFlag 32
#define PERSP_VisibleFlag 64

#define PanoramaNodeName _T("3DViewPanoramaNode")

// CRender3DView

IMPLEMENT_DYNCREATE(CRender3DView, C3DDragDropView)

CRender3DView::CRender3DView()
	: m_pCamera(NULL)
	, m_bReloadPaxDispProp(true)
	, m_bReloadPaxTagDispProp(true)
	, m_bReloadFlightDispProp(true)
	, m_bReloadFlightTagDispProp(true)
	, m_bReloadVehicleDispProp(true)
	, m_bReloadVehicleTagDispProp(true)

	, m_eWalkthroughMobType(CWalkthrough::Mob_None)
	, m_e3DMovieType(movie_None)
{
	/*m_landsideVehicleDispProps->ReadData();
	m_landsideVehicleTags.ReadData(GetDocument()->GetProjectID());*/
	//for each view have a unique camera name
	m_nUpdateRedSquaresTimer = 1;
	m_bAnimDirectionChanged = (FALSE);
	m_nAnimDirection = (CEventLogBufManager::ANIMATION_FORWARD);


	m_bReloadLandsideVehicleTagDispProp = true;
	m_bReloadLandsideVehicleDispProp = true;

}

CRender3DView::~CRender3DView()
{
}

BEGIN_MESSAGE_MAP(CRender3DView, C3DDragDropView)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateRedo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	//ON_COMMAND(ID_LAYOUTLOCK, OnLayoutLock)
	ON_COMMAND(ID_DISTANCEMEASURE, OnDistanceMeasure)
	ON_COMMAND(ID_VIEW_EXPORT, OnViewExport)
	//ON_UPDATE_COMMAND_UI(ID_LAYOUTLOCK, OnUpdateLayoutLock)
	ON_UPDATE_COMMAND_UI(ID_DISTANCEMEASURE, OnUpdateDistanceMeasure)
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
// 	ON_COMMAND(ID_3DNODEOBJECTEDIT_DELETE, OnDelete)
	ON_COMMAND(ID_CTX_TOGGLEPIPEPTEDIT, OnCtxTogglepipeptedit)
	ON_UPDATE_COMMAND_UI(ID_CTX_TOGGLEPIPEPTEDIT, OnUpdateCtxTogglepipeptedit)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_GRIDSPACING, OnUpdateGridSpacing)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ANIMTIME, OnUpdateAnimTime)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ANIMATIONSPEED , OnUpdateAnimationSpeed)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_FPS, OnUpdateFPS)
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CTX_LANDPROCPROPERTIES, OnCtxProcproperties)
	ON_COMMAND(ID_CTX_LANDDELETEPROC, OnCtxLanddeleteproc)
	ON_COMMAND(ID_CTX_COPYPROCESSOR,OnCtxCopyProc)
	ON_COMMAND(ID_CTRLPATHMENU_ADDCONTROLPOINT, OnCtxAddControlPoint)
	ON_COMMAND(ID_CTRLPOINTMENU_DELETE, OnCtxDelteControlPoint)
	ON_COMMAND(ID_CTRLPOINTMENU_CHANGETYPE, OnCtxChangeCtrlPointType)

	ON_COMMAND(ID_CTX_LAYOUTDISPPROPERTIES,OnCtxLayoutObjectDisplayProp )
	ON_COMMAND(ID_LAYOUTITEM_DELETE, OnCtxDeleteLayoutItem)

	ON_COMMAND(ID_SUBMENU_LOCKOBJ, OnCtxLockALTObject)
	ON_UPDATE_COMMAND_UI(ID_SUBMENU_LOCKOBJ, OnUpdateCtxLockALTObject)

	ON_COMMAND(ID_SUBMENU_ENABLEEDIT, OnEnabelEditAltObject)
	ON_UPDATE_COMMAND_UI(ID_SUBMENU_ENABLEEDIT, OnUpdateCtxEnableEditAltobject)
	ON_COMMAND(ID_SETALTIDTUDE_NEW, OnSetPointAltitude)
	ON_COMMAND(ID_SETALTITUDE_MANAGERSAVEDALTITUDES, OnManageSavedAlt)

	//ON_COMMAND_RANGE(MENU_LEVEL_ID,MENU_LEVEL_ID+255, OnSetPointIndexAltitude)
	ON_COMMAND_RANGE(MENU_LEVEL_ID,MENU_LEVEL_ID+255, OnSnapPointToLevel)
	ON_COMMAND_RANGE(MENU_PARKINGLOT_LEVEL,MENU_PARKINGLOT_LEVEL+255, OnSnapPointToParkLotLevel)

	ON_COMMAND(ID_PARKINGLOTITEM_COPY,OnCopyParkLotItem)
	ON_COMMAND(ID_PARKINGLOTITEM_DELETE,OnDelParkLotItem)

	//ON_COMMAND(ID_LAYOUT_HIDEPROCTAGS, OnShowObjectTag)
	//ON_UPDATE_COMMAND_UI(ID_LAYOUT_HIDEPROCTAGS, OnUpdateShowObjectTag)
	ON_COMMAND(ID_PARKINGLOTITEM_PROCPRO,OnParkingLotProperty)
	ON_MESSAGE(TP_DATA_BACK,OnTempFallbackFinished)
	ON_MESSAGE(VM_PROCESSORTYPE_CHECK,OnCheckProcessorType)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CRender3DView drawing

CRender3DFrame* CRender3DView::GetParentFrame() const
{
	CFrameWnd* pFrame = C3DDragDropView::GetParentFrame();
	if (pFrame && pFrame->IsKindOf(RUNTIME_CLASS(CRender3DFrame)))
	{
		return (CRender3DFrame*)pFrame;
	}
	ASSERT(FALSE);
	return NULL;
}

void CRender3DView::CreateOnboardFlightInSim()
{
	if (GetDocument()->GetTerminal().flightSchedule->IsEmpty())
		return;
	
	Flight* pFlight = GetDocument()->GetTerminal().flightSchedule->getItem(0);

	InputOnboard* pInputOnboard = GetDocument()->GetInputOnboard();
	COnBoardAnalysisCandidates* pOnboardAnalysis = pInputOnboard->GetOnBoardFlightCandiates();

	if (pOnboardAnalysis->getCount() == 0)
		return;
	
	if(pOnboardAnalysis->getItem(0)->getCount() == 0)
		return;

	COnboardFlight* pOnboardFlight = pOnboardAnalysis->getItem(0)->getItem(0);

	m_pOnboardFlightInSim = new OnboardFlightInSim(pFlight,true,pOnboardFlight);
}

// CRender3DView message handlers

bool CRender3DView::MoveCamera( UINT nFlags, const CPoint& ptTo , int vpIdx)
{
	bool bCamChange = C3DDragDropView::MoveCamera(nFlags,ptTo, vpIdx);
	//update 
	if(bCamChange)
	{
		Ogre::Camera* pcam = m_rdc.GetViewport(vpIdx).getCamera();
		m_coordDisplayScene.UpdateCamera( pcam );

		CCameraNode cam = pcam;
		cam.Save(*m_pCamera);
	}
	return bCamChange;
}

void CRender3DView::LoadCamera(bool bRelocate/* = false*/)
{
	if (bRelocate)
		RelocateCameraData();

	Ogre::Camera* pcam = m_vpCamera.getCamera();
	CCameraNode cam(pcam);
	cam.Load(*m_pCamera);

	m_coordDisplayScene.UpdateCamera( pcam );
}

void CRender3DView::SetUserCurrentCamera(const CCameraData& newCam)
{
	Ogre::Camera* pcam = m_vpCamera.getCamera();

	CCameraNode cam(pcam);
	cam.Load(newCam);

	m_coordDisplayScene.UpdateCamera( pcam );
}

void CRender3DView::OnUpdateDrawing()
{
	Invalidate(FALSE);
}

void CRender3DView::OnFinishMeasure( double dDistance )
{
	CString strText;
	CTermPlanDoc * pDoc = GetDocument();
	if(pDoc )
	{
		if (pDoc->m_systemMode == EnvMode_AirSide || pDoc->m_systemMode == EnvMode_LandSide)	
		{
			dDistance = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),dDistance);
			strText.Format("Distance: %.2f %s", dDistance, CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()));
		}
		else
		{
			dDistance = UNITSMANAGER->ConvertLength(dDistance);
			strText.Format("Distance: %.2f %s", dDistance, UNITSMANAGER->GetLengthUnitString());
		}
	}
	AfxMessageBox(strText, MB_OK|MB_ICONINFORMATION);
}


void CRender3DView::SetupViewport()
{
	if(!GetModelEditScene().IsValid())
		GetModelEditScene().Setup(GetEditor());

	CCameraNode camera = GetModelEditScene().GetCamera(mViewCameraName);
	Ogre::Camera* pcam = camera.mpCamera;
	RelocateCameraData();
	ASSERT(m_pCamera);
	camera.Load(*m_pCamera);


	int nVpIndex = m_rdc.AddViewport( pcam, true );
	m_vpCamera = m_rdc.GetViewport(nVpIndex);
	m_vpCamera.SetVisibleFlag(NormalVisibleFlag|PERSP_VisibleFlag);
	m_vpCamera.SetClear(RGB(164,164,164));
	GetModelEditScene().Get3DTags().RegShowInViewport(m_vpCamera);

	//set up coord displace scene
	m_coordDisplayScene.Setup();
	m_coordDisplayScene.UpdateCamera( pcam );
	Ogre::Camera* pCoordCam = m_coordDisplayScene.GetLocalCamera();
	nVpIndex = m_rdc.AddViewport( pCoordCam,false);
	m_rdc.GetViewport(nVpIndex).SetClear(0,false,false,false);
	m_rdc.SetViewportDim(nVpIndex,CRect(0,0,80,80),VPAlignPos(VPA_Left,VPA_Bottom),true);

	
	GetModelEditScene().AddListener(this);

	/*CTermPlanDoc* pDoc = GetDocument();
	CFloor2* pActiveFloor = pDoc->GetActiveFloor();
	GetModelEditScene().SetWorkingHeight(pActiveFloor ? pActiveFloor->Altitude() : 0.0);*/
	//UpdateShapesByMode(pDoc->m_systemMode);

	VERIFY(InitSnapshotBasicOverlay());

	
}

void CRender3DView::UnSetupViewport()
{
	GetModelEditScene().RemoveListener(this);
}

CRender3DScene& CRender3DView::GetModelEditScene()
{
	return GetParentFrame()->mModelEditScene;
}

CARCportLayoutEditor* CRender3DView::GetEditor() const
{
	return GetDocument()->GetLayoutEditor();
}

void CRender3DView::OnInitialUpdate()
{
	C3DDragDropView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class
	m_nUpdateRedSquaresTimer = SetTimer(m_nUpdateRedSquaresTimer, 1000, NULL);
	RelocateCameraData();
	InitSeledPaxList();
	CreateOnboardFlightInSim();
	Invalidate(FALSE);
}

bool CRender3DView::IsLayoutLock() const
{
	return !GetEditor()->IsEditable();
}


C3DSceneBase* CRender3DView::GetScene( Ogre::SceneManager* pScene )
{
	if (GetModelEditScene().getSceneManager() == pScene)
		return &GetModelEditScene();
	if(m_coordDisplayScene.getSceneManager() == pScene)
		return &m_coordDisplayScene;
	return NULL;
}


void CRender3DView::OnUpdateUndo(CCmdUI* pCmdUI)
{
	// TODO: Add your command handler code here
	pCmdUI->Enable(!IsLayoutLock() && GetEditor()->GetCurContext()->CanUndo());
}

void CRender3DView::OnUpdateRedo(CCmdUI* pCmdUI)
{
	// TODO: Add your command handler code here
	pCmdUI->Enable(!IsLayoutLock() && GetEditor()->GetCurContext()->CanRedo());
}

//void CRender3DView::OnUpdateLayoutLock( CCmdUI* pCmdUI )
//{
//	pCmdUI->SetCheck(IsLayoutLock());
//}

void CRender3DView::OnUpdateDistanceMeasure( CCmdUI* pCmdUI )
{
	pCmdUI->SetCheck(GetModelEditScene().isInDistanceMeasure());
}

void CRender3DView::OnUpdateCtxTogglepipeptedit( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(TRUE);
	//CTerminal3DObject::SharedPtr p3DObj = GetEditor()->GetSelObj3D();
	//if (p3DObj->GetBaseObject())
	//{
	//	// thought as pipe
	//	CRenderPipe3D* pPipe = (CRenderPipe3D*)p3DObj.get();
	//	pCmdUI->SetText(pPipe->GetEnableEdit() ? "Disable Point Editing" : "Enable Point Editing");
	//}
}



void CRender3DView::OnDistanceMeasure()
{
	// TODO: Add your command handler code here
	GetModelEditScene().BeginMouseOperation(this->GetSafeHwnd(),DISTANCE_MEARSURE);
	Invalidate(FALSE);
}

void CRender3DView::OnCtxTogglepipeptedit()
{
	// TODO: Add your command handler code here
	//CTerminal3DObject::SharedPtr p3DObj = GetEditor()->GetSelObj3D();
	//if (p3DObj->GetBaseObject())
	//{
	//	// thought as pipe
	//	CRenderPipe3D* pPipe = (CRenderPipe3D*)p3DObj.get();
	//	pPipe->EnableEdit(!pPipe->GetEnableEdit());
	//}
	Invalidate(FALSE);
}

//void CRender3DView::OnLayoutLock()
//{
//	GetModelEditScene().FlipLayoutLocked();
//}

bool CRender3DView::IsDropAllowed( const DragDropDataItem& dropItem ) const
{
	return !IsLayoutLock() && dropItem.GetDataType() == DragDropDataItem::Type_TerminalShape;
}

void CRender3DView::DoDrop( const DragDropDataItem& dropItem, CPoint point )
{
	// Not implemented
	ASSERT(FALSE);
}

CTermPlanDoc* CRender3DView::GetDocument() const
{
	return (CTermPlanDoc*)CView::GetDocument();
}


CRenderFloor* CRender3DView::FetchFloors(CRenderFloorList& floors, EnvironmentMode mode) const
{
	CFloorList& floorRaw = GetDocument()->GetFloorByMode(mode).m_vFloors;
	floors.insert(floors.end(), floorRaw.begin(), floorRaw.end());

	//CFloorList& floorAirside  =GetDocument()->GetFloorByMode(EnvMode_AirSide).m_vFloors;
	//floors.insert(floors.end(), floorAirside.begin(), floorAirside.end());

	CRenderFloorList::iterator ite = std::find_if(floors.begin(), floors.end(), boost::BOOST_BIND(&CRenderFloor::IsActive, _1));
	return floors.end() == ite ? *ite : NULL;
}
void CRender3DView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (lHint)
	{	
	case VM_RELOAD_PAX_DISPLAY_PROPERTY:
		{
			m_bReloadPaxDispProp = true;	
			Invalidate();
		}
		break;
	case VM_RELOAD_PAX_TAG_PROPERTY:
		{
			m_bReloadPaxTagDispProp = true;
			Invalidate();
		}
		break;
	case VM_RELOAD_FLIGHT_DISPLAY_PROPERTY:
		{
			m_bReloadFlightDispProp = true;
			Invalidate();
		}
		break;
	case VM_RELOAD_FLIGHT_TAG_PROPERTY:
		{
			m_bReloadFlightTagDispProp = true;
			Invalidate();
		}
		break;
	case VM_RELOAD_VEHICLE_DISPLAY_PROPERTY:
		{
			m_bReloadVehicleDispProp = true;
			m_bReloadLandsideVehicleTagDispProp = true; //need add new msg id , add it for tmp
			Invalidate();
		}
		break;
	case VM_RELOAD_VEHICLE_TAG_PROPERTY:
		{
			m_bReloadVehicleTagDispProp = true;
			Invalidate();
		}
		break;	
	case VM_RELOAD_LANDSIDE_VEHICLE_DISPLAY_PROPERTY:
		{
			m_bReloadLandsideVehicleDispProp = true;
			Invalidate();
		}
		break;
	case VM_RELOAD_LANDSIDE_VEHICLE_TAG_PROPERTY:
		{
			m_bReloadLandsideVehicleTagDispProp = true;
			Invalidate();
		}
		break;
	case VM_REDRAW:
		{
			Invalidate(FALSE);
		}
		break;
	case VM_UPDATETRACE:
		{
			UpdateLandsideVehicleTraces();
			Invalidate();
		}
		break;
	default:
		{
			bool bUpdate = GetModelEditScene().OnUpdate(lHint,pHint);			
			if(bUpdate)
				Invalidate(FALSE);
		}		
	}
}

void CRender3DView::UpdateAll()
{
	//EnvironmentMode curMode = GetDocument()->GetCurrentMode();
	////update terminal
	//{
	//	CRenderFloorList floors;
	//	CRenderFloor* pActiveFloor = FetchFloors(floors, EnvMode_Terminal);
	//	GetModelEditScene().UpdateTerminal(floors);		
	//}
	////update airside
	//{
	//	GetModelEditScene().UpdateAirpace();
	//	GetModelEditScene().UpdateTopgraphy();
	//	GetDocument()->GetInputAirside().GetActiveAirport();
	//	int nAirportCount = GetDocument()->GetInputAirside().GetAirportCount();
	//	for(int i=0;i<nAirportCount;i++)
	//	{
	//		ALTAirportLayout* pairport = GetDocument()->GetInputAirside().GetAirportbyIndex(i);
	//		CAirsideLevelList mlevelist(pairport->getID());
	//		mlevelist.ReadFullLevelList();
	//		CRenderFloorList floors;
	//		for(int j=0;j<mlevelist.GetCount();j++)
	//			floors.push_back(&mlevelist.GetLevel(j));			
	//		GetModelEditScene().UpdateAirport(pairport->getGuid(),floors);
	//	}
	//}
	////update landside	
	//{
	//	CRenderFloorList floors;
	//	CRenderFloor* pActiveFloor = FetchFloors(floors, EnvMode_LandSide);
	//	GetModelEditScene().UpdateLandside(floors,GetDocument()->GetInputLandside()->GetPortals());	
	//}
}

//void CRender3DView::BeginPickup(FallbackData* pData, LPARAM lParam)
//{
//	GetModelEditScene().BeginPickup(pData, lParam);
//// 	m_vSelectLandProcs.clear();
//// 	m_vSelectedStretchPos.clear();
//}

void CRender3DView::BeginPlaceAlignMarker()
{
	GetModelEditScene().BeginMouseOperation(this->GetSafeHwnd(),PLACE_ALIGN_MARKER);
}

void CRender3DView::UpdateAlignMarker(ARCVector3 ptMousePos)
{
	ReleaseCapture();
	//set marker location to active floor
	CRenderFloor* pActiveFloor = GetDocument()->GetActiveFloor();
	if (pActiveFloor)
	{
		CTermPlanDoc* pDoc = GetDocument();
		pDoc->SetActiveFloorMarker(ptMousePos);
		//GetModelEditScene().UpdateOneFloor(pActiveFloor);
		if(pDoc->GetCurrentMode() == EnvMode_Terminal)
			pDoc->ActivateFloor(pDoc->m_nLastActiveFloor);
		Invalidate(FALSE);
	}
}
int CRender3DView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (C3DDragDropView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	DRAG_DROP_VIEW_REGIST(this);

	return 0;
}

void CRender3DView::OnDelete()
{
// 	GetParentFrame()->UpdateViews(NULL, C3DObjModelEditFrame::Type_DeleteItem, (DWORD)GetEditor()->GetSelNodeObj().GetSceneNode());
}

void CRender3DView::OnEditUndo()
{
	if (!IsLayoutLock())
	{
		CEditOperationCommand* pCmd = GetEditor()->GetCurContext()->Undo();
		GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pCmd);		
	}
}

void CRender3DView::OnEditRedo()
{
	if (!IsLayoutLock())
	{
		CEditOperationCommand* pCmd =GetEditor()->GetCurContext()->Redo();
		GetDocument()->UpdateAllViews(NULL,VM_UPDATE_COMMAND,(CObject*)pCmd);
	}
}

void CRender3DView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (VK_DELETE == nChar)
	{
		OnDelete();
	}
	else if (VK_RETURN == nChar)
	{
		GetEditor()->GetCurContext()->DoneCurEditOp();
	}
	else if (VK_ESCAPE == nChar)
	{
		GetEditor()->GetCurContext()->CancelCurEditOp();
		
		Invalidate(FALSE);
	}
	C3DDragDropView::OnKeyDown(nChar, nRepCnt, nFlags);
}



bool CRender3DView::GetViewPortMousePos(ARCVector3& pos)
{
	POINT point;
	if (GetCursorPos(&point))
	{
		ScreenToClient(&point);
		return GetMouseWorldPos(point,pos);		
	}
	return false;
}

void CRender3DView::OnUpdateGridSpacing(CCmdUI *pCmdUI)
{
	CTermPlanDoc* pDoc = GetDocument();
	CRenderFloor* pActiveFloor = pDoc->GetActiveFloor();
	if (pActiveFloor)
	{
		CGrid* pGrid = pActiveFloor->GetGrid();
		ASSERT(pGrid != NULL);
		double ds = pGrid->dLinesEvery;
		CString s;
		CUnitsManager* pUM = CUnitsManager::GetInstance();
		if(pDoc)
		{
			if (pDoc->m_systemMode == EnvMode_AirSide || pDoc->m_systemMode == EnvMode_LandSide)
			{
				s.Format("\tGrid Spacing: %5.2f %s",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),ds),CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()));
			}
			
			else
			{
				s.Format("\tGrid Spacing: %5.2f %s",pUM->ConvertLength(ds),pUM->GetLengthUnitString(pUM->GetLengthUnits()));
			}
		}
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText( s );
	}
}

void CRender3DView::OnUpdateAnimTime(CCmdUI *pCmdUI)
{
	CTermPlanDoc* pDoc = GetDocument();
	if(pDoc->m_eAnimState != CTermPlanDoc::anim_none)
	{
		CString str = pDoc->m_animData.GetTimeTag();
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText( str );
		pCmdUI->m_pOther->UpdateWindow();
	}
	else
	{
		if(pCmdUI){
// 			if( GetParentFrame()->IsLoadingModel() )
// 			{
// 				CString loadingText;
// 				loadingText.Format("Loading Shapes (%d %)...", (int)GetParentFrame()->GetLoadingPercent() * 100);
// 				pCmdUI->Enable(TRUE);
// 				pCmdUI->SetText(loadingText);
// 				pCmdUI->m_pOther->UpdateWindow();
// 			}
// 			else
// 			{
			if(pDoc->m_eAnimState == CTermPlanDoc::anim_none)
			{
				pCmdUI->Enable(FALSE);
			}				
// 			}
		}
	}

}

void CRender3DView::OnUpdateAnimationSpeed(CCmdUI* pCmdUI)
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetParentFrame()->GetActiveDocument();
	if(pDoc->m_eAnimState != CTermPlanDoc::anim_none)
	{
		CString s(_T(""));
		s.Format("\tSpeed: %5dx",pDoc->m_animData.nAnimSpeed/10);
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText( s );
		pCmdUI->m_pOther->UpdateWindow();
	}
	else
	{
		if(pCmdUI){
			if(pDoc->m_eAnimState == CTermPlanDoc::anim_none)
			{
				pCmdUI->Enable(FALSE);
			}				
		}
	}
}

void CRender3DView::OnUpdateFPS(CCmdUI* pCmdUI)
{
	CString s;
	s.Format("\tFPS: %5.2f",m_dFrameRate);
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText( s );
}

void CRender3DView::OnDraw( CDC* pDC )
{
	C3DDragDropView::OnDraw(pDC);
	CRender3DFrame* pRender3DFrame = GetParentFrame();
	pRender3DFrame->GetStatusBar().OnUpdateCmdUI(pRender3DFrame, FALSE);
	//UpdateLandsideVehicleTraces();
}

void CRender3DView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	C3DDragDropView::OnMouseMove(nFlags, point);

	CRender3DFrame* pRender3DFrame = GetParentFrame();
	pRender3DFrame->GetStatusBar().OnUpdateCmdUI(pRender3DFrame, FALSE);

}



void CRender3DView::PrepareDraw()
{
	C3DDragDropView::PrepareDraw();
	CTermPlanDoc* pDoc = GetDocument();

	//update text show 
	if(GetEditor()->IsShowText())
		m_vpCamera.addVisibilityFlags(ProcessorTagVisibleFlag);
	else
		m_vpCamera.removeVisibilityFlags(ProcessorTagVisibleFlag);
	//bool bSharp = pDoc->m_bshw
	// 
	//update pax
	if(pDoc->m_eAnimState != CTermPlanDoc::anim_none)
	{
		UpdateAnimationShapes();
		UpdateMovieCamera();
	}
	else if( m_lastAnimState!= pDoc->m_eAnimState  )
	{
		GetModelEditScene().StopAnim();
	}
	
	m_lastAnimState = pDoc->m_eAnimState;
}

//static void GetMobileElmentOnDifferentFloorZ(const MobEventStruct& pes, double* dAlt,double& z)
//{
//	int nFloorLow = min(pes.nFromFloor,pes.nToFloor);
//	int nFloorHigh = max(pes.nFromFloor,pes.nToFloor);
//	int nDeta = nFloorHigh - nFloorLow;
//	double mult;
//	if (nDeta)
//	{
//		mult = (pes.z/SCALE_FACTOR - nFloorLow)/(nDeta);
//	}
//	else
//	{
//		mult = (pes.z/SCALE_FACTOR - nFloorLow);
//	}
//
//	double dAltDiff = 0;
//
//
//	dAltDiff = dAlt[nFloorHigh]-dAlt[nFloorLow];
//	z = dAlt[nFloorLow]+mult*dAltDiff;
//}

#define MAX_FLOOR	64

#ifdef _DEBUG
#include <Renderer/RenderEngine/Shape3DMeshManager.h>
#endif

void CRender3DView::UpdateAnimationShapes()
{
#ifdef _DEBUG // sometimes you can modify the static value to force Shape .ini file reloaded
	static bool s_bToReloadShapes = false;
	if (s_bToReloadShapes)
	{
		CShape3DMeshManager::GetInstance().Reload(PROJMANAGER->GetAppPath());
		s_bToReloadShapes = false;
	}
#endif

	CTermPlanDoc* pDoc = GetDocument();	
	AnimationData* pAnimData = &(pDoc->m_animData);
	long nTime = pAnimData->nLastAnimTime;
	if( GetModelEditScene().m_lastAnimaTime==nTime && !m_bReloadPaxDispProp && !m_bReloadPaxTagDispProp)
		return;
	
	GetModelEditScene().UpdateAnim(nTime);

	double dAlt[MAX_FLOOR+1];
	BOOL bOn[MAX_FLOOR+1];

	CFloorList& floorListTerm = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors;
	int nFloorCount = floorListTerm.size();
	for(int i=0; i<nFloorCount; i++)
	{
		// the Altitude of every floor
		dAlt[i] = floorListTerm[i]->Altitude();
		// if the floors are visible
		bOn[i] = floorListTerm[i]->IsVisible();
	}


	if (pDoc->m_animData.m_AnimationModels.IsAirsideSel() || pDoc->m_animData.m_AnimationModels.IsTerminalSel() )
	{
		DistanceUnit dAirportAlt = GetParentFrame()->GetAltAirport().getElevation();
		UpdateAnimationFlight(dAirportAlt,FALSE, !pDoc->m_bHideACTags);
	}	
	if(pDoc->m_animData.m_AnimationModels.IsTerminalSel() || pDoc->m_animData.m_AnimationModels.IsOnBoardSel()
|| pDoc->m_animData.m_AnimationModels.IsLandsideSel() )
	{
		UpdateAnimationPax(pDoc,dAlt,bOn);
	}
	if(pDoc->m_animData.m_AnimationModels.IsAirsideSel() )
	{
		DistanceUnit dAirportAlt = GetParentFrame()->GetAltAirport().getElevation();
		UpdateAnimationVehicle(dAirportAlt,FALSE, !pDoc->m_bHideACTags);
	}	
	if(pDoc->m_animData.m_AnimationModels.IsLandsideSel())
	{
		UpdateLandsideVehicleAnima(FALSE, !pDoc->m_bHideACTags);
		UpdateLandsideTrafficLightAnima();
	}

 	UpdateAnimationProcessor(pDoc,dAlt,bOn);	
 	UpdateAnimationEscaltor(pDoc,dAlt,bOn);
 	UpdateAnimationResoureElm(pDoc,dAlt,bOn);
 	UpdateAnimationTrain(pDoc,dAlt,bOn);
}

void CRender3DView::UpdateAnimationPax(CTermPlanDoc* pDoc, double* dAlt , BOOL* bOn)
{
	AnimationData* pAnimData = &(pDoc->m_animData);
	long nTime = pAnimData->nLastAnimTime;

	// load data if necessary
	if(pDoc->m_eAnimState==CTermPlanDoc::anim_pause) {
		if(m_bAnimDirectionChanged) {
			pDoc->GetTerminal().m_pLogBufManager->InitBuffer();
			m_bAnimDirectionChanged = FALSE;
		}
		pDoc->GetTerminal().m_pLogBufManager->loadDataIfNecessary( nTime, (CEventLogBufManager::ANIMATION_MODE)m_nAnimDirection,pDoc );
	}
	else if(pDoc->m_eAnimState==CTermPlanDoc::anim_playF) {
		pDoc->GetTerminal().m_pLogBufManager->loadDataIfNecessary( nTime, CEventLogBufManager::ANIMATION_FORWARD,pDoc );
	}
	else if(pDoc->m_eAnimState==CTermPlanDoc::anim_playB) {
		pDoc->GetTerminal().m_pLogBufManager->loadDataIfNecessary( nTime, CEventLogBufManager::ANIMATION_BACKWARD,pDoc );
	}

	//update pax status
	MobLogEntry element;
	PaxLog* pPaxLog = pDoc->GetTerminal().paxLog;
	const PaxEventBuffer& logData = pDoc->GetTerminal().m_pLogBufManager->getLogDataBuffer();

	CPaxDispProps& paxdispprops = pDoc->m_paxDispProps;
	const std::vector<int>& paxdisppropidx = pDoc->m_vPaxDispPropIdx;
	const std::vector<int>& paxtagpropidx = pDoc->m_vPaxTagPropIdx;
	CPaxTags& paxtagprops = pDoc->m_paxTags;

	bool bReloadPaxDispProp = m_bReloadPaxDispProp;
	m_bReloadPaxDispProp = false;

	

	CAnimaPax3DList& pax3DList = GetModelEditScene().GetPax3DList();
	CMovableTagList& paxTagList = GetModelEditScene().Get3DTags().m_paxTags;
	paxTagList.HideAll();
	pax3DList.UpdateAllMobile(false);
	
	for( PaxEventBuffer::const_iterator it = logData.begin(); it!= logData.end(); ++it )
	{	

		CPaxDispPropItem* pPDPI = paxdispprops.GetDispPropItem(paxdisppropidx[it->first]);
		CPaxTagItem* pPTI = paxtagprops.GetPaxTagItem(paxtagpropidx[it->first]);
		pPaxLog->getItem(element, it->first);
		int nPaxID= element.getID();
	
		if(pPDPI && pPDPI->IsVisible())
		{
			const MobDescStruct& pds = element.GetMobDesc();	
			long nStartTime = (long) element.getEntryTime();
			long nEndTime = (long) element.getExitTime();

			if(nStartTime <= nTime && nEndTime >= nTime) 
			{
				bool bCreated = false;
				CAnimaPax3D* pPax3D = pax3DList.CreateOrRetrieveMob3D(nPaxID, bCreated);
				if (bCreated)
					CRender3DScene::SetSceneNodeQueryDataInt(*pPax3D, SceneNode_Pax, pPax3D->GetPaxID());
				if (bCreated || bReloadPaxDispProp /*|| !pax3DList.IsMob3DShow(pPax3D)*/)
				{
					pPax3D->SetShape(pPDPI->GetShape());
					pPax3D->SetColor(pPDPI->GetColor());
				}

				const part_event_list& part_list = it->second;				

				int pesAIdx = 0;

				AnimationPaxGeometryData geoData;
				geoData.bBestSpotAngle = (m_e3DMovieType == movie_TrackFirstPerson || m_e3DMovieType == movie_TrackThirdPerson)
					&& m_movieNode && m_movieNode.HoldSameSceneNode(*pPax3D); // when use pax' first person track view, calculate best spot angle

				PaxAnimationData animaPaxData = pPax3D->GetAnimationData();

				//draw pax
				if( CAnimationManager::GetPaxShow(pDoc,part_list,nTime,bOn,dAlt,geoData,animaPaxData,pesAIdx) )
				{
					if (geoData.bBestSpotAngle)
					{
						m_dBestSpotAngle = geoData.bestSpotAngle;
					}

					ARCVector4 quat(0,0,0,1);
					if(geoData.bNeedRotate)
					{
						quat.x = element.getAngle().x;
						quat.y = element.getAngle().y;
						quat.z = element.getAngle().z;
						quat.w = element.getAngle().w;
					}

					const MobEventStruct& pesA = part_list[pesAIdx];	
					bool bAttachToFlight =false;
					if(pesA.state>=EntryOnboard && pesA.state<EntryLandside) //onboard animation
					{
						CAnimaFlight3DList& flight3DList = GetModelEditScene().GetFlight3DList();
						if( nTime < pds.enplanTime )
						{
							CAnimaFlight3D* pFlight3D = flight3DList.GetMob3D(pds.arrFlight);
							if(pFlight3D)
							{
								pPax3D->AttachTo(*pFlight3D);
								bAttachToFlight = true;
							}
						}
						else
						{
							CAnimaFlight3D* pFlight3D = flight3DList.GetMob3D(pds.depFlight);
							if(pFlight3D)
							{
								pPax3D->AttachTo(*pFlight3D);
								bAttachToFlight = true;
							}
						}

						pPax3D->SetAnimationShape(pPDPI->GetShape());
						pPax3D->SetAnimationData(animaPaxData);


						pPax3D->SetRotation(quat);
						pPax3D->Rotate( ARCVector3(0.0, 0.0, geoData.dAngleFromX) ); // Benny Tag
					}
					else
					{
						//if(pPax3D->GetShapeName().Find(_T("Casual Man")) >= 0)
							//pPax3D->SetRotation( ARCVector3(0.0, 0.0, geoData.dAngleFromX + 90) ); // Benny Tag
						/*else*/
						pPax3D->SetRotation(quat);
						pPax3D->Rotate( ARCVector3(0.0, 0.0, geoData.dAngleFromX) );
					}

					if(!bAttachToFlight)
					{
						pPax3D->AttachTo(GetModelEditScene().GetRoot());
					}							
					pPax3D->SetPosition( geoData.pos );


					if( pPTI->IsShowTags() && !pDoc->m_bHideACTags )
					{
						CString sPaxTag;
						CAnimationManager::BuildPaxTag(pDoc,pPTI->GetTagInfo(), pds, pesA, sPaxTag);
						paxTagList.SetTag(nPaxID,pPax3D->GetSceneNode(),sPaxTag);	
						paxTagList.Show(nPaxID,true);
					}					
					continue;
				}
			}
		}
	
		//default hide the pax
		//pax3DList.ShowMob3D(nPaxID,false);
		//paxTagList.Show(nPaxID,false);
	}	
}

void CRender3DView::UpdateShapesByMode(EnvironmentMode mode)
{
	//GetModelEditScene().GetFloors().AttachTo(GetModelEditScene().GetRoot());
 	/*switch (mode)
	{
	case EnvMode_AirSide:
		{
			GetModelEditScene().GetFloors().Detach();
			GetModelEditScene().GetPax3DList().UpdateAllPax(false);
		}
		break;
	case EnvMode_Terminal:
		{
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
		break;
	}*/
}

void CRender3DView::UpdateAnimationFlight(const DistanceUnit& airportAlt,BOOL bSelectMode,BOOL bShowTag)
{
	CTermPlanDoc* pDoc = GetDocument();
	CAirsideSimLogs& airsideSimLogs = pDoc->GetAirsideSimLogs();
	long nCurTime = pDoc->m_animData.nLastAnimTime;

	CAircraftDispProps& aircraftdispprops = pDoc->m_aircraftDispProps;
	CAircraftTags& aircrafttags = pDoc->m_aircraftTags;
	CAnimaFlight3DList& flight3DList = GetModelEditScene().GetFlight3DList();
	CMovableTagList& flightTags = GetModelEditScene().Get3DTags().m_fltTags;

	std::vector< std::pair<int,int> >& aircraftdisppropidx = pDoc->m_vACDispPropIdx;
	std::vector<int>& aircraftshapeidx = pDoc->m_vACShapeIdx;
	std::vector< std::pair<int,int> >& aircrafttagpropidx = pDoc->m_vACTagPropIdx;



	AirsideFlightLogEntry element;
	element.SetEventLog(&(airsideSimLogs.m_airsideFlightEventLog));

	AirsideFlightLog& airsideFlightLog = airsideSimLogs.m_airsideFlightLog;
	//	int nCount = airsideFlightLog.getCount();

	const CAirsideEventLogBuffer<AirsideFlightEventStruct> &airsideFlightEventLog = pDoc->GetOutputAirside()->GetLogBuffer()->m_flightLog;
	size_t nCount = airsideFlightLog.getCount();//airsideFlightEventLog.m_lstElementEvent.size();

	bool bReloadFlightDispProp = m_bReloadFlightDispProp;
	m_bReloadFlightDispProp = false;

	for (size_t i = 0; i < nCount; i++)
	{
		const CAirsideEventLogBuffer<AirsideFlightEventStruct>::ELEMENTEVENT& ItemEventList = airsideFlightEventLog.m_lstElementEvent[i];

		airsideFlightLog.getItem(element, ItemEventList.first);

		// the entry time and exit time of every flight
		long nStartTime = (long) element.getEntryTime();
		long nEndTime = (long) element.getExitTime();
		const AirsideFlightDescStruct& flightdesc = element.GetAirsideDesc();

		// test if it's the time for stop in airport
		if (nStartTime <= nCurTime && nEndTime >= nCurTime) 
		{			
			
			ARCVector3 pos;
			ARCVector3 dangles;
			double dSpeed =0;
			int pesAIdx = -1;

			if( CAnimationManager::GetFlightShow(ItemEventList,nCurTime,airportAlt
				,pos,dangles,dSpeed,pesAIdx)
				)
			{
				const AirsideFlightEventStruct& pesA = ItemEventList.second[pesAIdx];
				CAircraftDispPropItem* pADPI = NULL; 
				CAircraftTagItem* pATI =NULL;
				if(pesA.state == 'A')
				{
					pADPI = aircraftdispprops.GetDispPropItem(aircraftdisppropidx[i].first);		
					pATI = aircrafttags.GetTagItem(aircrafttagpropidx[i].first);
				}
				else
				{
					pADPI = aircraftdispprops.GetDispPropItem(aircraftdisppropidx[i].second);
					pATI = aircrafttags.GetTagItem(aircrafttagpropidx[i].second);
				}

				if( !(pADPI && pADPI->IsVisible()) )
					continue;

				bool bCreated = false;
				CAnimaFlight3D* pFlight3D = flight3DList.CreateOrRetrieveMob3D(flightdesc.id, bCreated);
				if(!pFlight3D)
					continue;

				bool bOnboard = flightdesc.nOnboardArrID>=0 || flightdesc.nOnboardDepID>=0;
				if (bCreated)
				{	
					pFlight3D->SetShape(flightdesc.acType, flightdesc.dLength,flightdesc.dWingSpan,bOnboard?GetDocument()->GetInputOnboard():NULL);							

					CRender3DScene::SetSceneNodeQueryDataInt(*pFlight3D, SceneNode_Flight, pFlight3D->GetFlightID());
				}
				if (bCreated || bReloadFlightDispProp || !flight3DList.IsMob3DShow(pFlight3D)) // set mat when created, color modified, re-showed
				{
					pFlight3D->SetMatByAirlineColor(pADPI->IsLogoOn() ? flightdesc._airline.GetValue() : _T(""), pADPI->GetColor(),bOnboard);
				}
				
				//update onboard layout if necessary
				if( pesA.state == 'A' &&  flightdesc.nOnboardArrID>=0 )
				{								
					pFlight3D->LoadLayout(GetDocument()->GetInputOnboard(),flightdesc.nOnboardArrID);
				}
				else if(pesA.state == 'D' && flightdesc.nOnboardDepID>=0 )
				{
					pFlight3D->LoadLayout(GetDocument()->GetInputOnboard(),flightdesc.nOnboardDepID);
				}
				
				//pFlight3D->updateOnboardFlight(m_pOnboardFlightInSim);
#ifdef DEBUG
				//DrawDeckCell(pFlight3D, m_pOnboardFlightInSim);
#endif

				flight3DList.ShowMob3D(pFlight3D, true);
				
				//------------------------------------------------------------
				double scale = pADPI->GetScaleSize();
				if(pos[VZ] < 10000 )
					scale = 1.0 +  (scale-1.0) * (pos[VZ]) / 10000.0; 

				pFlight3D->SetPosition( pos );
				pFlight3D->SetScale( ARCVector3(scale,scale,scale) ); // ??##	
				pFlight3D->SetRotation( dangles ,ER_YXZ);

				if(pATI && pATI->IsShowTags()&&bShowTag)
				{
					CString sTag;
					int h, w;
					bool bExpanded = false;
					std::vector<C3DTextManager::TextFormat> fmts;
					CAnimationManager::BuildFlightTag(flightdesc, pesA, *pATI, false, sTag, fmts, h, w,dSpeed,pos[VZ]);
					flightTags.SetTag(flightdesc.id,pFlight3D->GetSceneNode(),sTag);
					flightTags.Show(flightdesc.id,true);
				}
				else
					flightTags.Show(flightdesc.id,false);

				continue;				
			}
		}
		flight3DList.ShowMob3D(flightdesc.id, false); // hide flight by default
		flightTags.Show(flightdesc.id,false);
		

	}
}
void CRender3DView::DrawDeckCell(CAnimaFlight3D* pFlight3D, OnboardFlightInSim* pOnboardFlightInSim) const
{

	for (int i = 0; i < 1; i++)
	{
		OnboardDeckGroundInSim* pDeckGroupInSim = pOnboardFlightInSim->GetDeckInSim(i);
		if(pDeckGroupInSim == NULL)
			break;

		GroundInSim* pGroudInSim = pDeckGroupInSim->GetGround();
		RectMap& rectMap = pGroudInSim->getLogicMap();

		//draw cell path
		pOnboardFlightInSim->getCellGraph()->loadFile();
		int nPathCount = (int)pOnboardFlightInSim->getCellGraph()->m_mapCellPath.size();

		for (int nPath = 0; nPath < nPathCount; ++nPath)
		{
			std::vector<std::pair<ARCColor3, Path > > vCells;
			std::map< std::pair<OnboardCellInSim*, OnboardCellInSim *>, OnboardCellPath >::iterator iter = 
																				pOnboardFlightInSim->getCellGraph()->m_mapCellPath.begin();

			OnboardCellPath& cellPath = (*iter).second;
			for (int j = 0; j <cellPath.getCellCount(); j++)
			{
				OnboardCellInSim* pCell = cellPath.getCell(j);
				ARCColor3 color;
				color = ARCColor3(0,255,0);
				vCells.push_back(std::make_pair(color, pCell->GetPath()));
			}
			pFlight3D->DrawDeckCells(i,"",vCells);
		}




		//std::vector<std::pair<ARCColor3, Path > > vCells;
		//for (int j = 0; j <rectMap.GetTileCount(); j++)
		//{
		//	OnboardCellInSim* pCell = pGroudInSim->getCell(j);
		//	ARCColor3 color;
		//	if (pCell->entryPoint())
		//	{
		//		color = ARCColor3(0,0,255);
		//	}
		//	else if (pCell->GetState() == OnboardCellInSim::Idle)
		//	{
		//		color = ARCColor3(0,255,0);
		//	}
		//	else if(pCell->GetState() == OnboardCellInSim::HalfOccupied)
		//	{
		//		color = ARCColor3(128,0,0);
		//	}
		//	else if(pCell->GetState() == OnboardCellInSim::NoPath)
		//	{
		//		color = ARCColor3(128, 128,255);
		//	}
		//	else 
		//	{
		//		color = ARCColor3(255,0,0);
		//	}

		//	if(j%2 == 0)
		//		vCells.push_back(std::make_pair(color, pCell->GetPath()));

		//}
		//pFlight3D->DrawDeckCells(i,"",vCells);
	}
}
void CRender3DView::UpdateAnimationVehicle(const DistanceUnit& dAirprotAlt, BOOL bSelectMode,BOOL _bShowTag)
{
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CAnimaVehicle3DList& vehicle3DList = GetModelEditScene().GetVehicle3DList();
	CMovableTagList& vehicleTags = GetModelEditScene().Get3DTags().m_vehicleTags;

	//#if 0	
	CAirsideSimLogs& airsideSimLogs = pDoc->GetAirsideSimLogs();
	long nCurTime = pDoc->m_animData.nLastAnimTime;

	CVehicleDispProps& vehicledispprops = pDoc->m_vehicleDispProps;
	std::vector<int>& vehicledisppropidx = pDoc->m_vAVehicleDispPropIdx;
	std::vector<CString>& vehicleshapeidx = pDoc->m_vAVehicleShapeNameIdx;

	AirsideVehicleLogEntry element;	
	element.SetEventLog(&(airsideSimLogs.m_airsideVehicleEventLog));


	AirsideVehicleLog& airsideVehicleLog = airsideSimLogs.m_airsideVehicleLog;
	const CAirsideEventLogBuffer<AirsideVehicleEventStruct>& airsideVehicleEventLog = pDoc->GetOutputAirside()->GetLogBuffer()->m_vehicleLog;

	bool bReloadVehicleDispProp = m_bReloadVehicleDispProp;
	m_bReloadVehicleDispProp = false;

	//int nCount = airsideVehicleLog.getCount();
	size_t nCount = airsideVehicleEventLog.m_lstElementEvent.size();
	for (size_t i = 0; i < nCount; i++)
	{
		const CAirsideEventLogBuffer<AirsideVehicleEventStruct>::ELEMENTEVENT& ItemEventList = airsideVehicleEventLog.m_lstElementEvent[i];
		airsideVehicleLog.getItem(element, ItemEventList.first);

		long nStartTime = (long) element.getEntryTime();
		long nEndTime = (long) element.getExitTime();
		int nIndexID = element.GetAirsideDesc().IndexID;

		if (nStartTime <= nCurTime && nEndTime >= nCurTime) 
		{			
			ARCVector3 pos;
			ARCVector3 dangles;
			int pesAIdx =-1;
			
			double dSpeed = 0;
			if( CAnimationManager::GetVehicleShow(pDoc,ItemEventList,nCurTime,dAirprotAlt
				,pos,dangles,dSpeed,pesAIdx)	)
			{
				const AirsideVehicleEventStruct& pesA = ItemEventList.second[pesAIdx];
				bool bCreated = false;
				CAnimaVehicle3D* pVehicle3D = vehicle3DList.CreateOrRetrieveMob3D(nIndexID, bCreated);
				if (!pVehicle3D)
					continue;

				if (bCreated)
				{
					CRender3DScene::SetSceneNodeQueryDataInt(*pVehicle3D, SceneNode_Vehicle, pVehicle3D->GetVehicleID());
				}

				int nVehicleTypeID = element.GetAirsideDesc().id;
				CVehicleDispPropItem * pDispItem = vehicledispprops.FindBestMatch(nVehicleTypeID);
				if (NULL == pDispItem || (pDispItem && pDispItem->IsVisible()))
				{
					if (bCreated || bReloadVehicleDispProp || !vehicle3DList.IsMob3DShow(pVehicle3D))
					{
						if (pDispItem)
						{
							pVehicle3D->SetShape(SHAPESMANAGER->GetShapeName( pDispItem->GetShape().first, pDispItem->GetShape().second ));
							pVehicle3D->SetColor(pDispItem->GetColor());
						} 
						else
						{
							pVehicle3D->SetShape(_T("")); // use default shape
							pVehicle3D->SetColor(RGB(255, 255, 255)); // use white as default color
						}
					}

					pVehicle3D->SetPosition( pos );
					pVehicle3D->SetRotation( dangles ,ER_YXZ);

					ARCVector3 vDim;
					vDim[VX] = element.GetAirsideDesc().vehiclelength;
					vDim[VY] = element.GetAirsideDesc().vehiclewidth;
					vDim[VZ]  = element.GetAirsideDesc().vehicleheight;
					pVehicle3D->SetDimension(vDim);

					vehicle3DList.ShowMob3D(nIndexID, true);

					CVehicleTag& vehicleTag = pDoc->m_vehicleTags;
					std::vector<int>& vehicleDisppropidx = pDoc->m_vAVehicleDispPropIdx;			
					int CurNodeIndex = vehicleTag.GetCurSelNodeIndex();
					int CurItemIndex = vehicleTag.GetCurSelItemIndexInCurSelNode();
					DWORD dwTagInfo;
					AirsideVehicleDescStruct& getdesc  = element.GetAirsideDesc();	
					bool bShowTag = _bShowTag && vehicleTag.GetItemTagInfoInNode(CurNodeIndex,CurItemIndex,dwTagInfo);
					if(bShowTag)
					{			
						CString sTag;
						int h, w;
						bool bExpanded = false;
						CAnimationManager::BuildVehicleTag(element.GetAirsideDesc(), pesA, dwTagInfo, bExpanded, sTag, h, w);
						vehicleTags.SetTag(nIndexID,pVehicle3D->GetSceneNode(),sTag);
						vehicleTags.Show(nIndexID,true);	
					}
 					else
 						vehicleTags.Show(nIndexID,false);
					continue;
				}				
			}
		}
		vehicle3DList.ShowMob3D(nIndexID, false);
		vehicleTags.Show(nIndexID,true);
	}
}

//InputAirside* CRender3DView::OnQueryInputAirside()
//{
//	CTermPlanDoc* pDoc = GetDocument();
//
//	return	&(pDoc->GetInputAirside());	
//}

bool CRender3DView::GetAllRailWayInfo(std::vector<RailwayInfo*>& railWayVect)
{
	CTermPlanDoc* pDoc = GetDocument();
	if (pDoc)
	{
		pDoc->m_trainTraffic.GetAllRailWayInfo(railWayVect);	
		return true;
	}
	return false;
}

int CRender3DView::OnQueryFloorCount()
{
	int nFloorCount = 0;
	CTermPlanDoc* pDoc = GetDocument();
	if (pDoc)
	{
		nFloorCount = (int)pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size();
	}
	return nFloorCount;
}

bool CRender3DView::OnQueryFloorVisible(int floor)
{
	CTermPlanDoc* pDoc = GetDocument();
	int nFloorCount = OnQueryFloorCount();
	if (floor >= 0 && floor < nFloorCount)
	{
		if (pDoc)
		{
			return pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[floor]->IsVisible()?true:false;
		}
	}
	
	return false;
}

double CRender3DView::OnQueryFloorHeight(int floor)
{
	double dAlt = 0.0;
	CTermPlanDoc* pDoc = GetDocument();
	int nFloorCount = OnQueryFloorCount();
	if (floor >= 0 && floor < nFloorCount)
	{
		if (pDoc)
		{
			dAlt =  pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[floor]->Altitude();
		}
	}
	
	return dAlt;
}

void CRender3DView::UpdateAnimationProcessor( CTermPlanDoc* pDoc , double* dAlt , BOOL* bOn)
{
	CPROCESSOR2LIST* vProcList = &(pDoc->GetCurrentPlacement(EnvMode_Terminal)->m_vDefined);
	//CRenderProcessor3DList pro3DList;// = GetModelEditScene().GetProcessor3DList();
//	int nProcCount = vProcList->getNodeCount();
	int nProcCount = (int)vProcList->size();

	ARCColor3 stateOffColor(128,128,128);

	for(int i=0;i<nProcCount;i++)
	{		
	/*	CRenderProcessor3DList::Shared3DPtr& proc3D =  pro3DList.getShared3D(i);		
		if(!proc3D.get())
			continue;*/

		CProcessor2Base* pProc2  = vProcList->at(i);//->GetProcessor();
		if(!pProc2)
			continue;	

		int nProcFloor = pProc2->GetFloor();
		ProcessorProp* pProcProp = pProc2->GetProcessorProp();

		if (!pProcProp)
			continue;

		if (pProcProp->getProcessorType() == BarrierProc)//barrier did not have process assign event
			continue;

		long nCurTime = pDoc->m_animData.nLastAnimTime;
		bool bShowStateOnTag = CAnimationManager::DisplayProcessorStateOnTags(pDoc,(Processor*)pProcProp,nCurTime);
		
		if(pProcProp->getProcessorType()== BridgeConnectorProc)
		{
			pProc2->SetProcessorStateOff(bShowStateOnTag);
			
			CRenderBridge3D pBridge3D = GetModelEditScene().GetSceneNode(pProc2->getGuid());//CRenderBridge3D*) proc3D.get();
			
			BridgeConnector* pBridge = (BridgeConnector*)pProc2->GetProcessorProp();	
			ARCColor3 cDisplay;
			cDisplay = bShowStateOnTag ? pProc2->m_dispProperties.color[PDP_DISP_SHAPE] : stateOffColor;
			{
				//for(int idx=0;idx<pBridge->GetConnectPointCount();++idx)
				{
					CPoint2008 pF, pT;
					const BridgeConnector::ConnectPoint& pCon =  pBridge->m_connectPoint;					
					bool bMoving  = CAnimationManager::GetBridgeShow(pBridge,0,pDoc,dAlt[nProcFloor],pF,pT);
					pBridge3D.DrawConnector(0,bMoving,pF,pT,pCon.m_dWidth,/*pProc2->m_dispProperties.color[PDP_DISP_SHAPE]*/cDisplay);
				}					
			}			
		}
 		else if(bShowStateOnTag != pProc2->GetProcessorStateOffTag())
 		{
 			pProc2->SetProcessorStateOff(bShowStateOnTag);
 			CRenderProcessor3D pProc3D = GetModelEditScene().GetSceneNode(pProc2->getGuid());
 			ASSERT(pProc3D);
 			pProc3D.Update3D((CObjectDisplay*)pProc2);
 		}
	}

}

ViewPort CRender3DView::GetSnapshotViewPort()
{
	return m_vpCamera;
}

void CRender3DView::OnViewExport()
{
	// Get max width and height
	// ...
	const int nSnapshotMaxWidth = 2048;
	const int nSnapshotMaxHeight = 2048;

	CRect rc;
	GetClientRect(rc);

	CDlgExport3DView dlg(CSize(rc.right, rc.bottom), CSize(nSnapshotMaxWidth, nSnapshotMaxHeight));
	if (IDOK == dlg.DoModal())
	{
		CString strExportFile = dlg.GetFileName();

		// Process exporting
		Ogre::String str;
		CTermPlanDoc* pDoc = GetDocument();
		if (pDoc->m_eAnimState != CTermPlanDoc::anim_none)
		{
			str = (LPCTSTR)pDoc->m_animData.GetTimeTag();
		}
		SetSnapshotSize(dlg.GetWidth(), dlg.GetHeight());
		SetSnapshotTag(str);
		try
		{
			TakeSnapshot((LPCTSTR)dlg.GetFileName(),GetEditor()->IsShowText());
		}
		catch (...)
		{
			AfxMessageBox(_T("Error Export View"));
		}
	
	}
}

void CRender3DView::SnapshotRender(int nWidth,int nHeight,BYTE* pBits,const CCameraData& cameraData)
{
	SetSnapshotSize(nWidth,nHeight);
	SetSnapshotTag(GetDocument()->m_animData.GetTimeTag().GetBuffer());

	UpdateAnimationShapes();
	UpdateMovieCamera();
	TakeSnapshot(nWidth,nHeight,pBits,cameraData);
}

void CRender3DView::SnapshotRender( int nWidth,int nHeight,BYTE* pBits )
{
	SetSnapshotSize(nWidth,nHeight);
	SetSnapshotTag(GetDocument()->m_animData.GetTimeTag().GetBuffer());
   
	UpdateAnimationShapes();
	UpdateMovieCamera();

	TakeSnapshot(nWidth,nHeight,pBits,GetEditor()->IsShowText());
}

CCameraData CRender3DView::GetUserCurrentCamera()const
{
	CCameraData cameraData;
	CCameraNode cameraScene(m_vpCamera.getCamera());
	cameraScene.Save(cameraData);
	return cameraData;
}
void CRender3DView::OnClick3DNodeObject( C3DNodeObject* p3DObj,SceneNodeType eNodeType, int id)
{
	CTermPlanDoc* pDoc = GetDocument();
	switch (eNodeType)
	{
	case SceneNode_useGUID:
		{
			CModeBase3DObject* pMode3DObj = (CModeBase3DObject*)p3DObj;
			switch (pMode3DObj->GetObjectType())
			{
			case C3DSceneNodeObject::ObjectType_Terminal:
				{
					CTerminal3DObject* pTerm3DObj = (CTerminal3DObject*)pMode3DObj;
				}
				break;
			case C3DSceneNodeObject::ObjectType_Airside:
				{
					CAirside3DObject* pTerm3DObj = (CAirside3DObject*)pMode3DObj;
				}
				break;
			case C3DSceneNodeObject::ObjectType_Airport:
				{
					CAirport3DObject* pTerm3DObj = (CAirport3DObject*)pMode3DObj;
				}
				break;
			default:
				{
					ASSERT(FALSE);
				}
				break;
			}
		}
		break;
	case SceneNode_Pax:
		{
			CAnimaPax3D* pPax3D = (CAnimaPax3D*)p3DObj;
			OnSelMobElement(CWalkthrough::Mob_Pax, pPax3D->GetPaxID());
// 			m_movieNode = *pPax3D;
		}
		break;
	case SceneNode_Flight:
		{
			CAnimaFlight3D* pFlight3D = (CAnimaFlight3D*)p3DObj;
			OnSelMobElement(CWalkthrough::Mob_Flight, pFlight3D->GetFlightID());
// 			m_movieNode = *pFlight3D;
		}
		break;
	case SceneNode_Vehicle:
		{
			CAnimaVehicle3D* pVehicle3D = (CAnimaVehicle3D*)p3DObj;
			OnSelMobElement(CWalkthrough::Mob_Vehicle,id);
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
		break;
	}
}

void CRender3DView::ClearSeledPaxList()
{
// 	if(m_bAnimPax==FALSE)
	{
		m_selPaxDesc = SelMobDesc();
		m_vPaxSelDescList.clear();
		//Clear Selected Pax List
		CMainFrame* pMF=(CMainFrame*)AfxGetMainWnd();
		pMF->GetAnimPaxCB()->ResetContent();
	}

}

void CRender3DView::InitSeledPaxList()
{
	CMainFrame* pMF=(CMainFrame*)AfxGetMainWnd();
	CComboBox* pAnimaCB = pMF->GetAnimPaxCB();
	pAnimaCB->ResetContent();

	m_selPaxDesc = m_vPaxSelDescList.size() ? m_vPaxSelDescList.front() : SelMobDesc();
	for(SelMobDescList::iterator iter=m_vPaxSelDescList.begin();iter!=m_vPaxSelDescList.end();iter++)
	{
		int nIndex = pAnimaCB->InsertString(0, iter->strTag);
		pAnimaCB->SetItemData(nIndex, iter->nMobID);
	}
	if(pAnimaCB->GetCount()>0)
		pAnimaCB->SetCurSel(0);
}

void CRender3DView::OnSelMobElement( CWalkthrough::WalkthroughMobType mobType, int nMobID )
{
	CTermPlanDoc* pDoc = GetDocument();

	m_selPaxDesc.mobType = mobType;
	m_selPaxDesc.nMobID = nMobID;

	SelMobDescList::iterator itFind = std::find_if(m_vPaxSelDescList.begin(), m_vPaxSelDescList.end(), SelPaxDescTypeIDMatch(m_selPaxDesc));
	CMainFrame* pMF=(CMainFrame*)AfxGetMainWnd();
	CComboBox* pAnimaCB = pMF->GetAnimPaxCB();
	if(itFind==m_vPaxSelDescList.end())
	{
		m_selPaxDesc.strTag = pDoc->GetMobTagFromID(mobType, nMobID);
	}
	else
	{
		m_selPaxDesc = *itFind;
		int nOffsetIndex = itFind - m_vPaxSelDescList.begin();
		m_vPaxSelDescList.erase(itFind);
		pAnimaCB->DeleteString(nOffsetIndex);
	}

	int nIndexCB = pAnimaCB->InsertString(0, m_selPaxDesc.strTag);	
	m_vPaxSelDescList.insert(m_vPaxSelDescList.begin(), m_selPaxDesc);
	pAnimaCB->SetItemData(nIndexCB, m_selPaxDesc.nMobID/*m_selPaxDesc*/);
	pAnimaCB->SetCurSel(0);

	//CDlgWalkthroughRender* pDlg = pDoc->GetDlgWalkthroughRender();
	//if (pDlg)
	//{
	//	pDlg->SelectMob(mobType, nMobID, m_selPaxDesc.strTag);
	//}
	pDoc->OnSelectPAX(nMobID,m_selPaxDesc.strTag);
}

bool CRender3DView::StartViewTrackMovie( CWalkthrough::WalkthroughMobType mobType, int nMobID, CWalkthrough::VIEWPOINT viewPoint )
{
	if (m_e3DMovieType != movie_None)
		return false;

	C3DNodeObject* pTrackNode = NULL;
	bool bCreated = false;
	double zOffset = 0.0;
	ARCVector3 viewAspect;
	switch (mobType)
	{
	case CWalkthrough::Mob_Pax:
		{
			pTrackNode = GetModelEditScene().GetPax3DList().CreateOrRetrieveMob3D(nMobID, bCreated);
			m_movieRelLookAtPos = ARCVector3(90.0, 0.0, 180.0);
			if (CWalkthrough::FIRSTPERSON == viewPoint)
				m_movieRelCameraPos = ARCVector3(89.0, 0.0, 180.0);
			else
				m_movieRelCameraPos = ARCVector3(-5000.0, 0.0, 2000.0);
		}
		break;
	case CWalkthrough::Mob_Flight:
		{
			pTrackNode = GetModelEditScene().GetFlight3DList().CreateOrRetrieveMob3D(nMobID, bCreated);
			m_movieRelLookAtPos = ARCVector3(1000.0, 0.0, 2000.0);
			if (CWalkthrough::FIRSTPERSON == viewPoint)
				m_movieRelCameraPos = ARCVector3(999.0, 0.0, 2000.0);
			else
				m_movieRelCameraPos = ARCVector3(-10000.0, 0.0, 3000.0);
		}
		break;
	case CWalkthrough::Mob_Vehicle:
		{
			pTrackNode = GetModelEditScene().GetLandsideVehicle3DList().CreateOrRetrieveMob3D(nMobID, bCreated);
			m_movieRelLookAtPos = ARCVector3(90.0, 0.0, 180.0);
			if (CWalkthrough::FIRSTPERSON == viewPoint)
				m_movieRelCameraPos = ARCVector3(89.0, 0.0, 180.0);
			else
				m_movieRelCameraPos = ARCVector3(-5000.0, 0.0, 2000.0);
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
		break;
	}
	if (!pTrackNode || !*pTrackNode)
	{
		ASSERT(FALSE);
		return false;
	}

	m_eWalkthroughMobType = mobType;
	m_movieNode = *pTrackNode;
	m_e3DMovieType = CWalkthrough::FIRSTPERSON == viewPoint ? movie_TrackFirstPerson : movie_TrackThirdPerson;
	m_dBestSpotAngle = 0.0;
	return true;
}

void CRender3DView::UpdateTrackMovieCamera()
{
	if (m_e3DMovieType != movie_TrackFirstPerson && m_e3DMovieType != movie_TrackThirdPerson)
	{
		ASSERT(FALSE);
		return;
	}

	if (m_movieNode)
	{
		Ogre::Camera* pcam = m_vpCamera.getCamera();

		CCameraNode cam(pcam);
		ARCVector3 rotOld = m_movieNode.GetRotation();
		if (m_eWalkthroughMobType == CWalkthrough::Mob_Pax)
		{
			m_movieNode.SetRotation(ARCVector3(0.0, 0.0, m_dBestSpotAngle));
		}
		cam.LookAt(m_movieNode.ConvertPositionLocalToWorld(m_movieRelCameraPos),
			m_movieNode.ConvertPositionLocalToWorld(m_movieRelLookAtPos),
			ARCVector3(0.0, 0.0, 1.0));
		if (m_eWalkthroughMobType == CWalkthrough::Mob_Pax)
		{
			m_movieNode.SetRotation(rotOld);
		}
	
		m_coordDisplayScene.UpdateCamera( pcam );
		return;
	}
	ASSERT(FALSE);
}


bool CRender3DView::StartViewPanoramaMovie( const PanoramaViewData* pPanoramaData )
{
	if (m_e3DMovieType != movie_None)
		return false;

	m_movieNode = C3DNodeObject(GetModelEditScene().AddSceneNode(PanoramaNodeName));

	ALTAirport theAirport;
	int aptID = InputAirside::GetAirportID(GetDocument()->GetProjectID());
	theAirport.ReadAirport(aptID);

	ARCVector3 nodePos = pPanoramaData->m_pos;
	nodePos[VZ] = pPanoramaData->m_dAltitude - theAirport.getElevation();
	m_movieNode.SetPosition(nodePos);

	m_movieRelLookAtPos = abs(pPanoramaData->m_dInclination + 90) > 1e-6
		? ARCVector3(1.0, 0.0, tan(ARCMath::DegreesToRadians(pPanoramaData->m_dInclination)))
		: ARCVector3(0.0, 0.0, -1.0);

	m_e3DMovieType = movie_Panorama;
	return true;
}

void CRender3DView::UpdatePanoramaMovieCamera( double dRotateDegree )
{
	if (m_e3DMovieType != movie_Panorama)
	{
		ASSERT(FALSE);
		return;
	}

	if (m_movieNode)
	{
		m_movieNode.SetRotation(ARCVector3(0.0, 0.0, dRotateDegree));

		Ogre::Camera* pcam = m_vpCamera.getCamera();

		CCameraNode cam(pcam);
		cam.LookAt(m_movieNode.GetPosition(),
			m_movieNode.ConvertPositionLocalToWorld(m_movieRelLookAtPos),
			ARCVector3(0.0, 0.0, 1.0));

		m_coordDisplayScene.UpdateCamera( pcam );
		return;
	}
	ASSERT(FALSE);

}

void CRender3DView::EndViewMovie()
{
	if (m_e3DMovieType == movie_None)
	{
		ASSERT(FALSE);
		return;
	}
	m_movieNode = C3DNodeObject();

	m_eWalkthroughMobType = CWalkthrough::Mob_None;
	m_e3DMovieType = movie_None;
	m_dBestSpotAngle = 0.0;
}

void CRender3DView::SetCameraData( const CCameraData& camera )
{
	*m_pCamera = camera;
	LoadCamera();
}

void CRender3DView::UpdateMovieCamera()
{
	if (m_e3DMovieType == movie_TrackFirstPerson || m_e3DMovieType == movie_TrackThirdPerson)
	{
		UpdateTrackMovieCamera();
	}
// 	else if (m_e3DMovieType == movie_Panorama)
// 	{
// 		UpdatePanoramaMovieCamera();
// 	}
}


void CRender3DView::UpdateAnimationEscaltor( CTermPlanDoc* pDoc, double*dAlt, BOOL* bOn )
{
	CElevatorLog* pELog = pDoc->GetTerminal().m_pElevatorLog;
	CElevatorLogEntry elevatorElement;
	ElevatorDescStruct eds;
	int nECount = pELog->getCount();
	long nTime = pDoc->m_animData.nLastAnimTime;
	CAnimaElevator3DList& elevatorlist = GetModelEditScene().GetElvator3DList();

	for(int ii=0; ii<nECount; ii++)
	{
		pELog->getItem(elevatorElement, ii);
		elevatorElement.SetOutputTerminal(&(pDoc->GetTerminal()));
		elevatorElement.SetEventLog(pELog->getEventLog());
		elevatorElement.initStruct(eds);
		long nStartTime = (long) elevatorElement.GetStartTime();
		long nEndTime = (long) elevatorElement.GetEndTime();

		if(nStartTime <= nTime && nEndTime >= nTime)	
		{			
			//3)interpolate position
			long nEventCount = elevatorElement.getCount();
			long nextIdx = -1;
			for(long jj=0; jj<nEventCount; jj++) 
			{
				//ElevatorEventStruct ees = elevatorElement.getEvent(jj);
				//long attime = ees.time;
				if(elevatorElement.getEvent(jj).time >= nTime)
				{
					nextIdx = jj;
					break;
				}
			}
			if(nextIdx > 0) 
			{
				const ElevatorEventStruct& eesB = elevatorElement.getEvent(nextIdx);
				const ElevatorEventStruct& eesA = elevatorElement.getEvent(nextIdx-1);

				//bool bRet = elevatorElement.getEventsAtTime(nTime,eesA,eesB);
				//pELog->updateItem(elevatorElement,ii);

				//if(bRet){

					int w = eesB.time - eesA.time;
					int d = eesB.time - nTime;
					double r = ((double) d) / w;
					float xpos = static_cast<float>((1-r)*eesB.x + r*eesA.x);// - eds.liftLength/2.0;
					float ypos = static_cast<float>((1-r)*eesB.y + r*eesA.y);// + eds.liftWidth/2.0;
					float altA = static_cast<float>(dAlt[(int)(eesA.z/SCALE_FACTOR)]);
					float altB = static_cast<float>(dAlt[(int)(eesB.z/SCALE_FACTOR)]);
					float zpos = static_cast<float>((1-r)*altB + r*altA);

					bool bCreated = false;
					CAnimaElevator3D* pElv3D = elevatorlist.CreateOrRetrieveMob3D(ii,bCreated);
					if(pElv3D)
					{
						if(bCreated)
							pElv3D->BuildShape(eds.liftLength,eds.liftWidth,360.0f-eds.orientation,traincolor);
						pElv3D->SetPosition(ARCVector3(xpos,ypos,zpos));
						elevatorlist.ShowMob3D(ii,true);
						continue;
					}						
				//}
			}
		}
		elevatorlist.ShowMob3D(ii,false);
	}
}

void CRender3DView::UpdateAnimationTrain( CTermPlanDoc* pDoc, double*dAlt, BOOL* bOn )
{
	//draw train
	CTrainLog* pTrainLog = pDoc->GetTerminal().m_pTrainLog;
	CTrainLogEntry trainElement;
	TrainDescStruct tds;
	int nTrainCount = pTrainLog->getCount();
	long nTime = pDoc->m_animData.nLastAnimTime;
	int nFloorCount =  pDoc->GetFloorCount();

	CAnimaTrain3DList& trainList = GetModelEditScene().GetTrain3DList();

	for(int ii=0; ii<nTrainCount; ii++)
	{
		pTrainLog->getItem(trainElement,ii);
		trainElement.SetOutputTerminal(&(pDoc->GetTerminal()));
		trainElement.SetEventLog(pDoc->GetTerminal().m_pTrainEventLog);
		trainElement.initStruct(tds);
		long nStartTime = (long) trainElement.GetStartTime();
		long nEndTime = (long) trainElement.GetEndTime();
		if(nStartTime <= nTime && nEndTime >= nTime)
		{
			TrainEventStruct tesB;
			TrainEventStruct tesA;

			bool bRet = trainElement.getEventsAtTime(nTime,tesA,tesB);
			pTrainLog->updateItem(trainElement,ii);
			if(bRet) {

				//interpolate position based on the two PaxEventStructs and the time
				int w = tesB.time - tesA.time;
				int d = tesB.time - nTime;
				double r = ((double) d) / w;
				float xpos = static_cast<float>((1-r)*tesB.x + r*tesA.x);
				float ypos = static_cast<float>((1-r)*tesB.y + r*tesA.y);
				//get tesA altitude
				int nIndexInFloor=static_cast<int>((int)(tesA.z/SCALE_FACTOR)); 
				float dAltDiff;
				if(nIndexInFloor<nFloorCount-1) {
					dAltDiff=static_cast<float> (dAlt[nIndexInFloor+1]-dAlt[nIndexInFloor]);
				}
				else {
					dAltDiff=1000.0f;
				}
				float mult = static_cast<float>(tesA.z/SCALE_FACTOR)-nIndexInFloor;
				float altA = dAltDiff * mult + static_cast<float> (dAlt[nIndexInFloor]);
				//get tezB altitude
				nIndexInFloor=static_cast<int>((int)(tesB.z/SCALE_FACTOR)); 
				if(nIndexInFloor<nFloorCount-1) {
					dAltDiff=static_cast<float>(dAlt[nIndexInFloor+1]-dAlt[nIndexInFloor]);
				}
				else {
					dAltDiff=1000.0f;
				}
				mult = static_cast<float>(tesB.z/SCALE_FACTOR)-nIndexInFloor;
				float altB = dAltDiff * mult + static_cast<float>(dAlt[nIndexInFloor]);


				float zpos = static_cast<float>((1-r)*altB + r*altA);

				//					int zpos = static_cast<int>( tesB.z/SCALE_FACTOR );
				//					if(!bOn[zpos]) break;//suppose all cars in same floor
				//float rot = (1-r)*tesB.heading + r*tesA.heading;
				float rot = tesA.heading;
				rot = static_cast<float>(90.0-rot);


				bool bCreated = false;
				CAnimaTrain3D* pTrain3D = trainList.CreateOrRetrieveMob3D(ii,bCreated);
				if(pTrain3D)
				{
					if(bCreated)
						pTrain3D->BuildShape(tds.carLength,tds.carWidth,tds.numCars,traincolor);
					pTrain3D->SetPosition(ARCVector3(xpos,ypos,zpos));
					pTrain3D->SetRotation(ARCVector3(0,0,rot));
					trainList.ShowMob3D(ii,true);
					continue;
				}			

				//draw train
				//					static const float delta=500.0;
				//glColor3ubv(traincolor);
				//glPushMatrix();
				//DrawCar(xpos,ypos,zpos,tds.carLength,tds.carWidth,rot,tds.numCars);//dAlt[zpos]
				//glPopMatrix();
			}
		}

		trainList.ShowMob3D(ii,false);
	}

}

void CRender3DView::UpdateAnimationResoureElm( CTermPlanDoc* pDoc, double*dAlt, BOOL* bOn )
{
	ResourceElementLog* pResLog = pDoc->GetTerminal().resourceLog;
	ResourceElementLogEntry resElement;
	ResDescStruct rds;
	int nResCount = pResLog->getCount();
	int nFloorCount =  pDoc->GetFloorCount();
	long nTime = pDoc->m_animData.nLastAnimTime;

	CAnimaResourceElm3DList& resourceList = GetModelEditScene().GetResource3DList();

	for(int ii=0; ii<nResCount; ii++)
	{
		pResLog->getItem(resElement,ii);
		resElement.SetOutputTerminal(&(pDoc->GetTerminal()));
		resElement.SetEventLog(pDoc->GetTerminal().m_pResourceEventLog);
		resElement.initStruct(rds);


		long nStartTime = (long) resElement.getStartTime();
		long nEndTime = (long) resElement.getEndTime();
		if(nStartTime <= nTime && nEndTime >= nTime)
		{
			//interpolate position based on the two PaxEventStructs and the time

			ResEventStruct resA;
			ResEventStruct resB;

			bool bRet = resElement.getEventsAtTime(nTime,resA,resB);
			if(bRet)
			{
				int w = resB.time - resA.time;
				int d = resB.time - nTime;
				double r = ((double) d) / w;
				float xpos = static_cast<float>((1-r)*resB.x + r*resA.x);
				float ypos = static_cast<float>((1-r)*resB.y + r*resA.y);
				int zpos = static_cast<int>( resB.z/SCALE_FACTOR );


				if(bOn[zpos])
				{
					bool bCrearted =false;
					CAnimaResourceElm3D* pResource = resourceList.CreateOrRetrieveMob3D(ii,bCrearted);
					if(pResource)
					{
						if(bCrearted)
						{

						}
						pResource->SetPosition( ARCVector3(xpos,ypos,dAlt[zpos]) );
						resourceList.ShowMob3D(pResource,true);
						continue;
					}
					/*float rot = resA.heading;
					rot = 90.0-rot;*/
				}
			

				//glColor3ubv(traincolor);
				//DrawSphere( pView->GetParentFrame()->SphereDLID(), ARCVector3(xpos, ypos, dAlt[zpos]), 100.0 );


				/*GLuint nDLID = pView->GetParentFrame()->ResShapeDLIDs()[rds.pool_idx];

				if( pView->GetCamera()->PointInCamera(xpos, ypos, dAlt[zpos]))
					DrawPAX(xpos, ypos, dAlt[zpos],1.0f, 0.0, nDLID);*/


			}
		}	

		resourceList.ShowMob3D(ii,false);
	}
}

void CRender3DView::RelocateCameraData()
{
	//set the camera
	int row, col;
	if(GetParentFrame()->IsChildPane(this,row,col))
	{
		m_pCamera = &GetDocument()->GetCurModeRenderCameras().GetUserCamera().GetCameraData(row, col);
	}
	else
		ASSERT(FALSE);
}

void CRender3DView::OnNewMouseState()
{
	FallbackData falldata;
	falldata.hSourceWnd = m_hFallbackWnd;
	GetModelEditScene().BeginMouseOperation(m_hFallbackWnd,m_lFallbackParam);
}

void CRender3DView::OnContextMenu( CWnd* pWnd, CPoint point )
{
		
	CNewMenu menu ;
	CNewMenu* pCtxMenu = GetEditor()->GetContextMenu(menu);
	if(pCtxMenu != NULL)
	{
		//UpdatePopMenu(this, pCtxMenu);
		pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, point.x, point.y, AfxGetMainWnd() );
	}

}

void CRender3DView::OnCtxProcproperties()
{
	CTermPlanDoc* pDoc = GetDocument();	
	pDoc->GetLayoutEditor()->OnCtxProcproperties(this);
}

void CRender3DView::OnCtxLanddeleteproc()
{
	GetDocument()->GetLayoutEditor()->OnCtxDelLandsideProc();
}

void CRender3DView::OnCtxDelteControlPoint()
{
	GetDocument()->GetLayoutEditor()->OnCtxDeleteControlPoint();
}

void CRender3DView::OnCtxAddControlPoint()
{
	GetDocument()->GetLayoutEditor()->OnCtxAddControlPoint();
}

void CRender3DView::OnCtxCopyProc()
{
	GetDocument()->GetLayoutEditor()->OnCtxCopyLayoutObject();
}

void CRender3DView::ClearContextMenuInfo()
{
	GetDocument()->GetLayoutEditor()->GetCurContext()->clearRClickItemData();
}

void CRender3DView::OnCtxChangeCtrlPointType()
{
	GetDocument()->GetLayoutEditor()->OnCtxChangeControlPointType();	
}

void CRender3DView::OnCtxLockALTObject()
{
	QueryData& qdata = GetDocument()->GetLayoutEditor()->GetCurContext()->getRClickItemData();
	emLandsideItemType nodeType = _NoneItemType;
	qdata.GetData(KeyLandsideItemType,(int&)nodeType);
	LandsideLayoutObject *pObj = NULL;
	qdata.GetData(KeyAltObjectPointer,(int&)pObj);

	if(pObj && nodeType== _LayoutObject)
	{
		pObj->SetLocked(!pObj->GetLocked());
		GetDocument()->UpdateAllViews(NULL,VM_UPDATE_ALTOBJECT3D,(CObject*)pObj);
	}
}

void CRender3DView::OnUpdateCtxLockALTObject( CCmdUI *pCmdUI )
{
	QueryData& qdata = GetDocument()->GetLayoutEditor()->GetCurContext()->getRClickItemData();
	emLandsideItemType nodeType = _NoneItemType;
	qdata.GetData(KeyLandsideItemType,(int&)nodeType);
	ILayoutObject *pObj = NULL;
	qdata.GetData(KeyAltObjectPointer,(int&)pObj);

	if(pObj && nodeType== _LayoutObject)
	{
		pCmdUI->Enable(TRUE);
		if(pObj->GetLocked())
		{
			pCmdUI->SetText("UnLock");
		}
		else
		{
			pCmdUI->SetText("Lock");
		}
	}
	else
		pCmdUI->Enable(FALSE);

}

void CRender3DView::OnSnapPointToLevel( UINT nID )
{
	int nLevelIdex = nID - MENU_LEVEL_ID;
	GetEditor()->OnCtxMoveCtrlPointToLevel(nLevelIdex);
}
void CRender3DView::OnSnapPointToParkLotLevel(UINT nID)
{
	int nLevelIdex = nID - MENU_PARKINGLOT_LEVEL;
	GetEditor()->OnSnapPointToParkLotLevel(nLevelIdex);
}
void CRender3DView::OnCopyParkLotItem()
{
	GetEditor()->OnCopyParkLotItem();
}
void CRender3DView::OnDelParkLotItem()
{
	GetEditor()->OnDelParkLotItem();
}


void CRender3DView::UpdateLandsideVehicleAnima(BOOL bSelectMode,BOOL _bShowTag)
{
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	CMovableTagList& vehicleTags = GetModelEditScene().Get3DTags().m_landsidevehicleTags;

	//#if 0	
	CLandsideSimLogs& landsideSimLogs = pDoc->GetLandsideSimLogs();
	long nCurTime = pDoc->m_animData.nLastAnimTime;

	
	CLandsideEventLogBufferManager* logbuffer = landsideSimLogs.getEventLogBuffer();
	if(!logbuffer)
		return;
	CLandsideEventLogBuffer* pVehiclebuffer = logbuffer->loadVehicleDataIfNecessary(nCurTime, GetDocument()->m_eAnimState != CTermPlanDoc::anim_playB ,GetDocument());
	if(!pVehiclebuffer)
		return;

	LandsideVehicleLog& vehicleLog = landsideSimLogs.getVehicleLog();
	size_t nCount = vehicleLog.getCount();

	if (m_bReloadLandsideVehicleTagDispProp)
	{
		m_landsideVehicleTags.ReadData(GetDocument()->GetProjectID());
		m_bReloadLandsideVehicleTagDispProp = false;
	}
	if (m_bReloadLandsideVehicleDispProp)
	{
		m_landsideVehicleDispProps.ReadData();
		m_bReloadLandsideVehicleDispProp = false;
		m_displySet.clear();
		m_displySet.resize(nCount);
	}

	
	for (size_t i = 0; i < nCount; i++)
	{
		LandsideVehicleLogEntry element;
		vehicleLog.getItem(element,i);		
		int nVehicleID = element.GetVehicleDesc().vehicleID;	
		
		CLandsideEventLogBuffer::part_event_list& eventlist = pVehiclebuffer->getEventList(i);
		
		ARCVector3 dir;
		LandsideVehicleEventStruct pesM;  //intermediate event
		int pesAIdx = 0;

		if( CAnimationManager::GetLandsideVehicleShow(eventlist,nCurTime,pesM, dir,pesAIdx)	)
		{			
			//const LandsideVehicleEventStruct& pesA = eventlist[pesAIdx];
			CLandSideVehicleProbDispDetail* pDispDetail =   GetMatchDispProb(i,element.GetVehicleDesc().sName);
			
			if (pDispDetail)
			{
				int nCat = pDispDetail->getShape().first;
				int nSel = pDispDetail->getShape().second;
				CShapeCategoryManager* pShapeManager = CShapeList::getInstance()->getItem(nCat,nSel);
				if (pShapeManager)
				{
					CString sShape = pShapeManager->get3DImgName();
					float dlen = element.GetVehicleDesc().dlength;	
					float dwidht = element.GetVehicleDesc().dwidth;
					float dheight = element.GetVehicleDesc().dheight;
					
					bool bCreated = false;
					CLandsideVehicle3D* pVehicle3D = GetModelEditScene().GetLandsideVehicle3DList().CreateOrRetrieveMob3D(nVehicleID, bCreated);
					if(!pVehicle3D)
						continue;
  					if (bCreated)
  					{
						CRender3DScene::SetSceneNodeQueryDataInt(*pVehicle3D, SceneNode_Vehicle, nVehicleID);
					}
					if (pDoc->m_displayoverrides.m_pbProcDisplayOverrides[PDP_DISP_VehicleSCALE])
					{
						dheight = dheight*pDispDetail->GetScaleSize();
					}
					pVehicle3D->SetShape(sShape,dlen,dwidht,dheight);	
					pVehicle3D->SetPosition( ARCVector3(pesM.x,pesM.y,pesM.z) );
					pVehicle3D->SetDirection( dir, ARCVector3(0,0,1) );
					pVehicle3D->SetColor(pDispDetail->getColor());
					pVehicle3D->SetVisible(pDispDetail->getVisible() ? true : false);
					pVehicle3D->AttachTo(GetModelEditScene().GetRoot());

					int CurNodeIndex = m_landsideVehicleTags.GetCurSelNodeIndex();
					CLandsideVehicleTagSet* VehicleTagSet = m_landsideVehicleTags.GetNode(CurNodeIndex);
					CLandsideVehicleTypes * pTagItem = VehicleTagSet->FindBestMatch(element.GetVehicleDesc().sName,VehicleTagSet);
					if(pTagItem->m_bShowTags&&_bShowTag)
					{			
						CString sTag;
						//int h, w;
						bool bExpanded = false;
						CAnimationManager::BuildLandsideVehicleTag(element.GetVehicleDesc(), pesM, pTagItem->m_dwTagInfo, bExpanded,element.GetVehicleDesc().sName, sTag, pesAIdx);
						vehicleTags.SetTag(nVehicleID,pVehicle3D->GetSceneNode(),sTag);
						vehicleTags.Show(nVehicleID,true);	
					}
					else
					{	
						vehicleTags.Show(nVehicleID,false);
					}
				}
			}
		}
		else
		{
			GetModelEditScene().GetLandsideVehicle3DList().ShowMob3D(nVehicleID,false);
			vehicleTags.Show(nVehicleID,false);	
		}
	}
}

void CRender3DView::UpdateLandsideVehicleTraces()
{
	CTermPlanDoc *pDoc=GetDocument();
	bool bShow = pDoc->m_bShowTracers ? true : false;
	GetModelEditScene().RenderVehicleTraces(&pDoc->m_tempVehicleTracerData,bShow);
}

void CRender3DView::UpdateLandsideTrafficLightAnima()
{
	CTermPlanDoc* pDoc = GetDocument();

	CLandsideSimLogs& landsideSimLogs = pDoc->GetLandsideSimLogs();
	long nCurTime = pDoc->m_animData.nLastAnimTime;
	CLandsideEventLogBufferManager* logbuffer = landsideSimLogs.LoadLinkGroupDataIfNecessary(nCurTime);
	if(!logbuffer)
		return;

	//update intersection traffic light
	CLandsideEventLogBufferManager::LinkGroupLogBuff& linkGroupLogbuffer = logbuffer->m_vLinkGroupLogBuff;
		
	int pesAIdx =-1;
	size_t nCount = linkGroupLogbuffer.size();

	for (size_t i = 0; i < nCount; i++)
	{
		const IntersectionLinkGroupLogEntry& element = linkGroupLogbuffer[i];	
		int nGroupID = element.GetLinkGroupDesc().groupID;
		if (CAnimationManager::GetLandsideTrafficLightShow(element,nCurTime,pesAIdx))
		{
			const IntersectionLinkGroupEventStruct& pesA = element.getEventList()[pesAIdx];
			GetModelEditScene().SetIntersectionTrafficLight(element.GetLinkGroupDesc().intersectionID,nGroupID,pesA.state);
		}
	}

	//update cross walk traffic light
	nCurTime = pDoc->m_animData.nLastAnimTime;
	logbuffer = landsideSimLogs.LoadLinkCrossWalkIfNecessary(nCurTime);
	CLandsideEventLogBufferManager::CrossWalkLogBuff & crossWalkLogBuff=logbuffer->m_vCrossWalkLogBuff;
	nCount = crossWalkLogBuff.size();

	for (size_t i = 0; i < nCount; i++)
	{
		const CCrossWalkLogEntry& element = crossWalkLogBuff[i];	
// 		int nGroupID = element.GetCrossWalkDesc().crossID;
		if (CAnimationManager::GetCrossWalkLightShow(element,nCurTime,pesAIdx))
		{
			const CrossWalkEventStruct& pesA = element.getEventList()[pesAIdx];
			GetModelEditScene().SetCrossWalkTrafficLight(element.GetCrossWalkDesc().crossID,pesA.lightState);
		}
	}
	
}

void CRender3DView::OnCtxDeleteLayoutItem()
{
	GetDocument()->GetLayoutEditor()->OnCtxDeleteLayoutItem();
}

void CRender3DView::OnCtxLayoutObjectDisplayProp()
{
	GetDocument()->GetLayoutEditor()->OnCtxLayoutObjectDisplayProp(this);
}

//void CRender3DView::OnShowObjectTag()
//{
//	GetEditor()->flipShowText();
//	GetModelEditScene().UpdateDrawing();
//	Invalidate();
//}
//
//void CRender3DView::OnUpdateShowObjectTag( CCmdUI* pCmdUI )
//{
//	pCmdUI->SetCheck(GetEditor()->IsShowText()?1:0);
//}

LRESULT CRender3DView::OnTempFallbackFinished( WPARAM wParam,LPARAM lParam )
{	
	switch (lParam)
	{
	case DISTANCE_MEARSURE:
		{
			double* retdist =(double*)wParam;
			OnFinishMeasure(*retdist);
			break;
		}
	default:
		break;
	}
	return TRUE;
}

//void CRender3DView::SetWorkHeight( double d )
//{
//	GetModelEditScene().SetWorkingHeight(d);
//}

void CRender3DView::OnEnabelEditAltObject()
{
	QueryData& qdata = GetDocument()->GetLayoutEditor()->GetCurContext()->getRClickItemData();	
	CString sNodeName;
	if( qdata.GetData(Key3DSceneNodeName,sNodeName)  )
	{
		CRenderScene3DNode scenNode = GetModelEditScene().GetSceneNode(sNodeName);
		scenNode.FlipEditMode();
	}
}

void CRender3DView::OnUpdateCtxEnableEditAltobject(CCmdUI *pCmdUI)
{
	QueryData& qdata = GetDocument()->GetLayoutEditor()->GetCurContext()->getRClickItemData();
	CString sNodeName;
	if( qdata.GetData(Key3DSceneNodeName,sNodeName) )
	{
		pCmdUI->Enable(TRUE);
		CRenderScene3DNode scenNode = GetModelEditScene().GetSceneNode(sNodeName);
		if(!scenNode.IsInEditMode())
		{
			pCmdUI->SetText("Enable Edit");
		}
		else
		{	
			pCmdUI->SetText("Disable Edit");
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
	
	

}

void CRender3DView::OnSetPointAltitude()
{
	//popup dialog input altitude
	double d =  GetEditor()->m_landsideEdit->GetPointAlt();
	CDlgInputAltitude dlgInputAlt(d);
	if(dlgInputAlt.DoModal()==IDOK)
	{
		double dalt = dlgInputAlt.getAlt();
		GetEditor()->m_landsideEdit->OnSetPointToAlt(dalt);
	/*	if(dlgInputAlt.remberAlt())
		{
			GetEditor()->m_landsideEdit->AddRemAlt(dalt);
		}*/
	}

}

//void CRender3DView::OnSetPointIndexAltitude( UINT nID )
//{
//	int nIndex = nID - MENU_LEVEL_ID;
//	GetEditor()->m_landsideEdit->OnSetPointToIndexAlt(nIndex);
//
//}

void CRender3DView::OnManageSavedAlt()
{
	
}

void CRender3DView::OnParkingLotProperty()
{
	QueryData& qdata = GetDocument()->GetLayoutEditor()->GetCurContext()->getRClickItemData();
	emLandsideItemType itemType;
	//check whether belong to landside object
	if(!qdata.GetData(KeyLandsideItemType,(int&)itemType))
		return;
	
	if (itemType != _LayoutObject)
		return;
	
	//check whether belong to parking lot
	emLandsideItemType parkLotItemType;
	qdata.GetData(KeyParkLotItemType,(int&)parkLotItemType);
	if (parkLotItemType!=_ParkingLotSpace && parkLotItemType!=_ParkingLotEntry
		&& parkLotItemType!=_ParkingLotArea && parkLotItemType!=_ParkingLotDrivePipe)
		return;

	LandsideParkingLot* pLot = NULL;
	qdata.GetData(KeyAltObjectPointer,(int&)pLot);

	if (pLot == NULL)
		return;

	CTermPlanDoc* pDoc = GetDocument();	
	pDoc->GetLayoutEditor()->OnCtxProcproperties(pLot,this,&qdata);
}

void CRender3DView::UpdateFloorOverlay( int nLevelID, CPictureInfo& picInfo )
{
	CRenderFloor* pFloor = GetDocument()->GetCurModeFloor().GetFloor(nLevelID);
	if(!pFloor)	return;
	//GetModelEditScene().UpdateOneFloor(pFloor);
	Invalidate(FALSE);
}

void CRender3DView::OnTimer( UINT nIDEvent )
{
	if(nIDEvent == m_nUpdateRedSquaresTimer)
	{					
		if( GetModelEditScene().UpdateSelectRedSquares())
			Invalidate();
	}		
	C3DDragDropView::OnTimer(nIDEvent);
}

LRESULT CRender3DView::OnCheckProcessorType( WPARAM wParam,LPARAM lParam )
{
// 	switch (lParam)
// 	{
// 	case VM_PROCESSORTYPE_CHECK:
// 		{
/*	if(!GetModelEditScene().IsValid())*/
		GetModelEditScene().UpdataTerminalProcessors(GetEditor());		
// 		}
// 		break;
// 	default:
// 		break;
// 	}
	return TRUE;
}

CLandSideVehicleProbDispDetail* CRender3DView::GetMatchDispProb( int idx, const CString& s )
{
	if((int)m_displySet.size()<idx+1)
		m_displySet.resize(idx+1,NULL);

	CLandSideVehicleProbDispDetail* pDetail = m_displySet[idx];
	if(!pDetail)
	{
		pDetail = m_landsideVehicleDispProps.FindBestMatch(s);
		m_displySet[idx] = pDetail;
	}
	return pDetail;
}
