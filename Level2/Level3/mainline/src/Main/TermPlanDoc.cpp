// TermPlanDoc.cpp : implementation of the CTermPlanDoc class
//
#include "stdafx.h"
#include "ViewMsg.h"
#include "TermPlanDoc.h"
#include "./InputOnBoard/OnBoardAnalysisCondidates.h"
#include "3DCamera.h"
#include "3DView.h"
#include "ChildFrm.h"
#include "Render3DView.h"
#include "Render3DFrame.h"
#include "ActivityDensityData.h"
#include "AFTEMSBitmap.h"
#include "AnimationTimeManager.h"
#include "CFIDSView.h"
#include "common\WinMsg.h"
#include "HandleAirportDatabase.h"
#include "DensityGLView.h"
#include "DlgAnimationData.h"
#include "DlgAreaPortal.h"
#include "DlgArrayCopies.h"
#include "DlgBookmarkedCameras.h"
#include "DlgBookmarkedCamerasRender.h"
#include "DlgFireImpact.h"
#include "DlgFloorThickness.h"
#include "DlgInitArp.h"
#include "DlgLiveRecordParams.h"
#include "DlgMovieWriter.h"
#include "DlgMovieWriter2.h"
#include "DlgPickTime.h"
#include "DlgProcDisplayProperties.h"
#include "DlgProcDispOverrides.h"
#include "DlgProcessorSelect.h"
#include "DlgProjectName.h"
#include "DlgShapeSelect.h"
#include "DlgTracerDensity.h"
#include "DlgVisibleRegions.h"
#include "DlgWalkthrough.h"
#include "DlgWalkthroughRender.h"
#include "Floor2.h"
#include "FloorPropertiesDlg.h"
#include "MainFrm.h"
#include "MathematicView.h"
#include "MathFlowDataSet.h"
#include "MathProcessDataSet.h"
#include "./MFCExControl/ARCBaseTree.h"
#include "NodeView.h"
#include "ProcessFlowView.h"
#include "Processor2.h"
#include "ProcPropertiesDlg.h"
#include "RecordedCameras.h"
#include "RecordTimerManager.h"
#include "RepControlView.h"
#include "RepGraphView.h"
#include "RepListView.h"
#include "ShapesManager.h"
#include "SimEngineDlg.h"
#include "TermPlan.h"
#include "TVN.h"
#include "TVNode.h"

#include "DlgSelectedSimulationModels.h"
#include "common\SelectedSimulationData.h"
#include "ALTOException.h"
#include "DlgALTOException.h"
#include "common\airportdatabase.h"
#include "common\airportdatabaselist.h"
#include "Common\AirportDatabase.h"
//#include "common\ARCCriticalException.h"
//#include "common\ARCFatalException.h"
//#include "common\ARCImportantException.h"
#include "common\CodeTimeTest.h"
#include "common\SimAndReportManager.h"
#include "engine\ApronProc.h"
#include "engine\baggage.h"
#include "engine\barrier.h"
#include "engine\conveyor.h"
#include "engine\dep_sink.h"
#include "engine\dep_srce.h"
#include "engine\ElevatorProc.h"
#include "engine\escalator.h"
#include "engine\FixProc.h"
#include "engine\floorchg.h"
#include "engine\gate.h"
#include "engine\hold.h"
#include "engine\lineproc.h"
#include "engine\RetailProcessor.h"
#include "engine\MovingSideWalk.h"
#include "engine\proclist.h"
#include "engine\stair.h"
#include "inputs\AirsideInput.h"
#include "inputs\AllCarSchedule.h"
#include "inputs\MobileElemTypeStrDB.h"
#include "inputs\PipeDataSet.h"
#include "inputs\RailWayData.h"
#include "Inputs\MISCPROC.H"
#include "Inputs\PROCDATA.H"
#include "reports\AcOperationsReport.h"
#include "reports\ActivityBreakdownReport.h"
#include "reports\ActivityElements.h"
#include "reports\AverageQueueLengthReport.h"
#include "reports\BagCountReport.h"
#include "reports\BagDeliveryTimeReport.h"
#include "reports\BagDistReport.h"
#include "reports\BagWaitElement.h"
#include "reports\BagWaitReport.h"
#include "reports\CollisionReport.h"
#include "reports\DistanceReport.h"
#include "reports\DurationReport.h"
#include "reports\MutiRunReportAgent.h"
#include "reports\PassengerDensityReport.h"
#include "reports\PaxCountReport.h"
#include "reports\PrintLog.h"
#include "reports\QueueElement.h"
#include "reports\QueueLengthReport.h"
#include "reports\QueueReport.h"
#include "reports\ReportParameter.h"
#include "reports\servelem.h"
#include "reports\ServiceReport.h"
#include "reports\SpaceThroughputReport.h"
#include "reports\ThroughputReport.h"
#include "reports\UtilizationReport.h"
#include "results\EventLogBufManager.h"
#include "results\Fltlog.h"
#include "../Engine/StandProc.h"
#include "../Engine/RunwayProc.h"
#include "../Engine/TaxiwayProc.h"
#include "DlgSelectedAnimationModels.h"
#include "../Inputs/airportInfo.h"
#include <deque>
#include <direct.h>
#include <cmath>
#include "../Engine/BillboardProc.h"
#include ".\termplandoc.h"

#include "AirsideGround.h"
#include "AirsideGroundPropDlg.h"

#include "Airside3D.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/InputAirside.h"
#include "../Results/OutputAirside.h"
#include "../InputAirside/ARCCharManager.h"
#include "ImportExportManager.h"
#include "../InputAirside/ARCUnitManagerOwnerEnum.h"

#include "../AirsideReport/ReportManager.h"
#include "./Airport3D.h"

#include "../InputAirside/ProjectDatabase.h"
#include "../InputAirside/GlobalDatabase.h"
#include "../InputAirside/PanoramaViewData.h"
#include "DlgPanorama.h"
#include "Placement.h"

#include "../Common/UndoManager.h"
//chloe
//#include "../on_borad_impl/arc_engine_apdater.h"
//#include "OnBoradInput.h"
#include "InputAirsideDataCheck.h"

#include "..\InputOnBoard\cInputOnboard.h"
#include "..\InputOnBoard\AircaftLayOut.h"
#include "MainBarUpdateArgs.h"
#include "../Results/AirsideEventLogBuffer.h"
#include "../Common/INIFileHandle.h"
//#include "Onboard/AircraftModelDatabaseView.h"
//#include "Onboard/OnBoardAircraftModelComman.h"
#include  "LayoutView.h"
#include "./Onboard/AircraftLayout3dView.h"
#include <Renderer/RenderEngine/3DBaseView.h>
#include "AutomaticSaveTimer.h"
#include "UndoOptionHander.h"
#include "../Common/ACTypesManager.h"
#include <InputOnboard/AircraftComponentModelDatabase.h>
#include <InputOnboard/ComponentModel.h>
#include "FloorAdjustmentData.h"
#include "Common/ARCTracker.h"
#include "../InputAirside/ARCUnitManager.h"
#include "Onboard/MeshEdit3DView.h"
#include "Renderer/RenderEngine/RenderEngine.h"

#include "ACComponentShapesManager.h"
#include "ACFurnishingShapesManager.h"
#include "onboard/AircraftLayoutEditor.h"
#include "render3deditor.h"

#include <Renderer/RenderEngine/RenderEngine.h>

#include "Engine/Airside/CommonInSim.h"
#include "ARCportLayoutEditor.h"
#include "landside/InputLandside.h"
#include "Results/OutputLandside.h"
#include "ModelessDlgManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "AutoSaveHandler.h"
#include "DlgAirportDBNameInput.h"
#include "DlgFlightActypeMatch.h"

#include "..\Common\elaptime.h"
#include "Inputs/Walkthroughs.h"
#include "Proc2DataForClipBoard.h"
#include "Common/ARCStringConvert.h"



class CAircraftLayoutView;

//chloe
//class CTermPlanDocAdapter : public DocAdapter
//{
//	CTermPlanDoc* m_doc;
//public:
//	CTermPlanDocAdapter(CTermPlanDoc* doc) : m_doc(doc) { }
//	void update_all_view(void* view, UINT message, WPARAM wParam, LPARAM lParam)
//	{
//		if(!m_doc) return;
//		int data[] = { wParam, lParam };
//		m_doc->UpdateAllViews((CView*)view, (LPARAM)message, (CObject*)data);
//	}
//};


//
#define ACTYPECOUNT 22
const CString LOGSTRING_LOADPROJECT = "LOADPROJECT";

static LPCTSTR ACTypeStr[22]=
{
	"310",
	"32S",
	"330",
	"340",
	"737",
	"747",
	"74F",
	"74M",
	//"757",
	//"75F",
	//"767",
	"777",
	//"AB3",
	//"ABF",
	//"ABX",
	//"ABY",
	//"CR2",
	//"D10",
	//"D1F",
	//"M11",
	//"M1F",
	//"M90",
};
//

/////////////////////////////////////////////////////////////////////////////
// CTermPlanDoc
IMPLEMENT_DYNCREATE(CTermPlanDoc, CDocument)
BEGIN_MESSAGE_MAP(CTermPlanDoc, CDocument)
	//{{AFX_MSG_MAP(CTermPlanDoc)
	ON_COMMAND(ID_PROJECT_BACKUP, OnProjectBackup)
	ON_COMMAND(ID_PROJECT_RESTORE, OnProjectRestore)
	ON_UPDATE_COMMAND_UI(ID_CAMDOLLY, OnUpdateCamDolly)
	ON_COMMAND(ID_CAMDOLLY, OnCamDolly)
	ON_COMMAND(ID_CAMPAN, OnCamPan)
	ON_UPDATE_COMMAND_UI(ID_CAMPAN, OnUpdateCamPan)
	ON_COMMAND(ID_CAMTUMBLE, OnCamTumble)
	ON_UPDATE_COMMAND_UI(ID_CAMTUMBLE, OnUpdateCamTumble)
	ON_COMMAND(ID_LAYOUTLOCK, OnLayoutLock)
	ON_UPDATE_COMMAND_UI(ID_LAYOUTLOCK, OnUpdateLayoutLock)
	ON_COMMAND(ID_DISTANCEMEASURE, OnDistanceMeasure)
	ON_UPDATE_COMMAND_UI(ID_DISTANCEMEASURE, OnUpdateDistanceMeasure)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NAMEDVIEWS1, OnUpdateViewNamedViews3D)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NAMEDVIEWS9, OnUpdateViewNamedViews2D)
	ON_COMMAND(ID_CTX_PROCDISPPROPERTIES, OnCtxProcDisplayProperties)
	ON_UPDATE_COMMAND_UI(ID_CTX_COPYPROCESSOR, OnUpdateCtxCopyProcessor)
	ON_UPDATE_COMMAND_UI(ID_CTX_PROCDISPPROPERTIES, OnUpdateCtxProcDispProperties)
	ON_UPDATE_COMMAND_UI(ID_CTX_ROTATEPROCESSORS, OnUpdateCtxRotateProcessors)
	ON_UPDATE_COMMAND_UI(ID_CTX_SCALEPROCESSORS, OnUpdateCtxScaleProcessors)
	ON_UPDATE_COMMAND_UI(ID_CTX_SCALEPROCESSORSX, OnUpdateCtxScaleProcessorsX)
	ON_UPDATE_COMMAND_UI(ID_CTX_SCALEPROCESSORSY, OnUpdateCtxScaleProcessorsY)
	ON_UPDATE_COMMAND_UI(ID_CTX_SCALEPROCESSORSZ, OnUpdateCtxScaleProcessorsZ)
	ON_UPDATE_COMMAND_UI(ID_CTX_MOVEPROCESSORS, OnUpdateCtxMoveProcessors)
	ON_UPDATE_COMMAND_UI(ID_CTX_ROTATESHAPE, OnUpdateCtxRotateShape)
	ON_UPDATE_COMMAND_UI(ID_CTX_MOVESHAPE, OnUpdateCtxMoveShape)
	ON_UPDATE_COMMAND_UI(ID_ANIMATION_START, OnUpdateAnimationStart)
	ON_UPDATE_COMMAND_UI(ID_ANIMATION_STOP, OnUpdateAnimationStop)
	ON_UPDATE_COMMAND_UI(ID_ANIMATION_PAUSE, OnUpdateAnimationPause)
	ON_UPDATE_COMMAND_UI(ID_CTX_ACTIVATEFLOOR, OnUpdateActivateFloor)
	ON_COMMAND(ID_CTX_ACTIVATEFLOOR, OnActivateFloor)
	ON_COMMAND(ID_ANIMATION_START, OnAnimationStart)
	ON_COMMAND(ID_ANIMATION_STOP, OnAnimationStop)
	ON_COMMAND(ID_ANIMPAUSE, OnAnimationPause)
	ON_COMMAND(ID_ANIMPLAY, OnAnimationPlayF)
	ON_COMMAND(ID_ANIMREV, OnAnimationPlayB)
	ON_COMMAND(ID_ANIMPICKTIME, OnAnimationPickTime)
	ON_COMMAND(ID_CTX_NEWAREA, OnCtxNewArea)
	ON_COMMAND(ID_CTX_NEWPORTAL, OnCtxNewPortal)
	ON_COMMAND(ID_CTX_EDITAREA, OnCtxEditArea)
	ON_COMMAND(ID_CTX_EDITPORTAL, OnCtxEditPortal)
	ON_COMMAND(ID_CTX_DELETEAREA, OnCtxDeleteArea)
	ON_COMMAND(ID_CTX_DELETEPORTAL, OnCtxDeletePortal)
	ON_COMMAND(ID_CTX_AREACOLOR, OnCtxAreaColor)
	ON_COMMAND(ID_CTX_PORTALCOLOR, OnCtxPortalColor)
	ON_COMMAND(ID_CTX_NEWFLOOR, OnNewFloor)
	ON_COMMAND(ID_CTX_FLOORPROPERTIES, OnFloorProperties)
	ON_COMMAND(ID_CTX_GRIDVISIBLE, OnGridVisible)
	ON_COMMAND(ID_CTX_MONOCHROME, OnFloorMonochrome)
	ON_COMMAND(ID_CTX_FLOORVISIBLE, OnFloorVisible)
	ON_COMMAND(ID_CTX_SHOWALLFLOORS, OnShowAllFloors)
	ON_COMMAND(ID_CTX_SHOWALLGRIDS, OnShowAllFloorGrids)
	ON_COMMAND(ID_CTX_SHOWALLMAPS, OnShowAllFloorMaps)
	ON_COMMAND(ID_CTX_HIDEALLFLOORS, OnHideAllFloors)
	ON_COMMAND(ID_CTX_HIDEALLGRIDS, OnHideAllFloorGrids)
	ON_COMMAND(ID_CTX_HIDEALLMAPS, OnHideAllFloorMaps)
	ON_COMMAND(ID_CTX_ALLOPAQUE, OnAllFloorsThickness)
	ON_COMMAND(ID_CTX_ALLMONOCHROME, OnAllFloorsMonochrome)
	ON_COMMAND(ID_CTX_DEFAULTAREACOLOR, OnSetDefaultAreaColor)
	ON_COMMAND(ID_CTX_DEFAULTPORTALCOLOR, OnSetDefaultPortalColor)
	ON_COMMAND(ID_CTX_DEFAULTPROCDISPLAYPROPERTIES, OnSetDefaultProcDispProperties)
	ON_COMMAND(ID_CTX_PROCDISPPROPOVERRIDE, OnCtxProcDispPropOverride)
	ON_COMMAND(ID_CTX_HELP,OnCTXHelp)
	ON_UPDATE_COMMAND_UI(ID_CTX_NEWFLOOR, OnUpdateCtxNewFloor)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_UPDATE_COMMAND_UI(ID_TRACERS_ON, OnUpdateTracersOn)
	ON_COMMAND(ID_TRACERS_ON, OnTracersOn)
	ON_UPDATE_COMMAND_UI(ID_CTX_MOVESHAPE_Z, OnUpdateCtxMoveshapeZ)
	ON_COMMAND(ID_ANIMSTOP, OnAnimationStop)
	ON_UPDATE_COMMAND_UI(ID_CTX_MOVESHAPE_Z_0, OnUpdateCtxMoveshapeZ0)
	
	ON_UPDATE_COMMAND_UI_RANGE(ID_ANIMATION_SET1SPEED,  ID_ANIMATION_SETCUSTOMSPEED, OnUpdateAnimationSetSpeed)

	ON_UPDATE_COMMAND_UI(ID_CTX_PROCPROPERTIES, OnUpdateProcProperties)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RADAR, OnUpdateViewRadar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHARPENVIEW, OnUpdateViewSharpen)
	//}}AFX_MSG_MAP
     
	ON_UPDATE_COMMAND_UI(ID_CTX_DELETEFLOOR, OnUpdateFloorsDelete)



	ON_COMMAND(ID_CTX_REMOVELINE ,OnRemoveAlignLine)
    ON_UPDATE_COMMAND_UI(ID_CTX_REMOVELINE,OnUpdateRemoveAlignLine)
	ON_COMMAND(ID_CTX_PLACEMARKER, OnPlaceMarker)
	ON_COMMAND(ID_CTX_ALIGNFLOORS, OnAlignFloors)
	ON_COMMAND(ID_CTX_REMOVEMARKER, OnRemoveMarker)

    ON_UPDATE_COMMAND_UI(ID_CTX_PLACEMARKER, OnUpdatePlaceFloors)
	ON_UPDATE_COMMAND_UI(ID_CTX_ALIGNFLOORS, OnUpdateAlignFloors)
	ON_UPDATE_COMMAND_UI(ID_CTX_REMOVEMARKER, OnUpdateRemoveMarker)
	ON_COMMAND(ID_CTX_VISIBLEREGIONS, OnDefineVisibleRegions)
	ON_COMMAND(ID_VIEW_MANAGESAVEDVIEWS, OnManageBMViewpoints)
	ON_COMMAND_RANGE(ID_TRACERPARAMS_ON, ID_TRACERPARAMS_COLOR-1, OnTracerParamsOn)
	ON_COMMAND_RANGE(ID_TRACERPARAMS_COLOR, ID_TRACERPARAMS_DENSITY-1, OnTracerParamsColor)
	ON_COMMAND_RANGE(ID_TRACERPARAMS_DENSITY, ID_TRACERPARAMS_END-1, OnTracerParamsDensity)
	ON_UPDATE_COMMAND_UI_RANGE(ID_TRACERPARAMS_ON, ID_TRACERPARAMS_COLOR-1, OnUpdateTracerParamsOn)
	ON_UPDATE_COMMAND_UI_RANGE(ID_TRACERPARAMS_COLOR, ID_TRACERPARAMS_DENSITY-1, OnUpdateTracerParamsColor)
	ON_UPDATE_COMMAND_UI_RANGE(ID_TRACERPARAMS_DENSITY, ID_TRACERPARAMS_END-1, OnUpdateTracerParamsDensity)
	ON_COMMAND_RANGE(ID_TAGSPARAMS_ON, ID_TAGSPARAMS_END-1, OnTagsParamsOn)
	ON_UPDATE_COMMAND_UI_RANGE(ID_TAGSPARAMS_ON, ID_TAGSPARAMS_END-1, OnUpdateTagsParamsOn)
	ON_COMMAND(ID_PROJECT_RUNSIM, OnRunDelta)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_RUNSIM, OnUpdateRunDelta)
	ON_COMMAND(ID_TRACKERS_MOVIE, OnTrackersMovie)
	ON_COMMAND(ID_TRACKERS_WALKTHROUGH, OnTrackersWalkthrough)
	ON_COMMAND(ID_TRACKERS_PANORAMA, OnTrackersPanorama)
	ON_UPDATE_COMMAND_UI(ID_TRACKERS_MOVIE, OnUpdateTrackersMovie)
	ON_UPDATE_COMMAND_UI(ID_TRACKERS_WALKTHROUGH, OnUpdateTrackersWalkthrough)
	ON_UPDATE_COMMAND_UI(ID_TRACKERS_PANORAMA, OnUpdateTrackersPanorama)

	ON_COMMAND(ID_FIREIMPACT, OnFireImpact)
	ON_UPDATE_COMMAND_UI(ID_FIREIMPACT, OnUpdateFireImpact)
	ON_COMMAND(ID_ANIMATION_RECORD, OnAnimationRecord)
	ON_UPDATE_COMMAND_UI(ID_ANIMATION_RECORD, OnUpdateAnimationRecord)
	ON_COMMAND(ID_ANIMATION_RECSETTINGS, OnAnimationRecsettings)
	ON_COMMAND(ID_CTX_OPAQUEFLOOR, OnOpaqueFloor)
	ON_COMMAND(ID_CTX_SHOWHIDEMAP, OnShowHideMap)
	ON_COMMAND(ID_CTX_FLOORTHICKNESS_VALUE, OnFloorThickness)

	ON_COMMAND(ID_LAYOUT_INFO, OnClickInfoToolBtn)
	ON_UPDATE_COMMAND_UI(ID_LAYOUT_INFO, OnUpdateInfoToolBtn)

	ON_COMMAND(ID_LAYOUT_HIDEPROCTAGS, OnHideProcTags)
	ON_UPDATE_COMMAND_UI(ID_LAYOUT_HIDEPROCTAGS, OnUpdateHideProcTags)
	ON_COMMAND(ID_SCALE_SIZE,OnAnimScaleSize)
	ON_UPDATE_COMMAND_UI(ID_SCALE_SIZE,OnUpdateAnimScaleSize)
	ON_COMMAND(ID_TOOLLIGHT,OnTrafficLight)
	ON_UPDATE_COMMAND_UI(ID_TOOLLIGHT,OnUpdateTrafficLight)
	ON_COMMAND(ID_ANIMATION_HIDEACTAGS, OnHideACTags)
	ON_UPDATE_COMMAND_UI(ID_ANIMATION_HIDEACTAGS, OnUpdateHideACTags)
	ON_COMMAND(ID_CTX_INVISIBLEREGIONS, OnEditInvisibleregions)
	ON_COMMAND(ID_CTX_LANDDELETEPROC, OnCtxLanddeleteproc)
	ON_UPDATE_COMMAND_UI(ID_CTX_LANDDELETEPROC, OnUpdateCtxLanddeleteproc)
	ON_COMMAND(ID_CTX_SMOOTH_CTRLPOINTS, OnCtxSmoothCtrlpoints)
	ON_COMMAND(ID_CTX_EDIT_CTRLPOINTS, OnCtxEditCtrlpoints)
	ON_COMMAND(ID_LAYOUT_HIDETAXIWAYDIR, OnHideTaxiwayProc)
	ON_UPDATE_COMMAND_UI(ID_LAYOUT_HIDETAXIWAYDIR, OnUpdateHideTaxiwayProc)
	END_MESSAGE_MAP()

static void GetDefaultProcDispProperties(CDefaultDisplayProperties* pDDP, CProcessor2::CProcDispProperties* pPDP)
{
	pPDP->bDisplay[PDP_DISP_SHAPE] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcShapeOn);
	pPDP->bDisplay[PDP_DISP_SERVLOC] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcServLocOn);
	pPDP->bDisplay[PDP_DISP_QUEUE] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcQueueOn);
	pPDP->bDisplay[PDP_DISP_INC] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcInConstraintOn);
	pPDP->bDisplay[PDP_DISP_OUTC] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcOutConstraintOn);
	pPDP->bDisplay[PDP_DISP_PROCNAME] = pDDP->GetBoolValue(CDefaultDisplayProperties::ProcNameOn);
	pPDP->color[PDP_DISP_SHAPE] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcShapeColor);
	pPDP->color[PDP_DISP_SERVLOC] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcServLocColor);
	pPDP->color[PDP_DISP_QUEUE] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcQueueColor);
	pPDP->color[PDP_DISP_INC] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcInConstraintColor);
	pPDP->color[PDP_DISP_OUTC] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcOutConstraintColor);
	pPDP->color[PDP_DISP_PROCNAME] = pDDP->GetColorValue(CDefaultDisplayProperties::ProcNameColor);
}

static void SetDefaultProcDispProperties(CDefaultDisplayProperties* pDDP, CProcessor2::CProcDispProperties* pPDP)
{
	pDDP->SetBoolValue(CDefaultDisplayProperties::ProcShapeOn,pPDP->bDisplay[PDP_DISP_SHAPE]);
	pDDP->SetBoolValue(CDefaultDisplayProperties::ProcServLocOn,pPDP->bDisplay[PDP_DISP_SERVLOC]);
	pDDP->SetBoolValue(CDefaultDisplayProperties::ProcQueueOn,pPDP->bDisplay[PDP_DISP_QUEUE]);
	pDDP->SetBoolValue(CDefaultDisplayProperties::ProcInConstraintOn,pPDP->bDisplay[PDP_DISP_INC]);
	pDDP->SetBoolValue(CDefaultDisplayProperties::ProcOutConstraintOn,pPDP->bDisplay[PDP_DISP_OUTC]);
	pDDP->SetBoolValue(CDefaultDisplayProperties::ProcNameOn,pPDP->bDisplay[PDP_DISP_PROCNAME]);
	pDDP->SetColorValue(CDefaultDisplayProperties::ProcShapeColor,pPDP->bDisplay[PDP_DISP_SHAPE]);
	pDDP->SetColorValue(CDefaultDisplayProperties::ProcServLocColor,pPDP->bDisplay[PDP_DISP_SERVLOC]);
	pDDP->SetColorValue(CDefaultDisplayProperties::ProcQueueColor,pPDP->bDisplay[PDP_DISP_QUEUE]);
	pDDP->SetColorValue(CDefaultDisplayProperties::ProcInConstraintColor,pPDP->bDisplay[PDP_DISP_INC]);
	pDDP->SetColorValue(CDefaultDisplayProperties::ProcOutConstraintColor,pPDP->bDisplay[PDP_DISP_OUTC]);
	pDDP->SetColorValue(CDefaultDisplayProperties::ProcNameColor,pPDP->bDisplay[PDP_DISP_PROCNAME]);
}

/////////////////////////////////////////////////////////////////////////////
// CTermPlanDoc construction/destruction

CTermPlanDoc::CTermPlanDoc()
:m_CommandHistory(20),
m_umGlobalUnitManager(CARCUnitManager::GetInstance()),
m_CallOutManger(&m_calloutDispSpecs,this)
,m_arcport(this)
{
	m_bNeverSaved				= TRUE;
	m_pSelectedNode				= NULL;
	m_pADData					= NULL;
	m_nActiveFloor				= 0;
	//m_bShowGraph				= TRUE;
	m_uUniqueNumber				= 1;
	m_bMobileElementDisplay		= TRUE;
	m_bActivityDensityDisplay	= FALSE;
	m_bFlightInfoDisplay		= FALSE;
	m_bShowTracers				= FALSE;

	m_iScale = 10;
	m_bIsOpenStencil = FALSE;

 
	
	m_bRecord					= FALSE;

	m_bLayoutLocked				= FALSE;
	//m_pCurrentReportPara		= NULL; move to CARCReportManager
	m_pMenuTracerParams			= NULL;
	m_pMenuTagsParams			= NULL;
	m_pRecordedCameras			= NULL;

	//m_pDlgWalkthrough			= NULL;
	//m_pDlgWalkthroughRender		= NULL;
	
	//m_pPaxCountPerDispProp    = NULL;

	m_bNeedToSaveProjFile		= FALSE;

	m_animData.nAnimSpeed		= 100; //10x

	m_bHideACTags				= FALSE;
	
	m_bHideTrafficLight			= TRUE;

	m_bHideARP					= FALSE;
	m_vFlow.clear();
	m_vProcess.clear();

	m_simType = SimType_MontCarlo;
	m_systemMode = EnvMode_Terminal;

	m_pDlgSimEngine = NULL;
	
	m_bJustAfterCopyProc = false;

	//m_pLandsideDoc = new LandsideDocument(this);
	//m_pLandsideView = new LandsideDView(m_pLandsideDoc);
	//m_pAirsideSimulation = NULL;

	//m_airsideInput.SetInputTerminal(&GetTerminal());
	m_bdisplayTaxiDir = TRUE;

	m_pTempObj = NULL;


	m_bShowAirsideNodes = FALSE;
	m_bShowAirsideNodeNames = FALSE;

	//m_pAirsideReportManager = new CAirsideReportManager(NULL);// move to CARCReportManager

	m_walkthroughs = new CWalkthroughs(WalkthroughsFile);
	m_walkthroughsFlight = new CWalkthroughs(AirsideWalkthroughFile);
	m_walkthroughsVehicle = new CWalkthroughs(LandsideVehicleWalkthroughsFile);
	m_pPlacement = new CPlacement;
	m_pDlgPanorama = NULL;
 
	//chloe
	//m_doc_adapter = 0;
	m_bCanResposeEvent = true;


	m_playoutEditor = new CARCportLayoutEditor(this);
	//mpAircraftLayoutEditor = new CAircraftLayoutEditor(this);
	//m_pRender3DEditor = new CRender3DEditor(this);

	m_pModelessDlgMan = new CModelessDlgManager(this);

}

CTermPlanDoc::~CTermPlanDoc()
{
	if( m_bNeverSaved && IsModified() )
	{
		PROJMANAGER->DeleteProject(m_ProjInfo);
	}
	/*
	if( m_pCurrentReportPara )
	{
		delete m_pCurrentReportPara;
		m_pCurrentReportPara = NULL;
	}
	*/
	delete[] m_pMenuTracerParams;
	delete[] m_pMenuTagsParams;
	if (GetTerminal().m_pAirportDB != NULL)
	{	
		GetTerminal().m_pAirportDB->clearData();
	}

	//m_allPlacement.m_vDefined.clear();
	//m_allPlacement.m_vUndefined.clear();
	Cleanup();
	
	
	//delete m_pAirsideReportManager; move to CARCReportManager

	delete m_pPlacement;


	//delete m_doc_adapter; 
	//m_doc_adapter = NULL;

	delete m_walkthroughs;
	delete m_walkthroughsFlight;
	delete m_walkthroughsVehicle;
	//delete mpAircraftLayoutEditor;
	//delete m_pRender3DEditor; m_pRender3DEditor = NULL;
	delete m_playoutEditor;

	delete m_pModelessDlgMan;
}

void CTermPlanDoc::Cleanup()
{
	//DeleteAllCameras();
	
	//Added by Tim In 1/24/2003
	/*POSITION pos=m_listGroup.GetHeadPosition();
	while(pos)
	{
		delete m_listGroup.GetNext(pos);
	}
	m_listGroup.RemoveAll();*/

	delete m_pRecordedCameras;

	if (m_pDlgSimEngine)
	{
		if (::IsWindow(m_pDlgSimEngine->m_hWnd))
		{
			m_pDlgSimEngine->DestroyWindow();
		}
		delete m_pDlgSimEngine;
		m_pDlgSimEngine = NULL;
	}
}

BOOL CTermPlanDoc::OnNewDocument()
{
	m_bOpenDocSuccess=TRUE;
	if (!CDocument::OnNewDocument())
		return m_bOpenDocSuccess=FALSE;

    //env = new Environment();
	//CString appPath = ((CTermPlanApp*)AfxGetApp())->GetAppPath();
	//env->init( appPath );

	//Modified by Xie Bo 2002.4.21
	//For custom project
	//PROJMANAGER->CreateDefaultProject(&m_ProjInfo);
	CDlgProjectName dlg( _T("") );
	if(IDOK!=dlg.DoModal())
	{
		return m_bOpenDocSuccess=FALSE;
	}
	while((_mkdir(PROJMANAGER->m_csRootProjectPath + _T("\\") + dlg.m_strProjectName))!=0)//Director exist
	{
		AfxMessageBox("A project with the specified name already exists", MB_ICONEXCLAMATION | MB_OK);
		if(IDOK!=dlg.DoModal())
		{
			return m_bOpenDocSuccess=FALSE;
		}
	}
	_rmdir(PROJMANAGER->m_csRootProjectPath + _T("\\") + dlg.m_strProjectName);
	int nSelectedIndex=dlg.m_lSelectDB;
	CString sSelectDBName=dlg.m_sSelectedDBName;
    
	CAirportDatabase* _AirportDB = HandleAirportDatabase::OpenProjectDBForInitNewProject(sSelectDBName);
	ASSERT(_AirportDB);

	m_ProjInfo.lUniqueIndex=nSelectedIndex;
	m_ProjInfo.dbname = sSelectDBName;
	
	PROJMANAGER->CreateProject(dlg.m_strProjectName,&m_ProjInfo);
	InitializeAirside(FALSE);

	CAutoSaveHandler::GetInstance()->InitalAutoSaveHandler(((CTermPlanApp *)AfxGetApp()),PROJMANAGER->m_csRootProjectPath + _T("\\") + dlg.m_strProjectName,dlg.m_strProjectName) ;

	// before open the project , first open it's Golbal Databae
	if( !HandleAirportDatabase::OpenProjectDBForNewProject(this,_AirportDB))
		return false;
	
	//GetTerminal().m_AirsideInput->For_OnNewDocument( m_ProjInfo ); // deal AirsideInput.
	
	//deal AirsideInput
	try
	{
		GetTerminal().m_AirsideInput->GetPortals().loadDataSet( m_ProjInfo.path );
		GetTerminal().m_AirsideInput->GetFloors().loadDataSet( m_ProjInfo.path );
		GetTerminal().m_AirsideInput->GetDispProp().loadDataSet( m_ProjInfo.path );
		GetTerminal().m_AirsideInput->GetCameras().loadDataSet(m_ProjInfo.path);
		GetTerminal().m_AirsideInput->GetDispPropOverides().loadDataSet(m_ProjInfo.path);

		//m_pLandsideDoc->LoadDataSet(m_ProjInfo.path);
		m_arcport.m_pInputLandside->m_pFloors->LoadData(m_ProjInfo.path,GetProjectID());
		m_arcport.m_pInputLandside->LoadInputData(m_ProjInfo.path,GetProjectID());
		m_SurfaceMaterialLib.SetResourcePath(((CTermPlanApp*)AfxGetApp())->GetSurfaceTexturePath());
		m_SurfaceMaterialLib.Load();
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		AfxMessageBox( szBuf, MB_OK|MB_ICONWARNING );
		delete _pError;
		return FALSE;
	}


	// create the empty files
	try
	{
		GetTerminal().m_nProjID = GetProjectID();
	
		GetTerminal().m_pPipeDataSet->SetLandsideInput(m_arcport.m_pInputLandside);
		GetTerminal().loadInputs( m_ProjInfo.path );
		GetTerminal().GetSimReportManager()->loadDataSet( m_ProjInfo.path  );
		m_portals.loadDataSet( m_ProjInfo.path );
		m_floors.loadDataSet( m_ProjInfo.path );
		GetTerminal().m_AirsideInput->GetFloors().loadDataSet( m_ProjInfo.path );

		m_defDispProp.loadDataSet( m_ProjInfo.path );
		m_pPlacement->loadDataSet( m_ProjInfo.path );
		DoBridgeConvertCompatible();
		GetTerminal().m_AirsideInput->GetPlacementPtr()->loadDataSet( m_ProjInfo.path);
		m_arcport.m_economicManager.loadInputs( m_ProjInfo.path, &GetTerminal() );
		m_cameras.loadDataSet(m_ProjInfo.path);
		m_displayoverrides.loadDataSet(m_ProjInfo.path);

		m_arcport.m_congManager.loadDataSet(m_ProjInfo.path + "\\INPUT");

		//airside simulation add by hans
		GetAirsideSimLogs().GetSimReportManager()->loadDataSet2(m_ProjInfo.path,2);
		//
		GetTerminal().m_AirsideInput->getpStructurelist()->SetInputTerminal(&GetTerminal());
		GetTerminal().m_AirsideInput->getpStructurelist()->loadDataSet(m_ProjInfo.path);
		GetTerminal().m_pStructureList->SetInputTerminal(&GetTerminal());
		GetTerminal().m_pStructureList->loadDataSet(m_ProjInfo.path);

		GetTerminal().m_AirsideInput->getpWallShapeList()->SetInputTerminal(&GetTerminal());
		GetTerminal().m_AirsideInput->getpWallShapeList()->loadDataSet(m_ProjInfo.path);
		GetTerminal().m_pWallShapeList->SetInputTerminal(&GetTerminal());
		GetTerminal().m_pWallShapeList->loadDataSet(m_ProjInfo.path);

		//CPortals 
		//m_otherPlacement.InitPlacement(&m_portals);
		//m_otherPlacement.InitPlacement(GetTerminal().m_pPipeDataSet);
		//m_otherPlacement.InitPlacement(GetTerminal().m_pStructureList);
		//m_otherPlacement.InitPlacement(GetTerminal().m_pWallShapeList);
		//m_otherPlacement.InitPlacement(GetTerminal().m_pAreas);
		//landside
		/*m_arcport.landSideOperationList->SetInputTerminal(&GetTerminal());
		m_arcport.landSideOperationList->loadDataSet(m_ProjInfo.path);
		*/


       
		//LogFile 
//		m_logFile.loadDataSet(m_ProjInfo.path);
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		AfxMessageBox( szBuf, MB_OK|MB_ICONWARNING );
		delete _pError;
		return m_bOpenDocSuccess=FALSE;
	}
	
	m_animData.nAnimSpeed = 100; //10x 

	GetTerminal().setCurrentSimResult( -1 );
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->UpdateProjSelectBar();
	GetTerminal().setPreSimResult( -1 );
	SetTitle(m_ProjInfo.name);
	SetPathName(m_ProjInfo.path);

	SetModifiedFlag( TRUE );
	m_bNeverSaved = TRUE;
	m_bNeedToSaveProjFile = TRUE;

	CMainFrame* pWndMain = (CMainFrame*)theApp.GetMainWnd();
	ASSERT_KINDOF(CMainFrame, pWndMain);
	pWndMain->SetMainBarComboBox(true, true);

	/*CDlgInitArp dlgInitArp;
//	GetTerminal().m_AirsideInput->GetLL(m_ProjInfo.path,dlgInitArp.m_strLongitude,dlgInitArp.m_strLatitude);
	if(dlgInitArp.DoModal()==IDOK)
	{
		ALTAirport airport;
		airport.ReadAirport(m_nDefaultAirport);
		airport.setLongitude(dlgInitArp.m_lLongitude);
		airport.setlatitude(dlgInitArp.m_lLatitude);
		airport.UpdateAirport(m_nDefaultAirport);
		GetTerminal().m_AirsideInput->SetLL(m_ProjInfo.path,dlgInitArp.m_strLongitude,dlgInitArp.m_strLatitude,false);
		GetTerminal().m_AirsideInput->m_pAirportInfo->SetElevation(dlgInitArp.m_dElevation);
	}else
		return FALSE;*/
/*
	Processor *proc=new ArpProc();
	ProcessorID procID;
	procID.setID()
*/

	//initialize project information airside

	//initialize UnitManager for project id 
	InitiaUnitManager();
	//pWndMain->UpdateUnitBar(this);

	
	/*m_AutoSaveTimer = new CAutomaticSaveTimer(m_ProjInfo.path ,timer,((CTermPlanApp *)AfxGetApp()),&m_ProjInfo) ;
	m_AutoSaveTimer->BeginTimer() ;*/
	return m_bOpenDocSuccess=InitializeDefault();
}

void CTermPlanDoc::InitiaUnitManager(void)
{
	switch(m_systemMode)
	{
	case EnvMode_LandSide:
		m_umGlobalUnitManager.ReadData(GetProjectID(),UM_ID_GLOBAL_3);
		break;
	case EnvMode_Terminal:
		m_umGlobalUnitManager.ReadData(GetProjectID(),UM_ID_GLOBAL_1);
		break;
	case EnvMode_AirSide:
		m_umGlobalUnitManager.ReadData(GetProjectID(),UM_ID_GLOBAL_2);
		break;
	case EnvMode_Cargo:
		m_umGlobalUnitManager.ReadData(GetProjectID(),UM_ID_GLOBAL_1);
		break;
	case EnvMode_Environment:
		m_umGlobalUnitManager.ReadData(GetProjectID(),UM_ID_GLOBAL_1);
		break;
	case EnvMode_Financial:
		m_umGlobalUnitManager.ReadData(GetProjectID(),UM_ID_GLOBAL_1);
		break;
	default:
		{
			ASSERT(0);
			break;
		}
	}
}

bool CTermPlanDoc::InitializeAirside(BOOL bOpen)
{
	//open project database
	CMasterDatabase *pMasterDatabase = ((CTermPlanApp *)AfxGetApp())->GetMasterDatabase();
	//pMasterDatabase->Connect();
	CProjectDatabase projectDatabase(PROJMANAGER->GetAppPath());
	//projectDatabase.CloseProject(pMasterDatabase,m_ProjInfo.name);
	projectDatabase.setOpen(bOpen);
	try
	{
		if(projectDatabase.OpenProject(pMasterDatabase,m_ProjInfo.name,m_ProjInfo.path) == false)
			return false;
	}
	catch(CADOException&)
	{
		return false;
	}

	m_arcport.GetInputAirside().SetProjID(GetProjectID());
	int nAirportID = -1;
	if(ALTAirport::InitializeAirside(m_ProjInfo.name, nAirportID))
	{
		CAirsideGround asGround;
		asGround.setMainLevel(true);
		asGround.SaveData(nAirportID);
		asGround.SaveGridInfo(asGround.getID());
		asGround.SaveCADInfo(asGround.getID());	

		//GetCarmera
		ARCBoundingSphere bs( ARCVector3(0,0,asGround.Altitude()), asGround.GetBoundRadius());
		GetCamerasByMode(EnvMode_AirSide).InitDefault(bs);
		
	}
	SetCurrentAirport(nAirportID);
	m_nDefaultAirport = nAirportID;

	return true;
	//SetCameraWBS();
}
void CTermPlanDoc::CheckTheFlightActype()
{/*
	CACTypesManager* pManager = GetTerminal().m_pAirportDB->getAcMan() ;
	TCHAR actype[1024] = { 0 } ;
	CACType* Pactype = NULL ;
	std::map<CString,CDlgFlightActypeMatch::TY_DATA > m_NoMatchActype ;
	std::map<CString,CDlgFlightActypeMatch::TY_DATA> m_confiltActype  ;
	std::vector<CACType*> m_FindActypes;
	Flight* Pflight = NULL ;
	for (int i = 0 ; i < GetTerminal().flightSchedule->getCount() ;i++)
	{
		Pflight = GetTerminal().flightSchedule->getItem(i) ;

		Pflight->getACType(actype) ;
		Pactype = pManager->getACTypeItem(CString(actype)) ;
		if(Pactype == NULL)
		{
			if(m_NoMatchActype.find(actype) != m_NoMatchActype.end() )
			{
				m_NoMatchActype[CString(actype)].push_back(Pflight) ;
				continue ;
			}
			if(m_confiltActype.find(actype) != m_confiltActype.end())
			{
				m_confiltActype[CString(actype)].push_back(Pflight) ;
				continue ;
			}
			if(!pManager->FindActypeByIATACodeOrICAOCode(actype,m_FindActypes))
				m_NoMatchActype[CString(actype)].push_back(Pflight) ;
			else
			{
				if(m_FindActypes.size() == 1)
				{
					strcpy(actype,m_FindActypes[0]->GetIATACode()) ;
					Pflight->setACType(actype) ;
				}else
					m_confiltActype[CString(actype)].push_back(Pflight) ;
			}
		}
	}
	if(!m_confiltActype.empty())
	{
		CString Errormsg ;
		Errormsg.Format(_T("ARCTerm find some actype confilts in Flight schedule,please click \'Ok\'to solve them.")) ;
		MessageBox(NULL,Errormsg,_T("Warning"),MB_OK) ;
		CDlgFlightActypeMatch dlgflightActype(&m_confiltActype,pManager) ;
		dlgflightActype.DoModal() ;
		GetTerminal().flightSchedule->saveDataSet(m_ProjInfo.path,FALSE);
	}*/

}
BOOL CTermPlanDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	m_bNeverSaved = FALSE;
	m_bNeedToSaveProjFile = TRUE;
	TCHAR projPathName[MAX_PATH];
	_tcscpy(projPathName, lpszPathName);
	TCHAR* p = _tcsrchr(projPathName, _T('\\'));
	TCHAR* p2;
	if((p2 = _tcschr(p, _T('\\'))) != NULL) {
		p = ++p2;
	}
	m_bOpenDocSuccess=FALSE;//Modified by Tim In 2/8/2003
	if(PROJMANAGER->GetProjectInfo(p, &m_ProjInfo)) 
	{
		//ALTOException::setALTOExceptionHandler();

		try
		{
			if(InitializeAirside(TRUE) == false)
			{			
				AfxMessageBox("Open Airside failed.Database exception raises.");
				m_bOpenDocSuccess = FALSE;
				return m_bOpenDocSuccess;
			}
             CAutoSaveHandler::GetInstance()->InitalAutoSaveHandler(((CTermPlanApp *)AfxGetApp()),CString(lpszPathName),m_ProjInfo.name) ;
			// before open the project , first open it's Global Database
			 if( !HandleAirportDatabase::openGlobalDB(this))
				return false;

			 // load component models into aircraft component model shapes bar
			 ACCOMPONENTSHAPESMANAGER.Reload(GetTerminal().m_pAirportDB->getACCompModelDB());
			 ACFURNISHINGSHAPESMANAGER.Reload(GetTerminal().m_pAirportDB->getFurnishingMan());
			m_SurfaceMaterialLib.SetResourcePath(((CTermPlanApp*)AfxGetApp())->GetSurfaceTexturePath());
			m_SurfaceMaterialLib.Load();
			//read floor adjustment 
			m_iScale = CFloorAdjustmentData::ReadDataFromDataBase() ;

			try
			{
				GetInputOnboard()->LoadOnBoardData() ;
			}
			catch(CADOException&)
			{
				AfxMessageBox("Open Onboard failed.Database exception raises.");
				m_bOpenDocSuccess = FALSE;
				return m_bOpenDocSuccess;
			}

			//initialize UnitManager for project id
			InitiaUnitManager();

			CMainFrame* pMF = (CMainFrame*)AfxGetMainWnd();
			if(pMF->m_wndShapesBar)
			{
				delete pMF->m_wndShapesBar;
				pMF->m_wndShapesBar = NULL;
			}
			pMF->CreateShapesBar();
			pMF->m_wndShapesBar->m_strProjPath = lpszPathName;
			pMF->DockControlBar(pMF->m_wndShapesBar, AFX_IDW_DOCKBAR_RIGHT);
			pMF->ShowControlBar(pMF->m_wndShapesBar, FALSE, FALSE);

			m_bOpenDocSuccess = ReloadInputData();
			pMF->m_wndShapesBar->SetUserBarManager(GetTerminal().m_pUserBarMan);
			pMF->m_wndShapesBar->AddUserShapesToShapesBar();

			CheckTheFlightActype() ;
			ReadAllCameras();
		}
		catch (ALTOException& openError)
		{
			CDlgALTOException dlg(CDlgALTOException::DocOpenError, openError);
			dlg.DoModal();

			m_bOpenDocSuccess = FALSE;
			return m_bOpenDocSuccess; 
		}

		if(m_bOpenDocSuccess)
		{
			CWnd* pMF=AfxGetMainWnd();
			ToolBarUpdateParameters updatePara;
			updatePara.m_operatType = ToolBarUpdateParameters::OpenProject;
			updatePara.m_nCurMode = GetCurrentMode();
			updatePara.m_pDoc = this;
			pMF->SendMessage(UM_SHOW_CONTROL_BAR,1,(LPARAM)&updatePara) ;
		}

		CMainFrame* pWndMain = (CMainFrame*)theApp.GetMainWnd();
		ASSERT_KINDOF(CMainFrame, pWndMain);
		pWndMain->getCurrentEnviro(m_systemMode,m_simType);
		pWndMain->SetMainBarComboBox(TRUE,FALSE) ;
		//pWndMain->UpdateUnitBar(this);
		//pWndMain->UpdateMainFrameBar(this);
	}
	/*m_AutoSaveTimer = new CAutomaticSaveTimer(lpszPathName,timer,((CTermPlanApp *)AfxGetApp()),&m_ProjInfo) ;
	m_AutoSaveTimer->BeginTimer() ;*/

	return m_bOpenDocSuccess;
}

BOOL CTermPlanDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	m_bNeverSaved = FALSE;
	
	SetModifiedFlag( FALSE );
	//TODO: ADD SAVING CODE HERE (eg. update miscparams.dat etc..)
	//BOOL bRet = CDocument::OnSaveDocument(lpszPathName);
	//return bRet;
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CTermPlanDoc serialization

void CTermPlanDoc::Serialize(CArchive& ar)
{
	/*
	if (ar.IsStoring()) {
		ar << m_vCameras.size();
		for(int i=0; i<m_vCameras.size(); i++) {
			if(m_vCameras[i] == m_pCurrentCamera)
				ar << TRUE;
			else
				ar << FALSE;
			ar << m_vCameras[i];
		}
	}
	else {
		// TODO: add loading code here
		CString s;
		int size;
		BOOL bSelected;
		ar >> s;
		ar >> size;
		DeleteAllCameras();
		m_vCameras.reserve(size);
		for(int i=0; i<size; i++)
		{
			ar >> bSelected;
			C3DCamera* pCam;
			ar >> pCam;
			m_vCameras.push_back(pCam);
			if(bSelected)
				m_pCurrentCamera = pCam;
		}
	}
	*/	
}

/////////////////////////////////////////////////////////////////////////////
// CTermPlanDoc diagnostics

#ifdef _DEBUG
void CTermPlanDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTermPlanDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

CProcessor2* CTermPlanDoc::GetProcessor2FromID(const ProcessorID& id, EnvironmentMode mode)
{
	for(int i=0; i<static_cast<int>(GetCurrentPlacement(mode)->m_vDefined.size()); i++) {
		if(id == *(GetCurrentPlacement(mode)->m_vDefined[i]->GetProcessor()->getID()))
			return GetCurrentPlacement(mode)->m_vDefined[i];
	}
	return NULL;
}

CProcessor2* CTermPlanDoc::GetProcessor2FromID(const ALTObjectID& id, EnvironmentMode mode)
{
	for (int i = 0; i < static_cast<int>(GetCurrentPlacement(mode)->m_vUndefined.size()); i++)
	{
		if (id == GetCurrentPlacement(mode)->m_vUndefined[i]->ShapeName())
		{
			return GetCurrentPlacement(mode)->m_vUndefined[i];
		}
	}
	return NULL;
}
CProcessor2 *CTermPlanDoc::GetProcessor2FromName(const CString strName, EnvironmentMode mode /* = EnvMode_Unknow */)
{
	for(int i=0; i<static_cast<int>(GetCurrentPlacement(mode)->m_vDefined.size()); i++) 
	{
		Processor* proc=GetCurrentPlacement(mode)->m_vDefined[i]->GetProcessor();
		CString temp = proc->getID()->GetIDString();
		if (temp.CollateNoCase(strName)==0)
		{
			return GetCurrentPlacement(mode)->m_vDefined[i];
		}
	}
	return NULL;
}


BOOL CTermPlanDoc::InitializeDefault()
{	
	m_eAnimState = anim_none;
	//delete all cameras
	//DeleteAllCameras();

	//CreateDefaultCameras();
	//m_pLandsideDoc->InitializeDefault(m_ProjInfo);
	m_pSelectedNode = NULL;

	CFloor2* pDefaultFloor = new CFloor2(0);
	m_floors.AddFloor(pDefaultFloor);
	
	m_nActiveFloor = 0;
	pDefaultFloor->IsActive(TRUE);
	m_floors.saveDataSet(m_ProjInfo.path, false);
	m_nLastActiveFloor = -1;

	GetTerminal().m_AirsideInput->For_InitializeDefault( m_ProjInfo );

	InitActiveFloorCB();
	
	m_msManager.SetTermPlanDoc(this);
	m_msManager.InitMSManager();

	//set default cameras
	

	//save the project information(projinfo) resolved the debug that 
	//	when creating a new project cause a crash the project doesn't save.    
	DoFileSave();
	

	return TRUE;
}

//chloe
//void CTermPlanDoc::UpdateAllViewEx(void* view, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	if(!m_doc_adapter) return;
//	m_doc_adapter->update_all_view(view, message, wParam, lParam);
//}

BOOL CTermPlanDoc::InitializeProject()
{
	//chloe
	//m_doc_adapter = new CTermPlanDocAdapter(this);

	m_eAnimState = anim_none;

	GetTerminal().m_AirsideInput->For_InitializeProject( m_ProjInfo );
	//m_pLandsideDoc->InitializeProject(m_ProjInfo);

	
	m_nLastActiveFloor = -1;
	if(m_floors.GetCount()== 0) 
	{
		//create number of floors from proclist
		int nfloorCount = GetProcessorList().GetNumFloors();
		ASSERT(nfloorCount);//if the nfloorCount = 0, the 3Dview will crash
		for(int i=0; i<nfloorCount; i++)
		{
			CFloor2* pFloor = new CFloor2(i);
			m_floors.AddFloor(pFloor);
		}
		m_nActiveFloor = 0;
		m_floors.saveDataSet(m_ProjInfo.path, false);
	}
	else
	{
		//find active floor
		for(int i=0; i<m_floors.GetCount(); i++) 
		{
			if(m_floors.GetFloor(i)->IsActive()) 
			{
				m_nActiveFloor = i;
				break;
			}
		}
	}

	ASSERT(m_floors.GetCount());
	if( EnvMode_AirSide == m_systemMode)
		m_nActiveFloor = 0;

	for(int i=0; i<m_floors.GetCount(); i++) {
		CFloor2* pFloor = m_floors.GetFloor2(i);
		if(pFloor->LoadCAD())
			pFloor->LoadCADLayers();
	}

	InitActiveFloorCB();

	
	//if this is an old project, m_placements
	//will be empty.
	//In this case, build it from the procList
	if(GetTerminal().procList->getProcessorCount() != m_pPlacement->m_vDefined.size()) {
		//ASSERT(m_placement.m_vDefined.size() == 0);	is some case, size may not be 0, we should rebuild the placement.txt
		if ( AfxMessageBox(" File placement.txt may have invalid data, system will try to fixed that. Some infomation maybe lost, continue to fix ?", MB_YESNO ) == IDYES )
		{
			ProcessorList *procList = GetTerminal().procList;
			ASSERT(procList != NULL);
			CProcessor2::CProcDispProperties ddp;
			GetDefaultProcDispProperties(&m_defDispProp, &ddp);
			m_pPlacement->BuildFromProcList(procList, ddp);
			m_pPlacement->saveDataSet(m_ProjInfo.path, false);			
		}
		else
		{
			return FALSE;
		}
	}

	
	//frank
	if(GetTerminal().m_AirsideInput->GetProcessorList()->getProcessorCount() 
		!= GetTerminal().m_AirsideInput->GetPlacementPtr()->m_vDefined.size())
	{
		if ( AfxMessageBox(" File airside placement.txt may have invalid data, system will try to fixed that. Some infomation maybe lost, continue to fix ?", MB_YESNO ) == IDYES )
		{
			ProcessorList *procList = GetTerminal().m_AirsideInput->GetProcessorList();
			ASSERT(procList != NULL);
			CProcessor2::CProcDispProperties ddp;
			GetDefaultProcDispProperties(&m_defDispProp, &ddp);
			GetTerminal().m_AirsideInput->GetPlacementPtr()->BuildFromProcList(procList, ddp);
			GetTerminal().m_AirsideInput->GetPlacementPtr()->saveDataSet(m_ProjInfo.path, false);			
		}
		else
		{
			return FALSE;
		}
	}

	
	//check landside placement 
	if(GetCurrentPlacement(EnvMode_LandSide)->m_vDefined.size()!= GetProcessorList(EnvMode_LandSide).getProcessorCount())
	{
		if ( AfxMessageBox(" File landside placement.txt may have invalid data, system will try to fixed that. Some infomation maybe lost, continue to fix ?", MB_YESNO ) == IDYES )
		{
			ProcessorList& procList = GetProcessorList(EnvMode_LandSide);
			//ASSERT(procList != NULL);
			CProcessor2::CProcDispProperties ddp;
			GetDefaultProcDispProperties(&m_defDispProp, &ddp);
			GetCurrentPlacement(EnvMode_LandSide)->BuildFromProcList(&procList, ddp);
			GetCurrentPlacement(EnvMode_LandSide)->m_vUndefined.clear();
			GetCurrentPlacement(EnvMode_LandSide)->saveDataSet(m_ProjInfo.path, false);			
		}
		else
		{
			return FALSE;
		}
	}
	

	m_msManager.SetTermPlanDoc(this);
	m_msManager.InitMSManager();

	return TRUE;
}


BOOL CTermPlanDoc::RouteCmdToAllViews(CView *pView, UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo)
{
	POSITION pos = GetFirstViewPosition();
	while(pos != NULL)
	{
		CView* pNextView = GetNextView(pos);
		if(pNextView != pView)
		{
			if(pNextView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
				return TRUE;
		}
	}
	return FALSE;
}

void CTermPlanDoc::OnNewFloor()
{
	if(!m_pSelectedNode)
		return;

	CFloors& curModefloors = GetFloorByMode(m_systemMode);
	CFloor2* pFloor = curModefloors.NewFloor();	

	
	if(EnvMode_Terminal==m_systemMode)
	{		
		CTVNode* pNew = new CTVNode(pFloor->FloorName(), IDR_CTX_FLOOR);
		pNew->m_dwData = (DWORD) GetFloorByMode(m_systemMode).GetCount()-1;
		pNew->m_iImageIdx = ID_NODEIMG_FLOORNOTACTIVE;
		if(m_pSelectedNode->GetChildCount())
			m_pSelectedNode->AddChildBefore(pNew, m_pSelectedNode->GetChildByIdx(0));	
		else
			m_pSelectedNode->AddChild(pNew);

		//compatible for elevator behavior stop at floor
		InputTerminal* pInTerm = &(GetTerminal());
		MiscProcessorData* pMiscDB = pInTerm->miscData->getDatabase( Elevator );
		int nMiscDataCount = pMiscDB->getCount();
		for (int i = 0; i < nMiscDataCount; ++i)
		{
			MiscDataElement* pMiscDataElement = (MiscDataElement*)pMiscDB->getItem(i);
			MiscElevatorData* pElevatorElement = (MiscElevatorData*)pMiscDataElement->getData();
			pElevatorElement->addStopAtFloor(FALSE);
		}
		if (nMiscDataCount  >  0)
		{
			pInTerm->miscData->saveDataSet(m_ProjInfo.path, false);
		}
		UpdateAllViews(NULL,NODE_HINT_NEWNODE,(CObject*) pNew);
	}
	else
	{			
		UpdateAllViews(NULL,VM_NEW_ONE_FLOOE,(CObject*)pFloor->Level());
	}
	
	curModefloors.saveDataSet(m_ProjInfo.path, true);
	InitActiveFloorCB();

	//UpdateAllViews(NULL,VM_NEW_ONE_FLOOE,(C))
	//UpdateAllFloorsInRender3DView();
}

void CTermPlanDoc::OnFloorProperties() 
{	
	if(EnvMode_AirSide == m_systemMode )
		return;

	ASSERT(m_pSelectedNode!=NULL );
	CFloor2* pFloor;	
	INT_PTR nRet = IDCANCEL;
		
	pFloor = GetCurModeFloor().GetFloor2((int)m_pSelectedNode->m_dwData);
	CFloorPropertiesDlg dlg(pFloor,this);
	nRet = dlg.DoModal();	
	if(nRet== IDOK)
	{
		UpdateViewSharpTextures();
		UpdateAllViews(NULL, NODE_HINT_SELCHANGED, (CObject*) pFloor);
		UpdateFloorInRender3DView(pFloor);
		GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
		SetCameraWBS();
	}

}

void CTermPlanDoc::SetCameraWBS()
{
	if(!Get3DViewParent())
		return;
	
	ARCVector3 ptC;
	double r;
	ARCVector3 ptMin(DBL_MAX,DBL_MAX,DBL_MAX);
	ARCVector3 ptMax(-DBL_MAX,-DBL_MAX,-DBL_MAX);
	if( EnvMode_AirSide == m_systemMode){
		ARCBoundingSphere bs = Get3DViewParent()->GetAirside3D()->GetBoundingSphere();
		
		for(int i=0; i<4; i++) 
		{		
			Get3DViewParent()->GetWorkingCamera(0,i)->SetWBSData(bs.GetLocation(),bs.GetRadius());
		}	
		return;
	}

	CFloors* ptFloors = &GetCurModeFloor();

	for(int i=0; i<ptFloors->GetCount(); i++) {
		double dAlt = ptFloors->GetFloor2(i)->Altitude();
		if(dAlt < ptMin[VZ])
			ptMin[VZ] = dAlt;
		if(dAlt > ptMax[VZ])
			ptMax[VZ] = dAlt;
		CGrid* pGrid = ptFloors->GetFloor2(i)->GetGrid();
		double dGridSize = MAX(pGrid->dSizeX, pGrid->dSizeY);
		if(dGridSize > ptMax[VX])
			ptMax[VX] = dGridSize;
		if(-dGridSize < ptMin[VX])
			ptMin[VX] = -dGridSize;
		if(dGridSize > ptMax[VY])
			ptMax[VY] = dGridSize;
		if(-dGridSize < ptMin[VY])
			ptMin[VY] = -dGridSize;
	}
	ptC = (ptMax + ptMin)/2;
	r = (ptMax-ptC).Magnitude();
	for(int i=0; i<4; i++) 
	{		
		Get3DViewParent()->GetWorkingCamera(0,i)->SetWBSData(ptC,r);
	}
}

void CTermPlanDoc::OnFloorVisible() 
{
	ASSERT(m_pSelectedNode!=NULL /*&& m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR*/);
	CFloor2* pFloor ;
	if( EnvMode_AirSide == m_systemMode)
		pFloor = GetTerminal().m_AirsideInput->GetFloors().GetFloor2(0);
	else
		pFloor = GetCurModeFloor().GetFloor2((int)m_pSelectedNode->m_dwData);

	// toggle visibility
	pFloor->IsVisible(!pFloor->IsVisible());
	UpdateViewSharpTextures();
	UpdateAllViews(NULL);
	UpdateFloorInRender3DView(pFloor);

	if( EnvMode_AirSide == m_systemMode)
		GetTerminal().m_AirsideInput->GetFloors().saveDataSet(m_ProjInfo.path, true);
	else
		GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
}

void CTermPlanDoc::UpdateViewSharpTextures()
{
	C3DView* pView = Get3DView();
	if(pView != NULL) {
		pView->UseSharpTexture(FALSE);
		if(pView->IsAutoSharp())
			pView->GenerateSharpTexture();
	}
}

void CTermPlanDoc::OnGridVisible() 
{
	ASSERT(m_pSelectedNode!=NULL /*&& m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR*/);
	CFloor2* pFloor ;
	if( EnvMode_AirSide == m_systemMode)
		pFloor = GetTerminal().m_AirsideInput->GetFloors().GetFloor2(0);
	else
		pFloor = GetCurModeFloor().GetFloor2((int)m_pSelectedNode->m_dwData);
	// toggle grid visibility
	pFloor->IsGridVisible(!pFloor->IsGridVisible());

	UpdateViewSharpTextures();
	UpdateAllViews(NULL);
	UpdateFloorInRender3DView(pFloor);

	if( EnvMode_AirSide == m_systemMode)
		GetTerminal().m_AirsideInput->GetFloors().saveDataSet(m_ProjInfo.path, true);
	else
		GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
}

void CTermPlanDoc::OnFloorMonochrome()
{
	ASSERT(m_pSelectedNode!=NULL /*&& m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR*/);
	CFloor2* pFloor;
	if( EnvMode_AirSide == m_systemMode)
		pFloor = GetTerminal().m_AirsideInput->GetFloors().GetFloor2(0);
	else
		pFloor =  GetCurModeFloor().GetFloor2((int)m_pSelectedNode->m_dwData);

	// if the floor is currently *not* monochrome, get a color from user
	if(!pFloor->IsMonochrome()) {
		CColorDialog dlgColor(pFloor->GetMonochromeColor(), CC_ANYCOLOR | CC_FULLOPEN);
		if(dlgColor.DoModal() == IDOK) {
			pFloor->SetMonochromeColor(dlgColor.GetColor());
		}
	}
	// toggle floor monochrome
	pFloor->IsMonochrome(!pFloor->IsMonochrome());

	if( EnvMode_AirSide == m_systemMode)
		GetTerminal().m_AirsideInput->GetFloors().saveDataSet(m_ProjInfo.path, true);
	else
		GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);

	pFloor->InvalidateTexture();
	UpdateViewSharpTextures();
	UpdateAllViews(NULL);
	UpdateFloorInRender3DView(pFloor);
}

BOOL CTermPlanDoc::DeleteFloor(int nFloor)
{
	ASSERT(nFloor >= 0 && nFloor < GetCurModeFloor().GetCount());
	
	if( EnvMode_AirSide == m_systemMode)
		return FALSE;
	
	//cannot delete floor if it is the only floor
	if(GetCurModeFloor().GetCount() <= 1) {
		AfxMessageBox(_T("Cannot delete floor.\nAt least one floor must exist in a model."),
			MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	if(m_systemMode == EnvMode_Terminal)
	{
		//to delete a floor, all processors on it must first be deleted
		BOOL bFoundProc = FALSE;
		int i;
		for(i=0; i<static_cast<int>(GetCurrentPlacement()->m_vUndefined.size()); i++) {
			if(GetCurrentPlacement()->m_vUndefined[i]->GetFloor() == nFloor) {
				bFoundProc = TRUE;
				break;
			}
		}
		if(!bFoundProc) {
			for(i=0; i<static_cast<int>(GetCurrentPlacement()->m_vDefined.size()); i++) {
				if(GetCurrentPlacement()->m_vDefined[i]->GetFloor() == nFloor) {
					bFoundProc = TRUE;
					break;
				}
			}
		}
		if(bFoundProc) { //the floor has at least one processor on it
			AfxMessageBox(_T("Cannot delete floor.\nPlease remove all processors from floor before deleting,"),
				MB_OK | MB_ICONWARNING);
			return FALSE;
		}

		CString sMsg;
		sMsg.Format("Are you sure you want to delete the floor \'%s\'?", m_floors.GetFloor2(nFloor)->FloorName());
		if(AfxMessageBox(sMsg, MB_YESNO) == IDNO) {
			return FALSE;
		}

		InputTerminal* pInTerm = &(GetTerminal());
		ProcessorArray vElevatorList;
		pInTerm->procList->getProcessorsOfType(Elevator,vElevatorList);
		ProcessorArray vCheckList;
		for (int iElevator = 0; iElevator < vElevatorList.getCount(); iElevator++)
		{
			ElevatorProc* pElevator = (ElevatorProc*)vElevatorList.getItem(iElevator);

			int iMaxFloor = pElevator->GetMaxFloor();
			int iMinFloor = pElevator->GetMinFloor();
			if (nFloor > iMaxFloor)
				continue;
			
			if (nFloor > iMinFloor && nFloor <= iMaxFloor)
			{
				pElevator->SetMaxFloor(pElevator->GetMaxFloor() - 1);
				pElevator->GetWaitAreaPos().clear();
				for(int i=pElevator->GetMinFloor()+1;i<=pElevator->GetMaxFloor()+1;i++)
				{
					pElevator->GetWaitAreaPos().push_back(true);
				}
				vCheckList.Add(pElevator);
				continue;
			}

			if (nFloor <= iMinFloor)
			{
				pElevator->SetMinFloor(pElevator->GetMinFloor() - 1);
				pElevator->SetMaxFloor(pElevator->GetMaxFloor() - 1);
				pElevator->GetWaitAreaPos().clear();
				for(int i=pElevator->GetMinFloor()+1;i<=pElevator->GetMaxFloor()+1;i++)
				{
					pElevator->GetWaitAreaPos().push_back(true);
				}
				vCheckList.Add(pElevator);
				continue;
			}
		}

		if (vCheckList.IsEmpty() == false)
		{
			pInTerm->procList->saveDataSet(m_ProjInfo.path, false);
		}

		MiscProcessorData* pMiscDB = pInTerm->miscData->getDatabase( Elevator );
		int nMiscDataCount = pMiscDB->getCount();
		for (int i = 0; i < nMiscDataCount; ++i)
		{
			MiscDataElement* pMiscDataElement = (MiscDataElement*)pMiscDB->getItem(i);
			MiscElevatorData* pElevatorElement = (MiscElevatorData*)pMiscDataElement->getData();
			pElevatorElement->removeStopAtFloor(nFloor);
		}

		if (nMiscDataCount > 0)
		{
			pInTerm->miscData->saveDataSet(m_ProjInfo.path, false);
		}

	}	

	//now remove floor
	GetCurModeFloor().RemoveFloor(nFloor);
	

	//also remove tree view node associated with floor
	if(EnvMode_Terminal == m_systemMode )		
	{
		CTVNode* pFloorsNode = FloorsNode();
		ASSERT(pFloorsNode!=NULL);
		int i=pFloorsNode->GetChildCount()-1;
		for(; i>=0; i--) 
		{
			CTVNode* pFNode = (CTVNode*)pFloorsNode->GetChildByIdx(i);
			if(nFloor == (int)pFNode->m_dwData) 
			{
				//remove child
				pFloorsNode->RemoveChild(i);
				break;
			}
		}
		for(int j=i-1; j>=0; j--) 
		{
			CTVNode* pFNode = (CTVNode*)pFloorsNode->GetChildByIdx(j);
			pFNode->m_dwData = (DWORD) ((int)pFNode->m_dwData) - 1;
		}

		//adjust active floor to 1st floor
		if(m_nActiveFloor == 0) {
			CTVNode* pFNode = (CTVNode*)pFloorsNode->GetChildByIdx(pFloorsNode->GetChildCount()-1);
			pFNode->m_iImageIdx = ID_NODEIMG_FLOORACTIVE;
			//refresh floors node
			UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pFloorsNode);
		}
		else {
			ActivateFloor(0);
			UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pFloorsNode);
		}
	}
	else
	{
		UpdateAllViews(NULL,VM_DELETE_ONE_FLOOR,(CObject*)nFloor);
	}
	
	GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
	InitActiveFloorCB();
	//UpdateAllFloorsInRender3DView();
	return TRUE;
}
//CObjectDisplayList__
void CTermPlanDoc::DeleteProcessors(const CObjectDisplayList& proclist)
{
	//int c = proclist.size();
	CObjectDisplayList tempList(proclist);
	int nCount = tempList.GetCount();
	for(int i=0; i<nCount; i++) 
	{
		CObjectDisplay *pObjDisplay = tempList.GetItem(i);
		if (pObjDisplay && pObjDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			DeleteProcessor((CProcessor2*)pObjDisplay);
		}

	}
}

BOOL CTermPlanDoc::DeleteProcessor(CProcessor2* pProc2)
{
	//unselect all procs
	UnSelectAllProcs();

	//if pProc2's v1 processor == NULL then this is an undefined processor
	if(pProc2->GetProcessor() == NULL) {
		int i;
		//remove from undefined list
		int nSize = GetCurrentPlacement()->m_vUndefined.size();
		for(i=0; i<nSize; i++) {
			if(pProc2 == GetCurrentPlacement()->m_vUndefined[i]) {
				GetCurrentPlacement()->m_vUndefined.erase(GetCurrentPlacement()->m_vUndefined.begin()+i);
				GetCurrentPlacement()->m_vUndefined.removePro2(pProc2);
				break;
			}
		}
		if(i==nSize) {
			AfxMessageBox(_T("An error occured while atempting to delete the processor.\nThe processor was not found."), MB_OK | MB_ICONSTOP);
			return FALSE; //could not find pProc2 in undefined list
		}
		//refresh under construction node & redraw 2d/3d view
		//find ucnode with pProc as dwData
		CTVNode* pUCParent = UnderConstructionNode();
		ASSERT(pUCParent != NULL);
		//get node with dwData == pProc
		CTVNode* pUCNode = NULL;
		CTVNode* pLayoutNode = (CTVNode*) pUCParent->Parent();
		//if underconstruction node is now empty, remove it
			
		CTVNode* tvNode = NULL;
		std::deque<CTVNode*> vList;
		vList.push_front(pUCParent);
		while(vList.size() != 0) 
		{
			CTVNode* pNode = vList[0];
			vList.pop_front();
			if(pProc2 == (CProcessor2*)(pNode->m_dwData))
			{
				tvNode = pNode;
				break;
			}
			for(int i=0; i<pNode->GetChildCount(); i++) 
			{
				vList.push_front((CTVNode*) pNode->GetChildByIdx(i));
			}
		}
		if (tvNode != NULL)
		{
			CTVNode* pParentNode = NULL;
			pParentNode = (CTVNode*)(tvNode->Parent());
			CTVNode* pNode = NULL;
			pParentNode->RemoveChild(tvNode);
			while (pParentNode != pLayoutNode)
			{
				if (pParentNode->GetChildCount() == 0)
				{
					pNode = pParentNode;
					pParentNode =(CTVNode*) pParentNode->Parent();
					pParentNode->RemoveChild(pNode);
				}
				else
				{
					break;
				}
			}
			if (tvNode->m_nIDResource == IDR_MENU_CONTOUR)
			{
				UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pUCParent);
				GetCurrentPlacement()->saveDataSet(m_ProjInfo.path, true);
				return TRUE;
			}
		}

		UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*)pLayoutNode);
		GetCurrentPlacement()->saveDataSet(m_ProjInfo.path, true);
		return TRUE;
	}
	else {
			bool bIsStation = false;
			int iProcessorType = pProc2->GetProcessor()->getProcessorType();
			if( pProc2->GetProcessor()->getProcessorType() == IntegratedStationProc )
			{
				bIsStation = true;
				if ( AfxMessageBox( " Since the railway system and the train schedule system may use this station, after you delete it we will update all related system automatically! " ,MB_OKCANCEL) == IDCANCEL )
				{
					return FALSE;
				}
				
			}

			//remove from defined list
			int i;
			int nSize = GetCurrentPlacement()->m_vDefined.size();
			for(i=0; i<nSize; i++) {
				if(pProc2 == GetCurrentPlacement()->m_vDefined[i]) {
					GetCurrentPlacement()->m_vDefined.erase(GetCurrentPlacement()->m_vDefined.begin()+i);
					break;
				}
			}
			if(i==nSize) {
				AfxMessageBox(_T("An error occured while atempting to delete the processor.\nThe processor was not found."), MB_OK | MB_ICONSTOP);
				return FALSE; //could not find pProc2 in undefined list
			}

			//
			if (pProc2->GetProcessor()->getProcessorType() == TaxiwayProcessor)
			{
				GetTerminal().GetAirsideInput()->DeleteTaxiwayProc((TaxiwayProc*)pProc2->GetProcessor());
			}
			//check to see if we can delete the v1 processor
			GetTerminal().deleteProcessor( pProc2->GetProcessor(), m_ProjInfo.path );
			if( bIsStation )
			{
				//delete related railway and station
				int iDeleteStationIdxInTraffic = m_trainTraffic.DeleteStation( (IntegratedStation*)pProc2->GetProcessor() );

				// delete  related schedule ,update related schedule's railway keys
				GetTerminal().m_pAllCarSchedule->StationIsDeleted( iDeleteStationIdxInTraffic );

				//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
				{
					CString strLog = _T("");
					strLog = _T("Delete Processor;");
					ECHOLOG->Log(RailWayLog,strLog);
				}

				// update pRailWay
				std::vector<RailwayInfo*> allRailWay;
				m_trainTraffic.GetAllRailWayInfo(allRailWay);
				GetTerminal().pRailWay->SetAllRailWayInfoVector(allRailWay);
				GetTerminal().pRailWay->saveDataSet(m_ProjInfo.path,false);

				// build schedule's railway pointer
				GetTerminal().m_pAllCarSchedule->SetSystemRailWay( GetTerminal().pRailWay );
				GetTerminal().m_pAllCarSchedule->AdjustRailWay( &m_trainTraffic );
				GetTerminal().m_pAllCarSchedule->saveDataSet( m_ProjInfo.path,false );	
			}

			//refresh placements node & redraw 2d/3d view
			//find processor node with pProc2 as dwData
			//CTVNode* pPlacements = ProcessorsNode();
			CTVNode* pPlacements = NULL;
			if (m_systemMode == EnvMode_Terminal)
				pPlacements = ProcessorsNode();
			else if (m_systemMode == EnvMode_AirSide)
				pPlacements =m_msManager.m_msImplAirSide.FindProcTypeNode(iProcessorType);

			ASSERT(pPlacements != NULL);
			//get node with dwData == pProc2
			CTVNode* pProcNode =m_msManager.FindNodeByData((DWORD) pProc2);
			if (pProcNode == NULL)
				return FALSE;
			
			CTVNode* pParent = (CTVNode*) pProcNode->Parent();
			if (pParent == NULL)
				return FALSE;

			pParent->RemoveChild(pProcNode);
			
			CTVNode* tvNode = GetSelectedNode();
			if (tvNode != NULL)
			{
				if (tvNode->m_nIDResource == IDR_MENU_CONTOUR)
				{
					UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pParent);
					GetCurrentPlacement()->saveDataSet(m_ProjInfo.path, true);
					return TRUE;
				}
			}

			if(pParent->GetChildCount() == 0) {
				pProcNode = pParent;
				pParent = (CTVNode*) pProcNode->Parent();
			
				while(pProcNode != pPlacements && pProcNode->GetChildCount() == 0) {
					pParent->RemoveChild(pProcNode);
					pProcNode = pParent;
					pParent = (CTVNode*) pProcNode->Parent();
				}
			}

			CTVNode* pLayoutNode = (CTVNode*) pPlacements->Parent();
			UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pPlacements);
			GetCurrentPlacement()->saveDataSet(m_ProjInfo.path, true);
			return TRUE;
	}
}

void CTermPlanDoc::OnNewProcessor(CTVNode* pNode)
{	
}


CFloor2* CTermPlanDoc::GetActiveFloor()
{
	
	if(EnvMode_AirSide == m_systemMode)
	{
		if(Get3DViewParent() && Get3DViewParent()->GetAirside3D() && Get3DViewParent()->GetAirside3D()->GetAirportList().size() )
		{
			CAirport3D * pAirport = Get3DViewParent()->GetAirside3D()->GetActiveAirport();
			if( pAirport->GetLevelList().GetCount())
				return &pAirport->GetLevelList().GetLevel(0);
		}
		return NULL;
	}
	
	return GetCurModeFloor().GetActiveFloor();
}

double CTermPlanDoc::GetActiveFloorAltitude()
{
	//ASSERT(m_nActiveFloor >= 0 && m_nActiveFloor < static_cast<int>(GetCurModeFloor().GetCount()));
	CFloor2 * pFloor2 = GetActiveFloor();
	if(pFloor2)
		return GetActiveFloor()->Altitude();
	return 0;
}

static ProcessorID* GetNextProcessorID(const ProcessorID* id, ProcessorList* procList)
{
	char buf[128];
	ProcessorID* pNewID = new ProcessorID(*id);
	int lastLevel = pNewID->idLength()-1;
	pNewID->getNameAtLevel(buf, lastLevel);

	//check if last level of ID is numeric or alphanumeric
	char c = toupper(buf[0]);
	int len = strlen(buf);
	if(isAllNumeric(buf)) {
		//numeric
		int i = atoi(buf);
		do {
			i++;
			_itoa(i,buf,10);
			pNewID->setNameAtLevel(buf,lastLevel);
		} while(procList->findProcessor(*pNewID) != INT_MAX);
	}
	else if(len <= 2 && c >= 'A' && c <= 'Z') {
		char d = toupper(buf[len - 1]);
		if( d >= 'A' && d <= 'Z')
		{
			do {
				NextAlphaNumeric(buf);
				pNewID->setNameAtLevel(buf,lastLevel);
			} while(procList->findProcessor(*pNewID) != INT_MAX);
		}
		else
		{
			do {
				strcat(buf, "_COPY");
				pNewID->setNameAtLevel(buf,lastLevel);
			} while(procList->findProcessor(*pNewID) != INT_MAX);
		}
	}
	else {
		//other
		do {
			strcat(buf, "_COPY");
			pNewID->setNameAtLevel(buf,lastLevel);
		} while(procList->findProcessor(*pNewID) != INT_MAX);
	}

	return pNewID;
}

CProcessor2* CTermPlanDoc::CopyProcessor(CProcessor2* pProc2)
{
	static char buf[256];
	CProcessor2* pNewProc2= pProc2->GetCopy();
	
	pNewProc2->SetSelectName(GetUniqueNumber());

	//if this CProcessor2 has m_pProcessor == NULL (UC processor) then add to undefined list
	//otherwise add to defined list
	if(pProc2->GetProcessor() == NULL) {
		pNewProc2->SetProcessor(NULL);
	
		CString strProc2Name = pNewProc2->ShapeName().GetIDString();;
		CString strTemp = _T("");
		strTemp = GetCurrentPlacement()->m_vUndefined.GetName(strProc2Name);
		pNewProc2->ShapeName(strTemp);
		GetCurrentPlacement()->m_vUndefined.setNode((ALTObjectID)strTemp);
		GetCurrentPlacement()->m_vUndefined.InsertUnDefinePro2(std::make_pair(strTemp,pNewProc2));
		GetCurrentPlacement()->m_vUndefined.InsertShapeName(strTemp);
		GetCurrentPlacement()->m_vUndefined.insert( GetCurrentPlacement()->m_vUndefined.begin(), pNewProc2 );
		GetCurrentPlacement()->saveDataSet(m_ProjInfo.path, false);
		AddToUnderConstructionNode(pNewProc2);
	}
	else {
		//create a copy of Processor
		Processor* pProcToCopy = pProc2->GetProcessor();
		ProcessorID* pNewID = GetNextProcessorID(pProcToCopy->getID(), &GetProcessorList());

		Processor* pProcCopy = NULL;
		if(pProcToCopy->getProcessorType() == IntegratedStationProc) {
			IntegratedStation* pStation = (IntegratedStation*) pProcToCopy;
			pProcCopy = new IntegratedStation(*pStation);
			pProcCopy->init( *pNewID );
			pNewProc2->SetProcessor(pProcCopy);

			GetProcessorList().addProcessor(pProcCopy);
			GetProcessorList().setIndexes();			
			GetProcessorList().saveDataSet(m_ProjInfo.path, false);
			m_trainTraffic.AddNewStation((IntegratedStation*)pProcCopy);

			CTVNode* pOrigProcNode =m_msManager.FindNodeByData((DWORD) pProc2);
			CTVNode* pParent = (CTVNode*) pOrigProcNode->Parent();
			pNewID->printID(buf);
			//remove everything before and including last '-' is buf
			char* pLastPartOfProcName = strrchr(buf,'-');
			CTVNode* pNode = new CTVNode(++pLastPartOfProcName, IDR_CTX_PROCESSOR);
			pNode->m_dwData = (DWORD) pNewProc2;
			pParent->AddChild(pNode);

			GetCurrentPlacement()->m_vDefined.insert( GetCurrentPlacement()->m_vDefined.begin(), pNewProc2 );
			
			GetCurrentPlacement()->saveDataSet(m_ProjInfo.path, false);

			m_msManager.m_msImplTerminal.SetTermPlanDoc(this);
		//	m_msManager.m_msImplTerminal.RebuildProcessorTree(NULL);
		}
		else {
			//set type
			switch( pProcToCopy->getProcessorType() )
			{
			case PointProc:
				pProcCopy = new Processor;
				break;
			case DepSourceProc:
				pProcCopy = new DependentSource;
				break;
			case DepSinkProc:
				pProcCopy = new DependentSink;
				break;
			case LineProc:
				pProcCopy = new LineProcessor;
				break;
			case BaggageProc:
				pProcCopy = new BaggageProcessor;
				break;
			case HoldAreaProc:
				pProcCopy = new HoldingArea;
				break;
			case GateProc:
				pProcCopy = new GateProcessor;
				((GateProcessor*)pProcCopy)->setGateType(((GateProcessor*)pProcToCopy)->getGateType());
//				((GateProcessor*)pProcCopy)->setACStandGateFlag( ((GateProcessor*)pProcToCopy)->getACStandGateFlag() );
				break;
			case FloorChangeProc:
				pProcCopy = new FloorChangeProcessor;
				break;
			case MovingSideWalkProc:
				pProcCopy=new MovingSideWalk;
				break;
			case BarrierProc:
				pProcCopy = new Barrier;
				break;
			case Elevator:
				pProcCopy = new ElevatorProc;
				*((ElevatorProc*)pProcCopy) = * ((ElevatorProc*)pProcToCopy);
				break;
			case ConveyorProc:
				pProcCopy = new Conveyor;
				((Conveyor*)pProcCopy)->SetWidth( ((Conveyor*)pProcToCopy)->GetWidth() );
				break;
			case StairProc:
				pProcCopy = new Stair;
				((Stair*)pProcCopy)->SetWidth( ((Stair*)pProcToCopy)->GetWidth() );
				break;
			case EscalatorProc:
				pProcCopy = new Escalator;
				((Escalator*)pProcCopy)->SetWidth( ((Stair*)pProcToCopy)->GetWidth() );
				break;
			case FixProcessor :
				pProcCopy = new FixProc ;
				((FixProc*)pProcCopy)->SetLowerLimit(((FixProc*)pProcToCopy)->GetLowerLimit());
				((FixProc*)pProcCopy)->SetUpperLimit(((FixProc*)pProcToCopy)->GetUpperLimit());
				((FixProc*)pProcCopy)->SetNumber(((FixProc*)pProcToCopy)->GetNumber());
				break;
			case ApronProcessor :
				pProcCopy = new ApronProc ;
				break;
			case StandProcessor:
				pProcCopy = new StandProc;
				((StandProc *)pProcCopy)->UsePushBack( ((StandProc *)pProcToCopy)->IsUsePushBack());
				pProcCopy->SetBackup(pProcToCopy->IsBackup());
				break;

			case RunwayProcessor :
				pProcCopy = new RunwayProc;
				((RunwayProc *)pProcCopy)->SetWidth(((RunwayProc *)pProcToCopy)->GetWidth());
				((RunwayProc *)pProcCopy)->SetThreshold1(((RunwayProc *)pProcToCopy)->GetThreshold1());
				((RunwayProc *)pProcCopy)->SetThreshold2(((RunwayProc *)pProcToCopy)->GetThreshold2());
				((RunwayProc *)pProcCopy)->SetMarking1(((RunwayProc *)pProcToCopy)->GetMarking1());
				((RunwayProc *)pProcCopy)->SetMarking2(((RunwayProc *)pProcToCopy)->GetMarking2());
				break;

			case TaxiwayProcessor :
				pProcCopy = new TaxiwayProc;
				((TaxiwayProc *)pProcCopy)->SetWidth(((TaxiwayProc *)pProcToCopy)->GetWidth());
				((TaxiwayProc *)pProcCopy)->SetMarking(((TaxiwayProc *)pProcToCopy)->GetMarking());
				GetTerminal().GetAirsideInput()->AddTaxiwayProc((TaxiwayProc *)pProcCopy);

				break;

			case BillboardProcessor:
				pProcCopy = new BillboardProc;
//				((BillboardProc *)pProcCopy)->setAngle(((BillboardProc *)pProcToCopy)->getAngle());
				((BillboardProc *)pProcCopy)->setText(((BillboardProc *)pProcToCopy)->getText());
				((BillboardProc *)pProcCopy)->setType(((BillboardProc *)pProcToCopy)->getType());
				((BillboardProc *)pProcCopy)->setBitmapPath(((BillboardProc *)pProcToCopy)->getBitmapPath());
//				((BillboardProc *)pProcCopy)->setCoefficient(((BillboardProc *)pProcToCopy)->getCoefficient());
				((BillboardProc *)pProcCopy)->setHeight(((BillboardProc *)pProcToCopy)->getHeight());
				((BillboardProc *)pProcCopy)->setThickness(((BillboardProc *)pProcToCopy)->getThickness());
				break;
			case StretchProcessor :
			case TollGateProcessor :
			case TrafficLightProceesor :
			case StopSignProcessor : 
			case YieldDeviceProcessor :
				pProcCopy =((LandfieldProcessor*) pProcToCopy)->GetCopy();
				break;
			case BridgeConnectorProc:
				pProcCopy = new BridgeConnector;
				((BridgeConnector*)pProcToCopy)->CopyDataToProc((BridgeConnector*)pProcCopy);
				break;
			case RetailProc:
				pProcCopy = new RetailProcessor;
				break;
			}
			if (pProcCopy == NULL)
				return NULL;

			//set id
			pProcCopy->init( *pNewID );
			
			//set Terminal
			pProcCopy->SetTerminal( &GetTerminal() );
			//set properties
			int nPSLCount=pProcToCopy->serviceLocationLength() ;
			if(nPSLCount> 0)
			{
				if( pProcCopy->getProcessorType() == BaggageProc ) {
					Path* pPath = ((BaggageProcessor*) pProcToCopy)->serviceLocationPath();
					pProcCopy->initServiceLocation( pPath->getCount(), pPath->getPointList() );

					pPath = ((BaggageProcessor*) pProcToCopy)->GetBarrier()->serviceLocationPath();
					if( pPath )
						((BaggageProcessor*)pProcCopy)->GetBarrier()->initServiceLocation( pPath->getCount(), pPath->getPointList() );

					pPath = ((BaggageProcessor*) pProcToCopy)->GetPathBagMovingOn();
					if( pPath )
						((BaggageProcessor*)pProcCopy)->GetPathBagMovingOn()->init( *pPath );					
				}
				else {
					Point* pPointSL=new Point[nPSLCount];
					memcpy(pPointSL,pProcToCopy->serviceLocationPath()->getPointList(),sizeof(Point)*nPSLCount);
					pProcCopy->initServiceLocation(nPSLCount,pPointSL);
					delete[] pPointSL;
				}
			}
			int nPICCount=pProcToCopy->inConstraintLength();
			if( nPICCount> 0)
			{
				Point* pPointIC=new Point[nPICCount];
				memcpy(pPointIC,pProcToCopy->inConstraint()->getPointList(),sizeof(Point)*nPICCount);
				pProcCopy->initInConstraint( nPICCount, pPointIC);
				delete[] pPointIC;
			}
			
			int nPOCCount=pProcToCopy->outConstraintLength();
			if(pProcToCopy->outConstraintLength() > 0)
			{
				Point* pPointOC=new Point[nPOCCount];
				memcpy(pPointOC,pProcToCopy->outConstraint()->getPointList(),sizeof(Point)*nPOCCount);
				pProcCopy->initOutConstraint( nPOCCount, pPointOC);
				delete[] pPointOC;
			}
			
			int nPQCount=pProcToCopy->queueLength();
			if(nPQCount> 0)
			{
				Point* pPointQ=new Point[nPQCount];
				memcpy(pPointQ,pProcToCopy->queuePath()->getPointList(),sizeof(Point)*nPQCount);
				pProcCopy->initQueue( pProcToCopy->queueIsFixed(), nPQCount,pPointQ);
				delete[] pPointQ;
			}

			if( pProcCopy->getProcessorType() == Elevator )
			{					
				((ElevatorProc*)pProcCopy)->InitLayout();
			}
			if(pProcCopy->getProcessorType() == StandProcessor)
			{
				((StandProc *)pProcCopy)->UsePushBack( ((StandProc *)pProcToCopy)->IsUsePushBack());
				if (((StandProc *)pProcToCopy)->IsUsePushBack())
				{
					((StandProc *)pProcCopy)->setPushBackWay( ((StandProc *)pProcToCopy)->getPushBackWay());
				}
			}
			if(pProcCopy->getProcessorType() == PointProc || 
			   pProcCopy->getProcessorType() == LineProc  || 
			   pProcCopy->getProcessorType() == GateProc)
			{
				pProcCopy->setEmergentFlag(pProcToCopy->getEmergentFlag());
			}
			pNewProc2->SetProcessor(pProcCopy);

			GetProcessorList().addProcessor(pProcCopy);
			GetProcessorList().setIndexes();			
			GetProcessorList().saveDataSet(m_ProjInfo.path, false);

			CTVNode* pOrigProcNode =m_msManager.FindNodeByData((DWORD) pProc2);		
			CTVNode* pParent = (CTVNode*) pOrigProcNode->Parent();		
			pNewID->printID(buf);

			/*if(EnvMode_LandSide == m_systemMode)
				m_msManager.m_msImplLandSide.RebuildProcessorTree((ProcessorClassList)pNewProc2->GetProcessor()->getProcessorType());
			else*/
			{ //remove everything before and including last '-' is buf

				CString strNodeName = _T("");
				char* pLastPartOfProcName = strrchr(buf,'-');
	
				if(pLastPartOfProcName == NULL)
				{//processor's name matching 'xxx_COPY'
					strNodeName.Format("%s", buf);
				}
				else
				{
					strNodeName.Format("%s", ++pLastPartOfProcName);
				}				
								
				CTVNode* pNode ;
				if(EnvMode_LandSide == m_systemMode)
					pNode = new CTVNode(strNodeName, IDR_CTX_LANDPROCESSOR);
				else pNode = new CTVNode(strNodeName, IDR_CTX_PROCESSOR);					 
				pNode->m_dwData = (DWORD) pNewProc2;
				pParent->AddChild(pNode);
			}
			GetCurrentPlacement()->m_vDefined.insert( GetCurrentPlacement()->m_vDefined.begin(), pNewProc2 );
			GetCurrentPlacement()->saveDataSet(m_ProjInfo.path, false);
		}
	}

	SetJustAftCopyProc(true);
	return pNewProc2;
}

template <typename T>
class ptr_warpper
{
public:
	ptr_warpper(T* ptr = 0){ m_ptr = ptr; }
	T * get()const{ return m_ptr; }
protected:
	T* m_ptr;
};

//CObjectDisplayList__
void CTermPlanDoc::CopyProcessors(const CObjectDisplayList& ObjDsplist)
{
	ASSERT(ObjDsplist.GetCount()> 0);

	CTVNode* pPlacementsNode = ProcessorsNode();	//get placements node
	
	CTVNode* pUCNode = UnderConstructionNode();		//get under construction node

	CPROCESSOR2LIST vNewProcList;
	//vNewProcList.reserve(proclist.size());

	//CArray<TEMP_GROUP_INFO*,TEMP_GROUP_INFO*> listTGI;
	

	std::map<CObjectDisplayGroup* , ptr_warpper<CObjectDisplayGroup> > groupMap;


	CObjectDisplayList tempList(ObjDsplist);
	for(int i=0; i<static_cast<int>(tempList.GetCount()); i++) 
	{
		CObjectDisplay *pObjectDisplay = tempList.GetItem(i);
		if (pObjectDisplay == NULL)
		{
			TRACE(_T("Error occur void CTermPlanDoc::CopyProcessors(const CObjectDisplayList& proclist)"));
			continue;
		}
		if (pObjectDisplay->GetType() == ObjectDisplayType_OtherObject)
			continue;

		CProcessor2* pProcToCopy = (CProcessor2*)pObjectDisplay;
		CProcessor2* pProcCopy = CopyProcessor(pProcToCopy);		

		if( pProcToCopy->GetGroup() ) { //if processor is part of group
			CObjectDisplayGroup* pNewProcGroup = groupMap[pProcCopy->GetGroup()].get();
			if( !pNewProcGroup ){ 
				pNewProcGroup = new CObjectDisplayGroup;				
				groupMap[pProcToCopy->GetGroup()] = pNewProcGroup;	
				m_pPlacement->AddGroup(pNewProcGroup);
			}	
			pProcCopy->SetGroup(pNewProcGroup);
			pNewProcGroup->AddItem(pProcCopy);
		}

		vNewProcList.push_back(pProcCopy);
	}

	//update group info
	
	UnSelectAllProcs();
	for(int j=0;j<static_cast<int>(vNewProcList.size());j++)
	{
		AddProcToSelectionList(vNewProcList[j]);
	}

	UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) ProcessorsNode());
}

//CObjectDisplayList__
void CTermPlanDoc::CopySelectedProcessors()
{
	CopyProcessors(GetSelectProcessors());
}



#if 0
void CTermPlanDoc::CopySelectedProcessors()
{
	//This function makes a copy of all processors in the selection list
	char buf[256];
	//char buf2[256];
	//get placements node
	CTVNode* pPlacementsNode = ProcessorsNode();
	//ASSERT(pPlacementsNode != NULL);
	//get under construction node
	CTVNode* pUCNode = UnderConstructionNode();
	//ASSERT(pUCNode != NULL);

	ASSERT(GetSelectProcessors().size() > 0);
	//create a list of new CProcessor2s
	CPROCESSOR2LIST vNewProcList;
	vNewProcList.reserve(GetSelectProcessors().size());
	//Added by Tim In 1/30/2003************************************************
	CArray<TEMP_GROUP_INFO*,TEMP_GROUP_INFO*> listTGI;
	//*************************************************************************
		
	for(int i=0; i<GetSelectProcessors().size(); i++)
	{
		//create a CProcessor2 object
		CProcessor2* pNewProc2 = new CProcessor2();
		//set floor to active floor
		pNewProc2->SetFloor(GetSelectProcessors()[i]->GetFloor());
		//set location, scale and rotation
		ARCVector3 v2D;
		ARCVector3 v3D;
		/*
		//calc offset value
		C3DMath :: CVector3D vSize =m_vSelectedProcessors[i]->GetShape()->ExtentsMax() - m_vSelectedProcessors[i]->GetShape()->ExtentsMin();
		vSize[VZ]=0;
		if(vSize[VX]==0&&vSize[VY]==0)
		{
			vSize[VX]=100;
			vSize[VY]=100;
		}
		*/
		GetSelectProcessors()[i]->GetLocation(v2D);
		pNewProc2->SetLocation(v2D/*+vSize*/);//offset
		GetSelectProcessors()[i]->GetScale(v3D);
		pNewProc2->SetScale(v3D);
		pNewProc2->SetRotation(GetSelectProcessors()[i]->GetRotation());
		//set shape
		pNewProc2->SetShape(GetSelectProcessors()[i]->GetShape());
		//set selection id
		pNewProc2->SetSelectName(GetUniqueNumber());
		//set dispproperties
		pNewProc2->m_dispProperties = GetSelectProcessors()[i]->m_dispProperties;
		//Added by Tim In 1/30/2003************************************************
		//create new group of processor
		if(GetSelectProcessors()[i]->m_pGroup)
		{
			PROC_GROUP* pPG=(PROC_GROUP*)GetSelectProcessors()[i]->m_pGroup;
			if(pPG->m_listProc.GetHead()==GetSelectProcessors()[i])
			{
				TEMP_GROUP_INFO* pTGI=new TEMP_GROUP_INFO;
				pTGI->pGroup=pPG;
				pTGI->listProcIndex.Add(i);
				listTGI.Add(pTGI);
			}
			else
			{
				//find
				int nCount=listTGI.GetSize();
				for(int k=0;k<nCount;k++)
				{
					if(listTGI[k]->pGroup==pPG)
					{
						listTGI[k]->listProcIndex.Add(i);
						break;
					}
				}
			}
		}
		//*************************************************************************

		//if this CProcessor2 has m_pProcessor == NULL (UC processor) then add to undefined list
		//otherwise add to defined list
		if(GetSelectProcessors()[i]->GetProcessor() == NULL) 
		{
			pNewProc2->SetProcessor(NULL);
			GetCurrentPlacement()->m_vUndefined.insert( GetCurrentPlacement()->m_vUndefined.begin(), pNewProc2 );
			GetCurrentPlacement()->saveDataSet(m_ProjInfo.path, false);
			AddToUnderConstructionNode(pNewProc2);
		}
		else
		{
			//create a copy of Processor
			Processor* pProcToCopy = GetSelectProcessors()[i]->GetProcessor();
			ProcessorID* pNewID = GetNextProcessorID(pProcToCopy->getID(), GetProcessorList());
			
			Processor* pProcCopy = NULL;
			if(pProcToCopy->getProcessorType() == IntegratedStationProc) {
				IntegratedStation* pStation = (IntegratedStation*) pProcToCopy;
				pProcCopy = new IntegratedStation(*pStation);
				pProcCopy->init( *pNewID );
				pNewProc2->SetProcessor(pProcCopy);

				GetProcessorList().addProcessor(pProcCopy);
				GetProcessorList().setIndexes();			
				GetProcessorList().saveDataSet(m_ProjInfo.path, false);
				m_trainTraffic.AddNewStation((IntegratedStation*)pProcCopy);

				CTVNode* pOrigProcNode = FindNodeByData((DWORD) GetSelectProcessors()[i]);
				CTVNode* pParent = (CTVNode*) pOrigProcNode->Parent();
				pNewID->printID(buf);
				//remove everything before and including last '-' is buf
				char* pLastPartOfProcName = strrchr(buf,'-');
				CTVNode* pNode = new CTVNode(++pLastPartOfProcName, IDR_CTX_PROCESSOR);
				pNode->m_dwData = (DWORD) pNewProc2;
				pParent->AddChild(pNode);

				GetCurrentPlacement()->m_vDefined.insert( GetCurrentPlacement()->m_vDefined.begin(), pNewProc2 );
				
				GetCurrentPlacement()->saveDataSet(m_ProjInfo.path, false);

	//			RebuildProcessorTree(NULL);
			}
			else {
				//set type
				switch( pProcToCopy->getProcessorType() )
				{
				case PointProc:
					pProcCopy = new Processor;
					break;
				case DepSourceProc:
					pProcCopy = new DependentSource;
					break;
				case DepSinkProc:
					pProcCopy = new DependentSink;
					break;
				case LineProc:
					pProcCopy = new LineProcessor;
					break;
				case BaggageProc:
					pProcCopy = new BaggageProcessor;
					break;
				case HoldAreaProc:
					pProcCopy = new HoldingArea;
					break;
				case GateProc:
					pProcCopy = new GateProcessor;
					((GateProcessor*)pProcCopy)->setACStandGateFlag( ((GateProcessor*)pProcToCopy)->getACStandGateFlag() );
					break;
				case FloorChangeProc:
					pProcCopy = new FloorChangeProcessor;
					break;
				case MovingSideWalkProc:
					pProcCopy=new MovingSideWalk;
					break;
				case BarrierProc:
					pProcCopy = new Barrier;
					break;
				case Elevator:
					pProcCopy = new ElevatorProc;
					*((ElevatorProc*)pProcCopy) = * ((ElevatorProc*)pProcToCopy);
					break;
				case ConveyorProc:
					pProcCopy = new Conveyor;
					((Conveyor*)pProcCopy)->SetWidth( ((Conveyor*)pProcToCopy)->GetWidth() );
					break;
				case StairProc:
					pProcCopy = new Stair;
					((Stair*)pProcCopy)->SetWidth( ((Stair*)pProcToCopy)->GetWidth() );
					break;
				case EscalatorProc:
					pProcCopy = new Escalator;
					break;
				}

				//set id
				pProcCopy->init( *pNewID );
				
				//set Terminal
				pProcCopy->SetTerminal( &GetTerminal() );
				//set properties
				int nPSLCount=pProcToCopy->serviceLocationLength() ;
				if(nPSLCount> 0)
				{
					if( pProcCopy->getProcessorType() == BaggageProc )
					{
						Path* pPath = ((BaggageProcessor*) pProcToCopy)->serviceLocationPath();
						pProcCopy->initServiceLocation( pPath->getCount(), pPath->getPointList() );

						pPath = ((BaggageProcessor*) pProcToCopy)->GetBarrier()->serviceLocationPath();
						if( pPath )
						{
							((BaggageProcessor*)pProcCopy)->GetBarrier()->initServiceLocation( pPath->getCount(), pPath->getPointList() );
						}

						pPath = ((BaggageProcessor*) pProcToCopy)->GetPathBagMovingOn();
						if( pPath )
						{
							((BaggageProcessor*)pProcCopy)->GetPathBagMovingOn()->init( *pPath );
						}
						
					}
					else
					{
						Point* pPointSL=new Point[nPSLCount];
						memcpy(pPointSL,pProcToCopy->serviceLocationPath()->getPointList(),sizeof(Point)*nPSLCount);
						pProcCopy->initServiceLocation(nPSLCount,pPointSL);
						delete[] pPointSL;
					}
					
				}
				int nPICCount=pProcToCopy->inConstraintLength();
				if( nPICCount> 0)
				{
					Point* pPointIC=new Point[nPICCount];
					memcpy(pPointIC,pProcToCopy->inConstraint()->getPointList(),sizeof(Point)*nPICCount);
					/*
					for(int k=0;k<nPICCount;k++)
					{
						pPointIC[k].DoOffset(vSize[VX],vSize[VY],vSize[VZ]);
					}
					*/
					pProcCopy->initInConstraint( nPICCount, pPointIC);
					delete[] pPointIC;
				}
				
				int nPOCCount=pProcToCopy->outConstraintLength();
				if(pProcToCopy->outConstraintLength() > 0)
				{
					Point* pPointOC=new Point[nPOCCount];
					memcpy(pPointOC,pProcToCopy->outConstraint()->getPointList(),sizeof(Point)*nPOCCount);
					/*
					for(int k=0;k<nPOCCount;k++)
					{
						pPointOC[k].DoOffset(vSize[VX],vSize[VY],vSize[VZ]);
					}
					*/
					pProcCopy->initOutConstraint( nPOCCount, pPointOC);
					delete[] pPointOC;
				}
				
				int nPQCount=pProcToCopy->queueLength();
				if(nPQCount> 0)
				{
					Point* pPointQ=new Point[nPQCount];
					memcpy(pPointQ,pProcToCopy->queuePath()->getPointList(),sizeof(Point)*nPQCount);
					/*
					for(int k=0;k<nPQCount;k++)
					{
						pPointQ[k].DoOffset(vSize[VX],vSize[VY],vSize[VZ]);
					}
					*/
					pProcCopy->initQueue( pProcToCopy->queueIsFixed(), nPQCount,pPointQ);
					delete[] pPointQ;
				}

				if( pProcCopy->getProcessorType() == Elevator )
				{					
					((ElevatorProc*)pProcCopy)->InitLayout();
				}
			
				pNewProc2->SetProcessor(pProcCopy);

				GetProcessorList().addProcessor(pProcCopy);
				GetProcessorList().setIndexes();			
				GetProcessorList().saveDataSet(m_ProjInfo.path, false);

				CTVNode* pOrigProcNode = FindNodeByData((DWORD) GetSelectProcessors()[i]);
				CTVNode* pParent = (CTVNode*) pOrigProcNode->Parent();
				pNewID->printID(buf);
				//remove everything before and including last '-' is buf
				char* pLastPartOfProcName = strrchr(buf,'-');
				CTVNode* pNode = new CTVNode(++pLastPartOfProcName, IDR_CTX_PROCESSOR);
				pNode->m_dwData = (DWORD) pNewProc2;
				pParent->AddChild(pNode);

				GetCurrentPlacement()->m_vDefined.insert( GetCurrentPlacement()->m_vDefined.begin(), pNewProc2 );
				GetCurrentPlacement()->saveDataSet(m_ProjInfo.path, false);
				
				//RebuildProcessorTree(NULL);
				
				
			}
		}
		vNewProcList.push_back(pNewProc2);
	}
	//Added by Tim In 1/30/2003************************************************
	for(int k=0;k<listTGI.GetSize();k++)
	{
		PROC_GROUP* pNewPG=new PROC_GROUP;
		for(int j=0;j<listTGI[k]->listProcIndex.GetSize();j++)
		{
			int nIndex=listTGI[k]->listProcIndex[j];
			vNewProcList[nIndex]->m_pGroup=pNewPG;
			pNewPG->m_listProc.AddTail(vNewProcList[nIndex]);
		}
		m_listGroup.AddTail(pNewPG);
	}
	UnSelectAllProcs();
	for(int j=0;j<vNewProcList.size();j++)
	{
		AddProcToSelectionList(vNewProcList[j]);
	}
	//***********************************************************************
	//UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pPlacements);
	UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) ProcessorsNode());
	
	/*
	//unselect all procs
	UnSelectAllProcs();
	//now go through the new proc list and 
	//add it to selection list
	for(i=0; i<vNewProcList.size(); i++) {
		CProcessor2* pNewProc2 = vNewProcList[i];
		//add to selection list
		SelectProc(pNewProc2, TRUE);
	}
	*/
}


void CTermPlanDoc::OnMoveProc(double dx, double dy)
{
	ASSERT(FALSE);
	ARCVector2 loc;
	loc[VX] = dx;
	loc[VY] = dy;
	//m_pActiveProc->SetLocation(loc);
}

void CTermPlanDoc::OnMoveProcs(double dx, double dy)
{
	if(m_bLayoutLocked)
		return;
	if(GetSelectProcessors().size() > 0) {
		ARCVector3 disp, oldloc;
		disp = ARCVector3(dx,dy,0);
		for(int i=0; i<GetSelectProcessors().size(); i++) {
			GetSelectProcessors()[i]->GetLocation(oldloc);
			GetSelectProcessors()[i]->SetLocation(oldloc+disp);
			oldloc=oldloc+disp;
			//now move properties
			GetSelectProcessors()[i]->OffsetProperties(dx, dy);
			if( GetSelectProcessors()[i]->GetProcessor() && GetSelectProcessors()[i]->GetProcessor()->getProcessorType() == IntegratedStationProc )
			{
				AdjustStationLinkedRailWay( (IntegratedStation*)GetSelectProcessors()[i]->GetProcessor() , false);
				
			}
		}
	}
}
#endif

void CTermPlanDoc::MoveProcessor(CProcessor2* pProc2, double dx, double dy)
{
	ARCVector3 disp(dx,dy,0.0);
	pProc2->SetLocation(pProc2->GetLocation()+disp);
	//now move properties
	pProc2->OffsetProperties(dx, dy);
	if( pProc2->GetProcessor() && pProc2->GetProcessor()->getProcessorType() == IntegratedStationProc )
		AdjustStationLinkedRailWay( (IntegratedStation*)pProc2->GetProcessor() , false);
}

void CTermPlanDoc::OnMoveProcs(double dx, double dy,CPoint point, CView *pView)
{
	if(m_bLayoutLocked)
		return;
	C3DView* p3DView=(C3DView*)pView;
	int nSelectedCount = GetSelectProcessors().GetCount(); 
	if(nSelectedCount > 0)
	{
		ARCVector3 oldloc;
		ARCVector3 newloc;
		ARCVector3 offsetloc;
		for(int i=0; i<nSelectedCount; i++) 
		{
			CObjectDisplay *pObjectDisplay = GetSelectedObjectDisplay(i);
			if(pObjectDisplay == NULL)
			{
				TRACE(_T("Error occur void CTermPlanDoc::OnMoveProcs(double dx, double dy,CPoint point, CView *pView)"));
				continue;
			}
			if (pObjectDisplay->GetType() == ObjectDisplayType_Processor2)
			{
				MoveProcessor((CProcessor2 *)pObjectDisplay , dx, dy);
			}
			else if (pObjectDisplay->GetType() == ObjectDisplayType_OtherObject)
			{
				pObjectDisplay->OffsetProperties(dx,dy);
			}


		}
	}
//	m_vSelectPlacements.MovePlacement(dx, dy);
}

void CTermPlanDoc::OnMoveRailPt(int nRailIdx, int nPtIdx, double dx, double dy)
{
	if(m_bLayoutLocked)
		return;
	
	std::vector<RailwayInfo*> railWayVect;
	m_trainTraffic.GetAllRailWayInfo(railWayVect);
	ASSERT(nRailIdx >=0 && nRailIdx < static_cast<int>(railWayVect.size()));
	int numPts = railWayVect[nRailIdx]->GetRailWayPath().getCount();
	ASSERT(nPtIdx >= 0 && nPtIdx < numPts);
	if(nPtIdx == 0 || nPtIdx == numPts-1)
		return;
	
	Point pt;
	railWayVect[nRailIdx]->GetPoint(nPtIdx,pt);
	pt.setX(pt.getX()+dx);
	pt.setY(pt.getY()+dy);
	railWayVect[nRailIdx]->SetPoint(nPtIdx,pt);

	//GetTerminal().m_pAllCarSchedule->AdjustRailWay( &m_trainTraffic );
	//GetTerminal().pRailWay->saveDataSet( m_ProjInfo.path, false );
	//GetTerminal().m_pAllCarSchedule->saveDataSet( m_ProjInfo.path,false );
}

void CTermPlanDoc::OnMovePipePt(int nPipeIdx, int nPtIdx, double dx, double dy)
{
	if(m_bLayoutLocked)
		return;
	
	CPipeDataSet* pPipeDS = GetTerminal().m_pPipeDataSet;
	int nPipeCount = pPipeDS->GetPipeCount();
	ASSERT(nPipeIdx >=0 && nPipeIdx < nPipeCount);
	int nPtCount = pPipeDS->GetPipeAt(nPipeIdx)->GetPipePointCount();
	ASSERT(nPtIdx >= 0 && nPtIdx < nPtCount);
	Point pt = pPipeDS->GetPipeAt(nPipeIdx)->GetPipePoint(nPtIdx)->m_point;
	pt.setX(pt.getX()+dx);
	pt.setY(pt.getY()+dy);
	pPipeDS->GetPipeAt(nPipeIdx)->GetPipePoint(nPtIdx)->m_point.SetPoint(pt);
	pPipeDS->GetPipeAt(nPipeIdx)->CalculateTheBisectLine();
}

void CTermPlanDoc::OnMovePipeWidthPt(int nPipeIdx, int nPtIdx, double dx, double dy)
{
	if(m_bLayoutLocked)
		return;

	int nPipePtIdx = nPtIdx/2;
	bool bIs1 = (nPtIdx%2) ? true : false;
	
	CPipeDataSet* pPipeDS = GetTerminal().m_pPipeDataSet;
	int nPipeCount = pPipeDS->GetPipeCount();
	ASSERT(nPipeIdx >=0 && nPipeIdx < nPipeCount);
	int nPtCount = pPipeDS->GetPipeAt(nPipeIdx)->GetPipePointCount();
	ASSERT(nPipePtIdx >= 0 && nPipePtIdx < nPtCount);
	
	PIPEPOINT& p = *(pPipeDS->GetPipeAt(nPipeIdx)->GetPipePoint(nPipePtIdx));
	Point A(p.m_point, bIs1 ? p.m_bisectPoint1 : p.m_bisectPoint2);
	Point B(dx, dy, p.m_point.getZ());
	double dWidthChange = 4.0*(A*B)/p.m_width;
	p.m_width -= dWidthChange;


	Point pTest(p.m_bisectPoint1, p.m_bisectPoint2);
	double dTestLen = pTest.length();

	// TRACE("Moving pipe width\n");
	// TRACE("dx=%.2f, dy=%.2f\n", dx, dy);
	// TRACE("Original width=%.2f\n", p.m_width);
	// TRACE("Change width=%.2f\n", dWidthChange);

	pPipeDS->GetPipeAt(nPipeIdx)->CalculateTheBisectLine();
}

void CTermPlanDoc::AdjustStationLinkedRailWay( IntegratedStation* _pStation ,bool _bSaveDataToFile )
{
	ASSERT( _pStation );
	m_trainTraffic.AdjustLinkedRailWay( _pStation );
	GetTerminal().m_pAllCarSchedule->SetSystemRailWay( GetTerminal().pRailWay );
	GetTerminal().m_pAllCarSchedule->AdjustRailWay( &m_trainTraffic );

	//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
	{
		CString strLog = _T("");
		strLog = _T("AdjustStationLinkedRailWay;");
		ECHOLOG->Log(RailWayLog,strLog);
	}

	if( _bSaveDataToFile )
	{	

		GetTerminal().pRailWay->saveDataSet( m_ProjInfo.path, false );
		GetTerminal().m_pAllCarSchedule->saveDataSet( m_ProjInfo.path,false );
	}
}
void CTermPlanDoc::OnScaleProc(double dx, double dy)
{
	int nCount = GetSelectProcessors().GetCount();

	const double SCALE_SENSITIVITY = 0.03;
	if(m_bLayoutLocked || nCount <= 0)
		return;

	ARCVector3 scale;

	for(int i=0; i< nCount; i++) 
	{
		CObjectDisplay *pObjDisplay = GetSelectedObjectDisplay(i);
		if(pObjDisplay && pObjDisplay ->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2 *pProc2 = (CProcessor2*)pObjDisplay;

			pProc2->GetScale(scale);
			scale[VX] += dx*SCALE_SENSITIVITY;
			scale[VY] += dx*SCALE_SENSITIVITY;
			scale[VZ] += dx*SCALE_SENSITIVITY;
			pProc2->SetScale(scale);
		}

	}
}

void CTermPlanDoc::OnScaleProcX(double dx, double dy)
{
	int nCount = GetSelectProcessors().GetCount();

	const double SCALE_SENSITIVITY = 0.03;
	if(m_bLayoutLocked || nCount <= 0)
		return;
	
	ARCVector3 scale;
	for(int i=0; i<nCount; i++)
	{
		CObjectDisplay *pObjDisplay = GetSelectedObjectDisplay(i);
		if(pObjDisplay && pObjDisplay ->GetType() == ObjectDisplayType_Processor2)
		{

			CProcessor2 *pProc2 = (CProcessor2*)pObjDisplay;
			pProc2->GetScale(scale);
			scale[VX] += dx*SCALE_SENSITIVITY;
			pProc2->SetScale(scale);
		}

	}
}

void CTermPlanDoc::OnScaleProcY(double dx, double dy)
{
	int nCount = GetSelectProcessors().GetCount();

	const double SCALE_SENSITIVITY = 0.03;
	if(m_bLayoutLocked || nCount <= 0)
		return;
	
	ARCVector3 scale;
	for(int i=0; i<nCount; i++) 
	{
		CObjectDisplay *pObjDisplay = GetSelectedObjectDisplay(i);
		if(pObjDisplay && pObjDisplay ->GetType() == ObjectDisplayType_Processor2)
		{

			CProcessor2 *pProc2 = (CProcessor2*)pObjDisplay;
			pProc2->GetScale(scale);
			scale[VY] += dx*SCALE_SENSITIVITY;
			pProc2->SetScale(scale);
		}
	}
}

void CTermPlanDoc::OnScaleProcZ(double dx, double dy)
{
	int nCount = GetSelectProcessors().GetCount();

	const double SCALE_SENSITIVITY = 0.03;
	if(m_bLayoutLocked || nCount <= 0)
		return;
	
	ARCVector3 scale;
	for(int i=0; i<static_cast<int>(nCount); i++) 
	{
		CObjectDisplay *pObjDisplay = GetSelectedObjectDisplay(i);
		if(pObjDisplay && pObjDisplay ->GetType() == ObjectDisplayType_Processor2)
		{

			CProcessor2 *pProc2 = (CProcessor2*)pObjDisplay;
			pProc2->GetScale(scale);
			scale[VZ] += dx*SCALE_SENSITIVITY;
			pProc2->SetScale(scale);
		}
	}
}
//CObjectDisplayList
void CTermPlanDoc::OnRotateProc(double dx)
{
	if(m_bLayoutLocked)
		return;
	//	
	//if( m_systemMode == EnvMode_LandSide )
	//{
	//	GetLandsideDoc()->RotateSelectProcessor(dx);
	//	return;
	//}
	//rotate about location of last proc in sel list
	int nCount=GetSelectProcessors().GetCount();

	if(nCount <= 0)
		return;

	const ARCVector3& point = GetSelectedObjectDisplay(nCount - 1)->GetLocation();
//	 = GetSelectProcessors()[c-1]->GetLocation();
	

	double dRot;
	for(int i=0; i<nCount; i++)
	{
		CObjectDisplay *pObjDisplay = GetSelectedObjectDisplay(i);
		if(pObjDisplay && pObjDisplay ->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2 *pProc2 = (CProcessor2*)pObjDisplay;

			pProc2->GetRotation(dRot);
			pProc2->SetRotation(dRot+dx);
			
			//now rotate CProcessor2::m_vLocation about point
			const ARCVector3 loc3D = pProc2->GetLocation();
			ARCVector2 loc2D, point2D;
			loc2D = loc3D;
			point2D = point;
			loc2D.Rotate(-dx, point2D);
			pProc2->SetLocation(ARCVector3(loc2D[VX],loc2D[VY],loc3D[VZ]));
			//
			pProc2->OffsetProperties(loc2D[VX]-loc3D[VX], loc2D[VY]-loc3D[VY]);

			pProc2->RotateProperties(dx, pProc2->GetLocation());
			//m_vSelectedProcessors[i]->RotateProperties(dx, point);

			if(pProc2->GetProcessor() && pProc2->GetProcessor()->getProcessorType() == IntegratedStationProc)
			{
				AdjustStationLinkedRailWay(((IntegratedStation*)pProc2->GetProcessor()), false);
			}
		}
		else
		{
			//ASSERT(0);
			pObjDisplay->RotateProperties(dx,point);


		}


	}
	//GetLandsideDoc()->UpdateDView();
}

void CTermPlanDoc::OnRotateShape(double dx)
{
	int nCount=GetSelectProcessors().GetCount();

	if(m_bLayoutLocked || nCount <= 0)
		return;
	double dRot;
	for(int i=0; i<nCount; i++) 
	{
		CObjectDisplay *pObjDisplay = GetSelectedObjectDisplay(i);
		if(pObjDisplay && pObjDisplay ->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2 *pProc2 = (CProcessor2*)pObjDisplay;
			pProc2->GetRotation(dRot);
			pProc2->SetRotation(dRot+dx);
		}
	}
}

void CTermPlanDoc::OnMoveShape(double dx, double dy)
{
	if(m_bLayoutLocked)
		return;
	int nCount=GetSelectProcessors().GetCount();
	
	if(nCount > 0) 
	{
		ARCVector3 disp, oldloc;
		oldloc = GetSelectedObjectDisplay(nCount-1)->GetLocation();
		disp = ARCVector3(dx,dy,0);

		for(int i=0; i<nCount; i++) 
		{	
			CObjectDisplay *pObjDisplay = GetSelectedObjectDisplay(i);
			if(pObjDisplay && pObjDisplay ->GetType() == ObjectDisplayType_Processor2)
			{
				CProcessor2 *pProc2 = (CProcessor2*)pObjDisplay;

				oldloc = pProc2->GetLocation();
				pProc2->SetLocation(oldloc+disp);
			}
		}
	}
}



void CTermPlanDoc::OnMoveShapeZ(double dx, double dy,BOOL bZ2zero)
{
	if(m_bLayoutLocked)
		return;
	
	int nCount=GetSelectProcessors().GetCount();

	if(nCount > 0)
	{
		ARCVector3 disp, oldloc;
		oldloc = GetSelectedObjectDisplay(nCount-1)->GetLocation();
		disp = ARCVector3(0,0,dy);
		for(int i=0; i < nCount; i++) 
		{

			CObjectDisplay *pObjDisplay = GetSelectedObjectDisplay(i);
			if(pObjDisplay && pObjDisplay ->GetType() == ObjectDisplayType_Processor2)
			{
				CProcessor2 *pProc2 = (CProcessor2*)pObjDisplay;

				if(pProc2->GetProcessor() != NULL 
						&&pProc2->GetProcessor()->getProcessorType() == IntegratedStationProc)
					continue;
				oldloc = pProc2->GetLocation();
				if(bZ2zero)
					pProc2->SetLocation(ARCVector3(oldloc[VX],oldloc[VY],0));
				else
					pProc2->SetLocation(oldloc-disp);
			}
		}
	}

}

void CTermPlanDoc::OnProcEditFinished()
{
	GetCurrentPlacement()->saveDataSet( m_ProjInfo.path, false);
	GetProcessorList().saveDataSet(m_ProjInfo.path, true);
	GetTerminal().pRailWay->saveDataSet( m_ProjInfo.path, false );
	GetTerminal().m_pAllCarSchedule->saveDataSet( m_ProjInfo.path,false );

	m_portals.saveDataSet( m_ProjInfo.path,false);
	GetTerminal().m_pAreas->saveDataSet(  m_ProjInfo.path,false );
	GetTerminal().m_pStructureList->saveDataSet(  m_ProjInfo.path,false );
	GetTerminal().m_pPipeDataSet->saveDataSet(  m_ProjInfo.path,false );
	GetTerminal().m_pWallShapeList->saveDataSet(  m_ProjInfo.path,false );

	//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
	{
		CString strLog = _T("");
		strLog = _T("OnProcEditFinished;");
		ECHOLOG->Log(RailWayLog,strLog);
	}

	UpdateAllViews(NULL);
}

void CTermPlanDoc::SelectProcessors(const std::vector<CProcessor2*>& proclist)
{
	UnSelectAllProcs();
	int c=proclist.size();
	for(int i=0; i<c; i++) {
		SelectByGroup(proclist[i]);
	}
	UpdateAllViews(NULL, 0, NULL);
}

//If bAddToSelection is FALSE, all currently selected processors are unselected
//If pProc is already selected, we remove it from the selectionlist
//Otherwise, pProc (or group) is added to the selection list
BOOL CTermPlanDoc::SelectProc(CProcessor2* pProc, BOOL bAddToSelection)
{
	BOOL bRet = FALSE;
	if(!bAddToSelection)
		UnSelectAllProcs();
	if(pProc->IsSelected())
		UnSelectProc(pProc);
	else
		SelectByGroup(pProc);

	UpdateAllViews(NULL, 0, NULL);
	return bRet;
}

CProcessor2* CTermPlanDoc::GetProcWithSelName(GLuint nSelName)
{
	//get processor with SelName == nSelName
	//first check undefined list
	CProcessor2* pSelProc = NULL;
	for(int i=0; i<static_cast<int>(GetCurrentPlacement()->m_vUndefined.size());i++) {
		if(nSelName == (GetCurrentPlacement()->m_vUndefined[i])->GetSelectName()) {
			return (GetCurrentPlacement()->m_vUndefined[i]);
		}
	}
	//now check defined list
	for(int i=0; i<static_cast<int>(GetCurrentPlacement()->m_vDefined.size());i++) {
		if(nSelName == (GetCurrentPlacement()->m_vDefined[i])->GetSelectName()) {
			return (GetCurrentPlacement()->m_vDefined[i]);
		}
	}
	return NULL;
}

BOOL CTermPlanDoc::DoLButtonDownOnProc(GLuint nSelName, BOOL bCtrlDown)
{
	//if ctrl is down:
	//click on unselected proc adds to selection
	//clck on selected proc does nothing (until mouse up)
	//if ctrl is not down:
	//click on unselected or selected proc starts new selection
	CProcessor2* pSelProc = GetProcWithSelName(nSelName);

	if(pSelProc == NULL)
		return TRUE;

	//ASSERT(pSelProc != NULL);
	if(bCtrlDown) {
		if(!pSelProc->IsSelected()) {
			//m_vSelectedProcessors.push_back(pSelProc);
			SelectByGroup(pSelProc);
			m_bUnselectOnButonUp = FALSE;
		}
		else {
			m_bUnselectOnButonUp = TRUE;
		}

	}
	else {
		UnSelectAllProcs();
		m_bUnselectOnButonUp = FALSE;
		SelectByGroup(pSelProc);
		//m_vSelectedProcessors.push_back(pSelProc);
	}
	
	if(GetSelectProcessors().GetCount()>0)
		m_ptProcCmdPosStart = GetSelectedObjectDisplay(0)->GetLocation();
	
	return TRUE;
}

BOOL CTermPlanDoc::DoLButtonUpOnProc(GLuint nSelName, BOOL bCtrlDown)
{
	CProcessor2* pSelProc = GetProcWithSelName(nSelName);
	if(!pSelProc) return FALSE;
	ASSERT(pSelProc != NULL);
	if(pSelProc->IsSelected() && m_bUnselectOnButonUp)
		UnSelectProc(pSelProc);
	return TRUE;
}

//BOOL CTermPlanDoc::SelectProc2(CProcessor2 pProc, BOOL bAddToSelection)
//{
//	
//}

BOOL CTermPlanDoc::SelectProc(GLuint nSelName, BOOL bAddToSelection)
{
	//get processor with SelName == nSelName
	// TRACE("selected proc selname: %u\n", nSelName);
	CProcessor2* pSelProc = GetProcWithSelName(nSelName);
	//if proc was found
	if(pSelProc != NULL)
	{
		if(!bAddToSelection)
		{
			UnSelectAllProcs();
		}

		SelectByGroup(pSelProc)			;
		return TRUE;
	}
	return FALSE;
}
BOOL CTermPlanDoc::SelectObject(CObjectDisplay* pObjDisplay, BOOL bAddToSelection)
{
	//get processor with SelName == nSelName
	// TRACE("selected proc selname: %u\n", nSelName);
	//if proc was found
	if(pObjDisplay != NULL)
	{
		if(!bAddToSelection)
		{
			UnSelectAllProcs();
		}

		SelectByGroup(pObjDisplay)			;
		return TRUE;
	}
	return FALSE;
}
BOOL CTermPlanDoc::UnSelectProc(CProcessor2* pProc)
{
	int nCount = GetSelectProcessors().GetCount();

	for (int i=0; i< nCount; i++)
	{	
		CObjectDisplay *pObjDisplay = GetSelectedObjectDisplay(i);
		if(pObjDisplay &&pObjDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2 *pProc2 = (CProcessor2 *)pObjDisplay;
			if(pProc2 == pProc)
			{
				pProc2->Select(FALSE);
				GetSelectProcessors().DelItem(pObjDisplay);
			}
		}
	}

	//CPROCESSOR2LIST::iterator it;
	//for(it=GetSelectProcessors().begin(); it!=GetSelectProcessors().end(); it++) 
	//{
	//	if(*it == pProc) 
	//	{
	//		(*it)->Select(FALSE);
	//		GetSelectProcessors().erase(it);
	//		return TRUE;
	//	}
	//}
	return FALSE;
}

void CTermPlanDoc::UnSelectAllProcs()
{
	int nCount = GetSelectProcessors().GetCount();
	for(int i=0; i< nCount; i++) 
	{
		CObjectDisplay *pObjDisplay = GetSelectedObjectDisplay(i);

		if(pObjDisplay &&pObjDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2 *pProc2 = (CProcessor2 *)pObjDisplay;

			pProc2->Select(FALSE);
		}

	}
	GetSelectProcessors().Clear();
	//m_vSelectPlacements.Clear();
}

BOOL CTermPlanDoc::RecordTimerCallback(DWORD dwTime)
{
	ASSERT(m_pRecordedCameras);
	MovieCameraCase cam;
	dwTime = (10*m_animData.nAnimSpeed)/m_pRecordedCameras->GetFPS();
	if (m_pRecordedCameras->GetCameraCount() == 0)
	{
		cam.m_nStartTime = m_pRecordedCameras->GetStartTime();
		cam.m_nEndTime = cam.m_nStartTime + dwTime;
	}
	else
	{
		cam.m_nStartTime = m_pRecordedCameras->GetCameraAt(m_pRecordedCameras->GetCameraCount() - 1).m_nEndTime;
		cam.m_nEndTime = cam.m_nStartTime + dwTime;
	}
	if(m_animData.m_AnimationModels.IsOnBoardSel() || m_animData.m_AnimationModels.IsLandsideSel())
	{
		if (GetRender3DView())
		{
			(CCameraData&)cam.m_cam = GetRender3DView()->GetUserCurrentCamera();
			m_pRecordedCameras->AddCamera(cam);
		}
	}
	else
	{
		if (Get3DView())
		{
			cam.m_cam = *(Get3DView()->GetCamera());
			m_pRecordedCameras->AddCamera(cam);
		}
	}
	
	// TRACE("recorded camera\n");
	return TRUE;
}

BOOL CTermPlanDoc::AnimTimerCallback()
{
	static int c = 0;
	c++;
	if(c > CAnimationTimeManager::GetFrameRate()) {
		DisableViewPaint();
 		MSG msg;
 		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
 			TranslateMessage(&msg);
 			DispatchMessage(&msg);
 		}
		GetMainFrame()->m_wndAnimationBar.OnUpdateCmdUI(GetMainFrame(), FALSE);
		GetMainFrame()->m_wndCameraBar.OnUpdateCmdUI(GetMainFrame(), FALSE);
		EnableViewPaint();
		c = 0;
	}

	//calculate current time
	LARGE_INTEGER nThisSystemTime;
	//if(!QueryPerformanceCounter(&nThisSystemTime))
	//	return FALSE;
	VERIFY(QueryPerformanceCounter(&nThisSystemTime));
	LARGE_INTEGER nElapsedSystemTime;
	nElapsedSystemTime.QuadPart = nThisSystemTime.QuadPart - m_animData.nLastSystemTime.QuadPart;
	double dElapsedSystemTime = ((double)nElapsedSystemTime.QuadPart)/m_animData.nPerfTickFreq.QuadPart;

	long nThisAnimTime = 0;
	if(m_eAnimState == anim_playF)
		nThisAnimTime = m_animData.nLastAnimTime + ((long)(dElapsedSystemTime*10.0*m_animData.nAnimSpeed));
	else if(m_eAnimState == anim_playB)
		nThisAnimTime = m_animData.nLastAnimTime - ((long)(dElapsedSystemTime*10.0*m_animData.nAnimSpeed));
	else if(m_eAnimState == anim_pause)
		nThisAnimTime = m_animData.nLastAnimTime;
	else if(m_eAnimState == anim_viewmanip)
		nThisAnimTime = m_animData.nLastAnimTime;
	if(nThisAnimTime>=m_animData.nAnimEndTime || nThisAnimTime <= m_animData.nFirstEntryTime)
	{
		//StopAnimation();
		m_eAnimState = anim_pause;
		UpdateAllViews(NULL);
		return TRUE;
	}
	int nHour = nThisAnimTime / 360000;
	int nMin = (nThisAnimTime - nHour*360000)/6000;
	int nSec = (nThisAnimTime - nHour*360000 - nMin*6000)/100;
 
	//// TRACE("%.4f\n", dElapsedSystemTime);
	//// TRACE("time: %d = %02d:%02d:%02d\n", nThisAnimTime, nHour, nMin, nSec);

	//init speed case for movie
	if (m_pRecordedCameras)
	{
		MovieSpeedCase speedCase;
		speedCase.m_nStartTime = m_animData.nLastAnimTime;
		speedCase.m_nEndTime = nThisAnimTime;
		speedCase.m_speed = m_animData.nAnimSpeed;
		m_pRecordedCameras->AddSpeedCase(speedCase);
	}
	

	m_animData.nLastSystemTime = nThisSystemTime;
	m_animData.nLastAnimTime = nThisAnimTime;


	//if(m_bRecord)
	//	CheckWithAVI();
	if(m_eAnimState == anim_playF || m_eAnimState == anim_playB) {
		POSITION pos = GetFirstViewPosition();
		while(pos != NULL)
		{
			CView* pNextView = GetNextView(pos);
			
            // Is this a viewing class?
            if(    pNextView->IsKindOf(RUNTIME_CLASS(C3DView))
				|| pNextView->IsKindOf(RUNTIME_CLASS(CDensityGLView))
				|| pNextView->IsKindOf(RUNTIME_CLASS(CAircraftLayout3DView)) 
				|| pNextView->IsKindOf(RUNTIME_CLASS(CRender3DView))
			   )
            {
				// Repaint...                
                pNextView->Invalidate(FALSE);
            }

            // This is the FIDS class, let it handle its own updating..
            if(pNextView->IsKindOf(RUNTIME_CLASS(CFIDSView)))
                ((CFIDSView *) pNextView)->UpdateOutput();
		}
	}
	if(nThisAnimTime < m_animData.nAnimEndTime)
	{
		ElapsedTime _CurrentTime ;
		_CurrentTime.setPrecisely(nThisAnimTime) ;
		m_CallOutManger.NoticeTheTimeHasUpdated(_CurrentTime) ;
	}
	return TRUE;
}

CDensityGLView* CTermPlanDoc::GetDensityAnimView()
{
	POSITION pos = GetFirstViewPosition();
	while(pos != NULL)
	{
		CView* pNextView = GetNextView(pos);
		if(pNextView->IsKindOf(RUNTIME_CLASS(CDensityGLView)))
			return (CDensityGLView*) pNextView;
	}
	return NULL;
}

// Return the Flight Information Display frame, if created...
CFIDSView *CTermPlanDoc::GetFIDSView()
{
    // Variables...
    POSITION    Position    = 0;
    CView      *pNextView   = NULL;

    // Get the first position...
    Position = GetFirstViewPosition();

    // Scan through each position until we find appropriate view...
    while(Position)
    {
        // Fetch view...
        pNextView = GetNextView(Position);

        // Found Flight Information Display, return it...
        if(pNextView->IsKindOf(RUNTIME_CLASS(CFIDSView)))
			return (CFIDSView *) pNextView;
    }

    // Not found...
	return NULL;
}

C3DView* CTermPlanDoc::Get3DView() const //returns active 3D view
{
	POSITION pos = GetFirstViewPosition();
	while(pos != NULL)
	{
		CView* pNextView = GetNextView(pos);
		if(pNextView->IsKindOf(RUNTIME_CLASS(C3DView)))
			return ((C3DView*)pNextView)->GetParentFrame()->GetActivePane();
		
	}
	return NULL;
}

CRender3DView* CTermPlanDoc::GetRender3DView() const //returns active render 3D view of ogre
{
	POSITION pos = GetFirstViewPosition();
	while(pos != NULL)
	{
		CView* pNextView = GetNextView(pos);		
		if(pNextView->IsKindOf(RUNTIME_CLASS(CRender3DView)))
			return ((CRender3DView*)pNextView)->GetParentFrame()->GetActivePane();
	}
	return NULL;
}

IRender3DView* CTermPlanDoc::GetIRenderView() const
{
	POSITION pos = GetFirstViewPosition();
	while(pos != NULL)
	{
		CView* pNextView = GetNextView(pos);		
		if(pNextView->IsKindOf(RUNTIME_CLASS(CRender3DView)) && pNextView->IsWindowVisible())
			return ((CRender3DView*)pNextView)->GetParentFrame()->GetActivePane();				
		if(pNextView->IsKindOf(RUNTIME_CLASS(C3DView)) && pNextView->IsWindowVisible())
			return ((C3DView*)pNextView)->GetParentFrame()->GetActivePane();
	}
	return NULL;
}
CAircraftLayoutView* GetAircraftLayoutView(CTermPlanDoc* pDoc)
{
	/*POSITION pos = pDoc->GetFirstViewPosition();
	while(pos != NULL)
	{
		CView* pNextView = pDoc->GetNextView(pos);
		if(pNextView->IsKindOf(RUNTIME_CLASS(CAircraftLayoutView)))
			return ((CAircraftLayoutView*) pNextView);
	}*/
	return NULL;
}

//CAircraftModelEditView* CTermPlanDoc::GetModelEditView()
//{
//	POSITION pos = GetFirstViewPosition();
//	while (pos != NULL)
//	{
//		CView* pNextView = GetNextView(pos);
//		if (pNextView->IsKindOf(RUNTIME_CLASS(CAircraftModelEditView)))
//		{
//			return (CAircraftModelEditView*)pNextView;
//		}
//	}
//	return NULL;
//}

COnboardAircraftCommanView* CTermPlanDoc::GetAircraftCommandView()
{
	POSITION pos = GetFirstViewPosition();
	/*while(pos != NULL)
	{
		CView* pNextView = GetNextView(pos);
		if (pNextView->IsKindOf(RUNTIME_CLASS(COnboardAircraftCommanView)))
		{
			return (COnboardAircraftCommanView*)pNextView;
		}
	}*/
	return NULL;
}

CChildFrame* CTermPlanDoc::Get3DViewParent() //returns active CChildFrame
{
	POSITION pos = GetFirstViewPosition();
	while(pos != NULL)
	{
		CView* pNextView = GetNextView(pos);
		if(pNextView->IsKindOf(RUNTIME_CLASS(C3DView)))
			return (CChildFrame*) pNextView->GetParentFrame();
	}
	return NULL;
}

CView* CTermPlanDoc::GetSubView(CRuntimeClass *pViewClass)
{
	POSITION pos = GetFirstViewPosition();
	while(pos != NULL)
	{
		CView* pNextView = GetNextView(pos);
		if(pNextView->IsKindOf(pViewClass))
			return pNextView;
	}
	return NULL;
}

CNodeView* CTermPlanDoc::GetNodeView()
{
	POSITION pos = GetFirstViewPosition();
	while(pos != NULL)
	{
		CView* pNextView = GetNextView(pos);
		if(pNextView->IsKindOf(RUNTIME_CLASS(CNodeView)))
			return (CNodeView*) pNextView;
	}
	return NULL;
}

CMainFrame* CTermPlanDoc::GetMainFrame()
{
	return (CMainFrame*) theApp.GetMainWnd();
}

void CTermPlanDoc::OnProjectBackup() 
{
	//zip up project and store in backups folder	
}


void CTermPlanDoc::OnProjectRestore() 
{
	// unzip into temp dir and rename appropriately	
}

void CTermPlanDoc::OnCloseDocument() 
{
	// load component models into aircraft component model shapes bar
	ACCOMPONENTSHAPESMANAGER.Reload(NULL);
	ACFURNISHINGSHAPESMANAGER.Reload(NULL);

	if(GetTerminal().m_pAirportDB != NULL)
	{
      //  GetTerminal().m_pAirportDB->WriteAiportDataBase() ;
		//GetTerminal().m_pAirportDB->SaveDataToDB() ;
		delete GetTerminal().m_pAirportDB ;
		GetTerminal().m_pAirportDB = NULL ;
	}

	SaveMathematicData();
	CString strProjectName = m_ProjInfo.name;
	if( m_bNeedToSaveProjFile )
	{
//		m_logFile.saveDataSet(m_ProjInfo.path,false);
		m_ProjInfo.modifytime = CTime::GetCurrentTime();
		PROJMANAGER->UpdateProjInfoFile( m_ProjInfo );
		m_bNeedToSaveProjFile = FALSE;
	}
	
	ANIMTIMEMGR->EnrollDocument(NULL);
	CMainFrame* pMF=(CMainFrame*)AfxGetMainWnd();

	ToolBarUpdateParameters updatePara;
	updatePara.m_operatType = ToolBarUpdateParameters::CloseProject;
	pMF->SendMessage(UM_SHOW_CONTROL_BAR,1,(LPARAM)&updatePara) ;
	pMF->saveMainBarComboBoxToDB() ;
//	CMDIChildWnd* pMDIActive = pMF->MDIGetActive() ;
//	CView* view = pMDIActive->GetActiveView() ;
//	if(view == NULL)
       pMF->SetMainBarComboBox(FALSE , TRUE);
//	else
//		pMF->SetMainBarComboBox(TRUE , TRUE);
	pMF->DestroyFloorAdjustDlg();
	try
	{
	m_umGlobalUnitManager.SaveData();
	}
	catch (CADOException &e)
	{
		e.ErrorMessage() ;
		return ;
	}
    CString path = m_ProjInfo.path ;
	CDocument::OnCloseDocument();
	((CTermPlanApp*)AfxGetApp())->OnCloseProject();

	CMasterDatabase *pMasterDatabase = ((CTermPlanApp *)AfxGetApp())->GetMasterDatabase();

	//shrink database

	//ShrinkDatabase(pMasterDatabase,strProjectName);

	CProjectDatabase projectDatabase(PROJMANAGER->GetAppPath());
	projectDatabase.CloseProject(pMasterDatabase,strProjectName);
	
	if(GetMainFrame())
	{
		GetMainFrame()->LoadDefaultMenu();
	}

	//pMasterDatabase->Close();
}

void CTermPlanDoc::OnCamDolly() 
{
	if(Get3DView() == NULL)
		return;
	if(Get3DView()->m_eMouseState == NS3DViewCommon::_dolly) {
		Get3DView()->m_eMouseState = NS3DViewCommon::_default;
		return;
	}
	Get3DView()->m_eMouseState = NS3DViewCommon::_dolly;
	
}

void CTermPlanDoc::OnCamPan() 
{
	if(Get3DView() == NULL)
		return;
	if(Get3DView()->m_eMouseState == NS3DViewCommon::_pan) {
		Get3DView()->m_eMouseState = NS3DViewCommon::_default;
		return;
	}
	Get3DView()->m_eMouseState = NS3DViewCommon::_pan;
}

void CTermPlanDoc::OnCamTumble() 
{
	if(Get3DView() == NULL)
		return;
	if(Get3DView()->m_eMouseState == NS3DViewCommon::_tumble) {
		Get3DView()->m_eMouseState = NS3DViewCommon::_default;
		return;
	}
	Get3DView()->m_eMouseState = NS3DViewCommon::_tumble;
}

void CTermPlanDoc::OnUpdateCamDolly(CCmdUI* pCmdUI) 
{
	if(Get3DView() != NULL) {
		pCmdUI->Enable(TRUE);
		if(Get3DView()->m_eMouseState == NS3DViewCommon::_dolly)
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}
	else {
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}
void CTermPlanDoc::OnUpdateFloorsDelete(CCmdUI* pCmdUI) 
{
//	if( EnvMode_AirSide == m_systemMode )
//		pCmdUI->Enable(FALSE);
//
//	pCmdUI->Enable(TRUE);
}


void CTermPlanDoc::OnUpdateCamPan(CCmdUI* pCmdUI) 
{
	if(Get3DView() != NULL) {
		pCmdUI->Enable(TRUE);
		if(Get3DView()->m_eMouseState == NS3DViewCommon::_pan)
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}
	else {
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}

void CTermPlanDoc::OnUpdateCamTumble(CCmdUI* pCmdUI) 
{
	/*
	if (Get3DView() != NULL && 
		Get3DView()->GetCamera() != NULL &&
		Get3DView()->GetCamera()->GetProjectionType() == C3DCamera::perspective) {
	*/
	if (Get3DView() != NULL) {
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(Get3DView()->m_eMouseState == NS3DViewCommon::_tumble);
	}
	else {
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}

void CTermPlanDoc::OnLayoutLock()
{
	if ( EnvMode_OnBoard == m_systemMode )
	{
 		//C3DBaseView* pEditView = Get3DBaseView();
 		//if (pEditView)
 		//	pEditView->FlipLayoutLocked();
	}
	else
		m_bLayoutLocked = !m_bLayoutLocked;
}

void CTermPlanDoc::OnUpdateLayoutLock(CCmdUI* pCmdUI)
{
	if ( EnvMode_OnBoard == m_systemMode )
	{
// 		C3DBaseView* pEditView = Get3DBaseView();
// 		if (pEditView)
// 			pCmdUI->SetCheck(pEditView->GetLayoutLocked() ? 1 : 0);
	}
	else
		pCmdUI->SetCheck(m_bLayoutLocked);
}
/*
CTVNode* CTermPlanDoc::FindNodeByName(UINT idName)
{
	CString s;
	if(!s.LoadString(idName))
		return NULL;
	return FindNodeByName(s);
}

CTVNode* CTermPlanDoc::FindNodeByName(LPCTSTR sName)
{
	std::deque<CTVNode*> vList;
	//put top level nodes into list..
	CTVNode* pRootNode = NULL;
	if (m_systemMode == EnvMode_Terminal)
		pRootNode = GetTerminalRootTVNode();
	else if (m_systemMode == EnvMode_AirSide)
		pRootNode = GetAirSideRootTVNode();

	ASSERT(pRootNode != NULL);
	vList.push_front(pRootNode);

	while(vList.size() != 0) {
		CTVNode* pNode = vList[0];
		vList.pop_front();
		if(pNode->Name().CompareNoCase(sName) == 0)
			return pNode;
		for(int i=0; i<pNode->GetChildCount(); i++) {
			vList.push_front((CTVNode*) pNode->GetChildByIdx(i));
		}
	}
	return NULL;
}

CTVNode* CTermPlanDoc::FindNodeByData(DWORD dwData)
{
	std::deque<CTVNode*> vList;
	//put top level nodes into list..
	CTVNode* pRootNode = NULL;
	if (m_systemMode == EnvMode_Terminal)
		pRootNode = GetTerminalRootTVNode();
	else if (m_systemMode == EnvMode_AirSide)
		pRootNode = GetAirSideRootTVNode();
	
	ASSERT(pRootNode != NULL);
	vList.push_front(pRootNode);

	while(vList.size() != 0) {
		CTVNode* pNode = vList[0];
		vList.pop_front();
		if(pNode->m_dwData == dwData)
			return pNode;
		for(int i=0; i<pNode->GetChildCount(); i++) {
			vList.push_front((CTVNode*) pNode->GetChildByIdx(i));
		}
	}
	return NULL;
}
*/
BOOL CTermPlanDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
	// Save the document data to a file
	// lpszPathName = path name where to save document file
	// if lpszPathName is NULL then the user will be prompted (SaveAs)
	// note: lpszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
{
	CString newName = lpszPathName;
	if (newName.IsEmpty()) { //name is empty so we need to generate one
		CDocTemplate* pTemplate = GetDocTemplate();
		ASSERT(pTemplate != NULL);

		newName = m_strPathName;
		if (bReplace && newName.IsEmpty())
		{
			newName = m_strTitle;
			// check for dubious filename
			int iBad = newName.FindOneOf(_T(" #%;/\\"));
			if (iBad != -1)
				newName.ReleaseBuffer(iBad);
		}

		if (!AfxGetApp()->DoPromptFileName(newName,
		  bReplace ? AFX_IDS_SAVEFILE : AFX_IDS_SAVEFILECOPY,
		  OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, FALSE, pTemplate))
			return FALSE;       // don't even attempt to save
	}
//	newName += _T("\\projdata.arc");

	CWaitCursor wait;

	if (!OnSaveDocument(newName)) {
		if (lpszPathName == NULL) {
			// be sure to delete the file
			TRY {
				CFile::Remove(newName);
			}
			CATCH_ALL(e) {
				TRACE0("Warning: failed to delete file after failed SaveAs.\n");
			}
			END_CATCH_ALL
		}
		return FALSE;
	}

	// reset the title and change the document name
	if (bReplace)
		SetPathName(newName);
	
	// update project modify time
	if( m_bNeedToSaveProjFile )
	{
		m_ProjInfo.modifytime = CTime::GetCurrentTime();
		PROJMANAGER->UpdateProjInfoFile( m_ProjInfo );
		m_bNeedToSaveProjFile = FALSE;
	}
	//save Undo project 
   SaveUndoProject(newName) ;

	//////////////////////////////////////////////////////////////////////////
	return TRUE;        // success
}

static CONST TCHAR* CONST g_sNoSavedViews[] =
{
	_T("(No 2D Saved Views)"),
	_T("(No 3D Saved Views)"),
};

void CTermPlanDoc::LoadSavedRenderCamerasIntoCmdUI( CCmdUI* pCmdUI, const C3DViewCameras &cams, CCameraData::PROJECTIONTYPE pt )
{
	CMenu* pMenu = pCmdUI->m_pMenu;
	if(pMenu==NULL)
		return;
	UINT i = 0;
	while(pMenu->DeleteMenu(pCmdUI->m_nID + i, MF_BYCOMMAND)) { i++; }


	const C3DViewCameraDesc::List& savedCams = cams.m_savedCameras;
	size_t nSavedViewCount = savedCams.size();
	int nxDCount = 0;
	for(size_t i=0; i<nSavedViewCount; i++)
	{
		const C3DViewCameraDesc& camDesc = savedCams[i];
		if(camDesc.GetDefaultCameraData().GetProjectionType()==pt) {
			pMenu->InsertMenu(pCmdUI->m_nIndex++, MF_BYPOSITION | MF_STRING, pCmdUI->m_nID++, camDesc.GetName());
			nxDCount++;
		}
	}
	if(nxDCount==0)
	{
		pMenu->InsertMenu(pCmdUI->m_nIndex, MF_BYPOSITION | MF_STRING | MF_GRAYED, pCmdUI->m_nID, g_sNoSavedViews[pt]);
		pCmdUI->m_bEnableChanged = TRUE;
		return;
	}

	pCmdUI->m_nIndex--;
	pCmdUI->m_nIndexMax = pCmdUI->m_pMenu->GetMenuItemCount();
}
C3DViewCameraDesc& CTermPlanDoc::GetRenderCameraDescByIndex(C3DViewCameras& cams, size_t nIndex, CCameraData::PROJECTIONTYPE pt)
{
	size_t nSavedViewCount = cams.m_savedCameras.size();
	ASSERT(nIndex >= 0 && nIndex < nSavedViewCount);

	size_t nxDCount = 0;
	for(size_t i=0; i<nSavedViewCount; i++)
	{
		C3DViewCameraDesc& camDesc = cams.m_savedCameras[i];
		if (camDesc.GetDefaultCameraData().GetProjectionType()==pt)
		{
			if(nxDCount==nIndex)
			{
				return camDesc;
			}
			nxDCount++;
		}
	}
	ASSERT(FALSE);
	return cams.m_savedCameras.front();
}


void CTermPlanDoc::LoadSavedCamerasIntoCmdUI( CCmdUI* pCmdUI, const CCameras& cams, CCameraData::PROJECTIONTYPE pt )
{
	CMenu* pMenu = pCmdUI->m_pMenu;
	if(pMenu==NULL)
		return;
	UINT i = 0;
	while(pMenu->DeleteMenu(pCmdUI->m_nID + i, MF_BYCOMMAND)) { i++; }


	const std::vector<CViewDesc>& savedCams = cams.m_savedViews;
	size_t nSavedViewCount = savedCams.size();
	int nxDCount = 0;
	for(size_t i=0; i<nSavedViewCount; i++)
	{
		const CViewDesc& camDesc = savedCams[i];
		const C3DCamera* pDefCamera = camDesc.panecameras[0];
		if(pDefCamera->GetProjectionType()==pt) {
			pMenu->InsertMenu(pCmdUI->m_nIndex++, MF_BYPOSITION | MF_STRING, pCmdUI->m_nID++, camDesc.name);
			nxDCount++;
		}
	}
	if(nxDCount==0)
	{
		pMenu->InsertMenu(pCmdUI->m_nIndex, MF_BYPOSITION | MF_STRING | MF_GRAYED, pCmdUI->m_nID, g_sNoSavedViews[pt]);
		pCmdUI->m_bEnableChanged = TRUE;
		return;
	}

	pCmdUI->m_nIndex--;
	pCmdUI->m_nIndexMax = pCmdUI->m_pMenu->GetMenuItemCount();
}
CViewDesc& CTermPlanDoc::GetCameraDescByIndex(CCameras& cams, size_t nIndex, CCameraData::PROJECTIONTYPE pt)
{
	size_t nSavedViewCount = cams.m_savedViews.size();
	ASSERT(nIndex >= 0 && nIndex < nSavedViewCount);

	size_t nxDCount = 0;
	for(size_t i=0; i<nSavedViewCount; i++)
	{
		CViewDesc& camDesc = cams.m_savedViews[i];
		const C3DCamera* pDefCamera = camDesc.panecameras[0];
		if(pDefCamera->GetProjectionType()==pt)
		{
			if(nxDCount==nIndex)
			{
				return camDesc;
			}
			nxDCount++;
		}
	}
	ASSERT(FALSE);
	return cams.m_savedViews.front();
}


void CTermPlanDoc::OnUpdateViewNamedViews2D(CCmdUI* pCmdUI) 
{
	if (RenderEngine::getSingleton().GetAlwaysInUse() && GetRender3DView())
	{
		LoadSavedRenderCamerasIntoCmdUI(pCmdUI, GetCurModeRenderCameras(), CCameraData::parallel);
	} 
	else
	{
		LoadSavedCamerasIntoCmdUI(pCmdUI, GetCurModeCameras(), C3DCamera::parallel);
	}
}

void CTermPlanDoc::OnUpdateViewNamedViews3D(CCmdUI* pCmdUI) 
{
	if (RenderEngine::getSingleton().GetAlwaysInUse() && GetRender3DView())
	{
		LoadSavedRenderCamerasIntoCmdUI(pCmdUI, GetCurModeRenderCameras(), CCameraData::perspective);
	} 
	else
	{
		LoadSavedCamerasIntoCmdUI(pCmdUI, GetCurModeCameras(), C3DCamera::perspective);
	}
}

void CTermPlanDoc::OnCtxProcDisplayProperties() 
{
	int nCount = GetSelectProcessors().GetCount();
	ASSERT(nCount > 0);
	
	//CProcessor2* pProc = m_vSelectedProcessors[m_vSelectedProcessors.size()-1];
	// show processor display properties dialog

	CPROCESSOR2LIST proc2lst;
	int nObjCount =  GetSelectProcessors().GetCount();
	for (int nObj =0; nObj < nObjCount; ++ nObj)
	{
		CObjectDisplay *pObjDisplay = GetSelectedObjectDisplay(nObj);
		if (pObjDisplay && pObjDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			proc2lst.push_back((CProcessor2 *)pObjDisplay);
		}
	}
	if (proc2lst.size() ==0	)
		return;
	
	CDlgProcDisplayProperties dlg(&proc2lst);
	if(dlg.DoModal() == IDOK) 
	{
		GetCurrentPlacement()->saveDataSet(m_ProjInfo.path, false);
		/*
		CProcessor2::CProcDispProperties dispProp;
		for(int i=PDP_MIN; i<=PDP_MAX; i++) {
			dispProp.bDisplay[i] = dlg.m_bDisplay[i];
			dispProp.color[i] = dlg.m_cColor[i];
		}
		pProc->m_dispProperties = dispProp;
		*/
	}
}

BOOL NameExists(CPROCESSOR2LIST& vList, CString sName)
{
	/*
	for(long i=0; i<vList.size(); i++) {
		if(sName.CompareNoCase(vList[i]->Name()) == 0)
			return TRUE;
	}
	*/
	return FALSE;
}

CString GetProcessorCopyName(CPROCESSOR2LIST& vList, CProcessor2* pProc)
{
	/*
	const LPCTSTR numeric = _T("0123456789");
	CString sName = pProc->Name();
	CString sNewName;
	int dashidx = sName.ReverseFind(_T('-'));
	if(dashidx != -1) {
		sNewName = sName.Left(dashidx+1);
		int num = _ttoi(sName.Right(sName.GetLength()-(dashidx+1)));
		if(num != 0) {
			CString sExt;
			do {
				num++;
				sExt.Format("%.2d", num);
			}
			while(NameExists(vList, sNewName + sExt) && num < 100);
			if(num != 100)
				return sNewName + sExt;
		}
	}
	
	return sName;
	*/
	return _T("");
}

void CTermPlanDoc::OnUpdateCtxCopyProcessor(CCmdUI* pCmdUI) 
{
	if(GetSelectProcessors().GetCount() > 1)
		pCmdUI->SetText(_T("Copy Processors"));
	else
		pCmdUI->SetText(_T("Copy Processor"));
	
}

void CTermPlanDoc::OnUpdateCtxProcDispProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetSelectProcessors().GetCount() > 0);
}

void CTermPlanDoc::OnUpdateCtxRotateProcessors(CCmdUI* pCmdUI) 
{
	if(GetSelectProcessors().GetCount() > 1)
		pCmdUI->SetText(_T("Rotate Processors"));
	else
		pCmdUI->SetText(_T("Rotate Processor"));
}

void CTermPlanDoc::OnUpdateCtxScaleProcessors(CCmdUI* pCmdUI) 
{
	if(GetSelectProcessors().GetCount() > 1)
		pCmdUI->SetText(_T("Scale Shapes XYZ"));
	else
		pCmdUI->SetText(_T("Scale Shapes XYZ"));
	pCmdUI->Enable(TRUE);
}

void CTermPlanDoc::OnUpdateCtxScaleProcessorsX(CCmdUI* pCmdUI) 
{
	if(GetSelectProcessors().GetCount() > 1)
		pCmdUI->SetText(_T("Scale Shapes X"));
	else
		pCmdUI->SetText(_T("Scale Shape X"));
	pCmdUI->Enable(TRUE);
}

void CTermPlanDoc::OnUpdateCtxScaleProcessorsY(CCmdUI* pCmdUI) 
{
	if(GetSelectProcessors().GetCount() > 1)
		pCmdUI->SetText(_T("Scale Shapes Y"));
	else
		pCmdUI->SetText(_T("Scale Shape Y"));
	pCmdUI->Enable(TRUE);
}

void CTermPlanDoc::OnUpdateCtxScaleProcessorsZ(CCmdUI* pCmdUI) 
{
	if(GetSelectProcessors().GetCount() > 1)
		pCmdUI->SetText(_T("Scale Shapes Z"));
	else
		pCmdUI->SetText(_T("Scale Shape Z"));
	pCmdUI->Enable(TRUE);
}

void CTermPlanDoc::OnUpdateCtxMoveProcessors(CCmdUI* pCmdUI) 
{
	if(GetSelectProcessors().GetCount() > 1)
		pCmdUI->SetText(_T("Move Processors"));
	else
		pCmdUI->SetText(_T("Move Processor"));	
}

void CTermPlanDoc::OnUpdateCtxMoveShape(CCmdUI* pCmdUI) 
{
	if(GetSelectProcessors().GetCount() > 1)
		pCmdUI->SetText(_T("Move Shapes XY"));
	else
		pCmdUI->SetText(_T("Move Shape XY"));	
}

void CTermPlanDoc::OnUpdateCtxRotateShape(CCmdUI* pCmdUI) 
{
	if(GetSelectProcessors().GetCount() > 1)
		pCmdUI->SetText(_T("Rotate Shapes"));
	else
		pCmdUI->SetText(_T("Rotate Shape"));	
}

void CTermPlanDoc::OnUpdateViewRadar(CCmdUI* pCmdUI)
{
	C3DView* pView = Get3DView();
	if(pView && pView->GetCamera()->GetProjectionType() == C3DCamera::parallel)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);

	if(pView && pView->IsRadarOn())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CTermPlanDoc::GenerateReport()
{
	
	m_arcport.GetARCReportManager().GenerateReport(m_ProjInfo, &GetTerminal(),static_cast<int>(m_floors.GetCount()));
	
	UpdateAllViews(NULL);
//	delete pRep;
}

//int CTermPlanDoc::GetSpecType()
//{
//	return m_arcReportManager.GetSpecType();
//}
//
//CString CTermPlanDoc::GetRepTitle()
//{
//	return m_arcReportManager.GetRepTitle();
//}
//CString CTermPlanDoc::GetAirsideReportTitle()
//{
//	return m_arcReportManager.GetAirsideReportTitle();
//}
void CTermPlanDoc::OnUpdateAnimationStart(CCmdUI* pCmdUI) 
{
	if(m_eAnimState	== anim_none) {
		pCmdUI->SetText(_T("Start Animation"));
		pCmdUI->SetCheck(0);
	}
	else {
		pCmdUI->SetText(_T("Stop Animation"));
	}
	pCmdUI->Enable(TRUE);
}


void CTermPlanDoc::OnUpdateAnimationStop(CCmdUI* pCmdUI) 
{
	/*
	if(m_eAnimState != anim_none)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
		*/
}


void CTermPlanDoc::OnUpdateAnimationPause(CCmdUI* pCmdUI) 
{
	if(m_eAnimState	== anim_playF || m_eAnimState	== anim_playB) {
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText("Pause Animation");
	}
	else if(m_eAnimState == anim_pause) {
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText("Resume Animation");
	}
	else
		pCmdUI->Enable(FALSE);
}


BOOL CTermPlanDoc::BuildTempAirsideTracerData()
{
	CAirsideSimLogs& airsideSimLogs = GetAirsideSimLogs();
	AirsideFlightLog& airsideFlightLog = airsideSimLogs.m_airsideFlightLog;
	int nCount = airsideFlightLog.getCount();
	
	int aptID = InputAirside::GetAirportID(GetProjectID());
	ALTAirport altAirport;
	altAirport.ReadAirport(aptID);
	DistanceUnit dALt = altAirport.getElevation();

	AirsideFlightLogEntry element;
	element.SetEventLog(&(airsideSimLogs.m_airsideFlightEventLog));

	CProgressBar bar( "Generating Airside Tracers...", 100, nCount+10, TRUE );

	m_tempAirsideTracerData.clear();

	bar.SetPos(10);

	std::vector<CTrackerVector3> ArrTarce, DepTrace;
	for(long ii=0; ii<nCount; ii++) 
	{//for each aircraft			

		int nArrDspIdx = m_vACDispPropIdx[ii].first;
		int nDepDspIdx = m_vACDispPropIdx[ii].second;		

		CAircraftDispPropItem* pArrADPI = NULL;
		CAircraftDispPropItem* pDepADPI = NULL;
		
		if (nArrDspIdx > -1)
			pArrADPI = m_aircraftDispProps.GetDispPropItem(nArrDspIdx);
		else
			pArrADPI = m_aircraftDispProps.GetDefaultDispProp();

		if (nDepDspIdx > -1)
			pDepADPI = m_aircraftDispProps.GetDispPropItem(nDepDspIdx);
		else
			pDepADPI = m_aircraftDispProps.GetDefaultDispProp();

		int nArrCount = 0, nDepCount = 0;
		if (pArrADPI || pDepADPI)
		{
			airsideFlightLog.getItem(element, ii);

			long nEventCount = element.getCount();
			ArrTarce.clear();
			DepTrace.clear();

			for(long jj=1; jj<nEventCount-1; jj++)  //for each event of this pax
			{
				AirsideFlightEventStruct afes = element.getEvent(jj);
				if (afes.mode == OnTerminate)//OnTerminate should not add to tracer
					continue;
				
				if (afes.state == 'A')
				{
					CTrackerVector3 pos(afes.x, afes.y, afes.z - dALt);
					ArrTarce.push_back(pos);
					nArrCount++;
				}
				else
				{
					CTrackerVector3 pos(afes.x, afes.y, afes.z - dALt);
					DepTrace.push_back(pos);
					nDepCount++;
				}
			}
		}
		
		if(pArrADPI != NULL && (pArrADPI->IsLeaveTrail() && pArrADPI->IsVisible())) 
		{					
			int nTrackIdx = m_tempAirsideTracerData.AddTrack(nArrCount, nArrDspIdx, ii);
			std::vector<CTrackerVector3>& track = m_tempAirsideTracerData.GetTrack(nTrackIdx);
			track.assign(ArrTarce.begin(),ArrTarce.end());			
		}

		if(pDepADPI != NULL && (pDepADPI->IsLeaveTrail() && pDepADPI->IsVisible())) 
		{					
			int nTrackIdx = m_tempAirsideTracerData.AddTrack(nDepCount, nDepDspIdx, ii);
			std::vector<CTrackerVector3>& track = m_tempAirsideTracerData.GetTrack(nTrackIdx);
			track.assign(DepTrace.begin(),DepTrace.end());			
		}

		bar.StepIt();
	}
	return TRUE;
}

//pax tracer data
BOOL CTermPlanDoc::BuildTempTracerData()
{
	MobLogEntry element;
	PaxLog* pPaxLog = GetTerminal().paxLog;
	long nPaxCount = pPaxLog->getCount();

	CProgressBar bar( "Generating Tracers...", 100, nPaxCount+10, TRUE );

	m_pPaxCountPerDispProp.clear(); 
	m_pPaxCountPerDispProp.resize(m_paxDispProps.GetCount()+2,0);

	m_tempTracerData.clear();

	bar.SetPos(10);

	for(long ii=0; ii<nPaxCount; ii++) 
	{//for each pax				
		CPaxDispPropItem* pPDPI = m_paxDispProps.GetDispPropItem(m_vPaxDispPropIdx[ii]);
		if(pPDPI->IsLeaveTrail() && pPDPI->IsVisible()) {					
			int modulo = static_cast<int>(1.0/pPDPI->GetDensity());
			if(m_pPaxCountPerDispProp[m_vPaxDispPropIdx[ii]+2] % modulo == 0) {
				pPaxLog->getItem(element, ii);
				element.SetOutputTerminal(&(GetTerminal()));
				element.SetEventLog(GetTerminal().m_pMobEventLog);
				
				long nEventCount = element.getCount();
				int nTrackIdx = m_tempTracerData.AddTrack(nEventCount, m_vPaxDispPropIdx[ii], ii);
				std::vector<CTrackerVector3>& vTrack = m_tempTracerData.GetTrack(nTrackIdx);
			
				vTrack.clear();

				for(long jj=1; jj<nEventCount; jj++)  //for each event of this pax
				{
					
					MobEventStruct pes = element.getEvent(jj);

					if(pes.state==TerminalBegin || pes.state==TerminalEnd)
					{
						continue;
					}
					CTrackerVector3 track;
					track[VX] = pes.x;
					track[VY] = pes.y;

					if(pes.m_IsRealZ)
					{
						track.IsRealZ(pes.m_IsRealZ) ;
						track[VZ] = pes.m_RealZ;
					}
					else
						track[VZ] = pes.z;	
					vTrack.push_back(track);

				}
			}
			m_pPaxCountPerDispProp[m_vPaxDispPropIdx[ii]+2]++;
		}
		bar.StepIt();
	}
	return TRUE;
}

BOOL CTermPlanDoc::BuildTempVehicleTracerData()
{
	long firstTime,endTime;
	if (!LoadLandSideLogs(firstTime,endTime))
	{
		return FALSE;
	}
	LandsideVehicleLog& pVehicleLog = GetLandsideSimLogs().getVehicleLog();
	long nVehicleCount = pVehicleLog.getCount();

	CProgressBar bar("Generating Vehicle Tracers...",100,nVehicleCount+10,TRUE);

	m_pVehicleCountPerDispProp.clear();
	m_pVehicleCountPerDispProp.resize(m_vehicleDispProps.GetVehicleCount()+2,0);
	m_tempVehicleTracerData.clear();
	bar.SetPos(10);
	/////////////////////////
	for (long ii=0;ii<nVehicleCount;ii++)
	{
		LandsideVehicleLogEntry element;
		pVehicleLog.getItem(element,ii);
		element.SetEventLog(GetLandsideSimLogs().getVehicleEventLog());
		m_landsideVehicleDispProps.ReadData();
		CLandSideVehicleProbDispDetail* pDispDetail =  m_landsideVehicleDispProps.FindBestMatch(element.GetVehicleDesc().sName);
		if(pDispDetail->getTail()&&pDispDetail->getVisible())
		{
			int modulo = static_cast<int>(1.0/pDispDetail->getTraceDensity());
			int mIndex = m_landsideVehicleDispProps.getIndex(pDispDetail);
			if (m_pVehicleCountPerDispProp[mIndex+2]%modulo==0)
			{
				long nEventCount = element.getCount();
				int nTrackIdx = m_tempVehicleTracerData.AddTrack(nEventCount,mIndex,ii,pDispDetail->getColor(),pDispDetail->getLineType());
				std::vector<CTrackerVector3>&track = m_tempVehicleTracerData.GetTrack(nTrackIdx);				
				for(long jj=0;jj<nEventCount;jj++)
				{
					LandsideVehicleEventStruct pes = element.getEvent(jj);
					track[jj][VX]=pes.x;
					track[jj][VY]=pes.y;
					track[jj][VZ]=(float)GetFloorByMode(EnvMode_LandSide).GetVisualHeight(pes.z);
				}
			}
			m_pVehicleCountPerDispProp[mIndex+2]++;
		}
		bar.StepIt();
	}
	return TRUE;
}

//BOOL CTermPlanDoc::ReadACDescriptions(long* pnFIT, long* pnLOT)
//fills m_vACDispPropIdx with an index to a ac disp prop item for each ac desc
//fills m_vACTagPropIdx with an index to a ac tag item for each ac desc
//fills m_vACShapeIdx with an index to an ac shape for each ac desc
//sets *pnFIT to First ac In time
//sets *pnLOT to Last ac Out time
BOOL CTermPlanDoc::ReadACDescriptions(long* pnFIT, long* pnLOT)
{
	*pnFIT = LONG_MAX;
	*pnLOT = -1;

	// clear logs
	clearLogsIfNecessary();

	GetAirsideSimLogs().OpenLogs(m_ProjInfo.path);

	long count = GetAirsideSimLogs().m_airsideFlightLog.getCount();

	m_vACDispPropIdx.clear();
	m_vACDispPropIdx.reserve(count);
	m_vACTagPropIdx.clear();
	m_vACTagPropIdx.reserve(count);
	m_vACShapeIdx.clear();
	m_vACShapeIdx.reserve(count);

	for(long i=0; i<count; i++) {
		AirsideFlightLogEntry fle;
		GetAirsideSimLogs().m_airsideFlightLog.getItem(fle, i);

		const AirsideFlightDescStruct& fds = fle.GetAirsideDesc();
		if( fds.startTime < *pnFIT )
			*pnFIT = fds.startTime;

		if( fds.endTime > *pnLOT )
			*pnLOT = fds.endTime;

		// find matching shape
		fds.acCategory;

		//TODO: uncomment section below once we implement aircraft disp props
		
		int nMatchARR = -1; int nMatchDep = -1;
		nMatchARR = m_aircraftDispProps.FindBestMatch(fds, true, &GetTerminal());
		nMatchDep = m_aircraftDispProps.FindBestMatch(fds, false, &GetTerminal());

		m_vACDispPropIdx.push_back( std::pair<int,int> (nMatchARR,nMatchDep) );

		//CAircraftDispPropItem* pADPI = NULL;
		//if(nMatch == -1) 
		//{                     //if no match, use default settings
		//	//pADPI = m_aircraftDispProps.GetDefaultDispProp();
		//	m_vACDispPropIdx.push_back(-1);
		//	//// TRACE("Colour: %d %d %d, Airline:%s\n", GetRValue(pADPI->GetColor()), GetGValue(pADPI->GetColor()), GetBValue(pADPI->GetColor()), fds.icao_code);
		//}
		//else if(nMatch>=0)
		//{
		//	//pADPI = m_aircraftDispProps.GetDispPropItem(nMatch);
		//	m_vACDispPropIdx.push_back(nMatch);
		//	//// TRACE("****Colour: %d %d %d, Airline:%s\n", GetRValue(pADPI->GetColor()), GetGValue(pADPI->GetColor()), GetBValue(pADPI->GetColor()), fds.icao_code);
		//}
		//else
		//	ASSERT(FALSE);

		//ASSERT(pADPI != NULL);



		
		nMatchARR = m_aircraftTags.FindBestMatch(fds,true, &GetTerminal());
		nMatchDep = m_aircraftTags.FindBestMatch(fds,false,&GetTerminal());

		m_vACTagPropIdx.push_back( std::pair<int,int>(nMatchARR,nMatchDep) );

		//CAircraftTagItem* pATI = NULL;
		//if(nMatch == -1) {//if no match, use default settings
		//	pATI = m_aircraftTags.GetDefaultTags();
		//	m_vACTagPropIdx.push_back(-1);
		//}
		//else if(nMatch>=0)
		//{
		//	pATI = m_aircraftTags.GetTagItem(nMatch);
		//	m_vACTagPropIdx.push_back(nMatch);
		//}
		//else
		//	ASSERT(FALSE);

		//ASSERT(pATI != NULL);


		m_vACShapeIdx.push_back( SHAPESMANAGER->GetACShapeIdxByACType(fds.acType) );
	}
	
	return TRUE;
}

//BOOL CTermPlanDoc::ReadPAXDescriptions(long* pnFIT, long* pnLOT)
//fills m_vPaxDispPropIdx with an index to a pax disp prop item for each pax desc
//fills m_vPaxTagPropIdx with an index to a pax tag item for each pax desc
//sets *pnFIT to First Pax In time
//sets *pnLOT to Last Pax Out time
BOOL CTermPlanDoc::ReadPAXDescriptions(long* pnFIT, long* pnLOT)
{
	*pnFIT = LONG_MAX;
	*pnLOT = -1;
	
	// clear logs
	clearLogsIfNecessary();
/*	
	bool bopenlogflg = true;
	try
	{
		GetTerminal().openLogs(m_ProjInfo.path);
	}
	catch( FileOpenError* pErr )
	{
		delete pErr;
		bopenlogflg = false;
//		return FALSE;
	}
	catch( StringError* pErr )
	{
		delete pErr;
		bopenlogflg = false;
//		return FALSE;
	}
	catch( FileVersionTooNewError* pErr )
	{
		delete pErr;
		bopenlogflg = false;
//		return FALSE;
	}

	//
	try
	{
		m_airsideSimLogs.OpenLogs(m_ProjInfo.path);
	}
	catch( FileOpenError* pErr )
	{
		delete pErr;

		if(!bopenlogflg)
			return FALSE;
	}
	catch( StringError* pErr )
	{
		delete pErr;

		if(!bopenlogflg)
			return FALSE;
	}
	catch( FileVersionTooNewError* pErr )
	{
		delete pErr;

		if(!bopenlogflg)
			return FALSE;
	}

*/
	//modify by hans 8.31
	if(GetTerminal().getCurrentSimResult() >=0)
	{
		GetTerminal().GetSimReportManager()->SetCurrentSimResult(0);
		GetTerminal().openLogs(m_ProjInfo.path);

	}
	
	GetAirsideSimLogs().OpenLogs(m_ProjInfo.path);

	long count = GetTerminal().paxLog->getCount();	
	
//	int nMatch;
	for(long i=0; i<count; i++) {
		MobLogEntry ple;
	    GetTerminal().paxLog->getItem(ple,i);		
		//MobDescStructEx pdsX;
		const MobDescStruct& pds = ple.GetMobDesc();
		if( pds.startTime < *pnFIT )
			*pnFIT = pds.startTime;

		if( pds.endTime > *pnLOT )
			*pnLOT = pds.endTime;		
	}

	LoadPaxDisplay();

	return TRUE;
}

/*
BOOL ReadPAXEvents(char* filename, PaxEventList& vPES)
{
	ifstream lf(filename, ios::in|ios::binary|ios::nocreate);

	if(lf.bad()) {
		// TRACE("ERROR, log file \"%s\" is bad\n", filename);
		return FALSE;
	}

	lf.seekg (0l, ios::end);
	long fileSize = lf.tellg();
	long count = fileSize/(long)sizeof(PaxEventStruct);

	if(count < 1)
		return FALSE;

	// TRACE("sizeof(PaxEventStruct): %d\n", sizeof(PaxEventStruct));
	// TRACE("Number of passenger event entries: %d\n", count);

	vPES.clear();
	vPES.reserve(count);

	lf.seekg (0l, ios::beg);
	for(long i=0; i<count; i++) {
		PaxEventStruct pes;
		lf.read((char *) &pes, sizeof(PaxEventStruct));
		pes.z = pes.z / SCALE_FACTOR;
		vPES.push_back(pes);
	}
	lf.close();
	return TRUE;
}
*/


//BOOL CTermPlanDoc::StartAnimation()
//-initializes performance counters
//-ensures valid sim results
//-builds pax display info data
//-requests animation time range from user
//-sets up views (2D/3D and/or Density)
//-plays the animation (start in pause mode)

BOOL CTermPlanDoc::StartAnimation(BOOL bGetTimeRangeFromUser)
{
	
#ifdef CODE_PERFOMANCE_DEBUGE_FLAG
	CCodeTimeTest::InitTools() ;
#endif
	ASSERT(m_eAnimState == anim_none);

	//initializes performance counters
	LARGE_INTEGER lVal;
	if(!QueryPerformanceFrequency(&lVal)) 
	{
		DWORD dwError = GetLastError();
		AfxMessageBox(_T("Your system does not support the high-resolution timers required for ARCport animation.\nPlease contact ARC tech support."), MB_OK|MB_ICONERROR);
		return FALSE;
	}

	m_animData.nPerfTickFreq = lVal;

	m_animData.m_AnimationModels.Reset();
	switch(m_systemMode)
	{
	case EnvMode_Terminal:
		m_animData.m_AnimationModels.SelTerminalModel();
		break;
	case EnvMode_AirSide:
		m_animData.m_AnimationModels.SelAirsideModel();
		break;
	case EnvMode_LandSide:
		m_animData.m_AnimationModels.SelLandsideModel();
		break;
	case EnvMode_OnBoard:
		m_animData.m_AnimationModels.SelOnBoardModel();
		break;
	default:
		return FALSE;
	}

	//Popup dialog,let user select the needed animations
	CDlgSelectedAnimationModels dlg(m_animData);
	if (dlg.DoModal() != IDOK)
		return FALSE;
	
	long nFirstInTime = LONG_MAX, nLastOutTime = -1;
	long nUserStartTime = LONG_MAX, nUserEndTime = -1;
	
	m_animData.m_startDate = GetTerminal().GetSimReportManager()->GetStartDate();

	if (m_animData.m_AnimationModels.IsTerminalSel())
	{
		long firstTime,endTime;
		if (!LoadTerminalLogs(firstTime,endTime))
		{
			AfxMessageBox("ERROR reading PAX Descriptions", MB_OK|MB_ICONERROR);
			return FALSE;
		}
		if (firstTime < endTime)
		{
			nFirstInTime = __min(nFirstInTime,firstTime);
			nLastOutTime = __max(nLastOutTime,endTime);

			nUserStartTime = __min(nUserStartTime,(long)GetTerminal().GetSimReportManager()->GetUserStartTime());
			nUserEndTime   = __max(nUserEndTime,(long)GetTerminal().GetSimReportManager()->GetUserEndTime());
		}
		if (!LoadAirsideLogs(firstTime,endTime))
		{
			AfxMessageBox("ERROR reading Aircraft Descriptions", MB_OK|MB_ICONERROR);
			return FALSE;
		}
		if (firstTime < endTime)
		{
			nFirstInTime = __min(nFirstInTime,firstTime);
			nLastOutTime = __max(nLastOutTime,endTime);
		}
	}

	if (m_animData.m_AnimationModels.IsAirsideSel())
	{
		long firstTime,endTime;
		if (!LoadAirsideLogs(firstTime,endTime))
		{
			AfxMessageBox("ERROR reading Aircraft Descriptions", MB_OK|MB_ICONERROR);
			return FALSE;
		}
		if (firstTime < endTime)
		{
			m_animData.m_startDate = GetTerminal().GetSimReportManager()->GetStartDate();
			nFirstInTime = __min(nFirstInTime,firstTime);
			nLastOutTime = __max(nLastOutTime,endTime);

			nUserStartTime = __min(nUserStartTime,nFirstInTime);
			nUserEndTime   = __max(nUserEndTime,nLastOutTime);
		}
		
	}

	if (m_animData.m_AnimationModels.IsLandsideSel())
	{		
		long firstTime,endTime;
		if (!LoadLandSideLogs(firstTime,endTime))
		{
			AfxMessageBox("ERROR reading Landside Descriptions", MB_OK|MB_ICONERROR);
			return FALSE;
		}
		if (firstTime < endTime)
		{
			m_animData.m_startDate = GetTerminal().GetSimReportManager()->GetStartDate();
			nFirstInTime = __min(nFirstInTime, firstTime);
			nLastOutTime = __max(nLastOutTime, endTime);

			nUserStartTime = nFirstInTime;
			nUserEndTime   = nLastOutTime;
		}

		if (!m_animData.m_AnimationModels.IsTerminalSel())
		{
			if (!LoadTerminalLogs(firstTime,endTime))
			{
				AfxMessageBox("ERROR reading PAX Descriptions", MB_OK|MB_ICONERROR);
				return FALSE;
			}
			if (firstTime < endTime)
			{
				nFirstInTime = __min(nFirstInTime,firstTime);
				nLastOutTime = __max(nLastOutTime,endTime);

				nUserStartTime = __min(nUserStartTime,(long)GetTerminal().GetSimReportManager()->GetUserStartTime());
				nUserEndTime   = __max(nUserEndTime,(long)GetTerminal().GetSimReportManager()->GetUserEndTime());
			}
		}

	}

	if (m_animData.m_AnimationModels.IsOnBoardSel())
	{
		long firstTime,endTime;
		if (!LoadTerminalLogs(firstTime,endTime))
		{
			AfxMessageBox("ERROR reading PAX Descriptions", MB_OK|MB_ICONERROR);
			return FALSE;
		}
		if (firstTime < endTime)
		{
			nFirstInTime = __min(nFirstInTime,firstTime);
			nLastOutTime = __max(nLastOutTime,endTime);

			nUserStartTime = __min(nUserStartTime,(long)GetTerminal().GetSimReportManager()->GetUserStartTime());
			nUserEndTime   = __max(nUserEndTime,(long)GetTerminal().GetSimReportManager()->GetUserEndTime());
		}
		if (!LoadAirsideLogs(firstTime,endTime))
		{
			AfxMessageBox("ERROR reading Aircraft Descriptions", MB_OK|MB_ICONERROR);
			return FALSE;
		}
		if (firstTime < endTime)
		{
			nFirstInTime = __min(nFirstInTime,firstTime);
			nLastOutTime = __max(nLastOutTime,endTime);
		}
	}



	if (nFirstInTime >= nLastOutTime)
	{
		AfxMessageBox("Simulation must run before animation can be displayed", MB_OK);
		return FALSE;
	}

	m_animData.nFirstEntryTime = nFirstInTime;
	m_animData.nLastExitTime   = nLastOutTime;

	m_animData.nAnimStartTime  = nUserStartTime;
	m_animData.nAnimEndTime    = nUserEndTime;//49*100*3600;//


	CMainFrame* pMF = GetMainFrame();
	pMF->SendMessage(WM_SETANIMATIONSPEED, (WPARAM) ID_ANIMATION_SET10SPEED, NULL);
	if(bGetTimeRangeFromUser) 
	{
		CDlgAnimationData dlg(this,m_animData);
		if(dlg.DoModal() == IDCANCEL) 
		{
			return FALSE;
		}
	}

	if(m_bMobileElementDisplay) 
	{
		StartAnimView();		
	}
	if(m_bActivityDensityDisplay) 
	{
		m_pADData = new CActivityDensityData();
		m_pADData->BuildData(m_animData,&GetTerminal(),m_adParams);
		CDensityGLView* pDAView = NULL;
		if((pDAView = GetDensityAnimView()) == NULL) 
		{
			pMF->CreateOrActivateFrame(theApp.m_pDensityAnimTemplate, RUNTIME_CLASS(CDensityGLView));
			pDAView = GetDensityAnimView();
		}
		ASSERT(pDAView != NULL);
	}

    // Flight information display enabled...
    if(m_bFlightInfoDisplay)
    {
        // Variables...
        CFIDSView  *pFIDSView   = NULL;

        // Flight information display has not been created already...
        if((pFIDSView = GetFIDSView()) == NULL)
        {
            // Create or activate the Flight Information Display frame...
            pMF->CreateOrActivateFrame(theApp.m_pFIDSAnimTemplate, RUNTIME_CLASS(CFIDSView));
            
            // Store Flight Information Display view object...
            pFIDSView = GetFIDSView();
        }

        // Failed to create or activate FIDS view...
        if(!pFIDSView)
        {
            // Alert user...
            AfxMessageBox(_T("Unable to create the Flight Information Display."), 
                          MB_OK | MB_ICONERROR);

            // Abort...
            return FALSE;
        }

    }
	
    // User did not enable any animation from the animation menu to display...
    if(!m_bActivityDensityDisplay && !m_bMobileElementDisplay && 
       !m_bFlightInfoDisplay)
    {
		// Alert user...
        AfxMessageBox(_T("No animation has been selected to run.\nCommand cancelled."), 
                      MB_OK | MB_ICONINFORMATION);
        
        // Abort...
		return FALSE;
	}
	ANIMTIMEMGR->EnrollDocument(this);

	pMF->SendMessage(UM_CHANGE_ANIM_TOOLBTN_STATE,1,ID_ANIMPLAY);

	m_eAnimState = anim_pause;

	pMF->SendMessage(UM_SETTOOLBTN_RUNDELTA_DELTA);

	m_animData.nLastAnimTime = m_animData.nAnimStartTime;
	VERIFY(QueryPerformanceCounter(&lVal));
	m_animData.nLastSystemTime = lVal;

	m_CallOutManger.GenerateCallOutData() ;	// generate callout data by define 
	ElapsedTime _time ;
	_time.setPrecisely(m_animData.nAnimStartTime) ;
	m_CallOutManger.SetAnimationBeginTime(_time);
	m_CallOutManger.ShowAllTheCallOutDialog() ; // create callout dialog

	ANIMTIMEMGR->StartTimer();


	return TRUE;
}

void CTermPlanDoc::OnAnimationStart() 
{
	//take off traces
	m_bShowTracers = TRUE;
	OnTracersOn();
	GetMainFrame()->GetMenu()->GetSubMenu(5)->CheckMenuItem(0,MF_UNCHECKED);

	if(m_eAnimState == anim_none)
	{ 
		//start animation
		if(StartAnimation(TRUE)) {
			m_eAnimState = anim_playF;
		}
		else
			m_eAnimState = anim_none;
		GetMainFrame()->m_wndAnimationBar.OnUpdateCmdUI(GetMainFrame(), FALSE);
	}
	else 
	{ //stop animation
		ANIMATIONSTATE eOldState = m_eAnimState;
		m_eAnimState = anim_pause;
		GetMainFrame()->m_wndAnimationBar.OnUpdateCmdUI(GetMainFrame(), FALSE);
		const CString s = "Are you sure you want to stop the animation?\n";
		if(AfxMessageBox(s,MB_YESNO | MB_ICONQUESTION) == IDYES) 
		{
			StopAnimation();
			m_CallOutManger.ShutDownAllTheCallOutDialog() ;
			UpdateAllViews(NULL);
		}
		else
			m_eAnimState = eOldState;
		GetMainFrame()->m_wndAnimationBar.OnUpdateCmdUI(GetMainFrame(), FALSE);
	}
}

void CTermPlanDoc::StopAnimation()
{
	GetTerminal().m_pLogBufManager->InitBuffer();
	//GetOutputAirside()->GetLogBuffer()->InitBuffer();
	m_eAnimState = anim_none;
	if(m_pADData) {
		delete m_pADData;
		m_pADData = NULL;
	}
	//UpdateAllViews(NULL);
	ANIMTIMEMGR->EnrollDocument(NULL);
	ANIMTIMEMGR->EndTimer();
	AfxGetMainWnd()->SendMessage(UM_CHANGE_ANIM_TOOLBTN_STATE,1,ID_ANIMATION_START);
	AfxGetMainWnd()->SendMessage(UM_SETTOOLBTN_RUNDELTA_RUN);

	if(m_bRecord) {
		BOOL bWriteWMV = (AfxMessageBox("Stopping the animation will stop your active recording.\n"
			"Do you want to store your recording as a WMV movie?\n", MB_YESNO|MB_ICONQUESTION) == IDYES);
		StopRecording(bWriteWMV);
	}	
	

	//GetMainFrame()->CreateOrActiveEnvModeView();

	//CRender3DView* pRender3DView = GetRender3DView();
	//if (pRender3DView && GetCurrentMode()!=EnvMode_LandSide)
	//{
	//	CRender3DFrame* pRender3DFrame = pRender3DView->GetParentFrame();
	//	pRender3DFrame->SendMessage(WM_SYSCOMMAND,SC_CLOSE,0);
	//}

	////if (RenderEngine::getSingleton().GetAlwaysInUse())
	//{
	//	CRender3DView* pRender3DView = GetRender3DView();
	//	if (pRender3DView)
	//	{
	//		CRender3DFrame* pRender3DFrame = pRender3DView->GetParentFrame();
	//		pRender3DFrame->SendMessage(WM_SYSCOMMAND,SC_CLOSE,0);		
	//	
	//		C3DView* p3DView = Get3DView();
	//		if (p3DView)
	//		{
	//			CChildFrame* pChildFrame = p3DView->GetParentFrame();
	//			if(pChildFrame->GetWorkingCamera(0,0)->GetProjectionType() != C3DCamera::perspective) 
	//			{
	//				//save current working view to 2D user view
	//				pChildFrame->SaveWorkingViews(&GetCurModeCameras().m_userView2D);
	//			}

	//			pChildFrame->LoadWorkingViews(&GetCurModeCameras().m_userView3D);
	//			pChildFrame->SaveWorkingViews(&GetCurModeCameras().m_userView3D);
	//			pChildFrame->ShowWindow(SW_SHOW);
	//		}
	//	}		
	//}
	
}

void CTermPlanDoc::OnAnimationStop() 
{
	ANIMATIONSTATE eOldState = m_eAnimState;
	m_eAnimState = anim_pause;
	GetMainFrame()->m_wndAnimationBar.OnUpdateCmdUI(GetMainFrame(), FALSE);
	const CString s = "Are you sure you want to stop the animation?\n";
	if(AfxMessageBox(s,MB_YESNO | MB_ICONQUESTION) == IDYES) 
	{
		StopAnimation();
		GetMainFrame()->CreateOrActiveEnvModeView();
		m_CallOutManger.ShutDownAllTheCallOutDialog() ;
		UpdateAllViews(NULL);

	}
	else
		m_eAnimState = eOldState;
	GetMainFrame()->m_wndAnimationBar.OnUpdateCmdUI(GetMainFrame(), FALSE);
}

void CTermPlanDoc::OnAnimationPause() 
{
	/*
	if(m_eAnimState == anim_pause)
		m_eAnimState = anim_playF;
	else
		m_eAnimState = anim_pause;
	*/
	//GetTerminal().m_pLogBufManager->InitBuffer();
	m_eAnimState = anim_pause;
	GetMainFrame()->m_wndAnimationBar.OnUpdateCmdUI(GetMainFrame(), FALSE);
}

void CTermPlanDoc::OnAnimationPlayF() 
{
//	GetTerminal().m_pLogBufManager->InitBuffer();
	//GetAirsideSimLogs().m_pLogBufManager->InitBuffer();

	//take off traces
	m_bShowTracers = TRUE;
	OnTracersOn();
	GetMainFrame()->GetMenu()->GetSubMenu(5)->CheckMenuItem(0,MF_UNCHECKED);

	m_eAnimState = anim_playF;
	GetMainFrame()->m_wndAnimationBar.OnUpdateCmdUI(GetMainFrame(), FALSE);
}

void CTermPlanDoc::OnAnimationPlayB() 
{
//	GetTerminal().m_pLogBufManager->InitBuffer();
	//GetAirsideSimLogs().m_pLogBufManager->InitBuffer();
	m_eAnimState = anim_playB;
	GetMainFrame()->m_wndAnimationBar.OnUpdateCmdUI(GetMainFrame(), FALSE);
}

void CTermPlanDoc::OnCtxNewArea() 
{
	CArea* pNewArea = new CArea();
	pNewArea->color = m_defDispProp.GetColorValue(CDefaultDisplayProperties::AreasColor);
	//bring up 3d window
	C3DView* p3DView = NULL;
	if((p3DView = Get3DView()) == NULL) 
	{
		GetMainFrame()->CreateOrActive3DView();
		p3DView = Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}
	CDlgAreaPortal dlg(this,CDlgAreaPortal::area, pNewArea, p3DView);
	if(dlg.DoFakeModal() == IDOK) 
	{
		pNewArea->floor = m_nActiveFloor;
		GetTerminal().m_pAreas->m_vAreas.push_back(pNewArea);

		CTVNode* pAllAreasNode =m_msManager.FindNodeByName(IDS_TVNN_AREAS);
		ASSERT(pAllAreasNode != NULL);
		CAreaNode* pNode = new CAreaNode(GetTerminal().m_pAreas->m_vAreas.size()-1);
		pAllAreasNode->m_eState = CTVNode::expanded;
		pNode->Name(pNewArea->name);
		pAllAreasNode->AddChild(pNode);
	
		UpdateRender3DObject(pNewArea->getGuid(), CAreaBase::getTypeGUID());
		UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*)pAllAreasNode);
		GetTerminal().m_pAreas->saveDataSet(m_ProjInfo.path, true);
	}
	else {
		delete pNewArea;
	}
	
}

void CTermPlanDoc::OnCtxNewPortal() 
{
	CPortal* pNewPortal = new CPortal();
	pNewPortal->color = m_defDispProp.GetColorValue(CDefaultDisplayProperties::PortalsColor);
	//bring up 3d window
	C3DView* p3DView = NULL;
	if((p3DView = Get3DView()) == NULL) 
	{
		GetMainFrame()->CreateOrActive3DView();
		p3DView = Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}
	CDlgAreaPortal dlg(this,CDlgAreaPortal::portal, pNewPortal, p3DView );
	if(dlg.DoFakeModal() == IDOK) {
		pNewPortal->floor = m_nActiveFloor;
		m_portals.m_vPortals.push_back(pNewPortal);
		CTVNode* pAllPortalsNode =m_msManager.FindNodeByName(IDS_TVNN_PORTALS);
		ASSERT(pAllPortalsNode != NULL);
		CPortalNode* pNode = new CPortalNode(m_portals.m_vPortals.size()-1);
		pAllPortalsNode->m_eState = CTVNode::expanded;
		pNode->Name(pNewPortal->name);
		pAllPortalsNode->AddChild(pNode);

		UpdateRender3DObject(pNewPortal);
		UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pAllPortalsNode);
		m_portals.saveDataSet(m_ProjInfo.path, true);
	}
	else {
		delete pNewPortal;
	}
}

void CTermPlanDoc::OnCtxEditArea()
{
	ASSERT(m_pSelectedNode->IsKindOf(RUNTIME_CLASS(CAreaNode)));
	int idx = ((CAreaNode*) m_pSelectedNode)->GetIdx();
	CArea* pArea = GetTerminal().m_pAreas->m_vAreas[idx];
	//bring up 3d window
	C3DView* p3DView = NULL;
	if((p3DView = Get3DView()) == NULL) 
	{
		GetMainFrame()->CreateOrActive3DView();
		p3DView = Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}
	CDlgAreaPortal dlg(this,CDlgAreaPortal::area, pArea, p3DView );
	if(dlg.DoFakeModal() == IDOK) {
		CTVNode* pAreasNode =m_msManager.FindNodeByName(IDS_TVNN_AREAS);
		ASSERT(pAreasNode != NULL);
		(pAreasNode->GetChildByIdx(idx))->Name(pArea->name);

		UpdateRender3DObject(pArea);
		UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pAreasNode);
		GetTerminal().m_pAreas->saveDataSet(m_ProjInfo.path, true);
	}
}

void CTermPlanDoc::OnCtxEditPortal() 
{
	ASSERT(m_pSelectedNode->IsKindOf(RUNTIME_CLASS(CPortalNode)));
	int idx = ((CPortalNode*) m_pSelectedNode)->GetIdx();
	CPortal* pPortal = m_portals.m_vPortals[idx];
	//bring up 3d window
	C3DView* p3DView = NULL;
	if((p3DView = Get3DView()) == NULL) 
	{
		GetMainFrame()->CreateOrActive3DView();
		p3DView = Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}
	CDlgAreaPortal dlg(this,CDlgAreaPortal::portal, pPortal, p3DView );
	if(dlg.DoFakeModal() == IDOK) {
		CTVNode* pPortalsNode =m_msManager.FindNodeByName(IDS_TVNN_PORTALS);
		ASSERT(pPortalsNode != NULL);
		(pPortalsNode->GetChildByIdx(idx))->Name(pPortal->name);

		UpdateRender3DObject(pPortal);
		UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pPortalsNode);
		m_portals.saveDataSet(m_ProjInfo.path, true);
	}
}

void CTermPlanDoc::OnCtxDeleteArea() 
{
	ASSERT(m_pSelectedNode->IsKindOf(RUNTIME_CLASS(CAreaNode)));
	if(AfxMessageBox(_T("Are you sure?"), MB_YESNO | MB_ICONQUESTION) == IDYES) 
	{
		int idx = ((CAreaNode*) m_pSelectedNode)->GetIdx();
		CArea* pArea = GetTerminal().m_pAreas->m_vAreas[idx];
		CAreaList& areas = GetTerminal().m_pAreas->m_vAreas;
		CAreaList::iterator iteDel = areas.begin() + idx;
		const CGuid guid = (*iteDel)->getGuid();
		delete pArea;
		DeleteAreaObject(idx);
		areas.erase(iteDel);
		UpdateRender3DObject(guid, CAreaBase::getTypeGUID());
		CTVNode* pAreasNode =m_msManager.FindNodeByName(IDS_TVNN_AREAS);
		ASSERT(pAreasNode != NULL);
		pAreasNode->RemoveChild(m_pSelectedNode);
		for(int i=0; i<pAreasNode->GetChildCount(); i++) 
		{
			int oldIdx = ((CAreaNode*)pAreasNode->GetChildByIdx(i))->GetIdx();
			if(oldIdx > idx)
				((CAreaNode*)pAreasNode->GetChildByIdx(i))->SetIdx(oldIdx-1);
		}
		UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pAreasNode);
		GetTerminal().m_pAreas->saveDataSet(m_ProjInfo.path, true);

		CString strPath = m_ProjInfo.path + "\\INPUT";
		CCongestionAreaManager& dataset = m_arcport.m_congManager;
		INTVECTOR* pAreas = dataset.GetAreasVect();

		int j = 0;
		for(j = 0; j < static_cast<int>(pAreas->size()); j++){
			if(idx == pAreas->at(j)){
				pAreas->erase(pAreas->begin()+j);
				break;
			}
		}
		for(j = 0; j < static_cast<int>(pAreas->size()); j++){
			if(idx < pAreas->at(j)){
				pAreas->at(j)--;
			}
		}
		dataset.saveDataSet(strPath, FALSE);
	}
}

void CTermPlanDoc::OnCtxDeletePortal() 
{
	ASSERT(m_pSelectedNode->IsKindOf(RUNTIME_CLASS(CPortalNode)));
	if(AfxMessageBox(_T("Are you sure?"), MB_YESNO | MB_ICONQUESTION) == IDYES) 
	{
		int idx = ((CPortalNode*) m_pSelectedNode)->GetIdx();
		CPortal* pPortal = m_portals.m_vPortals[idx];
		CPortalList& portals = m_portals.m_vPortals;
		CPortalList::iterator iteDel = portals.begin() + idx;
		const CGuid guid = (*iteDel)->getGuid();
		delete pPortal;
		DeletePortalObject(idx);
		portals.erase(iteDel);
		UpdateRender3DObject(guid, CPortal::getTypeGUID());
		CTVNode* pPortalsNode =m_msManager.FindNodeByName(IDS_TVNN_PORTALS);
		ASSERT(pPortalsNode != NULL);
		pPortalsNode->RemoveChild(m_pSelectedNode);
		for(int i=0; i<pPortalsNode->GetChildCount(); i++) 
		{
			int oldIdx = ((CPortalNode*)pPortalsNode->GetChildByIdx(i))->GetIdx();
			if(oldIdx > idx)
				((CPortalNode*)pPortalsNode->GetChildByIdx(i))->SetIdx(oldIdx-1);
		}
		UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pPortalsNode);
		m_portals.saveDataSet(m_ProjInfo.path, true);
	}
}

void CTermPlanDoc::OnCtxAreaColor() 
{
	ASSERT(m_pSelectedNode->IsKindOf(RUNTIME_CLASS(CAreaNode)));
	int idx = ((CAreaNode*) m_pSelectedNode)->GetIdx();
	CNamedPointList* pNPL = GetTerminal().m_pAreas->m_vAreas[idx];
	CColorDialog colorDlg(pNPL->color, CC_ANYCOLOR | CC_FULLOPEN);
	if(colorDlg.DoModal() == IDOK) {
		pNPL->color = colorDlg.GetColor();
	}
}

void CTermPlanDoc::OnCtxPortalColor() 
{
	ASSERT(m_pSelectedNode->IsKindOf(RUNTIME_CLASS(CPortalNode)));
	int idx = ((CPortalNode*) m_pSelectedNode)->GetIdx();
	CNamedPointList* pNPL = m_portals.m_vPortals[idx];
	CColorDialog colorDlg(pNPL->color, CC_ANYCOLOR | CC_FULLOPEN);
	if(colorDlg.DoModal() == IDOK) {
		pNPL->color = colorDlg.GetColor();
	}
}

void CTermPlanDoc::OnCTXHelp()
{
	if (m_pSelectedNode == NULL)
		return;	
	ASSERT(m_pSelectedNode->IsKindOf(RUNTIME_CLASS(CTVNode)));
	switch(m_pSelectedNode->m_nIDResource)
	{
	case IDR_CTX_FLOORS:
	case IDR_CTX_FLOOR:
		HtmlHelp(NULL,"ARCTerm.chm::/NV_Floors.htm",HH_DISPLAY_TOPIC,0);
		break;
	case IDR_CTX_PLACEMENTS:
		HtmlHelp(NULL,"ARCTerm.chm::/NV_Placements.htm",HH_DISPLAY_TOPIC,0);
		break;
	case IDR_CTX_PROCESSOR :
		HtmlHelp(NULL,"ARCTerm.chm::/NV_Each Processor in Placements.htm",HH_DISPLAY_TOPIC,0);
		break;
	case IDR_CTX_PORTALS:
		HtmlHelp(NULL,"ARCTerm.chm::/NV_Portals.htm",HH_DISPLAY_TOPIC,0);
		break;
	case IDR_DEFINE_PIPE:
		HtmlHelp(NULL,"ARCTerm.chm::/NV_Pipe Definition.htm",HH_DISPLAY_TOPIC,0);
		break;
	case IDR_CTX_AREAS:
		HtmlHelp(NULL,"ARCTerm.chm::/NV_Areas.htm",HH_DISPLAY_TOPIC,0);
		break;
	case IDR_RAILWAY_LAYOUT:
		HtmlHelp(NULL,"ARCTerm.chm::/NV_Railway System.htm",HH_DISPLAY_TOPIC,0);
		break;
	case IDR_CTX_DEFAULT:
		HtmlHelp(NULL,"ARCTerm.chm::/NV_Layout.htm",HH_DISPLAY_TOPIC,0);
		break;
	case IDR_CTX_NAMELIST:
		HtmlHelp(NULL,"ARCTerm.chm::/NV_Name List.htm",HH_DISPLAY_TOPIC,0);
		break;
	}
}

void CTermPlanDoc::OnSetDefaultAreaColor()  
{
	ASSERT(m_pSelectedNode->IsKindOf(RUNTIME_CLASS(CTVNode)));
	CColorDialog colorDlg(m_defDispProp.GetColorValue(CDefaultDisplayProperties::AreasColor), CC_ANYCOLOR | CC_FULLOPEN);
	if(colorDlg.DoModal() == IDOK) {
		m_defDispProp.SetColorValue(CDefaultDisplayProperties::AreasColor, colorDlg.GetColor());
	}	
}

void CTermPlanDoc::OnSetDefaultPortalColor() 
{
	ASSERT(m_pSelectedNode->IsKindOf(RUNTIME_CLASS(CTVNode)));
	CColorDialog colorDlg(m_defDispProp.GetColorValue(CDefaultDisplayProperties::PortalsColor), CC_ANYCOLOR | CC_FULLOPEN);
	if(colorDlg.DoModal() == IDOK) {
		m_defDispProp.SetColorValue(CDefaultDisplayProperties::PortalsColor, colorDlg.GetColor());
	}	
}

void CTermPlanDoc::OnSetDefaultProcDispProperties() 
{
	ASSERT(m_pSelectedNode->IsKindOf(RUNTIME_CLASS(CTVNode)));
	CProcessor2* pProc = new CProcessor2();
	//assign default display properties.
	GetDefaultProcDispProperties(&m_defDispProp, &(pProc->m_dispProperties));
	// show processor display properties dialog
	CPROCESSOR2LIST vList;
	vList.push_back(pProc);
	CDlgProcDisplayProperties dlg(&vList);
	if(dlg.DoModal() == IDOK) {
		m_defDispProp.SetBoolValue(CDefaultDisplayProperties::ProcShapeOn,dlg.m_nDisplay[PDP_DISP_SHAPE]);
		m_defDispProp.SetBoolValue(CDefaultDisplayProperties::ProcServLocOn,dlg.m_nDisplay[PDP_DISP_SERVLOC]);
		m_defDispProp.SetBoolValue(CDefaultDisplayProperties::ProcQueueOn,dlg.m_nDisplay[PDP_DISP_QUEUE]);
		m_defDispProp.SetBoolValue(CDefaultDisplayProperties::ProcInConstraintOn,dlg.m_nDisplay[PDP_DISP_INC]);
		m_defDispProp.SetBoolValue(CDefaultDisplayProperties::ProcOutConstraintOn,dlg.m_nDisplay[PDP_DISP_OUTC]);
		m_defDispProp.SetBoolValue(CDefaultDisplayProperties::ProcNameOn,dlg.m_nDisplay[PDP_DISP_PROCNAME]);
		m_defDispProp.SetColorValue(CDefaultDisplayProperties::ProcShapeColor,dlg.m_cColor[PDP_DISP_SHAPE]);
		m_defDispProp.SetColorValue(CDefaultDisplayProperties::ProcServLocColor,dlg.m_cColor[PDP_DISP_SERVLOC]);
		m_defDispProp.SetColorValue(CDefaultDisplayProperties::ProcQueueColor,dlg.m_cColor[PDP_DISP_QUEUE]);
		m_defDispProp.SetColorValue(CDefaultDisplayProperties::ProcInConstraintColor,dlg.m_cColor[PDP_DISP_INC]);
		m_defDispProp.SetColorValue(CDefaultDisplayProperties::ProcOutConstraintColor,dlg.m_cColor[PDP_DISP_OUTC]);
		m_defDispProp.SetColorValue(CDefaultDisplayProperties::ProcNameColor,dlg.m_cColor[PDP_DISP_PROCNAME]);	
	}
	delete pProc;
}

CTVNode* CTermPlanDoc::ProcessorsNode()
{
	CTVNode* pNode =m_msManager.FindNodeByName(IDS_TVNN_PLACEMENTS);
	//ASSERT(pNode != NULL);
	return pNode;
}

CTVNode* CTermPlanDoc::UnderConstructionNode()
{
	CTVNode* pNode = NULL;
	if(m_systemMode == EnvMode_Terminal)
		pNode = m_msManager.FindNodeByName(IDS_TVNN_UNDERCONSTRUCTION);
	else if(m_systemMode == EnvMode_AirSide)
		pNode = m_msManager.FindNodeByName(IDS_TVNN_STRUCTURES);
	//can be NULL since this node will not appear when no under construction processors exist
	return pNode;
}

CTVNode* CTermPlanDoc::FloorsNode()
{	
	CTVNode* pNode = m_msManager.FindNodeByName(IDS_TVNN_FLOORS);
	if(m_systemMode == EnvMode_LandSide)
	{
		pNode = m_msManager.FindNodeByName(IDS_TVNN_LEVELS);
	}

	ASSERT(pNode != NULL);
	return pNode;
}

CTVNode* CTermPlanDoc::LayoutNode()
{
	CTVNode* pNode = m_msManager.FindNodeByName(IDS_TVNN_LAYOUT);
	ASSERT(pNode != NULL);
	return pNode;
}

BOOL CTermPlanDoc::AddToUnderConstructionNode(CProcessor2* pProc)
{
	CTVNode* pLayout = LayoutNode();
	CTVNode* pUCParent = UnderConstructionNode();
	if(pUCParent == NULL) {
		CTVNode* pFloors = FloorsNode();
		CString s;
		s.LoadString(IDS_TVNN_UNDERCONSTRUCTION);
		pUCParent = new CTVNode(s);
		pLayout->AddChildAfter(pUCParent, pFloors);
		pUCParent->m_eState = CTVNode::expanded;
		UpdateAllViews(NULL, NODE_HINT_NEWNODE, (CObject*) pUCParent);
	}
	CString sName;
	if(pProc->GetShape() != NULL)
	{
		int nIndex = 0;
		CTVNode* pNode = NULL;
		CTVNode* pTempNode = NULL;
		int idLength = pProc->ShapeName().idLength();
		CString strNodeName = _T("");

		pNode = pUCParent;
		for (nIndex; nIndex < idLength - 1; nIndex++)
		{
			pTempNode = pNode;
			strNodeName = pProc->ShapeName().at(nIndex).c_str();
			sName.Format(_T("%s"),strNodeName);
			sName.MakeUpper();
			pNode = (CTVNode*)pNode->GetChildByName(sName);
			if (pNode == NULL)
			{
				pNode = new CTVNode(sName);
				pTempNode->AddChild(pNode);
			}
		}
		strNodeName = pProc->ShapeName().at(idLength-1).c_str();
		sName.Format(_T("%s"),strNodeName);
		CTVNode* pChildNode = new CTVNode(sName,IDR_CTX_UCPROCESSOR);
		pNode->AddChild(pChildNode);
		pChildNode->m_dwData = (DWORD) pProc;
	}
	else
	{
		sName.Format("%s", "undefined", GetCurrentPlacement()->m_vUndefined.size());
		CTVNode* pUCProcessorNode = new CTVNode(sName, IDR_CTX_UCPROCESSOR);
		pUCParent->AddChild(pUCProcessorNode);
		pUCProcessorNode->m_eState = CTVNode::expanded;
		pUCProcessorNode->m_dwData = (DWORD)pProc;
	}
	

	UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pUCParent);
	return TRUE;
}

BOOL CTermPlanDoc::MoveFromUCToDefined(CProcessor2* pProc)
{
	//find ucnode with pProc as dwData
	CTVNode* pUCParent = UnderConstructionNode();
	//ASSERT(pUCParent != NULL);
	//get node with dwData == pProc
	CTVNode* pLayoutNode = (CTVNode*)pUCParent->Parent();
	if (pUCParent)
	{
		CTVNode* tvNode = NULL;
		std::deque<CTVNode*> vList;
		vList.push_front(pUCParent);
		while(vList.size() != 0) 
		{
			CTVNode* pNode = vList[0];
			vList.pop_front();
			if(pProc == (CProcessor2*)(pNode->m_dwData))
			{
				tvNode = pNode;
				break;
			}
			for(int i=0; i<pNode->GetChildCount(); i++) 
			{
				vList.push_front((CTVNode*) pNode->GetChildByIdx(i));
			}
		}

		if (tvNode != NULL)
		{
			CTVNode* pParentNode = (CTVNode*)(tvNode->Parent());
			CTVNode* pNode = NULL;
			pParentNode->RemoveChild(tvNode);
			while (pParentNode != pLayoutNode)
			{
				if (pParentNode->GetChildCount() == 0)
				{
					pNode = pParentNode;
					pParentNode =(CTVNode*) pParentNode->Parent();
					pParentNode->RemoveChild(pNode);
				}
				else
				{
					break;
				}
			}
		}
	}

	//now rebuild defined hierarchy
	if (m_systemMode == EnvMode_Terminal)
	{
		ProcessorID id = *(pProc->GetProcessor()->getID());
		
		std::stack<CString> vList;
		int nLength = id.idLength();
		for (int i = 0; i < nLength; i++)
		{
			vList.push(id.GetLeafName());
			id.clearLevel(id.idLength() - 1);
		}

		int nIndex = 0;
		CTVNode* pNode = NULL;
		CTVNode* pTempNode = NULL;
		int idLength = id.idLength();
		CString strNodeName = _T("");

		pNode = ProcessorsNode();
		pNode->m_eState = CTVNode::expanded;
		while(vList.size())
		{
			pTempNode = pNode;
			strNodeName = vList.top();
			vList.pop();
			pNode = (CTVNode*)pNode->GetChildByName(strNodeName);
			if (pNode == NULL)
			{
				if (vList.size() == 0)
				{
					CTVNode* pChildNode = new CTVNode(strNodeName,IDR_CTX_PROCESSOR);
					pTempNode->AddChild(pChildNode);
					pChildNode->m_dwData = (DWORD) pProc;
				}
				else
				{
					pNode = new CTVNode(strNodeName);
					pTempNode->AddChild(pNode);
					if (pTempNode == ProcessorsNode())
					{
						pNode->m_nIDResource = IDR_CTX_PROCESSORGROUP_WITH_CHANGETYPE;
					}
					else
					{
						pNode->m_nIDResource = IDR_CTX_PROCESSORGROUP;
					}
				}
			}	
		}
		
	}
	else if (m_systemMode == EnvMode_AirSide)
		m_msManager.m_msImplAirSide.AddProcessorGroup(pProc->GetProcessor()->getProcessorType());

	//refresh layout node
	UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pLayoutNode);

	return TRUE;
}

void CTermPlanDoc::SelectNode(CTVNode* pNode)
{
	m_pSelectedNode = pNode;
	if(!m_pSelectedNode)
		return;
	//special cases:
	//if node is a processor (leaf of placements), then select the processor
	if(m_pSelectedNode->m_nIDResource == IDR_CTX_PROCESSOR || m_pSelectedNode->m_nIDResource == IDR_CTX_UCPROCESSOR || m_pSelectedNode->m_nIDResource == IDR_MENU_CONTOUR) {
		SelectProc(((CProcessor2*)m_pSelectedNode->m_dwData), FALSE);
	}
	//if processor group then select all child processors
	else if(m_pSelectedNode->m_nIDResource == IDR_CTX_PROCESSORGROUP 
		|| m_pSelectedNode->m_nIDResource == IDR_CTX_PROCESSORGROUP_WITH_CHANGETYPE) {
		//build proc group id
		CTVNode* pNode = m_pSelectedNode;
		CString sID;
		while(pNode->m_nIDResource == IDR_CTX_PROCESSORGROUP
			|| pNode->m_nIDResource == IDR_CTX_PROCESSORGROUP_WITH_CHANGETYPE) {
			if(sID.IsEmpty())
				sID = pNode->Name() + sID;
			else
				sID = pNode->Name() + "-" + sID;
			pNode = (CTVNode*) pNode->Parent();
		}
		//now we have the id
	ProcessorID id;
		id.SetStrDict(GetTerminal().inStrDict);
		id.setID(sID);
		setSelectProcID(id);
		//get group index
		GroupIndex gidx = GetProcessorList().getGroupIndex(id);
		//select each proc in group
		if(gidx.start == -1 || gidx.end == -1)
			return;
		UnSelectAllProcs();
		for(int i=gidx.start; i<=gidx.end; i++) {
			Processor* proc = GetProcessorList().getProcessor(i);
			ASSERT(proc != NULL);
			SelectProc(GetProcessor2FromID(*(proc->getID())), TRUE);
		}
	}
	else if(m_pSelectedNode->m_nIDResource == IDR_CTX_PLACEMENTS) {
		UnSelectAllProcs();
	}else if(m_pSelectedNode->m_nIDResource==IDR_CTX_STRUCTURE){
		//select the surfaceArea;
		C3DView *pview=Get3DView();
		if(pview!=NULL)
			pview->setSelectStructure((CStructure *)m_pSelectedNode->m_dwData);
	}else if(m_pSelectedNode->m_nIDResource == IDR_CTX_WALLSHAPE){
		C3DView *pview=Get3DView();
		if(pview!=NULL)
			pview->setSelectWallShape((WallShape *)m_pSelectedNode->m_dwData);
	}
	/*else if( m_pSelectedNode->m_nIDResource ==  IDR_CTX_LANDPROCESSOR )
	{
		Processor* pProc =((CLandProcessor2*)m_pSelectedNode->m_dwData )->GetProcessor();
		GetLandsideDoc()->SelectProcessor( GetProcessor2FromID( *(pProc->getID())) ,false ) ;
	}*/
	

}

BOOL CTermPlanDoc::RenameProcessor(ProcessorID& id, LPCTSTR sNewName)
{
	return FALSE;
}

BOOL CTermPlanDoc::RenameProcessorGroup(ProcessorID& id, int nLevel, LPCTSTR sNewName)
{

	if(!GetTerminal().ProcessorRename(id,sNewName,m_ProjInfo.path))
		return FALSE;
//	for(int i=gidx.start; i<=gidx.end; i++) {
//		Processor* proc = GetTerminal().procList->getProcessor(i);
//		ASSERT(proc != NULL);
		//proc->getID()->setNameAtLevel(sNewName, nLevel);
//	}
//	GetTerminal().procList->saveDataSet(m_ProjInfo.path, true);
	return TRUE;
}

BOOL CTermPlanDoc::ActivateFloor(int nLevel)
{
	ASSERT(nLevel >= 0 && nLevel < static_cast<int>(GetCurModeFloor().GetCount()));
	m_nLastActiveFloor = m_nActiveFloor;
	if(nLevel == m_nActiveFloor)
		return TRUE;
	if( EnvMode_AirSide == m_systemMode)
		m_nActiveFloor = 0;
	else
		m_nActiveFloor = nLevel;

	if(m_systemMode==EnvMode_Terminal)
	{
		CTVNode* pFloorsNode = FloorsNode();
		for(int i=0; i<pFloorsNode->GetChildCount(); i++) {
			CTVNode* pFloorNode = (CTVNode*) pFloorsNode->GetChildByIdx(i);
			CFloor2* pFloor = GetCurModeFloor().GetFloor2(pFloorNode->m_dwData);
			if(pFloorNode->m_dwData == nLevel) {
				pFloorNode->m_iImageIdx = ID_NODEIMG_FLOORACTIVE;
				pFloor->IsActive(TRUE);
			}
			else {
				pFloorNode->m_iImageIdx = ID_NODEIMG_FLOORNOTACTIVE;
				pFloor->IsActive(FALSE);
			}
		}
	}
	if(m_systemMode==EnvMode_LandSide)
	{
		CTVNode* pFloorsNode = FloorsNode();
		for(int i=0; i<pFloorsNode->GetChildCount(); i++) {
			CTVNode* pFloorNode = (CTVNode*) pFloorsNode->GetChildByIdx(i);
			CFloor2* pFloor = GetCurModeFloor().GetFloor2(pFloorNode->m_dwData);
			if(pFloorNode->m_dwData == nLevel) {
				pFloorNode->m_iImageIdx = ID_NODEIMG_FLOORACTIVE;
				pFloor->IsActive(TRUE);
			}
			else {
				pFloorNode->m_iImageIdx = ID_NODEIMG_FLOORNOTACTIVE;
				pFloor->IsActive(FALSE);
			}
		}
	}
	if(m_systemMode!=EnvMode_LandSide)
	{
		C3DView* p3DView = Get3DView();
		if(p3DView)
		{
			p3DView->GetCamera()->AutoAdjustCameraFocusDistance(GetActiveFloorAltitude());
		}
	}
	ActiveFloorInRender3DView(GetActiveFloor());
	return TRUE;
}

void CTermPlanDoc::OnUpdateActivateFloor(CCmdUI* pCmdUI)
{
	ASSERT(m_pSelectedNode!=NULL /*&& m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR*/);
	if(!m_pSelectedNode)
		return;
	CFloor2* pFloor = GetCurModeFloor().GetActiveFloor();//GetCurModeFloor().getac((int)m_pSelectedNode->m_dwData);
	if(pFloor)
	{
		BOOL bIsActive = pFloor->Level() == (int)m_pSelectedNode->m_dwData;
		pCmdUI->Enable(!bIsActive);
	}
}

void CTermPlanDoc::OnActivateFloor()
{
	ASSERT(m_pSelectedNode!=NULL /*m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR*/);
	GetCurModeFloor().ActiveFloor((int)m_pSelectedNode->m_dwData);

	CFloor2* pFloor = GetCurModeFloor().GetFloor2((int)m_pSelectedNode->m_dwData);
	ActivateFloor(pFloor->Level()); 
	GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
	UpdateViewSharpTextures();
	CTVNode* pFloorsNode = FloorsNode();

	CMainFrame* pMainFrm = GetMainFrame();
	CComboBox* pCB = pMainFrm->m_wndLayoutBar.GetActiveFloorCB();
	pCB->SetCurSel(GetCurModeFloor().GetCount() - 1 - m_nActiveFloor);

	if(EnvMode_Terminal==m_systemMode)
        UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) (CTVNode*) pFloorsNode);
	else
		UpdateAllViews(NULL, VM_ACTIVE_ONE_FLOOR,(CObject*) pFloor);
}

UINT CTermPlanDoc::GetUniqueNumber()
{
	return m_uUniqueNumber++;
}

void CTermPlanDoc::OnAnimationPickTime()
{
	ANIMATIONSTATE eOldState = m_eAnimState;
	m_eAnimState = anim_pause;
	GetMainFrame()->m_wndAnimationBar.OnUpdateCmdUI(GetMainFrame(), FALSE);
	CDlgPickTime dlg;
	dlg.SetDateTime(m_animData.m_startDate,m_animData);
	//dlg.SetDateTime( m_animData.m_startDate, ElapsedTime( (long)(m_animData.nLastAnimTime / 100) ) );
	if(dlg.DoModal() == IDOK) 
	{	
		m_CallOutManger.ShutDownAllTheCallOutDialog() ;

		m_animData.nLastAnimTime = dlg.GetTime();
	//	GetTerminal().m_pLogBufManager->InitBuffer();
		ElapsedTime _time ;
		_time.setPrecisely(m_animData.nLastAnimTime ) ;
		m_CallOutManger.SetAnimationBeginTime(_time) ;
		m_CallOutManger.ShowAllTheCallOutDialog() ;
		//GetAirsideSimLogs().m_pLogBufManager->InitBuffer();
	}
	m_eAnimState = eOldState;
	GetMainFrame()->m_wndAnimationBar.OnUpdateCmdUI(GetMainFrame(), FALSE);
}
/*
void CTermPlanDoc::OnAnimationModeSwitch()
{
	if(m_eAnimState != anim_viewmanip) {
		m_eAnimState = anim_viewmanip;
	}
	else {
		C3DView* p3DView = NULL;
		if((p3DView = Get3DView()) == NULL) {
			GetMainFrame()->CreateOrActivateFrame(theApp.m_p3DTemplate, RUNTIME_CLASS(C3DView));
			p3DView = Get3DView();
		}
		ASSERT(p3DView != NULL);
		p3DView->GenerateAnimBKTexture();
		m_eAnimState = anim_playF;
		QueryPerformanceCounter(&m_animData.nLastSystemTime);
	}
}
*/
/*
void CTermPlanDoc::OnUpdateAnimButtons(CCmdUI* pCmdUI)
{
	if(pCmdUI->m_nID == ID_ANIMMODESWITCH || pCmdUI->m_nID == ID_ANIMPICKSPEED) {
		pCmdUI->Enable(TRUE);
		return;
	}
	BOOL bSetCheck;
	if(m_eAnimState == anim_none) {
		pCmdUI->SetCheck(0);
		pCmdUI->Enable(FALSE);
		return;
	}
	switch(pCmdUI->m_nID) {
	case ID_ANIMPLAY:
		bSetCheck = (m_eAnimState == anim_playF);
		if(bSetCheck)
			pCmdUI->SetCheck(1);
		else
			pCmdUI->SetCheck(0);
		break;
	case ID_ANIMREV:
		bSetCheck = (m_eAnimState == anim_playB);
		if(bSetCheck)
			pCmdUI->SetCheck(1);
		else
			pCmdUI->SetCheck(0);
		break;
	case ID_ANIMPAUSE:
		bSetCheck = (m_eAnimState == anim_pause);
		if(bSetCheck)
			pCmdUI->SetCheck(1);
		else
			pCmdUI->SetCheck(0);
		break;
	case ID_ANIMPICKTIME:
	case ID_ANIMSTOP:
		pCmdUI->SetCheck(0);
		break;
	}
	pCmdUI->Enable(TRUE);
}
*/


void CTermPlanDoc::OnCtxProcDispPropOverride() 
{
	CDlgProcDispOverrides dlg(m_displayoverrides.m_pbProcDisplayOverrides);
	if(dlg.DoModal() == IDOK) {
		for(int i=PDP_MIN; i<=PDP_MAX; i++) {
			m_displayoverrides.m_pbProcDisplayOverrides[i] = dlg.m_bDisplay[i];
		}
		m_displayoverrides.saveDataSet(m_ProjInfo.path, false);
		UpdateAllViews(NULL);
	}
}

void CTermPlanDoc::OnUpdateCtxNewFloor(CCmdUI* pCmdUI) 
{
	// comment by bird 2003/10/9, to can add any floor( not any 4 layer )
	/*
	if(m_floors.GetCount() < 4)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
	*/
}
static CStatusBarXP* _pStatusBar  = NULL;
static void CALLBACK CallBackSaveAsProject(LPCTSTR strPath)
{
	CString fileName;
	CString strFilePath = CString(strPath);
	int nPos = strFilePath.ReverseFind('\\');
	fileName = strFilePath.Mid(nPos + 1);
	CString strMsg;
	strMsg = _T("Copying   ") + fileName;

	_pStatusBar->SetPaneText(0,strMsg);	
}

void CTermPlanDoc::OnFileSaveas() 
{
	// TODO: Add your command handler code here
	// TODO: Add your command handler code here
	BOOL bSaveAs = FALSE;
	CDlgProjectName dlg( m_ProjInfo.name, false, m_ProjInfo.lUniqueIndex );
	if(IDOK!=dlg.DoModal())
	{
		return;
	}

	CString projDir = PROJMANAGER->m_csRootProjectPath + _T("\\") + dlg.m_strProjectName;
	
	_pStatusBar = &(GetMainFrame()->m_wndStatusBar);

	::CoInitialize(NULL);

	IProgressDialog *pProgressDialog;

	CoCreateInstance( CLSID_ProgressDialog,
		NULL,
		CLSCTX_ALL,
		IID_IProgressDialog,
		(LPVOID*)&pProgressDialog);
	pProgressDialog->SetTitle(L"Copy files...");
	pProgressDialog->SetCancelMsg(L"Cancelling...",NULL);
	pProgressDialog->SetAnimation(AfxGetResourceHandle(),IDR_COPYPROGRESS);
	pProgressDialog->StartProgressDialog(NULL, NULL, PROGDLG_NOMINIMIZE, NULL);
	CString strNote = _T("Copying project files");	
	pProgressDialog->SetLine(1,strNote.AllocSysString(),FALSE,NULL);
	CopyFileParam copyParam;
	copyParam.pProgressDialog = pProgressDialog;
	//now the project has 1144 files, if the file count changed,the value must be changed
	int nFileCount = 0;
	FileManager::GetFileCountInDir(m_ProjInfo.path,nFileCount);
	copyParam.nTotalFileCount = nFileCount;
	copyParam.nCurCopyFile = 0;
	copyParam.strDestDir = projDir;

	//export database files
	
	//CImportExportManager::ExportProject(m_ProjInfo.path,m_ProjInfo.name);


	//close project connection

	CMasterDatabase *pMasterDatabase = ((CTermPlanApp *)AfxGetApp())->GetMasterDatabase();
	CProjectDatabase projDatabase(PROJMANAGER->GetAppPath());
	projDatabase.CloseProject(pMasterDatabase,m_ProjInfo.name);
	bSaveAs = PROJMANAGER->CopyProject( dlg.m_strProjectName, projDir, m_ProjInfo,&copyParam);
	//open project connection
	projDatabase.OpenProject(pMasterDatabase,m_ProjInfo.name,m_ProjInfo.path);

	CString strCurrentDBName;
	pMasterDatabase->GetProjectDatabaseNameFromDB(m_ProjInfo.name,strCurrentDBName);
	pMasterDatabase->SetImportedProjectName(m_ProjInfo.name,strCurrentDBName);

	//copy folder with progress
	if (pProgressDialog)
	{
	
		pProgressDialog->StopProgressDialog();

		pProgressDialog->Release();
	}
	CoUninitialize();
	// copy project
//	PROJMANAGER->CopyProject( dlg.m_strProjectName, projDir, m_ProjInfo,CallBackSaveAsProject);
	

	// close project.
//	((CTermPlanApp*)AfxGetApp())->OnCloseProject();

	// open project.
	if(bSaveAs)
	{
		OnCloseDocument();

		//CImportExportManager::ImportProject(projDir,dlg.m_strProjectName);
		((CTermPlanApp*)AfxGetApp())->OpenDocumentFile( projDir );

	}

	GetMainFrame()->UpdateProjSelectBar();
	
}


//For StandProc convert
#include "InputAirside/stand.h"
class IsEqualProcessor
{
public:
	IsEqualProcessor(const Processor* pProc): m_pProcessor(pProc){}
	bool operator()(const CProcessor2* pProc2){
		return m_pProcessor->getID() == pProc2->GetProcessor()->getID();}
private:
	const Processor* m_pProcessor;
};

typedef LPDISPATCH(*GETCNNPTR)();
typedef void(*SETDATABASE)();
BOOL CTermPlanDoc::ReloadInputData()
{
	try
	{
		GetTerminal().m_nProjID = GetProjectID();
		m_arcport.m_pInputLandside->ReloadInputData(m_ProjInfo.path,GetProjectID());

		GetTerminal().m_pPipeDataSet->SetLandsideInput(m_arcport.m_pInputLandside);
		GetTerminal().loadInputs( m_ProjInfo.path );
		GetInputOnboard()->GetOnBoardFlightCandiates()->ReadData(GetProjectID());
		
		GetTerminal().GetSimReportManager()->loadDataSet( m_ProjInfo.path);
		m_portals.loadDataSet( m_ProjInfo.path );	

		
		m_floors.loadDataSet( m_ProjInfo.path );
		GetTerminal().m_AirsideInput->GetFloors().loadDataSet( m_ProjInfo.path );
//		GetTerminal().m_AirsideInput->GetFloors().m_vFloors[0]->IsGridVisible(FALSE);// The airside default have not grid.

		//check airside data 
		InputAirsideDataCheck airsidedatacheck(GetProjectID());
		airsidedatacheck.DoCheck();

		m_defDispProp.loadDataSet( m_ProjInfo.path );

		m_pPlacement->SetInputTerminal(&GetTerminal());
		m_pPlacement->loadDataSet( m_ProjInfo.path );
		DoBridgeConvertCompatible();

		GetTerminal().m_AirsideInput->GetPlacementPtr()->SetInputTerminal(&GetTerminal());
		GetTerminal().m_AirsideInput->GetPlacementPtr()->loadDataSet( m_ProjInfo.path);
		
		m_arcport.m_congManager.loadDataSet(m_ProjInfo.path + "\\INPUT");

		GetEconomicManager().loadInputs( m_ProjInfo.path, &GetTerminal() );
		m_paxDispProps.loadInputs( m_ProjInfo.path , &GetTerminal() );
		m_paxTags.loadInputs( m_ProjInfo.path , &GetTerminal() );
		m_adParams.loadInputs(m_ProjInfo.path, &GetTerminal() );
		m_resDispProps.loadInputs(m_ProjInfo.path, &GetTerminal() );
		m_aircraftDispProps.loadInputs(m_ProjInfo.path , &GetTerminal());
		m_aircraftTags.loadInputs(m_ProjInfo.path, &GetTerminal());
		m_vehicleDispProps.SetProjID(GetProjectID());
		m_vehicleDispProps.ReadData(GetProjectID());
		m_vehicleTags.ReadData(GetProjectID());
		//m_landsideVehicleDispProps.ReadData();
		//m_landsideVehicleTags.ReadData(GetProjectID());
		m_calloutDispSpecs.ReadData(GetProjectID(), this);
		
		GetCamerasByMode(EnvMode_Terminal).SetInputTerminal(&GetTerminal());
		GetCamerasByMode(EnvMode_AirSide).SetInputTerminal(&GetTerminal());

		GetCamerasByMode(EnvMode_Terminal).loadDataSet(m_ProjInfo.path);		
		GetCamerasByMode(EnvMode_AirSide).loadDataSet(m_ProjInfo.path);
		
		m_displayoverrides.loadDataSet(m_ProjInfo.path);
		
		m_movies.loadDataSet(m_ProjInfo.path);
		
		m_walkthroughs->loadDataSet(m_ProjInfo.path);

		m_walkthroughsFlight->loadDataSet(m_ProjInfo.path);
		
		m_walkthroughsVehicle->loadDataSet(m_ProjInfo.path);

		m_videoParams.loadDataSet(m_ProjInfo.path);
		
		m_procTags.loadDataSet(m_ProjInfo.path);

		//airside simulation add by hans 5/9
		GetAirsideSimLogs().GetSimReportManager()->loadDataSet2(m_ProjInfo.path,2);
		
//		m_logFile.loadDataSet(m_ProjInfo.path);
		
		//ben 2005-9-5
		GetTerminal().m_AirsideInput->getpStructurelist()->SetInputTerminal(&GetTerminal());
		GetTerminal().m_AirsideInput->getpStructurelist()->loadDataSet(m_ProjInfo.path);
		GetTerminal().m_pStructureList->SetInputTerminal(&GetTerminal());
		GetTerminal().m_pStructureList->loadDataSet(m_ProjInfo.path);

		
		
		GetTerminal().m_AirsideInput->getpWallShapeList()->SetInputTerminal(&GetTerminal());
		GetTerminal().m_AirsideInput->getpWallShapeList()->loadDataSet(m_ProjInfo.path);
		GetTerminal().m_pWallShapeList->SetInputTerminal(&GetTerminal());
		GetTerminal().m_pWallShapeList->loadDataSet(m_ProjInfo.path);

		//landside
		/*m_arcport.landSideOperationList->SetInputTerminal(&GetTerminal());
		m_arcport.landSideOperationList->loadDataSet(m_ProjInfo.path);*/

		//m_pLandsideDoc->LoadDataSet(m_ProjInfo.path);
	//	m_arcport.m_pInputLandside->ReloadInputData(m_ProjInfo.path,GetProjectID());

	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		AfxMessageBox( szBuf, MB_OK|MB_ICONWARNING );
		delete _pError;
		return FALSE;
	}
	catch ( FileFormatError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		AfxMessageBox( szBuf, MB_OK|MB_ICONWARNING );
		delete _pError;
		return FALSE;
	}
	catch (Exception* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		AfxMessageBox( szBuf, MB_OK|MB_ICONWARNING );
		delete _pError;
		return FALSE;
	}

	m_trainTraffic.Init(&GetTerminal());


	BOOL bRet = InitializeProject();

	
	GetTerminal().setCurrentSimResult( getSimResultCout()>0 ? 0 : -1 );
	((CMainFrame*)AfxGetApp()->m_pMainWnd)->UpdateProjSelectBar();
	GetTerminal().setPreSimResult( -1 );

	// set unique number for all the placements.
	int nCount = m_pPlacement->m_vUndefined.size();
	for( int i=0; i<nCount; i++ )
		m_pPlacement->m_vUndefined[i]->SetSelectName( GetUniqueNumber() );
	nCount = m_pPlacement->m_vDefined.size();
	for( int i=0; i<nCount; i++ )
		m_pPlacement->m_vDefined[i]->SetSelectName( GetUniqueNumber() );
	//frank
	CPlacement* pPlacement = GetTerminal().m_AirsideInput->GetPlacementPtr();
	nCount = pPlacement->m_vUndefined.size();
	for( int i=0; i<nCount; i++ )
		pPlacement->m_vUndefined[i]->SetSelectName( GetUniqueNumber() );
	nCount = pPlacement->m_vDefined.size();
	for( int i=0; i<nCount; i++ )
		pPlacement->m_vDefined[i]->SetSelectName( GetUniqueNumber() );
	
	GetTerminal().GetAirsideInput()->GetProcessorList()->RefreshRelatedDatabase();
	return bRet;
}

void CTermPlanDoc::InitActiveFloorCB()
{
	CMainFrame* pMainFrm = GetMainFrame();
	CComboBox* pCB = pMainFrm->m_wndLayoutBar.GetActiveFloorCB();

	//delete all entries
	int nCount = pCB->GetCount();
	int i=0; 
	for(;i<nCount; i++)
		pCB->DeleteString(0);

	//load floors into CB
	if( EnvMode_AirSide == m_systemMode)
	{
		pCB->AddString( GetTerminal().m_AirsideInput->GetFloors().GetFloor2(0)->FloorName() );
		pCB->SetItemData(i, GetTerminal().m_AirsideInput->GetFloors().GetFloor2(0)->Level() );
		pCB->SetCurSel( 0 );
		m_nActiveFloor = 0;
	}
	if (EnvMode_LandSide == m_systemMode)
	{
		int nFloorCount = GetCurModeFloor().GetCount();
		for(int i=0; i<nFloorCount; i++) 
		{
			if(GetCurModeFloor().GetFloor2(i)->IsActive()) 
			{
				m_nActiveFloor = i;
			}
		}
		for(int i=nFloorCount-1;i>=0;i--)
		{
			pCB->AddString(GetCurModeFloor().GetFloor2(i)->FloorName());
			pCB->SetItemData(i, GetCurModeFloor().GetFloor2(i)->Level());
		}

		pCB->SetCurSel(nFloorCount -1 -m_nActiveFloor);
	}
	else
	{
		int nFloorCount = GetCurModeFloor().GetCount();
		//	for(i=0; i<nFloorCount; i++) {
		for(int i=nFloorCount-1;i>=0;i--)
		{
			pCB->AddString(GetCurModeFloor().GetFloor2(i)->FloorName());
			pCB->SetItemData(i, GetCurModeFloor().GetFloor2(i)->Level());
		}

		pCB->SetCurSel(nFloorCount -1 -m_nActiveFloor);
	}
    
	
}

void CTermPlanDoc::OnUpdateTracersOn(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowTracers);
}

void CTermPlanDoc::OnTracersOn() //Toggles showing and hiding of tracers
{
	if(m_bShowTracers) {
		m_bShowTracers = FALSE;
		
		m_pPaxCountPerDispProp.clear();
		m_pACCountPerDispProp.clear();
		m_pVehicleCountPerDispProp.clear();

		m_tempTracerData.clear();
		m_tempAirsideTracerData.clear();
	}
	else {
		long nFirstInTime, nLastOutTime;
		int iCurSimResult = GetTerminal().getCurrentSimResult();
		if( iCurSimResult>=0 )	// have valid sim_result
		{
			GetTerminal().GetSimReportManager()->SetCurrentSimResult( iCurSimResult );
			GetAirsideSimLogs().GetSimReportManager()->SetCurrentSimResult(iCurSimResult);
			ReadPAXDescriptions(&nFirstInTime, &nLastOutTime);
			ReadACDescriptions(&nFirstInTime, &nLastOutTime);
			nFirstInTime = GetTerminal().GetSimReportManager()->getSimItem( iCurSimResult )->getBeginTime();
			nLastOutTime = GetTerminal().GetSimReportManager()->getSimItem( iCurSimResult )->getEndTime();
			
			BuildTempTracerData();
			
			if(GetCurrentMode() == EnvMode_AirSide)
			{
				BuildTempAirsideTracerData();
			}
			else if (GetCurrentMode()==EnvMode_LandSide)
			{
				BuildTempVehicleTracerData();
			}
			else
			{
				if (GetTerminal().GetSimReportManager() && GetTerminal().GetSimReportManager()->IsAirsideSim())
				{
					BuildTempAirsideTracerData();
				}
			}

				
			m_bShowTracers = TRUE;
		}
		else
		{
			AfxMessageBox("No Valid SimResult!", MB_OK|MB_ICONERROR);
			return;
		}
	}
	UpdateAllViews(NULL,VM_UPDATETRACE);
}

void CTermPlanDoc::OnDistanceMeasure()
{
	if ( EnvMode_OnBoard == m_systemMode )
	{
// 		C3DBaseView* pEditView = Get3DBaseView();
// 		if (pEditView)
// 			pEditView->OnDistanceMeasure();
	}
	else
	{
		C3DView* p3DView = Get3DView();

		if (p3DView)
		{
			p3DView->m_eMouseState = NS3DViewCommon::_distancemeasure;
			p3DView->ClearPointPickStruct();
		}
	}


}

void CTermPlanDoc::OnUpdateDistanceMeasure(CCmdUI* pCmdUI)
{
	if ( EnvMode_OnBoard == m_systemMode )
	{
		C3DBaseView* pEditView = Get3DBaseView();
		pCmdUI->Enable( pEditView != NULL);
		//if (pEditView)
		//	pCmdUI->SetCheck(pEditView->m_eMouseState == CSimpleMeshEditView::_distancemeasure);
	}
	else
	{
		C3DView* p3DView = Get3DView();

		pCmdUI->Enable(p3DView != NULL/*|| p3DBaseEditView != NULL*/);
		if(p3DView != NULL)
			pCmdUI->SetCheck(p3DView->m_eMouseState == NS3DViewCommon::_distancemeasure);
	}
}

void CTermPlanDoc::RefreshProcessorTree()
{
	m_msManager.m_msImplTerminal.SetTermPlanDoc(this);
	m_msManager.m_msImplTerminal.RebuildProcessorTree(NULL);
	UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) ProcessorsNode());			
}

void CTermPlanDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDocument::SetPathName(lpszPathName, bAddToMRU);
}

void CTermPlanDoc::AddProcToSelectionList(CObjectDisplay* pObj)
{
	if(pObj &&pObj->GetType() == ObjectDisplayType_Processor2)
	{
		CProcessor2 *pProc = dynamic_cast<CProcessor2 *>(pObj);
		if (NULL != pProc)
		{	
			if(pProc == NULL||pProc->IsSelected())
				return;	
			GetSelectProcessors().AddItem(pProc);
			pProc->Select(TRUE);

		}	
	}
	else
	{
		if(pObj)
		{
			GetSelectProcessors().AddItem(pObj);
		}

	}
	//char buf[256];
	//pProc->GetProcessor()->getID()->printID(buf);
	//// TRACE("Added proc with index %d & ptr=%d & id=%s to selection list\n", pProc->GetProcessor()->getIndex(), (int) pProc, buf);
}


void CTermPlanDoc::SelectByGroup(CObjectDisplay *pProc)
{
	if(pProc && pProc->GetGroup())
	{
		CObjectDisplayGroup* pGroup = pProc->GetGroup();
		for(int i=0;i<pGroup->GetCount(); ++i)
		{
			CObjectDisplay* pObj = pGroup->GetItem(i);
			AddProcToSelectionList(pObj);
		}		
	}
	else
	{
		AddProcToSelectionList(pProc);
	}
}

void CTermPlanDoc::OnUpdateCtxMoveshapeZ(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(GetSelectProcessors().GetCount() > 1)
		pCmdUI->SetText(_T("Raise Shapes"));
	else
		pCmdUI->SetText(_T("Raise Shape"));	

}


void CTermPlanDoc::OnUpdateCtxMoveshapeZ0(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetText(_T("Floor Height"));

	int nCount = GetSelectProcessors().GetCount();

	ARCVector3 v3DTemp;
	for(int i=0; i<nCount; i++)
	{
		CObjectDisplay* pObjDisplay = GetSelectedObjectDisplay(i);
		if (pObjDisplay && pObjDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2 *pProc2 = (CProcessor2 *)pObjDisplay;

			v3DTemp = pProc2->GetLocation();
			if(v3DTemp[VZ]!=0)
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}
	pCmdUI->Enable(FALSE);
	
	
}

void CTermPlanDoc::OnUpdateAnimationSetSpeed(CCmdUI* pCmdUI)
{
	switch(pCmdUI->m_nID)	
	{
	case ID_ANIMATION_SET1SPEED:
		pCmdUI->SetCheck(m_animData.nAnimSpeed==10);
		break;
	case ID_ANIMATION_SET5SPEED:
		pCmdUI->SetCheck(m_animData.nAnimSpeed==50);
		break;
	case ID_ANIMATION_SET10SPEED:
		pCmdUI->SetCheck(m_animData.nAnimSpeed==100);
		break;
	case ID_ANIMATION_SET25SPEED:
		pCmdUI->SetCheck(m_animData.nAnimSpeed==250);
		break;
	case ID_ANIMATION_SET50SPEED:
		pCmdUI->SetCheck(m_animData.nAnimSpeed==500);
		break;
	case ID_ANIMATION_SET100SPEED:
		pCmdUI->SetCheck(m_animData.nAnimSpeed==1000);
		break;
	case ID_ANIMATION_SETCUSTOMSPEED:
		pCmdUI->SetCheck(m_animData.nAnimSpeed!=10&&m_animData.nAnimSpeed!=50&&m_animData.nAnimSpeed!=100&&
			m_animData.nAnimSpeed!=250&&m_animData.nAnimSpeed!=500&&m_animData.nAnimSpeed!=1000&&m_animData.nAnimSpeed>0);
		break;
	}
}

void CTermPlanDoc::OnUpdateProcProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetSelectProcessors().GetCount()==1 );
	
}

void CTermPlanDoc::OnUpdateViewSharpen(CCmdUI* pCmdUI)
{
	C3DView* pView = Get3DView();
	if(pView)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
	if(pView && pView->IsAutoSharp())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CTermPlanDoc::OnPlaceMarker()
{
	//ASSERT(m_pSelectedNode!=NULL );
	if(GetCurrentMode() == EnvMode_Terminal)
	{
		CFloor2* pFloor = m_floors.GetFloor2((int)m_pSelectedNode->m_dwData);
		ActivateFloor(pFloor->Level());

		if (RenderEngine::getSingleton().GetAlwaysInUse())
		{
			CRender3DView* p3DView = GetRender3DView();
			if (p3DView)
				p3DView->BeginPlaceAlignMarker();
		} 
		else
		{
			C3DView* p3DView = Get3DView();
			if(p3DView) {
				p3DView->m_eMouseState = NS3DViewCommon::_placemarker;
				UpdateAllViews(NULL);
			}
		}
	}
}

void FuntionAlignFloors(CFloor2* pMasterFloor, std::vector<CFloor2*>vAllFloors, CAirsideGround* pAirsideFloor )
{
	
	if(!pMasterFloor->UseMarker() && !pMasterFloor->UseAlignLine() ) {
		//message error
		return;
	}

	ARCVector2 vMasterLoc;
	ARCVector2 vMasterDir(0,0);
	if(pMasterFloor->UseAlignLine())
	{
		CAlignLine alignLine = pMasterFloor->GetAlignLine();
		vMasterLoc = alignLine.getBeginPoint();
		vMasterDir = alignLine.getEndPoint() - alignLine.getBeginPoint();
	}
	else if(pMasterFloor->UseMarker())
	{
		pMasterFloor->GetMarkerLocation(vMasterLoc);
	}

	//for each floor, if the floor has a marker, offset the floor so that it's marker is aligned
	//with the master floor's marker
	//terminal floors
	
	//airside floors
	int nFloorCount = vAllFloors.size();
	for(int i=0; i<nFloorCount; i++)
	{
		CFloor2* piFloor = vAllFloors[i];
		if(piFloor!=pMasterFloor && (piFloor->UseMarker()|| piFloor->UseAlignLine()) )
		{
			ARCVector2 vSlaveLoc;
			ARCVector2 vSlaveDir;
			if(piFloor->UseAlignLine())
			{
				CAlignLine slaveLine = piFloor->GetAlignLine();
				vSlaveLoc = slaveLine.getBeginPoint();
				vSlaveDir = slaveLine.getEndPoint() - slaveLine.getBeginPoint();	
			}
			else if(piFloor->UseMarker())
			{
				piFloor->GetMarkerLocation(vSlaveLoc);				
			}	


			double dRotateAngle = 0;
			if(vSlaveDir.Magnitude() && vMasterDir.Magnitude())
				dRotateAngle = vSlaveDir.GetAngleOfTwoVector(vMasterDir);

			vSlaveLoc.Rotate(-dRotateAngle, piFloor->GetFloorOffset());
			piFloor->SetFloorOffset(piFloor->GetFloorOffset() + (vMasterLoc - vSlaveLoc) );

			piFloor->MapRotation(piFloor->MapRotation()+dRotateAngle);

			if(piFloor->UseMarker())
				piFloor->SetMarkerLocation(vMasterLoc);
			else if(piFloor->UseAlignLine())
			{
				CAlignLine newAlignLine;
				newAlignLine.setBeginPoint(vMasterLoc);
				vSlaveDir.Rotate(-dRotateAngle);
				newAlignLine.setEndPoint(vMasterLoc + vSlaveDir );
				piFloor->SetAlignLine(newAlignLine);
			}

		}		
	}
}

void CTermPlanDoc::OnAlignFloors()
{
	ASSERT(m_pSelectedNode!=NULL && m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR);
	int nSelelectedFloorIdx = (int)m_pSelectedNode->m_dwData;
	CFloor2* pMasterFloor = m_floors.GetFloor2(nSelelectedFloorIdx);

	std::vector<CFloor2*> vAllFloors;
	vAllFloors.insert(vAllFloors.end(), m_floors.m_vFloors.begin(), m_floors.m_vFloors.end());

	CAirsideGround *pAirsideFloor = NULL;
	if(Get3DView() && Get3DView()->GetAirside3D())
	{
		pAirsideFloor = Get3DView()->GetAirside3D()->GetActiveLevel();
		if(pAirsideFloor)
			vAllFloors.push_back(pAirsideFloor);
	}

	FuntionAlignFloors(pMasterFloor,vAllFloors, pAirsideFloor);

	m_floors.saveDataSet(m_ProjInfo.path, FALSE);
	if(pAirsideFloor)
		pAirsideFloor->SaveData(pAirsideFloor->GetAirportID() );
	UpdateAllViews(NULL);
}

void CTermPlanDoc::SetActiveFloorMarker(const ARCVector3& vMousePos)
{
	CFloor2* pFloor = m_floors.m_vFloors[m_nActiveFloor];
	if(GetCurrentMode() == EnvMode_Terminal)
	{
		
		pFloor->SetMarkerLocation(ARCVector2(vMousePos[VX], vMousePos[VY]));
		pFloor->UseMarker(TRUE);
		pFloor->UseAlignLine(FALSE) ;
        m_floors.saveDataSet(m_ProjInfo.path, FALSE);
		UpdateAllViews(NULL);
	}
}

void CTermPlanDoc::OnRemoveMarker()
{
	ASSERT(m_pSelectedNode!=NULL && m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR);
	CFloor2* pFloor = m_floors.m_vFloors[(int)m_pSelectedNode->m_dwData];
	pFloor->UseMarker(FALSE);
	m_floors.saveDataSet(m_ProjInfo.path, FALSE);
	UpdateAllViews(NULL);
}
void CTermPlanDoc::OnRemoveAlignLine()
{
	ASSERT(m_pSelectedNode!=NULL && m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR);
	CFloor2* pFloor = m_floors.m_vFloors[(int)m_pSelectedNode->m_dwData];
	pFloor->UseAlignLine(FALSE);
	//CFloor2::SaveAlignLineStateToDB(EnvMode_Terminal,pFloor->Level(),pFloor->UseAlignLine()) ;
	UpdateAllViews(NULL);
}
void CTermPlanDoc::OnUpdateRemoveAlignLine(CCmdUI* pCmdUI)
{
	CFloor2* pFloor = GetCurModeFloor().m_vFloors[(int)m_pSelectedNode->m_dwData];
	pCmdUI->Enable(pFloor->UseAlignLine());
}
void CTermPlanDoc::OnUpdateAlignFloors(CCmdUI* pCmdUI)
{
	ASSERT(m_pSelectedNode!=NULL /*&& m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR*/);
	CFloor2* pFloor = GetCurModeFloor().m_vFloors[(int)m_pSelectedNode->m_dwData];
	pCmdUI->Enable(pFloor->UseMarker() || pFloor->UseAlignLine());
}
void CTermPlanDoc::OnUpdatePlaceFloors(CCmdUI* pCmdUI)
{
	ASSERT(m_pSelectedNode!=NULL /*&& m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR*/);
	pCmdUI->Enable(TRUE);
}
void CTermPlanDoc::OnUpdateRemoveMarker(CCmdUI* pCmdUI)
{
	ASSERT(m_pSelectedNode!=NULL/* && m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR*/);
	CFloor2* pFloor = GetCurModeFloor().m_vFloors[(int)m_pSelectedNode->m_dwData];
	pCmdUI->Enable(pFloor->UseMarker());
}

void CTermPlanDoc::AlignSelectedProcessors(const std::vector<ARCVector3>& vLine)
{
	if (m_systemMode == EnvMode_Terminal)
	{
		ASSERT(GetSelectProcessors().GetCount() > 0);
		ASSERT(vLine.size() == 2);
		int nSelCount = GetSelectProcessors().GetCount();
		ARCVector3 line = vLine[1]-vLine[0];
		Point pointList[2];
		pointList[0].setPoint(vLine[0][VX],vLine[0][VY],vLine[0][VZ]);
		pointList[1].setPoint(vLine[1][VX],vLine[1][VY],vLine[1][VZ]);
		Path path;
		path.init(2,pointList);
		double dLineLength = line.Magnitude();
		double dStep = dLineLength/(nSelCount-1);
		for(int i=0; i<nSelCount; i++) 
		{
			CObjectDisplay *pObjDisplay = GetSelectedObjectDisplay(i);
			if (pObjDisplay &&pObjDisplay->GetType() == ObjectDisplayType_Processor2)
			{
				CProcessor2* pProc = (CProcessor2 *)pObjDisplay;
				ARCVector3 location = pProc->GetLocation();
				Point deltapos = path.GetDistPoint(i*dStep);
				MoveProcessor(pProc,deltapos.getX() - location[VX],deltapos.getY() - location[VY]);
			}
		}
		UnSelectAllProcs();
		OnProcEditFinished();
	}
	else if (m_systemMode == EnvMode_AirSide)
	{
		ALTObject3DList SelectedList;
		Get3DViewParent()->GetSelectableList().GetSelectObjects(SelectedList);
		ASSERT(SelectedList.size() > 0);
		ASSERT(vLine.size() == 2);
		int nSelCount = (int)SelectedList.size();
		ARCVector3 line = vLine[1]-vLine[0];
		Point pointList[2];
		pointList[0].setPoint(vLine[0][VX],vLine[0][VY],vLine[0][VZ]);
		pointList[1].setPoint(vLine[1][VX],vLine[1][VY],vLine[1][VZ]);
		Path path;
		path.init(2,pointList);
		double dLineLength = line.Magnitude();
		double dStep = dLineLength/(nSelCount-1);
		if( SelectedList.size() )
		{
			Get3DViewParent()->GetAirside3D()->ReflectChangeOf(SelectedList);
		}
		for(int i=0; i<nSelCount; i++) 
		{
			ARCVector3 location = SelectedList[i]->GetLocation();
			Point deltapos = path.GetDistPoint(i*dStep);
			SelectedList[i]->OnMove(deltapos.getX() - location[VX],deltapos.getY() - location[VY],0);
			SelectedList[i]->FlushChange();
		}
	}
}

void CTermPlanDoc::ArraySelectedProcessors(const std::vector<ARCVector3>& vLine, int nCopies)
{
	ASSERT(GetSelectProcessors().GetCount() > 0);
	ASSERT(vLine.size() > 1);
	ASSERT(vLine.size() == 2);
	ASSERT(nCopies > 0);

//get number of copies
	CDlgArrayCopies dlg(nCopies);
	if(dlg.DoModal()!=IDOK)
		return;

	nCopies = dlg.GetNumCopies();

	ARCVector3 line = vLine[1]-vLine[0];
	double dLineLength = line.Magnitude();
	double dStep = dLineLength/nCopies;
	line.Normalize();

	std::vector<CProcessor2 *> vNewProcList;
	std::vector<CObjectDisplay *> vOtherObjList;
	//vNewProcList.reserve(GetSelectProcessors().GetCount()*nCopies);

	int nSelCount = GetSelectProcessors().GetCount();
	for(int i=0; i<nSelCount; i++) 
	{
		CObjectDisplay *pObjDisplay = GetSelectedObjectDisplay(i);
		if (pObjDisplay &&pObjDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			
			CProcessor2* pProcToCopy = (CProcessor2 *)pObjDisplay;
			//copy proc nCopies times and translate along vLine
			for(int j=1; j<=nCopies; j++)
			{
				CProcessor2* pProcCopy = CopyProcessor(pProcToCopy);
				ARCVector3 deltapos = j*dStep*line;
				MoveProcessor(pProcCopy,deltapos[VX],deltapos[VY]);
				vNewProcList.push_back(pProcCopy);	
			}
		}
		else
		{
			//other object not copyed
			vOtherObjList.push_back( pObjDisplay);
		}
	}

	UnSelectAllProcs();
	for(int j=0;j<static_cast<int>(vNewProcList.size());j++)
		AddProcToSelectionList(vNewProcList[j]);

	for(int m=0;m<static_cast<int>(vOtherObjList.size());m++)
	{
		GetSelectProcessors().AddItem(vOtherObjList[m]);
	}


	GetCurrentPlacement()->saveDataSet(m_ProjInfo.path, false);

	UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) ProcessorsNode());
}

void CTermPlanDoc::MirrorSelectedProcessors(const std::vector<ARCVector3>& vLine)
{
	ASSERT(GetSelectProcessors().GetCount() > 0);
	ASSERT(vLine.size() > 1);
	ASSERT(vLine.size() == 2);

	

	int nSelCount = GetSelectProcessors().GetCount();
	for(int i=0; i<nSelCount; i++) 
	{
		CObjectDisplay *pObjDisplay = GetSelectedObjectDisplay(i);
		if(pObjDisplay && pObjDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2* pProc2 = (CProcessor2 *)pObjDisplay;
			

			//mirror properties
			pProc2->MirrorProperties(vLine);
		}

			
	}
	GetProcessorList().saveDataSet(m_ProjInfo.path, false);
	GetCurrentPlacement()->saveDataSet(m_ProjInfo.path, false);
}

void CTermPlanDoc::OnDefineVisibleRegions()
{
	ASSERT(m_pSelectedNode!=NULL && ( m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR||m_pSelectedNode->m_nIDResource == IDR_CTX_LEVEL) );
	CFloor2* pFloor = NULL;
	pFloor=GetFloorByMode(m_systemMode).m_vFloors[(int)m_pSelectedNode->m_dwData];
	CTVNode* pFloorsNode = FloorsNode();
	ActivateFloor(pFloor->Level());
	UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) (CTVNode*) pFloorsNode);
	CDlgFloorVisibleRegions dlg(pFloor,TRUE,this,GetMainFrame());
	if(dlg.DoModal()==IDOK) {
		GetCurModeFloor().saveDataSet(m_ProjInfo.path, FALSE);
		pFloor->InvalidateTexture();
		UpdateViewSharpTextures();
		UpdateFloorInRender3DView(pFloor);
	}
	ActivateFloor(m_nLastActiveFloor);
	UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) (CTVNode*) pFloorsNode);
}

void CTermPlanDoc::OnManageBMViewpoints()
{
	if ((m_animData.m_AnimationModels.IsOnBoardSel() || m_animData.m_AnimationModels.IsLandsideSel()) && GetRender3DView())
	{
		C3DViewCameras& cams = GetCurModeRenderCameras();
		CDlgBookmarkedCamerasRender dlg(cams, true);
		if (IDOK == dlg.DoModal())
		{
			cams.m_savedCameras = dlg.m_cams.m_savedCameras;
			SaveCurModeRenderCameras();
		}
	} 
	else
	{
		//save cameras data
		GetCurModeCameras().saveDataSet(m_ProjInfo.path,false);
		CDlgBookmarkedCameras dlg(this,GetMainFrame());
		if(dlg.DoModal() == IDOK) {
			//save cameras data
			GetCurModeCameras().saveDataSet(m_ProjInfo.path,false);
		}
		else {
			//load cameras data
			GetCurModeCameras().loadDataSet(m_ProjInfo.path);
		}
	}
}

BOOL CTermPlanDoc::SaveNamedView(const CString& _s)
{
	GetCurModeCameras().m_savedViews.resize(GetCurModeCameras().m_savedViews.size()+1);
	//build the CViewDesc structure in place
	CViewDesc& vd = GetCurModeCameras().m_savedViews[GetCurModeCameras().m_savedViews.size()-1];
	vd.name = _s;
	CChildFrame* pCF = Get3DViewParent();
	pCF->SetPaneInfo(vd);
	//vd.panecameras.resize(vd.paneinfo.nNumRows*vd.paneinfo.nNumCols);
	vd.panecameras.resize(4);
	for(int r=0; r<2; r++) {
		for(int c=0; c<2; c++) {
			//if(pCF->) {
				vd.panecameras[c+2*r] = new C3DCamera( *(pCF->GetWorkingCamera(r,c)) );
			//}
			//else {
			//	vd.panecameras[c+vd.paneinfo.nNumCols*r] = new C3DCamera( *(pCF->GetWorkingCamera(r,c)) );
			//}
		}
	}
	return TRUE;
}

BOOL CTermPlanDoc::SaveNamedRenderView(const CString& _sName)
{
	GetCurModeRenderCameras().AddSavedCameraDesc(_sName);
	SaveCurModeRenderCameras();
	return true;
}


void CTermPlanDoc::PasteProc(int nPasteToFloorIndex,
	PROCESSOR2_DATA_FOR_CLIPBOARD* pPDFC)
{
	if(pPDFC->ProcType==IntegratedStationProc)
	{
		CString strMsg="Processor "+pPDFC->ProcName+"'s type is Integrated station.It can not be pasted";
		AfxMessageBox(strMsg);
		return;		
	}
	CProcessor2* pNewProc2=new CProcessor2;
	//set floor
	pNewProc2->SetFloor(nPasteToFloorIndex);
	//set location,scale and rotation
	
	pNewProc2->SetLocation(pPDFC->m_vLocation);
	pNewProc2->SetScale(pPDFC->m_vScale);
	pNewProc2->SetRotation(pPDFC->m_dRotation);
	pNewProc2->m_dispProperties = pPDFC->m_dispProperties;
	//set shape
	CShape* pShape=SHAPESMANAGER->GetShapeByName(pPDFC->m_strShapeName);
	if(!pShape)
	{
		delete pNewProc2;
		return;
	}
	pNewProc2->SetShape(pShape);
	//set selection id
	pNewProc2->SetSelectName(GetUniqueNumber());
	//if it has processor then add to defined list,otherwise add to undefined list
	if(pPDFC->bHasProcessor)
	{
		Processor tempProc,*pProc;
		ProcessorID PID,PID2;
		PID.SetStrDict(GetTerminal().inStrDict);
		PID.setID(pPDFC->ProcName.GetBuffer(pPDFC->ProcName.GetLength()));
		pPDFC->ProcName.ReleaseBuffer();
		//check Processor Name and Type--------------------------
		char nameLevel[256];
		PID.getNameAtLevel(nameLevel,0);
		PID2.SetStrDict(GetTerminal().inStrDict);
		PID2.setID(nameLevel);
		GroupIndex GI=GetProcessorList().getGroupIndex(PID2);
		if(GI.start!=-1)
		{
			Processor* pProcFinded=GetProcessorList().getProcessor(GI.start);
			if(pProcFinded->getProcessorType()!=pPDFC->ProcType)
			{
				CString strMsg="Processor "+pPDFC->ProcName+"'s type can not match.It can not be pasted";
				AfxMessageBox(strMsg);
				delete pNewProc2;
				return;
			}
		}
		//-------------------------------------------------------
		tempProc.init(PID);
		ProcessorID* pNewID=GetNextProcessorID(tempProc.getID(),&GetProcessorList());
		//check processor Name-----------------------------------
		char pNewIDName[256];
		pNewID->printID(pNewIDName);
		BOOL bCheck=GetProcessorList().IsValidProcessorName(pNewIDName);
		if(!bCheck)
		{
			delete pNewID;
			delete pNewProc2;
			return;
		}
		//-------------------------------------------------------
		if(pPDFC->ProcType==IntegratedStationProc)
		{
			
		}
		else
		{
			//set processor type
			switch(pPDFC->ProcType)
			{
			case PointProc:
				pProc = new Processor;
				break;
			case DepSourceProc:
				pProc = new DependentSource;
				break;
			case DepSinkProc:
				pProc = new DependentSink;
				break;
			case LineProc:
				pProc = new LineProcessor;
				break;
			case BaggageProc:
				pProc = new BaggageProcessor;
				break;
			case HoldAreaProc:
				pProc = new HoldingArea;
				break;
			case GateProc:
				pProc = new GateProcessor;
				break;
			case FloorChangeProc:
				pProc = new FloorChangeProcessor;
				break;
			case MovingSideWalkProc:
				pProc=new MovingSideWalk;
				break;
			case BarrierProc:
				pProc = new Barrier;
				break;
			case Elevator:
				pProc = new ElevatorProc;
				break;
			case ConveyorProc:
				pProc = new Conveyor;
				break;			
			case StairProc:
				pProc = new Stair;
				break;
			case EscalatorProc:
				pProc = new Escalator;
				break;					
			}
			// set processor id
			pProc->init(*pNewID);

			//set terminal
			pProc->SetTerminal(&GetTerminal());
				//set properties
				int nPSLCount=pPDFC->m_location.getCount() ;
				if(nPSLCount> 0)
				{
					if( pProc->getProcessorType() == BaggageProc )
					{
						Path* pPath = &(pPDFC->m_location);
						pProc->initServiceLocation( pPath->getCount(), pPath->getPointList() );						
					}
					else
					{
						Point* pPointSL=new Point[nPSLCount];
						memcpy(pPointSL,pPDFC->m_location.getPointList(),sizeof(Point)*nPSLCount);
						pProc->initServiceLocation(nPSLCount,pPointSL);
						delete[] pPointSL;
					}
					
				}
				int nPICCount=pPDFC->In_Constr.getCount();
				if( nPICCount> 0)
				{
					Point* pPointIC=new Point[nPICCount];
					memcpy(pPointIC,pPDFC->In_Constr.getPointList(),sizeof(Point)*nPICCount);
					pProc->initInConstraint( nPICCount, pPointIC);
					delete[] pPointIC;
				}
				
				int nPOCCount=pPDFC->Out_Constr.getCount();
				if(nPOCCount > 0)
				{
					Point* pPointOC=new Point[nPOCCount];
					memcpy(pPointOC,pPDFC->Out_Constr.getPointList(),sizeof(Point)*nPOCCount);
					pProc->initOutConstraint( nPOCCount, pPointOC);
					delete[] pPointOC;
				}
				
				int nPQCount=pPDFC->m_qCorners.getCount();
				if(nPQCount> 0)
				{
					Point* pPointQ=new Point[nPQCount];
					memcpy(pPointQ,pPDFC->m_qCorners.getPointList(),sizeof(Point)*nPQCount);
					pProc->initQueue( FALSE, nPQCount,pPointQ);
					delete[] pPointQ;
				}

				if(pPDFC->bIsBaggageDevice)
				{
					BaggageProcessor* pBP=(BaggageProcessor*)pProc;
					int nBMOCount=pPDFC->m_pathBagMovingOn.getCount();
					if(nBMOCount> 0)
					{
						Point* pPointBMO=new Point[nBMOCount];
						memcpy(pPointBMO,pPDFC->m_pathBagMovingOn.getPointList(),sizeof(Point)*nBMOCount);
						pBP->GetPathBagMovingOn()->init(nBMOCount,pPointBMO);
						delete[] pPointBMO;
					}

					int nBSLCount=pPDFC->m_pathBarrierSL.getCount();
					if(nBSLCount> 0)
					{
						Point* pPointBSL=new Point[nBSLCount];
						memcpy(pPointBSL,pPDFC->m_pathBarrierSL.getPointList(),sizeof(Point)*nBSLCount);
						pProc->initQueue( FALSE, nBSLCount,pPointBSL);
						pBP->GetBarrier()->initServiceLocation(nBSLCount,pPointBSL);
						delete[] pPointBSL;
					}

				}
				pNewProc2->SetProcessor(pProc);

				GetProcessorList().addProcessor(pProc);
				GetProcessorList().setIndexes();			
				GetProcessorList().saveDataSet(m_ProjInfo.path, false);

			
				GetCurrentPlacement()->m_vDefined.insert( GetCurrentPlacement()->m_vDefined.begin(), pNewProc2 );
				GetCurrentPlacement()->saveDataSet(m_ProjInfo.path, false);
				m_msManager.m_msImplTerminal.SetTermPlanDoc(this);
//				m_msManager.m_msImplTerminal.RebuildProcessorTree(NULL);
// 				CTVNode* pPlacementsNode = ProcessorsNode();
// 				UpdateAllViews(NULL,NODE_HINT_REFRESHNODE,pPlacementsNode);
				
		}
	}
	else
	{
		pNewProc2->SetProcessor(NULL);
		GetCurrentPlacement()->m_vUndefined.insert(GetCurrentPlacement()->m_vUndefined.begin(),pNewProc2);
		GetCurrentPlacement()->saveDataSet(m_ProjInfo.path,false);
		AddToUnderConstructionNode(pNewProc2);
	}
	UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) ProcessorsNode());
}

//void CTermPlanDoc::SetReportPara( CReportParameter* _pPara )
//{
//	/*
//	if( m_pCurrentReportPara )
//	{
//		delete m_pCurrentReportPara;
//		m_pCurrentReportPara = NULL;
//	}
//
//	m_pCurrentReportPara = _pPara->Clone();
//	*/
//	m_arcReportManager.SetReportPara(_pPara);
//	//m_pCurrentReportPara = _pPara;
//}

void CTermPlanDoc::OnTracerParamsOn(UINT nID)
{
	int idx = nID-ID_TRACERPARAMS_ON;
	// TRACE("TRACERS ON for id:%d\n", idx);
	ASSERT(idx >= 0);
	CPaxDispPropItem* pPDPI = 0;
	CAircraftDispPropItem *pADPI = 0;
	if(idx == 0)
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetDefaultDispProp();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetDefaultDispProp();
	}
	else if(idx==1)
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetOverLapDispProp();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetOverLapDispProp(); 
	}
	else 
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetDispPropItem(idx-2);
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetDispPropItem(idx-2);
	}

	if(GetCurrentMode() == EnvMode_Terminal)
	{
		pPDPI->SetLeaveTrail(!pPDPI->IsLeaveTrail());
		m_paxDispProps.saveDataSet(m_ProjInfo.path, false);
	}
	else if(GetCurrentMode() == EnvMode_AirSide)
	{
		pADPI->SetLeaveTrail(!pADPI->IsLeaveTrail());
		m_aircraftDispProps.saveDataSet(m_ProjInfo.path, false); 	
	}

	if (m_bShowTracers)
	{
		// let's clear all first, code copied from CTermPlanDoc::OnTracersOn()
		m_pPaxCountPerDispProp.clear();
		m_pACCountPerDispProp.clear();
		m_tempTracerData.clear();
		m_tempAirsideTracerData.clear();

		// then build, code copied from CTermPlanDoc::OnTracersOn()
		long nFirstInTime, nLastOutTime;
		int iCurSimResult = GetTerminal().getCurrentSimResult();
		if( iCurSimResult>=0 )	// have valid sim_result
		{
			GetTerminal().GetSimReportManager()->SetCurrentSimResult( iCurSimResult );
			ReadPAXDescriptions(&nFirstInTime, &nLastOutTime);
			ReadACDescriptions(&nFirstInTime, &nLastOutTime);
			nFirstInTime = GetTerminal().GetSimReportManager()->getSimItem( iCurSimResult )->getBeginTime();
			nLastOutTime = GetTerminal().GetSimReportManager()->getSimItem( iCurSimResult )->getEndTime();
	
			BuildTempTracerData();
			if(GetCurrentMode() == EnvMode_AirSide)
			{
				BuildTempAirsideTracerData();
			}
			else
			{
				if (GetTerminal().GetSimReportManager() && GetTerminal().GetSimReportManager()->IsAirsideSim())
				{
					BuildTempAirsideTracerData();
				}
			}
		}
		else
		{
			AfxMessageBox("No Valid SimResult!", MB_OK|MB_ICONERROR);
			return;
		}
	}
	else
	{
		return;
	}

	
	UpdateAllViews(NULL);
}

void CTermPlanDoc::OnTracerParamsColor(UINT nID)
{
	int idx = nID-ID_TRACERPARAMS_COLOR;
	// TRACE("TRACERS COLOR for id:%d\n", idx);
	ASSERT(idx >= 0);
	CPaxDispPropItem* pPDPI = 0;
	CAircraftDispPropItem * pADPI = 0;
	if(idx == 0)
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetDefaultDispProp();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetDefaultDispProp();
	}
	else if(idx==1)
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetOverLapDispProp();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetOverLapDispProp(); 
	}
	else 
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetDispPropItem(idx-2);
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetDispPropItem(idx-2);
	}
	if(GetCurrentMode() == EnvMode_Terminal)
	{
		CColorDialog colorDlg(pPDPI->GetColor(), CC_ANYCOLOR | CC_FULLOPEN, NULL);
		if(colorDlg.DoModal() == IDOK) {
			pPDPI->SetColor(colorDlg.GetColor());
			m_paxDispProps.saveDataSet(m_ProjInfo.path, false);
		}
	}
	else if(GetCurrentMode() == EnvMode_AirSide)
	{
		CColorDialog colorDlg(pADPI->GetColor(), CC_ANYCOLOR | CC_FULLOPEN, NULL);
		if(colorDlg.DoModal() == IDOK) {
			pADPI->SetColor(colorDlg.GetColor());
			m_aircraftDispProps.saveDataSet(m_ProjInfo.path, false);
		}
	} 
	
	UpdateAllViews(NULL);
}

void CTermPlanDoc::OnTracerParamsDensity(UINT nID)
{
	int idx = nID-ID_TRACERPARAMS_DENSITY;
	// TRACE("TRACERS DENSITY for id:%d\n", idx);
	ASSERT(idx >= 0);
	CPaxDispPropItem* pPDPI = 0;
	CAircraftDispPropItem* pADPI = 0;
	if(idx == 0)
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetDefaultDispProp();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetDefaultDispProp();
	}
	else if(idx==1)
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetOverLapDispProp();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetOverLapDispProp(); 
	}
	else 
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetDispPropItem(idx-2);
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetDispPropItem(idx-2);
	}
	if(GetCurrentMode() == EnvMode_Terminal)
	{
		CDlgTracerDensity dlg(pPDPI->GetDensity()*100.0);
		if(dlg.DoModal() == IDOK) {
			pPDPI->SetDensity(dlg.GetDensity()/100.0);
			m_paxDispProps.saveDataSet(m_ProjInfo.path, false);
		}
	}
	else if(GetCurrentMode() == EnvMode_AirSide)	
	{
		CDlgTracerDensity dlg(pADPI->GetDensity()*100.0);
		if(dlg.DoModal() == IDOK) {
			pADPI->SetDensity(float(dlg.GetDensity()/100.0));
			m_aircraftDispProps.saveDataSet(m_ProjInfo.path, false);
		}
	}
	
	UpdateAllViews(NULL);
}


void CTermPlanDoc::OnUpdateTracerParamsOn(CCmdUI* pCmdUI)
{
	int idx = pCmdUI->m_nID - ID_TRACERPARAMS_ON; 

	CPaxDispPropItem* pPDPI = 0;
	CAircraftDispPropItem* pADPI = 0;
	if(idx == 0)
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetDefaultDispProp();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetDefaultDispProp();
	}
	else if(idx==1)
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetOverLapDispProp();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetOverLapDispProp(); 
	}
	else 
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetDispPropItem(idx-2);
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetDispPropItem(idx-2);
	}
	if(GetCurrentMode() == EnvMode_Terminal)
		pCmdUI->SetCheck(pPDPI->IsLeaveTrail());
	else if(GetCurrentMode() == EnvMode_AirSide)
		pCmdUI->SetCheck(pADPI->IsLeaveTrail());
}

void CTermPlanDoc::OnUpdateTracerParamsColor(CCmdUI* pCmdUI)
{
	int idx = pCmdUI->m_nID - ID_TRACERPARAMS_COLOR;
	CPaxDispPropItem* pPDPI = 0;
	CAircraftDispPropItem* pADPI = 0;
	if(idx == 0)
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetDefaultDispProp();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetDefaultDispProp();
	}
	else if(idx==1)
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetOverLapDispProp();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetOverLapDispProp(); 
	}
	else 
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetDispPropItem(idx-2);
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetDispPropItem(idx-2);
	}

	COLORREF col;
	if(GetCurrentMode() == EnvMode_Terminal)
		col = pPDPI->GetColor();
	else if(GetCurrentMode() == EnvMode_AirSide)
		col = pADPI->GetColor();
	
	CString s;
	s.Format("Color (%d, %d, %d)...", GetRValue(col), GetGValue(col), GetBValue(col));
	pCmdUI->SetText(s);
}

void CTermPlanDoc::OnUpdateTracerParamsDensity(CCmdUI* pCmdUI)
{
	int idx = pCmdUI->m_nID - ID_TRACERPARAMS_DENSITY;
	CPaxDispPropItem* pPDPI = 0;
	CAircraftDispPropItem* pADPI = 0;
	if(idx == 0)
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetDefaultDispProp();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetDefaultDispProp();
	}
	else if(idx==1)
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetOverLapDispProp();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetOverLapDispProp(); 
	}
	else 
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPDPI = m_paxDispProps.GetDispPropItem(idx-2);
		else if(GetCurrentMode() == EnvMode_AirSide)
			pADPI = m_aircraftDispProps.GetDispPropItem(idx-2);
	}
	CString s;
	if(GetCurrentMode() == EnvMode_Terminal)
		s.Format("Density (%.2f%%)...", pPDPI->GetDensity()*100.0);
	else if(GetCurrentMode() == EnvMode_AirSide)
		s.Format("Density (%.2f%%)...", pADPI->GetDensity()*100.0);
	pCmdUI->SetText(s);
}


void CTermPlanDoc::OnTagsParamsOn(UINT nID)
{
	int idx = nID-ID_TAGSPARAMS_ON;
	// TRACE("Tags ON for id:%d\n", idx);
	ASSERT(idx >= 0);
	CPaxTagItem* pPTI = 0;
	CAircraftTagItem * pATI = 0;
	if(idx == 0)
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPTI = m_paxTags.GetDefaultPaxTags();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pATI = m_aircraftTags.GetDefaultTags();
	}
	else if(idx==1)	
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPTI = m_paxTags.GetOverLapPaxTags();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pATI = m_aircraftTags.GetOverLapTags(); 
	}
	else 
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPTI = m_paxTags.GetPaxTagItem(idx-2);
		else if(GetCurrentMode() == EnvMode_AirSide)
			pATI = m_aircraftTags.GetTagItem(idx-2);
	}
	if(GetCurrentMode() == EnvMode_Terminal)
	{
		pPTI->SetShowTags(!pPTI->IsShowTags());
		m_paxTags.saveDataSet(m_ProjInfo.path, false);
	}
	else if(GetCurrentMode() == EnvMode_AirSide)
	{
		pATI->SetShowTags(!pATI->IsShowTags());
		m_aircraftTags.saveDataSet(m_ProjInfo.path, false);
	}
}

void CTermPlanDoc::OnUpdateTagsParamsOn(CCmdUI* pCmdUI)
{
	int idx = pCmdUI->m_nID - ID_TAGSPARAMS_ON;
	CPaxTagItem* pPTI = 0;
	CAircraftTagItem * pATI = 0;
	if(idx == 0)
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPTI = m_paxTags.GetDefaultPaxTags();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pATI = m_aircraftTags.GetDefaultTags();
	}
	else if(idx==1)	
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPTI = m_paxTags.GetOverLapPaxTags();
		else if(GetCurrentMode() == EnvMode_AirSide)
			pATI = m_aircraftTags.GetOverLapTags(); 
	}
	else 
	{
		if(GetCurrentMode() == EnvMode_Terminal)
			pPTI = m_paxTags.GetPaxTagItem(idx-2);
		else if(GetCurrentMode() == EnvMode_AirSide)
			pATI = m_aircraftTags.GetTagItem(idx-2);
	}
	if(GetCurrentMode() == EnvMode_Terminal)
		pCmdUI->SetCheck(pPTI->IsShowTags());
	else if(GetCurrentMode() == EnvMode_AirSide)
		pCmdUI->SetCheck(pATI->IsShowTags());
}

void CTermPlanDoc::UpdateTrackersMenu()
{
	CMainFrame* pMainFrm = GetMainFrame();
	
	CString s;
	//CNewMenu m;
	
	//m.LoadMenu(IDR_MENU_POPUP);
	CNewMenu* pTrackerMenu=NULL;
	CNewMenu* pMainMenu =DYNAMIC_DOWNCAST(CNewMenu, pMainFrm->GetMenu());
	int nMenuCount=pMainMenu->GetMenuItemCount();
	for(int k=0;k<nMenuCount;k++)
	{
		pMainMenu->GetMenuString(k, s, MF_BYPOSITION);
		if(s=="Trackers")
		{
			pTrackerMenu=DYNAMIC_DOWNCAST(CNewMenu,pMainMenu->GetSubMenu(k));
			break;
		}
	}
	if(pTrackerMenu==NULL)
		return;
	
	/////////////////////////////
	//TRACERS
	nMenuCount=pTrackerMenu->GetMenuItemCount();
	for(int k=0;k<nMenuCount;k++)
	{
		pTrackerMenu->DeleteMenu(0,MF_BYPOSITION);
	}

	m_menuTracersList.DestroyMenu();
	m_menuTracersList.CreateMenu();
	m_menuTracersList.AppendMenu(MF_STRING, ID_TRACERS_ON, "On");
	m_menuTracersList.AppendMenu(MF_SEPARATOR);
	
	int nCount = 0;
	if(GetCurrentMode() == EnvMode_Terminal)
		nCount = m_paxDispProps.GetCount();
	else if(GetCurrentMode() == EnvMode_AirSide)
		nCount = m_aircraftDispProps.GetCount();

	delete[] m_pMenuTracerParams;
	m_pMenuTracerParams=new CNewMenu[nCount+2];
	for(int i=0; i<nCount+2; i++) {
		m_pMenuTracerParams[i].CreateMenu();
		
		CPaxDispPropItem* pPDPI = 0;
		CAircraftDispPropItem* pADPI = 0;
		if(i==0)
		{
			if(GetCurrentMode() == EnvMode_Terminal)
				pPDPI = m_paxDispProps.GetDefaultDispProp();
			else if(GetCurrentMode() == EnvMode_AirSide)
				pADPI = m_aircraftDispProps.GetDefaultDispProp();
		}
		else if(i==1)
		{
			if(GetCurrentMode() == EnvMode_Terminal)
				pPDPI = m_paxDispProps.GetOverLapDispProp();
			else if(GetCurrentMode() == EnvMode_AirSide)
				pADPI = m_aircraftDispProps.GetOverLapDispProp();
		}
		else
		{
			if(GetCurrentMode() == EnvMode_Terminal)
				pPDPI = m_paxDispProps.GetDispPropItem(i-2);
			else if(GetCurrentMode() == EnvMode_AirSide)
				pADPI = m_aircraftDispProps.GetDispPropItem(i-2);
		}

		if(GetCurrentMode() == EnvMode_Terminal)
		{
			if(pPDPI->IsLeaveTrail())
				m_pMenuTracerParams[i].AppendMenu(MF_STRING | MF_CHECKED, ID_TRACERPARAMS_ON+i, "On");
			else
				m_pMenuTracerParams[i].AppendMenu(MF_STRING | MF_UNCHECKED, ID_TRACERPARAMS_ON+i, "On");

			COLORREF col = pPDPI->GetColor();
			CString s;
			s.Format("Color (%d, %d, %d)...", GetRValue(col), GetGValue(col), GetBValue(col));
			m_pMenuTracerParams[i].AppendMenu(MF_STRING, ID_TRACERPARAMS_COLOR+i, s);

			s.Format("Density (%.2f%%)...", pPDPI->GetDensity()*100.0); 
			m_pMenuTracerParams[i].AppendMenu(MF_STRING, ID_TRACERPARAMS_DENSITY+i, s);

			m_menuTracersList.AppendMenu(MF_POPUP, (UINT) m_pMenuTracerParams[i].m_hMenu, pPDPI->GetPaxType()->GetName());
		}
		else if(GetCurrentMode() == EnvMode_AirSide)
		{
			if(pADPI->IsLeaveTrail())
				m_pMenuTracerParams[i].AppendMenu(MF_STRING | MF_CHECKED, ID_TRACERPARAMS_ON+i, "On");
			else
				m_pMenuTracerParams[i].AppendMenu(MF_STRING | MF_UNCHECKED, ID_TRACERPARAMS_ON+i, "On");

			COLORREF col = pADPI->GetColor();
			CString s;
			s.Format("Color (%d, %d, %d)...", GetRValue(col), GetGValue(col), GetBValue(col));
			m_pMenuTracerParams[i].AppendMenu(MF_STRING, ID_TRACERPARAMS_COLOR+i, s);

			s.Format("Density (%.2f%%)...", pADPI->GetDensity()*100.0); 
			m_pMenuTracerParams[i].AppendMenu(MF_STRING, ID_TRACERPARAMS_DENSITY+i, s);

			m_menuTracersList.AppendMenu(MF_POPUP, (UINT) m_pMenuTracerParams[i].m_hMenu, pADPI->GetFlightType()->GetName());
		}
	}
	///////////////////////////////////////////////////
	//TAGS
	
	m_menuTags.DestroyMenu();
	m_menuTags.CreateMenu();
	m_menuTags.AppendMenu(MF_STRING, ID_TAGS_ON, "On");
	m_menuTags.AppendMenu(MF_SEPARATOR);
	if(GetCurrentMode() == EnvMode_Terminal)
		nCount = m_paxTags.GetCount();
	else if(GetCurrentMode() == EnvMode_AirSide)
		nCount = m_aircraftTags.GetCount();
	
	delete[] m_pMenuTagsParams;
	m_pMenuTagsParams=new CNewMenu[nCount+2];
	for(int i=0; i<nCount+2; i++) {
		m_pMenuTagsParams[i].CreateMenu();
		CPaxTagItem* pPTI = 0;
		CAircraftTagItem* pATI = 0;
		if(i==0)
		{
			if(GetCurrentMode() == EnvMode_Terminal)
				pPTI = m_paxTags.GetDefaultPaxTags();
			else if(GetCurrentMode() == EnvMode_AirSide)
				pATI = m_aircraftTags.GetDefaultTags();
		}
		else if(i==1)
		{
			if(GetCurrentMode() == EnvMode_Terminal)
				pPTI = m_paxTags.GetOverLapPaxTags();
			else if(GetCurrentMode() == EnvMode_AirSide)
				pATI = m_aircraftTags.GetOverLapTags(); 
		}
		else
		{
			if(GetCurrentMode() == EnvMode_Terminal)
				pPTI = m_paxTags.GetPaxTagItem(i-2);
			else if(GetCurrentMode() == EnvMode_AirSide)
				pATI = m_aircraftTags.GetTagItem(i-2);
		}
		if(GetCurrentMode() == EnvMode_Terminal)
		{
			if(pPTI->IsShowTags())
				m_pMenuTagsParams[i].AppendMenu(MF_STRING | MF_CHECKED, ID_TAGSPARAMS_ON+i, "On");
			else
				m_pMenuTagsParams[i].AppendMenu(MF_STRING | MF_UNCHECKED, ID_TAGSPARAMS_ON+i, "On");

			m_menuTags.AppendMenu(MF_POPUP, (UINT) m_pMenuTagsParams[i].m_hMenu, pPTI->GetPaxType()->GetName());
		}
		else if(GetCurrentMode() == EnvMode_AirSide)
		{
			if(pATI->IsShowTags())
				m_pMenuTagsParams[i].AppendMenu(MF_STRING | MF_CHECKED, ID_TAGSPARAMS_ON+i, "On");
			else
				m_pMenuTagsParams[i].AppendMenu(MF_STRING | MF_UNCHECKED, ID_TAGSPARAMS_ON+i, "On");

			m_menuTags.AppendMenu(MF_POPUP, (UINT) m_pMenuTagsParams[i].m_hMenu, pATI->GetFlightType()->GetName());
		}
	}
	
	
	pTrackerMenu->AppendMenu(MF_POPUP, (UINT) m_menuTracersList.m_hMenu, "Tracers");
	pTrackerMenu->AppendMenu(MF_POPUP, (UINT) m_menuTags.m_hMenu, "Tags");
	pTrackerMenu->AppendMenu(MF_STRING, ID_TRACKERS_MOVIE, "Movie...");
	pTrackerMenu->AppendMenu(MF_STRING, ID_TRACKERS_WALKTHROUGH, "Walkthrough...");
	if( GetCurrentMode() == EnvMode_AirSide )
		pTrackerMenu->AppendMenu(MF_STRING, ID_TRACKERS_PANORAMA, "Panorama...");
}

void CTermPlanDoc::OnUpdateRunDelta(CCmdUI* pCmdUI)
{
	
}

static CCompRepLogBar* __pLogBar = NULL;
static void CALLBACK UpdateAirsideSimProgress(int nPos, const CString& strText)
{
	if (__pLogBar != NULL)
	{
		__pLogBar->SetProgressPos(nPos);
		__pLogBar->AddLogText(strText);
	}
}

void CTermPlanDoc::OnRunDelta()
{
	CView* pView = GetNodeView(); //???
	if(!pView) {
		pView = Get3DView();
		if(!pView) 
		{
			GetMainFrame()->CreateOrActiveEnvModeView();
			pView = GetNodeView();
		}
	}
	if (!pView)
	{
		ASSERT(FALSE);
		return;
	}

	if(!m_pDlgSimEngine)
		m_pDlgSimEngine = new CSimEngineDlg();

	ASSERT( m_pDlgSimEngine );
	if(!::IsWindow(m_pDlgSimEngine->m_hWnd))
	{
		m_pDlgSimEngine->SetTermPlanDoc(this);
		if(m_pDlgSimEngine->Create(IDD_DIALOG_SIMENGINE, pView))
		{
			m_pDlgSimEngine->SetOwner(pView);
		}
	}
	else
	{
		m_pDlgSimEngine->ClearText();
	}


	SelectedSimulationData simSelData;
	
	//getcur
	if(m_systemMode == EnvMode_LandSide)
	{
		simSelData.SelLandsideModel();
	}
	else if (m_systemMode == EnvMode_AirSide)
	{
		simSelData.SelAirsideModel();
	}
	else if (m_systemMode == EnvMode_Terminal)
	{
		simSelData.SelTerminalModel();
	}
	else if (m_systemMode == EnvMode_OnBoard)
	{
		simSelData.SelOnBoardModel();
	}

	CDlgSelectedSimulationModels dlgSimSel(simSelData);
	if(dlgSimSel.DoModal() == IDCANCEL)
		return;
	

	InputTerminal* pInputTerm = (InputTerminal*)&GetTerminal();
	if (pInputTerm && dlgSimSel.SelectedAirside())
	{
		bool bUnassignStand = false;

		int nCount = pInputTerm->flightSchedule->getCount();
		for (int i = 0; i < nCount;i++)
		{
			Flight * pFlt =  pInputTerm->flightSchedule->getItem(i);
			ALTObjectID standID = pFlt->getStand();
			if (standID.IsBlank())
			{
				bUnassignStand = true;
				//char strbuf[512]={'\0',};
				//pFlt->getFlightIDString(strbuf);
				//CString strID;
				//strID.Format("%s, ",_T(strbuf));
				//strFlights += strID;
				break;
			}
		}
		if (bUnassignStand)
		{			
			CString strFlights ="There are flights unassigned stand,they probably will be terminate when simulation,do you want to continue?";

			int nAnswer = AfxMessageBox(strFlights, MB_YESNO | MB_ICONQUESTION);
			if (nAnswer == IDNO)
				return;
		}
	}

	
	{
		if(m_eAnimState == anim_none && (dlgSimSel.SelectedAirside() || dlgSimSel.SelectedLandside() 
			|| dlgSimSel.SelectedTerminal() || dlgSimSel.SelectedOnBoard()))
		{
			
			//ASSERT( m_pDlgSimEngine );
			m_pDlgSimEngine->setSelectedSimulationData(simSelData);
			if(!::IsWindow(m_pDlgSimEngine->m_hWnd))
			{
				m_pDlgSimEngine->SetTermPlanDoc(this);
				if(m_pDlgSimEngine->Create(IDD_DIALOG_SIMENGINE, pView))
				{
					m_pDlgSimEngine->ShowWindow(SW_SHOW);
					m_pDlgSimEngine->SetTimer(1, 100, NULL);
				}
			}
			else
			{
				m_pDlgSimEngine->ShowWindow(SW_SHOW);	
				m_pDlgSimEngine->SetTimer(1, 100, NULL);
			}
	//
		}		
	}
}

void CTermPlanDoc::OnTrackersMovie()
{
	m_pModelessDlgMan->ShowMovieDialog(NULL);
}

void CTermPlanDoc::OnTrackersWalkthrough()
{
	CRender3DView* pRender3DView = GetRender3DView();
	if (pRender3DView && (m_animData.m_AnimationModels.IsOnBoardSel() || m_animData.m_AnimationModels.IsLandsideSel()))
	{
		//ASSERT(m_pDlgWalkthroughRender == NULL);
// 		C3DView* pView = Get3DView();
// 		ASSERT(pView);

// 		if(!pRender3DView->m_bAnimPax) {
// 			pRender3DView->m_bAnimPax=TRUE;
			pRender3DView->ClearSeledPaxList();
// 		}
		m_pModelessDlgMan->ShowDialogWalkThroughRender(pRender3DView);
		//m_pDlgWalkthroughRender = new CDlgWalkthroughRender(this, pRender3DView);
		//m_pDlgWalkthroughRender->Create(IDD_DIALOG_WALKTHROUGH, pRender3DView);
		//m_pDlgWalkthroughRender->ShowWindow(SW_SHOW);
	}
	else
	{
		//ASSERT(m_pDlgWalkthrough == NULL);
		C3DView* pView = Get3DView();
		ASSERT(pView);

		if(!pView->m_bAnimPax) {
			pView->m_bAnimPax=TRUE;
			pView->ClearSeledPaxList();
		}
		
		m_pModelessDlgMan->ShowDialogWalkThrough(pView);
		//m_pDlgWalkthrough = new CDlgWalkthrough(this, pView, pView);
		//m_pDlgWalkthrough->Create(IDD_DIALOG_WALKTHROUGH, pView);
		//m_pDlgWalkthrough->ShowWindow(SW_SHOW);
	}
}

void CTermPlanDoc::OnTrackersPanorama()
{	
	if(m_pDlgPanorama)
	{
		m_pDlgPanorama->DestroyWindow();
		delete m_pDlgPanorama;
		m_pDlgPanorama = NULL;
	}
	
	int nAptID  = InputAirside::GetAirportID(GetProjectID());
	m_pDlgPanorama = new CDlgPanorama(GetProjectID(),nAptID ,Get3DView());
	m_pDlgPanorama->Create(CDlgPanorama::IDD, Get3DView());
	m_pDlgPanorama->ShowWindow(SW_SHOW);	
}


void CTermPlanDoc::OnSelectPAX(UINT nPaxID, const CString& sDesc)
{
	if(m_pModelessDlgMan) {
		m_pModelessDlgMan->OnSelectPax(nPaxID,sDesc);
	}
}

void CTermPlanDoc::SetSelectPaxID(UINT nPaxID)
{
	C3DView* pView =  Get3DView();
	if (pView == NULL)
		return;
	pView->SetSelectPaxID(nPaxID);
}

//////////////////////////////////////////////////////////////////////////
//void CTermPlanDoc::SetLoadReportType( LoadReportType _type )
//{
//	m_arcReportManager.SetLoadReportType(_type);
//
//}
//
//void CTermPlanDoc::SetUserLoadReportFile( const CString& _strFile )
//{
//	m_arcReportManager.SetUserLoadReportFile(_strFile);
//}
//
//LoadReportType CTermPlanDoc::GetLoadReportType(void) const
//{
//	return m_arcReportManager.GetLoadReportType();
//}
//
//const CString& CTermPlanDoc::GetUserLoadReportFile( void ) const
//{
//	return m_arcReportManager.GetUserLoadReportFile(); move to CARCReportManager
//}

bool CTermPlanDoc::clearLogsIfNecessary()
{	
	int iPreResult = GetTerminal().getPreSimResult();
	int iCurResult = GetTerminal().getCurrentSimResult();

	if( iPreResult != iCurResult )
	{
		GetTerminal().clearLogs();
		GetTerminal().setPreSimResult( iCurResult );
		GetAirsideSimLogs().ClearLogs();
		GetLandsideSimLogs().ClearLogs();
		return true;
	}

	return false;
}

void CTermPlanDoc::OnFireImpact()
{
	ElapsedTime et(m_animData.nLastAnimTime/100L);// 1/100 Second
	StopAnimation();
	UpdateAllViews(NULL);
	POSITION pos = GetFirstViewPosition();
	CView* pView = GetNextView(pos);
	
	m_arcport.SetFireEvacuation(et);
	m_arcport.SetFireOccurFlag(TRUE);
	CSimEngineDlg* pSEdlg=new CSimEngineDlg( pView );
	CDlgFireImpact* pFIdlg = new CDlgFireImpact( pView );
	pFIdlg->SetSimEngineDlg(pSEdlg);
	
	pSEdlg->SetTermPlanDoc(this); // by aivin add.

	pSEdlg->Create(CSimEngineDlg::IDD);
	SelectedSimulationData simSelData;
	simSelData.SelTerminalModel();
	pSEdlg->setSelectedSimulationData(simSelData);

	pSEdlg->SetTimer(1, 100, NULL);

	pFIdlg->Create(CDlgFireImpact::IDD);
	pFIdlg->ShowWindow(SW_SHOW);
	pFIdlg->RunModalLoop();
	delete pFIdlg;
	pFIdlg = NULL;
	//dlg.DoModal();

	// Start Animation-----------------------------------------------------------------------
	if(m_eAnimState == anim_none)
	{ //start animation
		//fill animation data structure
		//get perf counter frequency
		LARGE_INTEGER lVal;
		//if(QueryPerformanceFrequency(&(m_animData.nPerfTickFreq)))
		if(QueryPerformanceFrequency(&lVal))
		{
			
			m_animData.nPerfTickFreq = lVal;
			//read PAX desc data (compute firstentrytime and lastexittime)
			long nFirstInTime, nLastOutTime;
			int iCurSimResult = GetTerminal().getCurrentSimResult();
			if( iCurSimResult>=0 )	// have valid sim_result
			{
				GetTerminal().GetSimReportManager()->SetCurrentSimResult( iCurSimResult );
				CSimItem* pItem =  GetTerminal().GetSimReportManager()->getSimItem( iCurSimResult );
				assert( pItem );
				if(ReadPAXDescriptions(&nFirstInTime, &nLastOutTime)) 
				{
					nFirstInTime = m_animData.nLastAnimTime;
					nLastOutTime = pItem->getEndTime();
					
					//m_animData.nAnimSpeed = 100;	//10 speed;
					m_animData.nFirstEntryTime = nFirstInTime;
					m_animData.nLastExitTime = nLastOutTime;
					m_animData.nAnimStartTime = nFirstInTime;
					m_animData.nAnimEndTime = nLastOutTime;
					CMainFrame* pMF = GetMainFrame();
					if(m_animData.nAnimSpeed>0)
					switch(m_animData.nAnimSpeed)
					{
					case 10:
						GetMainFrame()->SendMessage(WM_SETANIMATIONSPEED, (WPARAM) ID_ANIMATION_SET1SPEED, NULL);
						break;
					case 50:
						GetMainFrame()->SendMessage(WM_SETANIMATIONSPEED, (WPARAM) ID_ANIMATION_SET5SPEED, NULL);
						break;
					case 100:
						GetMainFrame()->SendMessage(WM_SETANIMATIONSPEED, (WPARAM) ID_ANIMATION_SET10SPEED, NULL);
						break;
					case 250:
						GetMainFrame()->SendMessage(WM_SETANIMATIONSPEED, (WPARAM) ID_ANIMATION_SET25SPEED, NULL);
						break;
					case 500:
						GetMainFrame()->SendMessage(WM_SETANIMATIONSPEED, (WPARAM) ID_ANIMATION_SET50SPEED, NULL);
						break;
					case 1000:
						GetMainFrame()->SendMessage(WM_SETANIMATIONSPEED, (WPARAM) ID_ANIMATION_SET100SPEED, NULL);
						break;
					default:
						GetMainFrame()->SendMessage(WM_SETANIMATIONSPEED, (WPARAM) ID_ANIMATION_SETCUSTOMSPEED, NULL);
						break;
					}
				}
				else 
				{
					AfxMessageBox("ERROR reading PAX Descriptions", MB_OK|MB_ICONERROR);
				}
			}
			else
			{
				AfxMessageBox("No Valid SimResult!", MB_OK|MB_ICONERROR);
			}
			
		}
		else 
		{
			DWORD dwError = GetLastError();
			AfxMessageBox(_T("Your system does not support the high-resolution timers required for ARCTERM animation."), MB_OK|MB_ICONERROR);
		}
		
		if(m_bMobileElementDisplay)
		{			
			StartAnimView();
		}
		if(m_bActivityDensityDisplay) 
		{
			m_pADData = new CActivityDensityData();
			m_pADData->BuildData(m_animData,&GetTerminal(),m_adParams);
			CDensityGLView* pDAView = NULL;
			if((pDAView = GetDensityAnimView()) == NULL) 
			{
				GetMainFrame()->CreateOrActivateFrame(theApp.m_pDensityAnimTemplate, RUNTIME_CLASS(CDensityGLView));
				pDAView = GetDensityAnimView();
			}
			ASSERT(pDAView != NULL);
		}
		if(!m_bActivityDensityDisplay && !m_bMobileElementDisplay) 
		{
			AfxMessageBox(_T("No animation has been selected to run.\nCommand cancelled."), MB_OK|MB_ICONINFORMATION);
			return;
		}
		ANIMTIMEMGR->EnrollDocument(this);
		//p3DView->GenerateAnimBKTexture();
		m_eAnimState = anim_playF;
		
		GetMainFrame()->SendMessage(UM_SETTOOLBTN_RUNDELTA_DELTA);
		//GetMainFrame()->GetControlBar(ID_ANIMATIONTOOLBAR)
		//if(!QueryPerformanceCounter(&m_animData.nLastSystemTime))
		VERIFY(QueryPerformanceCounter(&lVal));
		m_animData.nLastSystemTime = lVal;
		
		m_animData.nLastAnimTime = m_animData.nAnimStartTime;
		ANIMTIMEMGR->StartTimer();
	}
	else 
	{ //stop animation
		ANIMATIONSTATE eOldState = m_eAnimState;
		m_eAnimState = anim_pause;
		const CString s = "Are you sure you want to stop the animation?\n";
		if(AfxMessageBox(s,MB_YESNO | MB_ICONQUESTION) == IDYES) 
		{
			StopAnimation();
			UpdateAllViews(NULL);
		}
		else
			m_eAnimState = eOldState;
	}	
}

void CTermPlanDoc::OnUpdateFireImpact(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	
	//pCmdUI->Enable(m_eAnimState != anim_none);
}


CProcessor2* CTermPlanDoc::getProcessor2ByProcessor( const Processor* _pProc )
{
	assert( _pProc );
	CProcessor2* pProc2 = NULL;
	for( int i = 0; i<static_cast<int>(GetCurrentPlacement()->m_vDefined.size()); i++ )
	{
		pProc2 = GetCurrentPlacement()->m_vDefined[i];
		if( pProc2->GetProcessor() == _pProc )
			return pProc2;
	}
	
	return NULL;
}


int CTermPlanDoc::CreateProcessor2ByProcList( std::vector<ProcCopyPair>& _procPairList )
{
	int iCreatedNum		= 0;
	int iListSize		= _procPairList.size();
	if( iListSize == 0)
		return iCreatedNum;
	
	char buf[256];
	//get placements node
	CTVNode* pPlacementsNode = ProcessorsNode();
	
	for( int i=0; i<iListSize; i++ )
	{
		// get src processor2
		Processor* pSrcProc = _procPairList[i].first;
		CProcessor2* pSrcProc2 = getProcessor2ByProcessor( pSrcProc );
		assert( pSrcProc2 );
		
		//create a CProcessor2 object
		CProcessor2* pNewProc2 = new CProcessor2();
		
		//set floor to active floor
		pNewProc2->SetFloor( pSrcProc2->GetFloor() );
		
		//set location, scale and rotation
		Point offset = _procPairList[i].second->GetServiceLocation() - pSrcProc->GetServiceLocation();
		ARCVector3 vOffset2D( offset.getX(), offset.getY(), offset.getZ() );
		ARCVector3 vSrc2D;
		vSrc2D = pSrcProc2->GetLocation();
		vOffset2D[VZ]=0.0;
		pNewProc2->SetLocation( vSrc2D + vOffset2D );//offset
		ARCVector3 v3D;
		pSrcProc2->GetScale(v3D);
		pNewProc2->SetScale(v3D);
		pNewProc2->SetRotation(pSrcProc2->GetRotation());
		
		//set shape
		pNewProc2->SetShape( pSrcProc2->GetShape() );
		//set selection id
		pNewProc2->SetSelectName( GetUniqueNumber() );
		//set dispproperties
		pNewProc2->m_dispProperties = pSrcProc2->m_dispProperties;
		
		// set Processor of Processor2
		//ProcessorID* pNewID = GetNextProcessorID( pSrcProc->getID(), GetTerminal().procList );
		//_procPairList[i].second->init( *pNewID );
		pNewProc2->SetProcessor( _procPairList[i].second );
		
		//GetTerminal().procList->addProcessor(_procPairList[i].second);
		//GetTerminal().procList->setIndexes();			
		//GetTerminal().procList->saveDataSet(m_ProjInfo.path, false);
		
		CTVNode* pOrigProcNode =m_msManager.FindNodeByData((DWORD) pSrcProc2 );
		CTVNode* pParent = (CTVNode*) pOrigProcNode->Parent();
		_procPairList[i].second->getID()->printID(buf);
		//remove everything before and including last '-' is buf
		char* pLastPartOfProcName = strrchr(buf,'-');
		CTVNode* pNode = new CTVNode(++pLastPartOfProcName, IDR_CTX_PROCESSOR);
		pNode->m_dwData = (DWORD) pNewProc2;
		pParent->AddChild(pNode);
		
		GetCurrentPlacement()->m_vDefined.insert( GetCurrentPlacement()->m_vDefined.begin(), pNewProc2 );
		GetCurrentPlacement()->saveDataSet(m_ProjInfo.path, false);
		
		iCreatedNum++;
	}
	
	UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pPlacementsNode );
	
	return iCreatedNum;
}

void CTermPlanDoc::CreateMovieFromLiveRecording()
{
	ANIMTIMEMGR->EndTimer();
	CDlgMovieWriter dlg(this, CDlgMovieWriter::LIVE, -1, Get3DView());
	dlg.DoFakeModal();
	ANIMTIMEMGR->StartTimer();
	/*
	CDlgMovieWriter2* pDlgMW = new CDlgMovieWriter2(this, Get3DView());
	pDlgMW->Create(IDD_DIALOG_MOVIEWRITER, Get3DView());
	pDlgMW->ShowWindow(SW_SHOW);
	pDlgMW->RecordLive();
	pDlgMW->RunModalLoop();
	*/
	
}

void CTermPlanDoc::OnAnimationRecord() 
{
	if(!m_bRecord) { //start recording

		if(!m_pRecordedCameras)
			m_pRecordedCameras = new CRecordedCameras;
		ASSERT(m_pRecordedCameras);
		RECORDMGR->SetFrameRate(m_videoParams.GetFPS());
		RECORDMGR->EnrollDocument(this);
		m_pRecordedCameras->Start(m_animData.nLastAnimTime,m_videoParams.GetFPS());
		RECORDMGR->StartTimer();
		m_bRecord = TRUE;
	}
	else {
		StopRecording(TRUE);
	}
}

void CTermPlanDoc::StopRecording(BOOL bAndWriteMovie)
{
	m_pRecordedCameras->End(m_animData.nLastAnimTime);
	RECORDMGR->EndTimer();
	m_bRecord = FALSE;
	if(bAndWriteMovie)
		CreateMovieFromLiveRecording();
}

void CTermPlanDoc::OnUpdateAnimationRecord(CCmdUI* pCmdUI) 
{
	if(!m_bRecord)
		pCmdUI->SetText("Start Recording");
	else
		pCmdUI->SetText("Stop Recording");

	pCmdUI->Enable(m_eAnimState!=anim_none);
}

void CTermPlanDoc::OnAnimationRecsettings() 
{
	CDlgLiveRecordParams d(this);
	if(d.DoModal()==IDOK) {
		m_videoParams.saveDataSet(m_ProjInfo.path, TRUE);
	}
	else {
		m_videoParams.loadDataSet(m_ProjInfo.path);
	}
}

void CTermPlanDoc::OnUpdateTrackersMovie(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( GetIRenderView()!=NULL );
}

void CTermPlanDoc::OnUpdateTrackersWalkthrough(CCmdUI* pCmdUI)
{	
	pCmdUI->Enable( GetIRenderView()!=NULL && m_eAnimState!=anim_none);
}

//LRESULT CTermPlanDoc::OnMsgDestroyDlgWalkthrough(WPARAM wParam, LPARAM lParam)
//{
//	//m_pDlgWalkthrough->SendMessage(MB_OK, 0L, 0L);
//	delete m_pDlgWalkthrough;
//	m_pDlgWalkthrough = NULL;
//	return TRUE;
//}
//
//void CTermPlanDoc::DestroyDlgWalkthroughRender()
//{
//	delete m_pDlgWalkthroughRender;
//	m_pDlgWalkthroughRender = NULL;
//}

void CTermPlanDoc::OnOpaqueFloor()
{
	ASSERT(m_pSelectedNode!=NULL /*&& m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR*/);
	CFloor2* pFloor = GetCurModeFloor().m_vFloors[(int)m_pSelectedNode->m_dwData];
	// toggle opaque
	pFloor->IsOpaque(!pFloor->IsOpaque());
	UpdateViewSharpTextures();
	UpdateAllViews(NULL);
	UpdateFloorInRender3DView(pFloor);
	GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
}

void CTermPlanDoc::OnShowHideMap()
{
	ASSERT(m_pSelectedNode!=NULL /*&& m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR*/);
	CFloor2* pFloor = GetCurModeFloor().m_vFloors[(int)m_pSelectedNode->m_dwData];
	// toggle show map
	pFloor->IsShowMap(!pFloor->IsShowMap());	
	UpdateViewSharpTextures();
	UpdateAllViews(NULL);
	UpdateFloorInRender3DView(pFloor);
	GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
}

void CTermPlanDoc::OnFloorThickness()
{
	ASSERT(m_pSelectedNode!=NULL /*&& m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR*/);
	CFloor2* pFloor = GetCurModeFloor().m_vFloors[(int)m_pSelectedNode->m_dwData];
	// get floor thickness
	CDlgFloorThickness dlg(pFloor->Thickness(),this);
	if(IDOK == dlg.DoModal()) {
		// TRACE("Set floor thickness: %.2f\n", dlg.GetThickness());
		pFloor->Thickness(dlg.GetThickness());
		UpdateViewSharpTextures();
		UpdateAllViews(NULL);
		UpdateFloorInRender3DView(pFloor);
		GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
	}
}

DistanceUnit CTermPlanDoc::ConvertLogZ2RealZ( DistanceUnit _dZ )const
{
	int nFloorCount = GetCurModeFloor().GetCount();
	std::vector<double> vFloorAltitudes;
	for(int i=0; i<nFloorCount; i++) 
	{
		vFloorAltitudes.push_back( GetCurModeFloor().m_vFloors[i]->RealAltitude() );		
	}

	int iFloor = static_cast<int>(_dZ / SCALE_FACTOR) ;
	if( iFloor < nFloorCount -1 )
	{
		DistanceUnit dFloorHeight = vFloorAltitudes[ iFloor +1 ] - vFloorAltitudes[ iFloor ] ;
		DistanceUnit dDlt = ( _dZ - iFloor * SCALE_FACTOR ) / SCALE_FACTOR * dFloorHeight ;
		return dDlt + vFloorAltitudes[ iFloor ] ;			
	}
	else
	{
		return vFloorAltitudes[ nFloorCount -1 ] ;
	}
}
DistanceUnit CTermPlanDoc::ConvertRealZ2LogZ( DistanceUnit _dZ )const
{
	int nFloorCount = GetCurModeFloor().GetCount();
	std::vector<double>vFloorAltitudes;
	for(int i=0; i<nFloorCount; i++) 
	{
		vFloorAltitudes.push_back( GetCurModeFloor().m_vFloors[i]->RealAltitude() );		
	}

	int iFloorIdx = -1;
	for( int j=nFloorCount-1; j>=0; --j )
	{
		if( _dZ >= vFloorAltitudes[j] )
		{
			iFloorIdx = j;		
			break;
		}
	}

	ASSERT( iFloorIdx >= 0 );

	if( iFloorIdx < nFloorCount -1 )
	{
		DistanceUnit dDltZ = ( _dZ - vFloorAltitudes[ iFloorIdx ] ) / ( vFloorAltitudes[iFloorIdx + 1 ] - vFloorAltitudes[ iFloorIdx ] ) * SCALE_FACTOR;
		return iFloorIdx * SCALE_FACTOR + dDltZ;
	}
	else
	{
		return iFloorIdx * SCALE_FACTOR;
	}
}

void CTermPlanDoc::OnClickInfoToolBtn()
{
	C3DView* pView = Get3DView();
	if(pView)
		pView->GetParentFrame()->ToggleInfoBtnActive();
}

void CTermPlanDoc::OnUpdateInfoToolBtn(CCmdUI* pCmdUI)
{
	C3DView* pView = Get3DView();
	if(pView) {
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(pView->GetParentFrame()->IsInfoBtnActive());
	}
	else {
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}

void CTermPlanDoc::OnShowAllFloors()
{
	ASSERT(m_pSelectedNode!=NULL /*&& m_pSelectedNode->m_nIDResource == IDR_CTX_FLOORS*/);
	int nFloorCount =  GetCurModeFloor().GetCount();
	for(int i=0; i<nFloorCount; i++) {
		CFloor2* pFloor = GetCurModeFloor().m_vFloors[i];
		pFloor->IsVisible(TRUE);
	}
	UpdateViewSharpTextures();
	UpdateAllViews(NULL);
	UpdateAllFloorsInRender3DView();
	GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
}

void CTermPlanDoc::OnShowAllFloorMaps()
{
	ASSERT(m_pSelectedNode!=NULL/* && m_pSelectedNode->m_nIDResource == IDR_CTX_FLOORS*/);
	int nFloorCount =  GetCurModeFloor().GetCount();
	for(int i=0; i<nFloorCount; i++) {
		CFloor2* pFloor =  GetCurModeFloor().m_vFloors[i];
		pFloor->IsShowMap(TRUE);
	}
	UpdateViewSharpTextures();
	UpdateAllViews(NULL);
	UpdateAllFloorsInRender3DView();
	GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
}

void CTermPlanDoc::OnShowAllFloorGrids()
{
	ASSERT(m_pSelectedNode!=NULL /*&& m_pSelectedNode->m_nIDResource == IDR_CTX_FLOORS*/);
	int nFloorCount =  GetCurModeFloor().GetCount();
	for(int i=0; i<nFloorCount; i++) {
		CFloor2* pFloor = GetCurModeFloor().m_vFloors[i];
		pFloor->IsGridVisible(TRUE);
	}
	UpdateViewSharpTextures();
	UpdateAllViews(NULL);
	UpdateAllFloorsInRender3DView();
	GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
}

void CTermPlanDoc::OnHideAllFloors()
{
	ASSERT(m_pSelectedNode!=NULL /*&& m_pSelectedNode->m_nIDResource == IDR_CTX_FLOORS*/);
	int nFloorCount =  GetCurModeFloor().GetCount();
	for(int i=0; i<nFloorCount; i++) {
		CFloor2* pFloor =  GetCurModeFloor().m_vFloors[i];
		pFloor->IsVisible(FALSE);
	}
	UpdateViewSharpTextures();
	UpdateAllViews(NULL);
	UpdateAllFloorsInRender3DView();
	GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
}

void CTermPlanDoc::OnHideAllFloorMaps()
{
	ASSERT(m_pSelectedNode!=NULL /*&& m_pSelectedNode->m_nIDResource == IDR_CTX_FLOORS*/);
	int nFloorCount =  GetCurModeFloor().GetCount();
	for(int i=0; i<nFloorCount; i++) {
		CFloor2* pFloor = GetCurModeFloor().m_vFloors[i];
		pFloor->IsShowMap(FALSE);
	}
	UpdateViewSharpTextures();
	UpdateAllViews(NULL);
	UpdateAllFloorsInRender3DView();
	GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
}

void CTermPlanDoc::OnHideAllFloorGrids()
{
	ASSERT(m_pSelectedNode!=NULL/* && m_pSelectedNode->m_nIDResource == IDR_CTX_FLOORS*/);
	int nFloorCount =  GetCurModeFloor().GetCount();
	for(int i=0; i<nFloorCount; i++) {
		CFloor2* pFloor =  GetCurModeFloor().m_vFloors[i];
		pFloor->IsGridVisible(FALSE);
	}
	UpdateViewSharpTextures();
	UpdateAllViews(NULL);
	UpdateAllFloorsInRender3DView();
	GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
}

void CTermPlanDoc::OnAllFloorsThickness()
{
	ASSERT(m_pSelectedNode!=NULL /*&& m_pSelectedNode->m_nIDResource == IDR_CTX_FLOORS*/);
	int nFloorCount = GetCurModeFloor().GetCount();
	for(int i=0; i<nFloorCount; i++) {
		CFloor2* pFloor =  GetCurModeFloor().m_vFloors[i];
		pFloor->IsOpaque(TRUE);
	}
	UpdateViewSharpTextures();
	UpdateAllViews(NULL);
	UpdateAllFloorsInRender3DView();
	GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
}

void CTermPlanDoc::OnAllFloorsMonochrome()
{
	ASSERT(m_pSelectedNode!=NULL/* && m_pSelectedNode->m_nIDResource == IDR_CTX_FLOORS*/);
	CColorDialog dlgColor(RGB(0,0,0), CC_ANYCOLOR | CC_FULLOPEN);
	if(dlgColor.DoModal() == IDOK) {
		int nFloorCount =  GetCurModeFloor().GetCount();
		for(int i=0; i<nFloorCount; i++) {
			CFloor2* pFloor =  GetCurModeFloor().GetFloor2(i);
			pFloor->SetMonochromeColor(dlgColor.GetColor());
			pFloor->IsMonochrome(!pFloor->IsMonochrome());
			pFloor->InvalidateTexture();
		}
		GetCurModeFloor().saveDataSet(m_ProjInfo.path, true);
		UpdateViewSharpTextures();
		UpdateAllFloorsInRender3DView();
	}
	UpdateAllViews(NULL);
}

void CTermPlanDoc::UpdateProjModifyTime( void )
{
	m_ProjInfo.modifytime = CTime::GetCurrentTime();
	//SetModifiedFlag( TRUE );
	m_bNeedToSaveProjFile = TRUE;
}

void CTermPlanDoc::OnHideProcTags()
{
	m_displayoverrides.m_pbProcDisplayOverrides[PDP_DISP_PROCNAME] = !m_displayoverrides.m_pbProcDisplayOverrides[PDP_DISP_PROCNAME];
	m_displayoverrides.saveDataSet(m_ProjInfo.path, false);
	UpdateAllViews(NULL);
}

void CTermPlanDoc::OnHideACTags()
{
	m_bHideACTags=!m_bHideACTags;
}

void CTermPlanDoc::OnUpdateHideProcTags(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(!m_displayoverrides.m_pbProcDisplayOverrides[PDP_DISP_PROCNAME]);
}

void CTermPlanDoc::OnUpdateHideACTags(CCmdUI* pCmdUI)
{
	if(m_eAnimState==anim_none) {
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
	}
	else {
		pCmdUI->SetCheck(!m_bHideACTags);
		pCmdUI->Enable(TRUE);
	}
}

void CTermPlanDoc::OnHideTaxiwayProc()
{
	m_bdisplayTaxiDir = !m_bdisplayTaxiDir;
	UpdateAllViews(NULL);
}
void CTermPlanDoc::OnUpdateHideTaxiwayProc(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bdisplayTaxiDir);
}

void CTermPlanDoc::UpdateEnvandSimMode()
{
	InitActiveFloorCB();

	//GetTerminal().SetSystemMode(m_systemMode);

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	ToolBarUpdateParameters updatePara;
	updatePara.m_operatType = ToolBarUpdateParameters::ChangeEnvMode;
	updatePara.m_nCurMode = m_systemMode;
	updatePara.m_pDoc = this;
	pMainFrame->SendMessage(UM_SHOW_CONTROL_BAR,1,(LPARAM)&updatePara) ;

	if (m_simType == SimType_Mathmatical)
	{
		CMDIChildWnd* pNewFrame	= (CMDIChildWnd*)(theApp.m_pMathematicTemplate->CreateNewFrame(this, NULL));
		theApp.m_pMathematicTemplate->InitialUpdateFrame(pNewFrame, this);
		
		CMDIChildWnd* pActiveChildFrame = pMainFrame->MDIGetActive();
		while (pActiveChildFrame)
		{ 
			if (pActiveChildFrame != pNewFrame)
				pActiveChildFrame->DestroyWindow();
			pMainFrame->MDINext();
			CMDIChildWnd* pTempMDIChild = pMainFrame->MDIGetActive();
			pActiveChildFrame = (pTempMDIChild == pActiveChildFrame) ? NULL : pTempMDIChild;
		}

		//will be deleted
		pNewFrame = (CMDIChildWnd*)(theApp.m_pProcessFlowTemplate->CreateNewFrame(this, NULL));
		theApp.m_pProcessFlowTemplate->InitialUpdateFrame(pNewFrame, this);

		//end of
		BOOL bLoad = LoadMathematicData();

		UpdateAllViews(NULL);
	}
	else // MontCarlo
	{
		pMainFrame->SendMessage(WM_COMMAND, ID_VIEW_DETAILVIEW);
		
		// if the processflow window was created then destroy it.
		POSITION pos = GetFirstViewPosition();
		bool b1 = false, b2 = false, b3 = false;
		while (pos != NULL)
		{
			CView* pView = GetNextView(pos);
			if( !pView )
				break;

			if(pView->IsKindOf(RUNTIME_CLASS(CRepControlView)))
				b1 = true;
			if(pView->IsKindOf(RUNTIME_CLASS(CRepGraphView)))
				b2 = true;
			if(pView->IsKindOf(RUNTIME_CLASS(CRepListView)))
				b3 = true;
			if (pView->IsKindOf(RUNTIME_CLASS(CProcessFlowView)) 
				|| pView->IsKindOf(RUNTIME_CLASS(CMathematicView))
//				|| pView->IsKindOf(RUNTIME_CLASS(CAirsideMSV))
//				|| pView->IsKindOf(RUNTIME_CLASS(CLandsideMSV))
				)
			{
				pView->GetParentFrame()->DestroyWindow();
			}

			if(b1&&b2&&b3)
			{
				pView->GetParentFrame()->DestroyWindow();
				b1 = b2 = b3 = false;
			}
		}
				
		if (m_systemMode == EnvMode_Terminal)
		{
			pMainFrame->SendMessage(WM_COMMAND, ID_VIEW_CHANGEENVVIEW);
			UpdateAllViews(NULL, UPDATETREE_TERMINAL);
		}
		else if (m_systemMode == EnvMode_AirSide)
		{
//			m_airsideInput.SetProjName(m_ProjInfo.name);
			//m_airsideInput.ReadAllData();
			pMainFrame->SendMessage(WM_COMMAND, ID_VIEW_CHANGEENVVIEW);
			UpdateAllViews(NULL, UPDATETREE_AIRSIDE);

			/*
			CMDIChildWnd* pNewFrame	= (CMDIChildWnd*)(theApp.m_pAirsideMSVTemplate->CreateNewFrame(this, NULL));
			theApp.m_pAirsideMSVTemplate->InitialUpdateFrame(pNewFrame, this);

			CMDIChildWnd* pActiveChildFrame = pMainFrame->MDIGetActive();
			while (pActiveChildFrame)
			{ 
				if (pActiveChildFrame != pNewFrame)
					pActiveChildFrame->DestroyWindow();
				pMainFrame->MDINext();
				CMDIChildWnd* pTempMDIChild = pMainFrame->MDIGetActive();
				pActiveChildFrame = (pTempMDIChild == pActiveChildFrame) ? NULL : pTempMDIChild;
			}
			*/
		}
		else if (m_systemMode == EnvMode_LandSide)
		{
			pMainFrame->SendMessage(WM_COMMAND, ID_VIEW_CHANGEENVVIEW);
			UpdateAllViews(NULL, UPDATETREE_LANDSIDE);

/*			CMDIChildWnd* pNewFrame	= (CMDIChildWnd*)(theApp.m_pLandsideMSVTemplate->CreateNewFrame(this, NULL));
			theApp.m_pLandsideMSVTemplate->InitialUpdateFrame(pNewFrame, this);

			CMDIChildWnd* pActiveChildFrame = pMainFrame->MDIGetActive();
			while (pActiveChildFrame)
			{ 
				if (pActiveChildFrame != pNewFrame)
					pActiveChildFrame->DestroyWindow();
				pMainFrame->MDINext();
				CMDIChildWnd* pTempMDIChild = pMainFrame->MDIGetActive();
				pActiveChildFrame = (pTempMDIChild == pActiveChildFrame) ? NULL : pTempMDIChild;
			}
*/		}
		else if (m_systemMode == EnvMode_OnBoard)
		{
			pMainFrame->SendMessage(WM_COMMAND, ID_VIEW_CHANGEENVVIEW);
			UpdateAllViews(NULL, UPDATETREE_ONBOARD);

		}
		else if (m_systemMode == EnvMode_Cargo)
		{
			pMainFrame->SendMessage(WM_COMMAND, ID_VIEW_CHANGEENVVIEW);
			UpdateAllViews(NULL, UPDATETREE_ONBOARD);
		}
		else if (m_systemMode == EnvMode_Environment)
		{
			pMainFrame->SendMessage(WM_COMMAND, ID_VIEW_CHANGEENVVIEW);
			UpdateAllViews(NULL, UPDATETREE_ONBOARD);
		}
		else if (m_systemMode == EnvMode_Financial)
		{
			pMainFrame->SendMessage(WM_COMMAND, ID_VIEW_CHANGEENVVIEW);
			UpdateAllViews(NULL, UPDATETREE_ONBOARD);
		}

	}
}

BOOL CTermPlanDoc::LoadMathematicData()
{
	CString strPath = m_ProjInfo.path + "\\Mathematic";

	CFileFind fd;
	BOOL bFind = fd.FindFile(strPath);
	if(!bFind)//not find the directory, create it
	{
		BOOL b = CreateDirectory(strPath, NULL);
	}

	CMathProcessDataSet dataProc;
	dataProc.SetProcesses(&m_vProcess);
	dataProc.loadDataSet(strPath);
	
	CMathFlowDataSet dataFlow;
	dataFlow.SetFlows(&m_vFlow);
	dataFlow.loadDataSet(strPath);

	POSITION pos = GetFirstViewPosition();
	CView* pView = GetNextView(pos);
	while(pView)
	{
		if(pView->IsKindOf(RUNTIME_CLASS(CMathematicView)))
		{
			((CMathematicView*)pView)->SetProcessPtr(&m_vProcess);
			((CMathematicView*)pView)->SetFlowPtr(&m_vFlow);
		}
		//------------------------will be deleted
		else if(pView->IsKindOf(RUNTIME_CLASS(CProcessFlowView)))
		{
			((CProcessFlowView*)pView)->SetMathFlowPtr(&m_vFlow);
			((CProcessFlowView*)pView)->InitTabWnd();
		}
		//------------------------end of
		pView = GetNextView(pos);
	}

	return TRUE;
}

BOOL CTermPlanDoc::SaveMathematicData()
{
	CString strPath = m_ProjInfo.path + "\\Mathematic";

	CFileFind fd;
	BOOL bFind = fd.FindFile(strPath);
	if(!bFind)//not find the directory, create it
		BOOL b = CreateDirectory(strPath, NULL);

	CMathProcessDataSet data;
	data.SetProcesses(&m_vProcess);
	data.saveDataSet(strPath, false);

	CMathFlowDataSet dataFlow;
	dataFlow.SetFlows(&m_vFlow);
	dataFlow.saveDataSet(strPath, false);

	//if processflowview not exists, create and show it
/*	bFind = false;
	POSITION pos  = GetFirstViewPosition();
	CView *pView = NULL;
	while(pos)
	{
		pView = GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CProcessFlowView)))
		{
			bFind = true;
			break;
		}
	}

	if(!bFind)
	{
		CMDIChildWnd* pNewFrame	= (CMDIChildWnd*)(theApp.m_pProcessFlowTemplate->CreateNewFrame(this, NULL));
		theApp.m_pProcessFlowTemplate->InitialUpdateFrame(pNewFrame, this);

		pos  = GetFirstViewPosition();
		while(pos)
		{
			pView = GetNextView(pos);
			if(pView->IsKindOf(RUNTIME_CLASS(CProcessFlowView)))
			{
				bFind = true;
				break;
			}
		}
	
		((CProcessFlowView*)pView)->SetMathFlowPtr(&m_vFlow);
		((CProcessFlowView*)pView)->InitTabWnd();
		((CProcessFlowView*)pView)->SaveFlowChartData();
	}
	else
	{
		((CProcessFlowView*)pView)->SaveFlowChartData();
	}
	//-----end create and show processflowview
*/
	return TRUE;
}

BOOL CTermPlanDoc::LoadMathResult()
{
	CString strPath = m_ProjInfo.path + "\\MathResult";
	m_MathResultManager.SetResultFileDir(strPath);
	m_MathResultManager.LoadResult();

	return TRUE;
}

bool CTermPlanDoc::CopyProcessorToDest(CProcessor2* _pProc, CTermPlanDoc* _pDestDoc, const int _nFloor)
{
	CProcessor2* pNewProc = new CProcessor2;
	
	if(!pNewProc->CopyProcToDest(this, _pDestDoc, _pProc, _nFloor))
	{
		delete pNewProc;
		return false;
	}

	return true;
}

bool CTermPlanDoc::CopyProcessors(CTermPlanDoc* _pDestDoc, const int _nFloor)
{
	int nCount = GetSelectProcessors().GetCount();
//	CPROCESSOR2LIST::iterator iter;
//	for (iter = GetSelectProcessors().begin(); iter != GetSelectProcessors().end(); iter++)
	for(int i=0; i < nCount; ++i)
	{
		CObjectDisplay *pObjDisplay = GetSelectedObjectDisplay(i);
		if(pObjDisplay &&pObjDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2 *pProc2 = (CProcessor2 *)pObjDisplay;
			CopyProcessorToDest(pProc2, _pDestDoc, _nFloor);
		}
	}

	if (_pDestDoc == this)
		_pDestDoc->UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*)_pDestDoc->ProcessorsNode());
	else
		_pDestDoc->UpdateAllViews(NULL);

	return true;
}

const CFloors& CTermPlanDoc::GetFloorByMode( EnvironmentMode mode ) const
{ 	
	if( mode == EnvMode_LandSide)
	{
		return m_landsidefloors;
	}
	return m_floors;
}
CFloors& CTermPlanDoc::GetFloorByMode( EnvironmentMode mode ) 
{ 	
	if( mode == EnvMode_LandSide)
	{
		return m_landsidefloors;
	}
	return m_floors;
}

CFloors& CTermPlanDoc::GetCurModeFloor()
{ 
	return GetFloorByMode( m_systemMode );
}

CCameras& CTermPlanDoc::GetCamerasByMode( EnvironmentMode mode ) 
{ 
	if( mode == EnvMode_AirSide )
		return GetTerminal().m_AirsideInput->GetCameras(); 
	/*if( mode == EnvMode_LandSide )
		return m_pLandsideDoc->GetCameras();*/
	return m_cameras;
}

CCameras& CTermPlanDoc::GetCurModeCameras()
{ 
	return GetCamerasByMode( m_systemMode );
}


C3DViewCameras& CTermPlanDoc::GetCurModeRenderCameras()
{
	return GetRenderCamerasByMode( m_systemMode );
}

C3DViewCameras& CTermPlanDoc::GetRenderCamerasByMode( EnvironmentMode mode )
{
	if( mode == EnvMode_AirSide )
		return m_airsideCameras; 
	if( mode == EnvMode_LandSide )
		return m_lansideCameras;
	if( mode == EnvMode_OnBoard )
		return m_onboardCameras;
	return m_terminalCameras;

}

void CTermPlanDoc::SaveCurModeRenderCameras()
{
	GetCurModeRenderCameras().SaveIntoDatabase(C3DViewCameras::enumEnvironment, m_systemMode);
}

void CTermPlanDoc::SaveAllCameras()
{
	m_terminalCameras.SaveIntoDatabase(C3DViewCameras::enumEnvironment, EnvMode_Terminal);
	m_airsideCameras.SaveIntoDatabase(C3DViewCameras::enumEnvironment, EnvMode_AirSide);
	m_lansideCameras.SaveIntoDatabase(C3DViewCameras::enumEnvironment, EnvMode_LandSide);
	m_onboardCameras.SaveIntoDatabase(C3DViewCameras::enumEnvironment, EnvMode_OnBoard);
}

void CTermPlanDoc::ReadAllCameras()
{
	ReadRenderCamerasByMode(EnvMode_Terminal);
	ReadRenderCamerasByMode(EnvMode_AirSide);
	ReadRenderCamerasByMode(EnvMode_LandSide);
	ReadRenderCamerasByMode(EnvMode_OnBoard);
}
void CTermPlanDoc::ReadRenderCamerasByMode( EnvironmentMode envMode )
{
	C3DViewCameras& cams = GetRenderCamerasByMode(envMode);
	if (C3DViewCameras::GetXMLDataID(C3DViewCameras::enumEnvironment, envMode)>0)
	{
		cams.ReadFromDatabase(C3DViewCameras::enumEnvironment, envMode);
	}
	else
	{
		GetCamerasByMode(envMode).Convert(cams);
	}
}

void CTermPlanDoc::GetAllCameras(std::vector<CCameras *>& vCamera)
{ 
	vCamera.push_back(&m_cameras);
	vCamera.push_back(&(GetTerminal().m_AirsideInput->GetCameras()));
	//vCamera.push_back(&(m_pLandsideDoc->GetCameras()));
}

CPlacement* CTermPlanDoc::GetCurrentPlacement(EnvironmentMode mode)// = EnvMode_Unknow
{
	if(mode == EnvMode_Unknow)
		mode = m_systemMode;

	CPlacement* pPlacement = NULL;

	switch(mode)
	{
	//case EnvMode_LandSide:
	//	pPlacement = m_pLandsideDoc->GetPlacementPtr();
	//	break;
	case EnvMode_Terminal:
		pPlacement = m_pPlacement;
		break;
	case EnvMode_AirSide:
		pPlacement = GetTerminal().m_AirsideInput->GetPlacementPtr();
		break;
	case EnvMode_OnBoard:
//		ASSERT(0);
		pPlacement = m_pPlacement;
		break;
	case EnvMode_All:
		pPlacement = m_pPlacement;
		break;
	default:
		pPlacement = m_pPlacement;
//		ASSERT(0);
	}

	return pPlacement;
}

//Get the index of floors number. the EnvMode_Terminal is base index 0, and EnvMode_AirSide index
//is base EnvMode_Terminal index count add one.
int CTermPlanDoc::GetDrawFloorsNumberByMode(EnvironmentMode mode)
{// get first floor index by mode.
	int curfloorindex = 0;

	if(mode == EnvMode_Unknow)
		curfloorindex = 0;
	else if(mode == EnvMode_Terminal)
		curfloorindex = 0;
	else if(mode == EnvMode_AirSide)
		curfloorindex = GetFloorByMode(EnvMode_Terminal).GetCount();
	else if(mode == EnvMode_LandSide)
		curfloorindex = GetDrawFloorsNumberByMode(EnvMode_AirSide) + GetFloorByMode(EnvMode_AirSide).GetCount();

	return curfloorindex;
}

EnvironmentMode CTermPlanDoc::GetModeByFloorsNumber( int szFloorNum )
{ 
	EnvironmentMode mode = EnvMode_Unknow;
	//return m_systemMode;
	if( szFloorNum < GetDrawFloorsNumberByMode(EnvMode_AirSide))
		mode = EnvMode_Terminal;
	else if( szFloorNum< GetDrawFloorsNumberByMode(EnvMode_LandSide))
	{// air side.
		mode = EnvMode_AirSide;
	}
	else
	{
		mode = EnvMode_LandSide;
	}

	return mode;
}

int CTermPlanDoc::GetAbsFloorIndexByMode(int floorcount, EnvironmentMode mode)
{
	int absfloorindex = floorcount;
	
	if(mode == EnvMode_Terminal)
		absfloorindex = floorcount - GetDrawFloorsNumberByMode(EnvMode_Terminal);
	else if(mode == EnvMode_AirSide)
		absfloorindex = floorcount - GetDrawFloorsNumberByMode(EnvMode_AirSide);
	else if(mode == EnvMode_LandSide)
		absfloorindex = floorcount - GetDrawFloorsNumberByMode(EnvMode_LandSide);

	return absfloorindex;
}
//ben 2005-9-5
CStructureList & CTermPlanDoc::GetCurStructurelist(){
	return GetStructurelistByMode(m_systemMode);
}
CStructureList & CTermPlanDoc::GetStructurelistByMode(EnvironmentMode mode){
	if (mode==EnvMode_Terminal){
		return *(GetTerminal().m_pStructureList);
	}else{
		return * (GetTerminal().m_AirsideInput->getpStructurelist());
	}
}
WallShapeList & CTermPlanDoc::GetWallShapeListByMode(EnvironmentMode mode){
	if (mode==EnvMode_Terminal){
		return *(GetTerminal().m_pWallShapeList);
	}else {
		return * (GetTerminal().m_AirsideInput->getpWallShapeList());
	}
}

bool CTermPlanDoc::UpdateStructureChange(bool bRefreshNodeView){
	 this->GetCurStructurelist().saveDataSet(this->m_ProjInfo.path, false);
	 if(bRefreshNodeView){
		 CNodeView * pNodeView=GetNodeView();
		 pNodeView->UpdateSurfaceNode();
	
	 }
	 return true;

}

void CTermPlanDoc::UpdateDeiceStandNode()
{

    int iType = DeiceBayProcessor;
	CTVNode* pGroupNode = m_msManager.m_msImplAirSide.FindProcTypeNode(iType);
	UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pGroupNode);

	iType = StandProcessor;
	 pGroupNode = m_msManager.m_msImplAirSide.FindProcTypeNode(iType);
	UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pGroupNode);

}

MobLogEntry CTermPlanDoc::GetMobLogEntry( int idx )
{
	PaxLog* pPaxLog = GetTerminal().paxLog;
	int nCount = pPaxLog->getCount();
	ASSERT( idx < nCount );
	
	MobLogEntry element;	
	pPaxLog->getItem(element, idx);

	return element;
}

AirsideFlightLogEntry CTermPlanDoc::GetAirsideFlightLogEntry( int idx )
{
	CAirsideSimLogs& airsideSimLogs = GetAirsideSimLogs();

	AirsideFlightLog& airsideFlightLog = airsideSimLogs.m_airsideFlightLog;
	int nCount = airsideFlightLog.getCount();
//	ASSERT( idx < nCount );

	AirsideFlightLogEntry element;
	element.SetEventLog(&(airsideSimLogs.m_airsideFlightEventLog));

	airsideFlightLog.getItem(element, idx);

	return element;
}

CString CTermPlanDoc::GetPaxTypeID(const MobDescStruct& _MDS)
{
	CString stmp;
	switch( _MDS.mobileType )
	{
	case 0:
		stmp.Format("Pax ID: %d", _MDS.id);
		break;
	case 1:
		stmp.Format("Visitor ID: %d (%d)", _MDS.id, _MDS.ownerID );
		break;
	case 2:
		stmp.Format("Checked Baggage ID: %d (%d)", _MDS.id, _MDS.ownerID);
		break;
	case 3:
		stmp.Format("Hand Bag ID: %d (%d)", _MDS.id, _MDS.ownerID);
		break;
	case 4:
		stmp.Format("Carte ID: %d (%d)", _MDS.id, _MDS.ownerID);
		break;
	case 5:
		stmp.Format("Wheelchair ID: %d (%d)", _MDS.id, _MDS.ownerID);
		break;
	case 6:
		stmp.Format("Animal ID: %d (%d)", _MDS.id, _MDS.ownerID);
		break;
	case 7:
		stmp.Format("Car ID: %d", _MDS.id);
		break;
	case 8:
	case 9:
	case 10:
		stmp.Format("%s ID: %d (%d)", GetTerminal().m_pMobElemTypeStrDB->GetString( _MDS.mobileType ), _MDS.id, _MDS.ownerID );
		break;;

	}
	
	int idx = -1;
	if(_MDS.depFlight != -1)
		idx = _MDS.depFlight;
	else if(_MDS.arrFlight != -1)
		idx = _MDS.arrFlight;
	if(idx != -1) {
		char buf[256];
		GetTerminal().flightLog->getFlightID( buf, idx );
		CString strTemp;
		strTemp.Format(",  %s",buf);
		stmp+=strTemp;

	}

	return stmp;
}

CString CTermPlanDoc::GetPaxTagFromID(UINT ii)
{
	if(EnvMode_Terminal == m_systemMode) {
		if(ii>=C3DView::SEL_FLIGHT_OFFSET)
			return "";

		MobLogEntry element;

		GetTerminal().paxLog->getItem(element, ii);
		const MobDescStruct& pds = element.GetMobDesc();

		return GetPaxTypeID(pds);
	}
	if(EnvMode_AirSide == m_systemMode) {
		if(ii>=C3DView::SEL_VEHICLE_OFFSET||ii<C3DView::SEL_FLIGHT_OFFSET)
			return "";
		ii -= C3DView::SEL_FLIGHT_OFFSET;
		AirsideFlightLogEntry fle = GetAirsideFlightLogEntry(ii);
		CString stmp;
		stmp.Format("Flight: %s", fle.GetAirsideDesc().flightID);
		return  stmp;
	}
	else {
		ASSERT(FALSE);
		return "";
	}

	/*
	AnimationData* pAnimData = &(pDoc->m_animData);
	long nTime = pAnimData->nLastAnimTime;
	long nStartTime = (long) element.getEntryTime();
	long nEndTime = (long) element.getExitTime();
	if(nStartTime <= nTime && nEndTime >= nTime) 
	{


	MobEventStruct pesB;
	MobEventStruct pesA;

	bool bRet = element.getEventsAtTime(nTime,pesA,pesB);
	if(bRet)
	{
	// interpolate position based on the two PaxEventStructs and the time
	int w = pesB.time - pesA.time;
	int d = pesB.time - nTime;
	double r = ((double) d) / w;
	float xpos = static_cast<float>((1-r)*pesB.x + r*pesA.x);
	float ypos = static_cast<float>((1-r)*pesB.y + r*pesA.y);
	int zpos = (int) pesB.z/100;

	return GetPaxTypeID(pds);
	}
	}
	return "";
	*/
}


CString CTermPlanDoc::GetMobTagFromID( CWalkthrough::WalkthroughMobType mobType, int nMobID )
{
	switch (mobType)
	{
	case CWalkthrough::Mob_Pax:
		{
// 			if(nMobID>=CRender3DView::SEL_FLIGHT_OFFSET)
// 				return "";

			MobLogEntry element;

			VERIFY(GetTerminal().paxLog->GetItemByID(element, nMobID));
			const MobDescStruct& pds = element.GetMobDesc();

			return GetPaxTypeID(pds);
		}
		break;
	case CWalkthrough::Mob_Flight:
		{
			//
			CAirsideSimLogs& airsideSimLogs = GetAirsideSimLogs();
			AirsideFlightLog& airsideFlightLog = airsideSimLogs.m_airsideFlightLog;
			AirsideFlightLogEntry fle;
			fle.SetEventLog(&(airsideSimLogs.m_airsideFlightEventLog));
			VERIFY(airsideFlightLog.GetItemByID(fle, nMobID));

			CString stmp;
			stmp.Format("Flight: %s", fle.GetAirsideDesc().flightID);
			return stmp;
		}
		break;
	case CWalkthrough::Mob_Vehicle:
		{
			LandsideVehicleLogEntry element;

			CLandsideSimLogs& LandsideSimLogs = GetLandsideSimLogs();
			LandsideVehicleLog& VehicleLog = LandsideSimLogs.getVehicleLog();		

			LandsideVehicleLogEntry fle;
			fle.SetEventLog(LandsideSimLogs.getVehicleEventLog());
			VERIFY(VehicleLog.GetItemByID(fle, nMobID));

			CString stmp;
			stmp.Format("Vehicle: %d", fle.GetVehicleDesc().vehicleID);
			return stmp;
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
		break;
	}
	return _T("");
}


void CTermPlanDoc::BuildPaxTag( DWORD _ti, const MobDescStruct& _MDS, MobEventStruct& _MESa, CString& _sPaxTag )
{
	static char buf[256];
	CString stmp;

	Terminal* pTerm = &GetTerminal();

	if(_ti & PAX_ID) {
		switch( _MDS.mobileType )
		{
		case 0:
			stmp.Format("\nPax ID: %d", _MDS.id);
			break;
		case 1:
			stmp.Format("\nVisitor ID: %d (%d)", _MDS.id, _MDS.ownerID );
			break;
		case 2:
			stmp.Format("\nChecked Baggage ID: %d (%d)", _MDS.id, _MDS.ownerID);
			break;
		case 3:
			stmp.Format("\nHand Bag ID: %d (%d)", _MDS.id, _MDS.ownerID);
			break;
		case 4:
			stmp.Format("\nCarte ID: %d (%d)", _MDS.id, _MDS.ownerID);
			break;
		case 5:
			stmp.Format("\nWheelchair ID: %d (%d)", _MDS.id, _MDS.ownerID);
			break;
		case 6:
			stmp.Format("\nAnimal ID: %d (%d)", _MDS.id, _MDS.ownerID);
			break;
		case 7:
			stmp.Format("\nCar ID: %d", _MDS.id);
			break;
		//case 8:
		//case 9:
		//case 10:
		//case 11:
		//case 12:
		//case 13:
		//case 14:
		//case 15:
		//case 16:
		default:
			stmp.Format("\n%s ID: %d (%d)", pTerm->m_pMobElemTypeStrDB->GetString( _MDS.mobileType), _MDS.id, _MDS.ownerID );
			break;
		}
		_sPaxTag=_sPaxTag+stmp;
	}
	if(_ti & PAX_FLIGHTID) {
		int idx = -1;
		if(_MDS.depFlight != -1)
		{
			idx = _MDS.depFlight;
			if(idx != -1) 
			{
				pTerm->flightLog->getFlightID( buf, idx, 'D' );
				stmp.Format("\nFlight ID: %s",buf);
				_sPaxTag = _sPaxTag+stmp;
			}
		}
		else if(_MDS.arrFlight != -1)
		{
			idx = _MDS.arrFlight;
			if(idx != -1) 
			{
				pTerm->flightLog->getFlightID( buf, idx, 'A' );
				stmp.Format("\nFlight ID: %s",buf);
				_sPaxTag = _sPaxTag+stmp;
			}
		}
	}
	if(_ti & PAX_PAXTYPE) {
		CMobileElemConstraint pc(pTerm);
		//pc.SetInputTerminal(pTerm);
		pc.setIntrinsicType(_MDS.intrinsic);
		for(int l=0; l<MAX_PAX_TYPES; l++)
			pc.setUserType(l,_MDS.paxType[l]);
		pc.SetTypeIndex( _MDS.mobileType );
		pc.screenPrint(buf,0,256);
		stmp.Format("\nPax Type: %s",buf);
		_sPaxTag = _sPaxTag+stmp;
	}
	if(_ti & PAX_GATE) {
		//int idx = -1;
		//if(_MDS.depFlight != -1)
		//	idx = _MDS.depFlight;
		//else if(_MDS.arrFlight != -1)
		//	idx = _MDS.arrFlight;
		//if(idx != -1) 
		//{
		//	FlightLogEntry fltLogEntry;
		//	pTerm->flightLog->getItem( fltLogEntry, idx );
		//	int nGateIdx = fltLogEntry.getGate();
		//	ProcLogEntry proLogEntry;
		//	pTerm->procLog->getItem( proLogEntry, nGateIdx );
		//	proLogEntry.SetOutputTerminal(pTerm);
		//	proLogEntry.getID( buf );
		//	stmp.Format("\nGate: %s",buf);
		//	_sPaxTag = _sPaxTag+stmp;
		//}
		FlightLogEntry fltLogEntry;
		ProcLogEntry   proLogEntry;
		int idx = -1,nGateIdx;
		if(_MDS.depFlight != -1)
		{
			idx = _MDS.depFlight;
			if (idx != -1)
			{
				pTerm->flightLog->getItem(fltLogEntry, idx);
				nGateIdx = fltLogEntry.getDepGate();
				pTerm->procLog->getItem(proLogEntry, nGateIdx);
				proLogEntry.SetOutputTerminal(pTerm);
				proLogEntry.getID(buf);
				stmp.Format("\nGate: %s",buf);
				_sPaxTag = _sPaxTag+stmp;
			}
		}
		else if (_MDS.arrFlight != -1)
		{
			idx = _MDS.arrFlight;
			if (idx != -1)
			{
				pTerm->flightLog->getItem(fltLogEntry, idx);
				nGateIdx = fltLogEntry.getArrGate();
				pTerm->procLog->getItem(proLogEntry, nGateIdx);
				proLogEntry.SetOutputTerminal(pTerm);
				proLogEntry.getID(buf);
				stmp.Format("\nGate: %s",buf);
				_sPaxTag = _sPaxTag+stmp;
			}

		}
	}
	if(_ti & PAX_GATETIME) {
		int idx = -1;
		if(_MDS.depFlight != -1)
			idx = _MDS.depFlight;
		else if(_MDS.arrFlight != -1)
			idx = _MDS.arrFlight;
		if(idx != -1) {
			FlightLogEntry fltLogEntry;
			pTerm->flightLog->getItem( fltLogEntry, idx );
			fltLogEntry.getGateTime().printTime( buf, FALSE );
			stmp.Format("\nGate Time: %s",buf);
			_sPaxTag = _sPaxTag+stmp;
		}
	}
	if(_ti & PAX_BAGCOUNT) {
		stmp.Format("\nBag Count: %d",_MDS.bagCount);
		_sPaxTag = _sPaxTag+stmp;
	}
	if(_ti & PAX_CARTCOUNT) {
		stmp.Format("\nCart Count: %d",_MDS.cartCount);
		_sPaxTag = _sPaxTag+stmp;
	}
	if(_ti & PAX_STATE) {
		stmp.Format("\nState: %s",STATE_NAMES[_MESa.state]);
		_sPaxTag = _sPaxTag+stmp;
	}
	if(_ti & PAX_PROCESSOR) {
		pTerm->procList->getProcessor(_MESa.procNumber)->getID()->printID(buf);
		stmp.Format("\nProcessor: %s",buf);
		_sPaxTag = _sPaxTag+stmp;
	}
	if(_ti & PAX_WALKINGSPEED) {		
		stmp.Format("\nWalking Speed: %.1f",_MDS.speed);
		_sPaxTag = _sPaxTag+stmp;
	}


}



void CTermPlanDoc::OnEditInvisibleregions()
{
	ASSERT(m_pSelectedNode!=NULL /*&& m_pSelectedNode->m_nIDResource == IDR_CTX_FLOOR*/);
	CFloor2* pFloor = NULL;
	pFloor=GetFloorByMode(m_systemMode).GetFloor2((int)m_pSelectedNode->m_dwData);
	CTVNode* pFloorsNode = FloorsNode();
	ActivateFloor(pFloor->Level());
	UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) (CTVNode*) pFloorsNode);
	CDlgFloorVisibleRegions dlg(pFloor,FALSE,this,GetMainFrame());
	if(dlg.DoModal()==IDOK) {
		GetCurModeFloor().saveDataSet(m_ProjInfo.path, FALSE);
		pFloor->InvalidateTexture();
		UpdateViewSharpTextures();
		UpdateFloorInRender3DView(pFloor);
	}
	ActivateFloor(m_nLastActiveFloor);
	UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) (CTVNode*) pFloorsNode);
}
bool CTermPlanDoc::UpdateWallShapeChange(){
	bool bRes=false;
	this->GetCurWallShapeList().saveDataSet(this->m_ProjInfo.path, false);
	CNodeView * pNodeView=GetNodeView();
	pNodeView->UpdateWallShapeNode();
	bRes=true;
	return bRes;
}

void CTermPlanDoc::LoadPaxDisplay()
{
	long count = GetTerminal().paxLog->getCount();

	m_vPaxDispPropIdx.clear();
	m_vPaxDispPropIdx.reserve(count);
	m_vPaxTagPropIdx.clear();
	m_vPaxTagPropIdx.reserve(count);

	
	for(long i=0; i<count; i++) {
		MobLogEntry ple;
		GetTerminal().paxLog->getItem(ple,i);

		//MobDescStructEx pdsX;
		const MobDescStruct& pds = ple.GetMobDesc();
		int nMatch = m_paxDispProps.FindBestMatch(pds,&GetTerminal());

		CPaxDispPropItem* pPDPI = NULL;
		if(nMatch == -1) 
		{                     //if no match, use default settings
			pPDPI = m_paxDispProps.GetDefaultDispProp();
			m_vPaxDispPropIdx.push_back(-1);
		}
		else if(nMatch==-2)
		{
			pPDPI = m_paxDispProps.GetDispPropItem(nMatch);
			m_vPaxDispPropIdx.push_back(nMatch);
		}
		else if(nMatch>=0)
		{
			pPDPI = m_paxDispProps.GetDispPropItem(nMatch);
			m_vPaxDispPropIdx.push_back(nMatch);
		}
		else
			ASSERT(FALSE);
		ASSERT(pPDPI != NULL);

		nMatch = -1;
		nMatch = m_paxTags.FindBestMatch(pds,&GetTerminal());

		CPaxTagItem* pPTI = NULL;
		if(nMatch == -1) {//if no match, use default settings
			pPTI = m_paxTags.GetDefaultPaxTags();
			m_vPaxTagPropIdx.push_back(-1);
		}
		else if(nMatch>=0)
		{
			pPTI = m_paxTags.GetPaxTagItem(nMatch);
			m_vPaxTagPropIdx.push_back(nMatch);
		}
		else
			ASSERT(FALSE);
		ASSERT(pPTI != NULL);

	}
}
/************************************************************************
FUNCTION: Load terminal logs
IN		:
OUT		:nFirstInTime, the first EVENT time(start time)
:nLastOutTime, the last EVENT time(end time)
RETRUEN	: TRUE,load success
FLASE,fail ,The model don't have simulation result 
/************************************************************************/
BOOL CTermPlanDoc::LoadTerminalLogs(long &nFirstInTime,long &nLastOutTime)
{
	nFirstInTime = LONG_MAX;
	nLastOutTime = -1;

	// clear logs
	clearLogsIfNecessary();

	if(GetTerminal().getCurrentSimResult() >=0)
	{
		int iCurrentSimIdx = GetTerminal().getCurrentSimResult();
		ASSERT( iCurrentSimIdx>=0 );
		GetTerminal().GetSimReportManager()->SetCurrentSimResult(iCurrentSimIdx);
		try
		{
			GetTerminal().openLogs(m_ProjInfo.path);
		}
		catch(StringError* pErr)
		{
			AfxMessageBox("The log file is old, run simulation please!");
			delete pErr;
			return FALSE;
		}
	}
	long count = GetTerminal().paxLog->getCount();	
	
	for(long i=0; i<count; i++) {
		MobLogEntry ple;
		GetTerminal().paxLog->getItem(ple,i);
		//MobDescStructEx pdsX;
		const MobDescStruct& pds = ple.GetMobDesc();
		if( pds.startTime < nFirstInTime )
			nFirstInTime = pds.startTime;

		if( pds.endTime > nLastOutTime)
			nLastOutTime = pds.endTime;
	}

	LoadPaxDisplay();
	return TRUE;
}
//according to LoadTerminalLogs
BOOL CTermPlanDoc::LoadAirsideLogs(long &nFirstInTime,long &nLastOutTime)
{
	nFirstInTime = LONG_MAX;
	nLastOutTime = -1;

	// clear logs
	clearLogsIfNecessary();

	//GetAirsideSimLogs().OpenLogs(m_ProjInfo.path);
	if(GetTerminal().getCurrentSimResult() >=0)
	{
		int iCurrentSimIdx = GetTerminal().getCurrentSimResult();
		ASSERT( iCurrentSimIdx>=0 );
		GetAirsideSimLogs().GetSimReportManager()->SetCurrentSimResult(iCurrentSimIdx);
		try
		{
			GetOutputAirside()->PrepareAnimLogs(m_ProjInfo.path);
		}
		catch(StringError* pErr)
		{
			AfxMessageBox("The airside log file is old, run simulation please!");
			delete pErr;
			return FALSE;
		}
	}
	long count = GetAirsideSimLogs().m_airsideFlightLog.getCount();

	m_vACDispPropIdx.clear();
	m_vACDispPropIdx.reserve(count);
	m_vACTagPropIdx.clear();
	m_vACTagPropIdx.reserve(count);
	m_vACShapeIdx.clear();
	m_vACShapeIdx.reserve(count);
	//code below are just for temp coding need to be adjust soon or late
	m_vACDispSwitchs.clear();
	m_vACDispSwitchs.reserve(count);
	for(long i=0;i<count;i++)
		m_vACDispSwitchs.push_back(AircraftDisplaySwitch());

	
	for(long i=0; i<count; i++) {
		AirsideFlightLogEntry fle;
		GetAirsideSimLogs().m_airsideFlightLog.getItem(fle, i);
	
		AirsideFlightDescStruct& fds = fle.GetAirsideDesc();
		if( fds.startTime < nFirstInTime )
			nFirstInTime = fds.startTime;

		if( fds.endTime > nLastOutTime )
			nLastOutTime = fds.endTime;

		// find matching shape
		fds.acCategory;

		int nMatchARR = -1; int nMatchDep = -1;
		nMatchARR = m_aircraftDispProps.FindBestMatch(fds, true, &GetTerminal());
		nMatchDep = m_aircraftDispProps.FindBestMatch(fds, false, &GetTerminal());

		m_vACDispPropIdx.push_back( std::pair<int,int> (nMatchARR,nMatchDep) );

		nMatchARR = m_aircraftTags.FindBestMatch(fds,true, &GetTerminal());
		nMatchDep = m_aircraftTags.FindBestMatch(fds,false,&GetTerminal());

		m_vACTagPropIdx.push_back( std::pair<int,int>(nMatchARR,nMatchDep) );

		//TODO: uncomment section below once we implement aircraft disp props	
		CAircraftTagItem* pATI = NULL;
		pATI = m_aircraftTags.GetDefaultTags();
		if(nMatchARR>=0)
		{
			pATI = m_aircraftTags.GetTagItem(nMatchARR);			
		}
		if(nMatchDep>=0)
		{
			pATI = m_aircraftTags.GetTagItem(nMatchDep);			
		}
		
		ASSERT(pATI != NULL);
		m_vACDispSwitchs[i].m_bTagOn = pATI->IsShowTags()?true:false;

		int RandAcTypeid = rand() % (ACTYPECOUNT-13);	
		
		m_vACShapeIdx.push_back( SHAPESMANAGER->GetACShapeIdxByACType(ACTypeStr[RandAcTypeid]) );
	}

	long nVehicleCount = GetAirsideSimLogs().m_airsideVehicleLog.getCount();
	m_vehicleDispProps.SetProjID(this->GetProjectID());
	m_vehicleDispProps.ReadData(this->GetProjectID());
	m_vAVehicleDispPropIdx.clear();
	m_vAVehicleShapeNameIdx.clear();
	for(int i=0;i< nVehicleCount ;i++)
	{
		AirsideVehicleLogEntry vehileE;		
		GetAirsideSimLogs().m_airsideVehicleLog.getItem(vehileE,i);
		//CVehicleDispPropItem *displayItem = m_vehicleDispProps.FindBestMatch(vehileE.GetAirsideDesc().id);		
		m_vAVehicleDispPropIdx.push_back(0);
		m_vAVehicleShapeNameIdx.push_back("default");
	}


	return TRUE;
}
const CFloors& CTermPlanDoc::GetCurModeFloor()const
{
	return GetFloorByMode( m_systemMode );
}

BOOL CTermPlanDoc::LoadLandSideLogs(long &nFirstInTime,long &nLastOutTime)
{
	clearLogsIfNecessary();

	GetLandsideSimLogs().OpenLogs(m_ProjInfo.path);

	ElapsedTime startTime, endTime;
	GetLandsideSimLogs().GetTimeRange(startTime, endTime);
	nFirstInTime = startTime;
	nLastOutTime = endTime;

	return TRUE;
}

ProcessorList& CTermPlanDoc::GetProcessorList(EnvironmentMode mode)
{
	if(mode==EnvMode_Unknow) 
		mode = m_systemMode;
	/*if(mode == EnvMode_LandSide)
		return GetLandsideDoc()->GetProcessorList();*/
	else if(mode == EnvMode_Terminal)
		return *GetTerminal().GetTerminalProcessorList();
	else if(mode == EnvMode_AirSide)
	{
		return *GetTerminal().GetAirsideProcessorList();
	}
	return *GetTerminal().GetTerminalProcessorList();
}

extern CNewMenu m_NewMenu;
HMENU CTermPlanDoc::GetDefaultMenu()
{
	if (m_systemMode == EnvMode_AirSide)
	{
		return m_NewMenu.m_hMenu;
	}
	return NULL;
}
//delete selected landside Processor
void CTermPlanDoc::OnCtxLanddeleteproc()
{
	// TODO: Add your command handler code here
	//GetLandsideDoc()->DeleteSelectProcessor();
	//LandsideDelLayoutObjectCmd pDelCmd = new LandsideDelLayoutObjectCmd()
	
}

void CTermPlanDoc::OnUpdateCtxLanddeleteproc(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	//if(GetLandsideDoc()->GetSelectProcessor().GetCount() > 1)	
		//pCmdUI->Enable(TRUE);

}

CObjectDisplayList& CTermPlanDoc::GetSelectProcessors()
{
	return m_vSelectedProcessors;
}
void CTermPlanDoc::OnCtxSmoothCtrlpoints()
{
	//// TODO: Add your command handler code here
	//CPROCESSOR2LIST& proc2list = GetSelectProcessors();
	//for(int i=0;i<(int)proc2list.size();i++)
	//{
	//	LandfieldProcessor  * proc  =(LandfieldProcessor  *) proc2list[i]->GetProcessor();
	//	//proc->SmoothCtrlPoints(1.5);
	//}
	//GetLandsideDoc()->UpdateDView();
}

void CTermPlanDoc::OnCtxEditCtrlpoints()
{
	// TODO: Add your command handler code here
	/*if(GetLandsideView()->m_bInEditPointMode)
		GetLandsideView()->ChangeToSelectMode(ProcessorRenderer::SelectMain);
	else
		GetLandsideView()->ChangeToSelectMode(ProcessorRenderer::SelectCtrlPoints);*/
	
}
void CTermPlanDoc::GetCurModeFloorsRealAltitude(std::vector<double>& vRealAltitude)
{	
	int nFloorCount = GetCurModeFloor().GetCount();
	for (int i =0; i < nFloorCount; ++i)
	{
		vRealAltitude.push_back(GetCurModeFloor().GetFloor2(i)->RealAltitude());
	}
}

void CTermPlanDoc::SetCurrentAirport( int nAirportID )
{
	m_nCurrentAirport = nAirportID;
}
int CTermPlanDoc::GetCurrentAirport()
{
	return m_nCurrentAirport;
}

int CTermPlanDoc::GetProjectID()
{
	return InputAirside::GetProjectID( m_ProjInfo.name );
}

CAirsideSimLogs& CTermPlanDoc::GetAirsideSimLogs()
{
	return m_arcport.GetOutputAirside()->m_airsideLog;
}

void CTermPlanDoc::DeleteALTObjects( ALTObjectList& vObjList )
{
	CString strMessage;

	strMessage = "Delete these Objects: \n";
	for(int i=0;i< (int)vObjList.size();i++)
	{
		ALTObject * pObj  = vObjList.at(i).get();
		if (pObj->GetLocked())
		{
			strMessage.Format(_T("Can not delete %s: %s.\nIt is locked!"),
				pObj->GetTypeName(), pObj->GetObjNameString());
			AfxMessageBox(strMessage,MB_OK);
			vObjList.clear();
			return;
		}
		strMessage += pObj->GetObjNameString();
		strMessage += _T("\n");
	}
	strMessage += "Are you sure?";
	if(AfxMessageBox(strMessage,MB_YESNO)!=IDYES )
	{
		vObjList.clear();
		return;
	}

	ALTObjectList ObjDeleted;
	for(int i=0; i < (int) vObjList.size();i++)
	{
		ALTObject * pObj  = vObjList.at(i).get();
		if( ALTObject::DeleteObjectByID(pObj->getID()) )
		{
			ObjDeleted.push_back(pObj);
		}
		else
		{
			CString strMsg;
			strMsg.Format("The %s %s is relate with other data,try move it away from the other layout fle! ", pObj->GetTypeName(), pObj->GetObjNameString());
			::MessageBox(NULL,strMsg,_T("Error"),MB_OK);
		}
	}
	vObjList = ObjDeleted;
}

void CTermPlanDoc::SetTempObject( ALTObject * pObj )
{
	m_pTempObj = pObj;
}


void CTermPlanDoc::OnUpdateTrackersPanorama( CCmdUI* pCmdUI )
{
	pCmdUI->Enable(Get3DView()!=NULL);
}

void CTermPlanDoc::ShrinkDatabase( CMasterDatabase*pMasterDatabase, const CString& strProjName )
{
	return;

	if(pMasterDatabase == NULL)
		return;
	CString strDatabaseName = _T("");
	pMasterDatabase->GetProjectDatabaseNameFromDB(strProjName,strDatabaseName);

	strDatabaseName = _T("ARC")+strDatabaseName;
	//dump log
	{
		CString strDumpSQL;
		strDumpSQL.Format(_T("DUMP TRANSACTION %s WITH NO_LOG"),strDatabaseName);
		
		pMasterDatabase->Execute(strDumpSQL);

	}

	//backup log 
	{
		CString strBackupSQL;
		strBackupSQL.Format(_T("BACKUP LOG  %s WITH TRUNCATE_ONLY"),strDatabaseName);
		pMasterDatabase->Execute(strBackupSQL);
	}

	//shrink log
	{
		CString strShrinkSQL;
		strShrinkSQL.Format(_T("DBCC shrinkdatabase (%s)"),strDatabaseName);
		pMasterDatabase->Execute(strShrinkSQL);
	}

}
//void  CTermPlanDoc::GetCurrentLayerAlignLine(ARCLine& Line) 
//{
//	CFloor2* pFloor = m_floors.m_vFloors[(int)m_pSelectedNode->m_dwData];
//	CFloor2::CAlignLine line ;
//	CFloor2::ReadAlignLineFromDB(EnvMode_Terminal , pFloor->getLayNode() ,line) ;
//	Line.Init(line.getBeginPoint() , line.getEndPoint()) ;
//}
CPlacement* CTermPlanDoc::GetTerminalPlacement()const
{ 
	return m_pPlacement;
}
int CTermPlanDoc::GetSelectProcessorsCount()
{
	return static_cast<int>(GetSelectProcessors().GetCount());
	//return 0;
}
CObjectDisplay *CTermPlanDoc::GetSelectedObjectDisplay(int nIndex)
{ 
	return GetSelectProcessors().GetItem(nIndex);
	//return NULL;
}

void CTermPlanDoc::DeletePipe( int nPipeIndex )
{
	CPipeDisplay *pPipeDisplay = m_pPlacement->GetPipeDislplay(GetTerminal().m_pPipeDataSet,nPipeIndex);
	if (pPipeDisplay)
	{
		GetSelectProcessors().DelItem(pPipeDisplay);
		m_pPlacement->DeletePipe(pPipeDisplay);
	}


}

void CTermPlanDoc::DeleteWallShapeObject( int nIndex)
{
	CObjectDisplay *pObjectDisplay = m_pPlacement->GetWallShapeDisplay(GetTerminal().m_pWallShapeList ,nIndex);
	if (pObjectDisplay)
	{
		GetSelectProcessors().DelItem(pObjectDisplay);
		m_pPlacement->DeleteWallShapeDisplay(pObjectDisplay);
	}
}

void CTermPlanDoc::DeleteStructueObject( int nIndex)
{
	CObjectDisplay *pObjectDisplay = m_pPlacement->GetStructureDisplay(GetTerminal().m_pStructureList ,nIndex);
	if (pObjectDisplay)
	{
		GetSelectProcessors().DelItem(pObjectDisplay);
		m_pPlacement->DeleteStructureDisplay(pObjectDisplay);
	}
}

void CTermPlanDoc::DeleteAreaObject(int nIndex)
{
	CObjectDisplay *pObjectDisplay = m_pPlacement->GetAreaDisplay(GetTerminal().m_pAreas ,nIndex);
	if (pObjectDisplay)
	{
		GetSelectProcessors().DelItem(pObjectDisplay);
		m_pPlacement->DeleteAreaDisplay(pObjectDisplay);
	}
}

void CTermPlanDoc::DeletePortalObject(int nIndex)
{
	CObjectDisplay *pObjectDisplay = m_pPlacement->GetPortalDisplay(&m_portals ,nIndex);
	if (pObjectDisplay)
	{
		GetSelectProcessors().DelItem(pObjectDisplay);
		m_pPlacement->DeletePortalDisplay(pObjectDisplay);
	}
}
//////////////////////////////////////////////////////////////////////////
//
//save project' input  to undo directory 
//////////////////////////////////////////////////////////////////////////
void CTermPlanDoc::SaveUndoProject(CString& pro_path)
{
	CMasterDatabase *pMasterDatabase = ((CTermPlanApp *)AfxGetApp())->GetMasterDatabase();
	CProjectDatabase projDatabase(PROJMANAGER->GetAppPath());
	projDatabase.CloseProject(pMasterDatabase,m_ProjInfo.name);
	CUndoManager* p_undoManger= CUndoManager::GetInStance(pro_path) ;
	if(p_undoManger  != NULL)
		p_undoManger->AddNewUndoProject();
	projDatabase.OpenProject(pMasterDatabase,m_ProjInfo.name,m_ProjInfo.path);
}

CARCPort * CTermPlanDoc::getARCport()
{
	return &m_arcport;
}
C3DBaseView* CTermPlanDoc::Get3DBaseView() // for onboard editing
{
	C3DBaseView* p3DBaseEditView = NULL;
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pNextView = GetNextView(pos);
		if (pNextView->IsKindOf(RUNTIME_CLASS(C3DBaseView)))
		{
			p3DBaseEditView = (C3DBaseView*)pNextView;
			break;
		}
	}

	return p3DBaseEditView;
}

CMeshEdit3DView* CTermPlanDoc::GetMeshEdit3DView()
{
	POSITION pos = GetFirstViewPosition();
	while(pos != NULL)
	{
		CView* pNextView = GetNextView(pos);
		if(pNextView->IsKindOf(RUNTIME_CLASS(CMeshEdit3DView)))
			return (CMeshEdit3DView*) pNextView;
	}
	return NULL;
}

void CTermPlanDoc::UpdateAllFloorsInRender3DView()
{
	UpdateAllViews(NULL, VM_UPDATE_ALL_FLOORS, NULL);
	ActiveFloorInRender3DView(GetActiveFloor());
}

void CTermPlanDoc::UpdateFloorInRender3DView(CRenderFloor* pFloor)
{
	UpdateAllViews(NULL, VM_UPDATE_ONE_FLOOR, (CObject*)pFloor);
}

void CTermPlanDoc::ActiveFloorInRender3DView(CRenderFloor* pFloor)
{
	UpdateAllViews(NULL, VM_ACTIVE_ONE_FLOOR, (CObject*)pFloor);
}


void CTermPlanDoc::UpdateRender3DObject(const CGuid& guid, const GUID& cls_guid) const
{
	CRender3DView* pRender3DView = GetRender3DView();
	if (pRender3DView)
	{
		//pRender3DView->GetModelEditScene().Update3DObject(guid, cls_guid);
		pRender3DView->OnUpdateDrawing();
	}
}

int CTermPlanDoc::GetFloorCount() const
{
	return m_floors.GetCount();
}

CAircraftLayoutEditor* CTermPlanDoc::GetAircraftLayoutEditor()
{
	return m_playoutEditor->mpAircraftLayoutEditor;
}

CLandsideSimLogs& CTermPlanDoc::GetLandsideSimLogs()
{
	return m_arcport.GetOutputLandside()->m_SimLogs;
}

BOOL CTermPlanDoc::StartAnimView()
{
	CMainFrame* pMF = GetMainFrame();
	if (m_animData.m_AnimationModels.IsOnBoardSel() || m_animData.m_AnimationModels.IsLandsideSel())
	{
		C3DView* p3DView = Get3DView();
		if (p3DView)
		{
			CChildFrame* pChildFrm = p3DView->GetParentFrame();
			if (pChildFrm)
			{
				pChildFrm->StorePanInfo();
				pChildFrm->ShowWindow(SW_HIDE);
			}
		}
		pMF->CreateOrActiveRender3DView();
		CRender3DView* pRender3DView = GetRender3DView();
		if (!pRender3DView)
		{
			ASSERT(FALSE);
			return FALSE;
		}
	} 
	else
	{
		C3DView* p3DView = NULL;
		if((p3DView = Get3DView()) == NULL) 
		{
			pMF->CreateOrActive3DView();
			p3DView = Get3DView();
		}
		if (!p3DView)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		p3DView->GetParentFrame()->GenPaxShapeDLIDs(&m_paxDispProps);
		p3DView->GetParentFrame()->GenResShapeDLIDs(&m_resDispProps);
		p3DView->GetParentFrame()->GenAircraftShapeDLIDs(&m_aircraftDispProps);
	}
	return TRUE;
}

void CTermPlanDoc::OnAnimScaleSize()
{
	m_displayoverrides.m_pbProcDisplayOverrides[PDP_DISP_VehicleSCALE] = !m_displayoverrides.m_pbProcDisplayOverrides[PDP_DISP_VehicleSCALE];
	m_displayoverrides.saveDataSet(m_ProjInfo.path, false);
	UpdateAllViews(NULL);
}

void CTermPlanDoc::OnUpdateAnimScaleSize(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_displayoverrides.m_pbProcDisplayOverrides[PDP_DISP_VehicleSCALE]);
}

void CTermPlanDoc::OnTrafficLight()
{
	m_bHideTrafficLight = !m_bHideTrafficLight;
	UpdateAllViews(NULL,VM_UPDATE_TRAFFICLIGHT);
}

void CTermPlanDoc::OnUpdateTrafficLight(CCmdUI* pCmdUI)
{
	if(m_systemMode == EnvMode_LandSide)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(!m_bHideTrafficLight);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

CSurfaceMaterialLib& CTermPlanDoc::GetSurfaceMaterialLib()
{
	return m_SurfaceMaterialLib;
}

CCalloutDlgLineData* CTermPlanDoc::GetCallOutLineData()
{
	return m_CallOutManger.GetDrawLineData();
}

int CTermPlanDoc::getSimResultCout( void )
{
	return GetTerminal().GetSimReportManager()->getSubSimResultCout();
}

void CTermPlanDoc::DisableViewPaint()
{
	IRender3DView* pRenderView = GetIRenderView();
	if (pRenderView)
	{
		pRenderView->SetBusy(TRUE);
	}
}

void CTermPlanDoc::EnableViewPaint()
{
	IRender3DView* pRenderView = GetIRenderView();
	if (pRenderView)
	{
		pRenderView->SetBusy(FALSE);
	}
}

void CTermPlanDoc::DoBridgeConvertCompatible()
{
	if (GetTerminal().procList->m_vBridgeConvert.empty() == true)
		return;
	
	unsigned iCount = GetTerminal().procList->m_vBridgeConvert.size();
	for (unsigned i = 0; i < iCount; i++)
	{
		BridgeConvert pConvert = GetTerminal().procList->m_vBridgeConvert[i];
		Processor* pBridgeConnector = pConvert.GetBridgeConnector();
		if (pBridgeConnector == NULL)
			continue;
		
		CPROCESSOR2LIST* pDefList = &(m_pPlacement->m_vDefined);
		unsigned size = pDefList->size();
		for(unsigned j=0; j<size; j++) 
		{
			CProcessor2* pProc2 = pDefList->at(j);
			if(pProc2->GetProcessor() == pBridgeConnector) 
			{
				for (int iConvert = 0; iConvert < pConvert.GetConvertCount(); iConvert++)
				{
					CProcessor2* pNewProc2 = pProc2->GetCopy();
					pNewProc2->SetSelectName(GetUniqueNumber());
					Processor* pConvertBridge = pConvert.GetBridgeConvert(iConvert);
					pNewProc2->SetProcessor(pConvertBridge);

					m_pPlacement->m_vDefined.insert( m_pPlacement->m_vDefined.begin(), pNewProc2 );
					m_pPlacement->saveDataSet(m_ProjInfo.path, false);
				}

				break;
			}
		}
	}
}
