// MainFrm.cpp : implementation of the CMainFrame class
//
#include "stdafx.h"
#include "TermPlan.h"  

#include "3DCamera.h"
#include "3DView.h"
#include "CFIDSView.h"
#include "CFIDSFrameView.h"
#include "./MFCExControl/ARCBaseTree.h"
#include "NodeView.h"
#include "RepListView.h"
#include "RepGraphView.h"
#include "RepControlView.h"
#include "OnBoardLayoutView.h"

#include "TermPlanDoc.h"
//#include "OpenDWGToolkit.h"
#include "../common/UnitsManager.h"    
#include <CommonData/3DTextManager.h>
#include "AnimationTimeManager.h"
#include "..\common\ACTypesManager.h"
#include "..\common\AirlineManager.h"
#include "..\common\AirportsManager.h"
#include "..\common\ProjectManager.h"
#include "..\common\WinMsg.h"
#include "..\inputs\PipeDataSet.h"
#include "..\common\ProbDistManager.h"
#include "ShapesManager.h"
#include "ShapesListBox.h"
#include "DlgDBAirports.h"
#include "DlgDBAirline.h" 
#include "DlgDBAircraftTypes.h"
#include "dlgACTypeModelDB.h"
//#include "DlgAircraftModelDefine.h"
#include "DlgShapeSelect.h"
#include "DlgFloorAdjust.h"
//#include "ProbDistDlg.h"
#include "DlgProbDist.h"
#include "UndoDialog.h"
#include "InputRepDlg.h"
#include "EconRepDlg.h"
#include "OptionsDlg.h"
#include "ProjectCommentDlg.h"
#include "ProjectControlDlg.h"
#include <CommonData/Fallback.h>
#include <set>
#include "ChildFrm.h"
#include "MainFrm.h"
#include "reports\ReportParaDB.h"
#include "reports\ReportParameter.h"
#include "common\InfoZip.h"
#include "Common\ZipInfo.h"
#include "mapi.h"	
#include "ImportIntoOldProjectDlg.h"
#include "ImportIntoNewProject.h"
#include "ModelSelectionDlg.h"
#include "ReportProperty.h"
#include "common\AirportDatabaseList.h"
#include "common\AirportDatabase.h"
#include "globaldbsheet.h"
#include "INI.h"
#include "inputdataView.h"
#include "CompareReportDoc.h"
#include "FrameCompareReport.h"
#include "FrameCompareReportResult.h"
#include "DlgAircraftModelEdit.h"
#include "MathematicView.h"
#include "ProcessFlowView.h"
#include "DlgAirportDBNameInput.h"
#include "ProcessDefineDlg.h"
#include "OpenComparativeReportsGroup.h"
#include "DockBarEx.h"
#include "MathProcessDefine.h"
#include "MathResultManager.h"
#include "ReportChildFrameSplit.h"
#include "Version.h"
#include "Floor2.h"
#include "Floors.h"
#include "Shlwapi.h"
#include "AirsideReportView.h"
#include "CompareReportView.h"
#include "LogSettingDlg.h"
#include "LogDisplayDlg.h"
#include "AirsideMSView.h"
#include "LandsideMSView.h"
#include "NodeViewFrame.h"
#include "LogWarningDlg.h"
#include "LogClearingDlg.h"
#include "DlgObstructionReport.h"
#include "DlgMobElementsDiagnosis.h"
//#include "../InputAirside/AirsideImportExportManager.h"
//#include "AirsideGround.h"
#include "MultiRunRepControlView.h"
#include "../compare/CmpReportCtrl.h"
#include "common/MultiRunReport.h"

#include "ImportExportManager.h"
#include "FlightDialog.h"
#include ".\mainfrm.h"

#include "../Common/AircraftAliasManager.h"
#include <InputOnBoard/AircraftComponentModelDatabase.h>

#include "AirsideRepControlView.h"
#include "AirsideReportGraphView.h"
#include "AirsideReportListView.h"
#include "AirsideReportChildFrameSplit.h"

#include "../AirsideReport/ReportManager.h"
#include "../InputAirside/ProjectDatabase.h"

#include "ProjectSelectDlg.h"

#include "OnBoardMsview.h"
#include "onboard/OnboardViewMsg.h"
#include <inputonboard/ComponentEditContext.h>
#include "Onboard/3DObjModelEditTreeView.h"
#include "Onboard/ACModelEditFrame.h"
#include "Onboard/ACModelEdit3DView.h"
#include "Onboard/ACFurnishingEditFrame.h"
#include "Onboard/ACFurnishingEdit3DView.h"


#include "Render3DView.h"
#include "Render3DFrame.h"

#include "CargoMSView.h"
#include "EnvironmentMSView.h"
#include "FinancialMSView.h"

#include "ProjectExport.h"
#include "ViewMsg.h"
//#include "OnBoardLayoutComponentView.h"
//#include "OnboardEditComponetView.h"
//#include "Onboard/ComponentModelEditView.h"
//#include "Onboard/AircraftModelDatabaseView.h"
// #include "Onboard/AircraftLayoutView.h"
#include "MainBarUpdateArgs.h"
#include "../InputOnBoard/CInputOnboard.h"
#include "../InputOnboard/CabinCrewGeneration.h"
//chloe
//#include "../on_borad_impl/arc_engine_apdater.h"
#include "DlgAircraftFurnishingDB.h"
#include "Common\mutilreport.h"
#include "DlgAircraftDoors.h"
#include "DlgActypeRelationShip.h"
#include "CargoMSView.h"
#include "EnvironmentMSView.h"
#include "FinancialMSView.h"

#include <Renderer/RenderEngine/3DObjModelEditContext.h>
#include <InputOnBoard/AircraftFurnishingSection.h>

#include "./DlgComponentDBDefine.h"
#include "Renderer/RenderEngine/RenderEngine.h"

#include "ACComponentShapesManager.h"
#include "Onboard/MeshEdit3DView.h"
#include <Common/OleDragDropManager.h>
#include <CommonData/3DViewCommon.h>
#include "onboard/AircraftLayout3DView.h"
#include <common/AircraftModel.h>

#include "OnboardReport/ReportType.h"
#include "OnboardReport/OnboardReportManager.h"
#include "OnboardReportChildSplitFrame.h"
#include "OnboardReportControlView.h"
#include "OnboardReportListView.h"
#include "OnboardReportGraphView.h"

#include "LandsideReportChildSplitFrame.h"
#include "LandsideReportControlView.h"
#include "LandsideReportListView.h"
#include "LandsideReportGraphView.h"
#include "../LandsideReport/LandsideReportType.h"
#include "../LandsideReport/LandsideReportManager.h"
#include "ARCportLayoutEditor.h"
#include "Common/IEnvModeEditContext.h"
#include "DlgAnimationSpeed.h"
#include "CompareReportDoc.h"

//IMPL_CHLOE_SYSTEM()
//
#include "ArcTermDockState.h"
#include "Common/SimAndReportManager.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning( disable : 4309 )

#define SPEEDBUTTON1	6
#define SPEEDBUTTON5	7
#define SPEEDBUTTON10	8
#define SPEEDBUTTON25	9
#define SPEEDBUTTON50	10
#define SPEEDBUTTON100	11
#define SPEEDBUTTONCUSTOM	12
#define ID_PIPEDISPLAY  10000
#define MAXPIPECOUNT	200

const CString __database_file[] =
{
	"acdata.acd",
		"category.cat",
		"Airlines.dat",
		"subairlines.sal",
		"Airports.dat",
		"Sector.sec",
		"probdist.pbd",
		"FlightGroup.frp"
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CNewMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CNewMDIFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CHANGEENVVIEW, OnMessageChangeEnvView)
	ON_COMMAND(ID_VIEW_3DTERMINALVIEW, OnView3dterminalview)
	ON_COMMAND(ID_VIEW_2DTERMINALVIEW, OnView2dterminalview)
	ON_COMMAND(ID_VIEW_OPENDEFAULT3D, OnView3DDefaultView)
	ON_COMMAND(ID_VIEW_OPENDEFAULT2D, OnView2DDefaultView)
	ON_COMMAND(ID_VIEW_DETAILVIEW, OnViewDetailview)
	//ON_COMMAND(ID_VIEW_ONBOARDLAYOUT, OnViewOnBoardLayoutView)
	ON_WM_CLOSE()
	//chloe
	ON_WM_DESTROY()
	//
	ON_COMMAND(ID_DATABASE_AIRPORTS, OnDatabaseAirports)
	ON_COMMAND(ID_DATABASE_PROBDISTS, OnDatabaseProbDists)
	ON_COMMAND(ID_REPORTS_PASSENGER_ACTIVITYLOG, OnReportsPassengerActivitylog)
	ON_COMMAND(ID_DATABASES_SHAPES, OnDatabaseShapes)
	ON_COMMAND(ID_REPORTS_PROCESSOR_UTILIZATION, OnReportsProcessorUtilization)
	ON_COMMAND(ID_DATABASE_AIRCRAFTTYPES, OnDatabaseACTypes)
	ON_COMMAND(ID_DATABASE_ACTYPEMODELS,OnDatabaseACTypeModels)
	ON_UPDATE_COMMAND_UI(ID_DATABASE_ACTYPEMODELS,OnUpdateACTypeModels)
	ON_COMMAND(ID_VIEW_ONBOARDLAYOUT,OnDatabaseAircraftComponents)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ONBOARDLAYOUT,OnUpdateAircraftComponents)
	ON_COMMAND(ID_DATABASE_ACFURNISHINGS, OnDatabaseACFurnishings)
	ON_UPDATE_COMMAND_UI(ID_DATABASE_ACFURNISHINGS,OnUpdateDatabaseFurnishings)

	ON_COMMAND(ID_REPORTS_PASSENGER_DISTANCETRAVELLED, OnReportsPassengerDistancetravelled)
	ON_COMMAND(ID_REPORTS_SPACE_OCCUPANCY, OnReportsSpaceOccupancy)
	ON_COMMAND(ID_DATABASES_AIRLINES, OnDatabasesAirlines)
	ON_COMMAND(ID_REPORTS_PASSENGER_TIMEINQUEUES, OnReportsPassengerTimeinqueues)
	ON_COMMAND(ID_REPORTS_PASSENGER_NUMBER, OnReportsPassengerNumber)
	ON_COMMAND(ID_REPORTS_PROCESSOR_THROUGHPUT, OnReportsProcessorThroughput)
	ON_COMMAND(ID_REPORTS_PROCESSOR_QUEUELENGTH, OnReportsProcessorQueuelength)
	ON_COMMAND(ID_REPORTS_PROCESSOR_CRITICALQUEUEPARAMETERS, OnReportsProcessorCriticalqueueparameters)
	ON_COMMAND(ID_REPORTS_PASSENGER_TIMEINSERVICE, OnReportsPassengerTimeInService)
	ON_COMMAND(ID_REPORTS_PASSENGER_TIMEINTERMINAL, OnReportsPassengerTimeinTerminal)
	ON_COMMAND(ID_REPORTS_BAGGAGE_BAGGAGECOUNT, OnReportsBaggageBaggagecount)
	ON_COMMAND(ID_REPORTS_BAGGAGE_BAGGAGEWAITINGTIME, OnReportsBaggageBaggagewaitingtime)
	ON_COMMAND(ID_REPORTS_SPACE_DENSITY, OnReportsSpaceDensity)
	ON_COMMAND(ID_REPORTS_PASSENGER_ACTIVITYBREAKDOWN, OnReportsPassengerActivitybreakdown)
	ON_COMMAND(ID_ANIMATION_ACTIVITYDENSITYDISPLAY, OnAnimationActivitydensitydisplay)
	ON_COMMAND(ID_ANIMATION_MOBILEELEMENTDISPLAY, OnAnimationMobileelementdisplay)
	ON_COMMAND(ID_ANIMATION_FLIGHTINFODISPLAY, OnAnimationFlightInfoDisplay)
	ON_COMMAND(ID_ANIMATION_TRAFFICCOUNTDISPLAY, OnAnimationTrafficcountdisplay)
	ON_COMMAND(ID_PROJECT_DELETE, OnProjectDelete)
	ON_COMMAND(ID_PROJECT_UNDELETE, OnProjectUndelete)
	ON_COMMAND(ID_PROJECT_PURGE, OnProjectPurge)
	ON_COMMAND(ID_REPORTS_INPUTDATA, OnReportsInputdata)
	ON_COMMAND(ID_REPORTS_SPACE_THROUGHPUT, OnReportsSpaceThroughput)
	ON_COMMAND(ID_REPORTS_SPACE_COLLISIONS, OnReportsSpaceCollisions)
	ON_COMMAND(ID_REPORTS_ECONOMIC, OnReportsEconomic)
	ON_COMMAND(ID_REPORTS_BAGGAGE_DELIVERYTIMES, OnReportsBaggageDeliverytimes)
	ON_COMMAND(ID_REPORTS_BAGGAGE_DEPARTUREDISTRIBUTION, OnReportsBaggageDeparturedistribution)
	ON_COMMAND(ID_REPORTS_AIRCRAFTOPERATIONS, OnReportsAircraftoperations)
	ON_UPDATE_COMMAND_UI(ID_ANIMATION_ACTIVITYDENSITYDISPLAY, OnUpdateAnimationActivitydensitydisplay)
	ON_UPDATE_COMMAND_UI(ID_ANIMATION_MOBILEELEMENTDISPLAY, OnUpdateAnimationMobileelementdisplay)
	ON_UPDATE_COMMAND_UI(ID_ANIMATION_FLIGHTINFODISPLAY, OnUpdateAnimationFlightInfoDisplay)
	ON_COMMAND(ID_WINDOW_SIDEBYSIDE, OnWindowSidebyside)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_SIDEBYSIDE, OnUpdateWindowSidebyside)
	ON_COMMAND(ID_PROJECT_UNDO, OnProjectUndo)
	ON_COMMAND(ID_PROJECT_OPTIONS, OnProjectOptions)
	ON_COMMAND(ID_PROJECT_COMMENT, OnProjectComment)
	ON_COMMAND(ID_PROJECT_CONTROL, OnProjectControl)
	ON_COMMAND(ID_EXPORTPROJECT_ASZIPFILE, OnNoOpenProjectExportAsZipFile)
	ON_COMMAND(ID_EXPORTPROJECT_ASINPUT_EMAIL, OnNoOpenProjectExportAsInputAndEmail)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(IDM_BAR_SHOW_MAIN, OnBarShowMain)
	ON_UPDATE_COMMAND_UI(IDM_BAR_SHOW_MAIN, OnUpdateBarShowMain)
	ON_COMMAND(IDM_BAR_SHOW_ANIMATION, OnBarShowAnimation)
	ON_UPDATE_COMMAND_UI(IDM_BAR_SHOW_ANIMATION, OnUpdateBarShowAnimation)
	ON_COMMAND(IDM_BAR_SHOW_PROJECT, OnBarShowProject)
	ON_UPDATE_COMMAND_UI(IDM_BAR_SHOW_PROJECT, OnUpdateBarShowProject)
	ON_COMMAND(IDM_BAR_SHOW_CAMERA, OnBarShowCamera)
	ON_UPDATE_COMMAND_UI(IDM_BAR_SHOW_CAMERA, OnUpdateBarShowCamera)
	ON_COMMAND(IDM_BAR_SHOW_SHAPE, OnBarShowShape)
	ON_COMMAND(IDM_BAR_SHOW_COMPONENT, OnBarShowACComponentShape)
	ON_UPDATE_COMMAND_UI(IDM_BAR_SHOW_COMPONENT,OnUpdateBarShowACComponentShape)
	ON_COMMAND(IDM_BAR_SHOW_FURNISHING, OnBarShowACFurnishingShape)
	ON_UPDATE_COMMAND_UI(IDM_BAR_SHOW_FURNISHING,OnUpdateBarShowACFurnishingShape)
	ON_UPDATE_COMMAND_UI(IDM_BAR_SHOW_SHAPE, OnUpdateBarShowShape)
	ON_COMMAND(IDM_BAR_SHOW_UNIT, OnBarShowUnit)
	ON_UPDATE_COMMAND_UI(IDM_BAR_SHOW_UNIT, OnUpdateBarShowUnit)
	ON_COMMAND(IDM_BAR_SHOW_LAYOUT, OnBarShowLayout)
	ON_UPDATE_COMMAND_UI(IDM_BAR_SHOW_LAYOUT, OnUpdateBarShowLayout)
	ON_COMMAND(IDM_BAR_SHOW_PIPE, OnBarShowPipe)
	ON_UPDATE_COMMAND_UI(IDM_BAR_SHOW_PIPE, OnUpdateBarShowPipe)
	ON_COMMAND(ID_BUT_PIPEBAR, OnButPipebar)
	ON_UPDATE_COMMAND_UI(ID_BUT_PIPEBAR, OnUpdateButPipebar)
	ON_COMMAND(ID_BUT_AIROUTE, OnButPipebar)
	ON_UPDATE_COMMAND_UI(ID_BUT_AIROUTE, OnUpdateButPipebar)
	ON_COMMAND(ID_BUT_ALLPROCESSORTYPE, OnButPipebar)
	ON_UPDATE_COMMAND_UI(ID_BUT_ALLPROCESSORTYPE, OnUpdateButPipebar)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_HELP_INDEX, OnHelpIndex)
	ON_COMMAND(ID_HELP_TECH_SUPPORT, OnHelpTechSupport)
	ON_COMMAND_RANGE(ID_MULTIPLERUNS_DISTANCETRAVELLED, ID_MULTIPLERUNS_OCCUPANCY, OnCompareReport)
	ON_COMMAND_RANGE(ID_MULTIPLERUNS_CRITICALQUEUE, ID_MULTIPLERUNS_CRITICALQUEUE, OnCompareReport)
	ON_COMMAND_RANGE(ID_MULTIPLERUNS_DENSITY,ID_MULTIPLERUNS_DENSITY,OnCompareReport)
	ON_COMMAND(ID_PROJECT_EXPORT_AS_INPUTZIP, OnProjectExportAsInputzip)
	ON_COMMAND(ID_PROJECT_EXPORT_AND_EMAIL, OnProjectExportAndEmail)
	ON_COMMAND(ID_PROJECT_EXPORT_AS_INPUTZIP_WITHOUT_DB, OnProjectExportAsInputzipWithoutDB)
	ON_COMMAND(ID_PROJECT_EXPORT_AND_EMAIL_WITHOUT_DB, OnProjectExportAndEmailWithoutDB)
	ON_COMMAND(ID_PROJECT_IMPORT_INTO_OLD_PROJECT, OnProjectImportIntoOldProject)
	ON_COMMAND(ID_PROJECT_IMPORT_INTO_NEW_PROJECT, OnProjectImportIntoNewProject)
	ON_UPDATE_COMMAND_UI(ID_DATABASE_AIRCRAFTTYPES, OnUpdateDatabaseMenu)
	ON_COMMAND(ID_DATABASE_DBLISTMAN, OnDatabaseDblistman)
	ON_UPDATE_COMMAND_UI(ID_DATABASE_DBLISTMAN, OnUpdateDatabaseDblistman)
	ON_COMMAND(ID_REPORTS_PASSENGER_FIREEVACUTION, OnReportsPassengerFireevacution)
	ON_COMMAND(ID_REPORTS_CONVEYOR_WAIT_LENGTH, OnReportsConveyorWaitLength)
	ON_COMMAND(ID_REPORTS_PASSENGER_CONVEYOR_WAIT_TIME, OnReportsPassengerConveyorWaitTime)
	ON_WM_SIZE()
	ON_COMMAND(ID_COMPARATIVEREPORTOPEN, OnComparativereportopen)
	ON_COMMAND(ID_COMPARATIVEREPORTNEW, OnComparativereportnew)
	ON_COMMAND(ID_COMPARATIVERUN, OnComparativerun)
	ON_UPDATE_COMMAND_UI(ID_COMPARATIVERUN, OnUpdateComparativerun)
	ON_COMMAND(ID_COMPARE_WINDOW_SIDEBYSIDE, OnCompareWindowSidebyside)
	ON_UPDATE_COMMAND_UI(ID_COMPARE_WINDOW_SIDEBYSIDE, OnUpdateCompareWindowSidebyside)
	ON_COMMAND(IDM_BAR_SHOW_SHAPEMATH, OnBarShowShapemath)
	ON_UPDATE_COMMAND_UI(IDM_BAR_SHOW_SHAPEMATH, OnUpdateBarShowShapemath)
	ON_COMMAND(ID_VIEW_MATHFLOWVIEW, OnViewMathflowview)
	ON_COMMAND(ID_REPORTS_MATHQTIME, OnReportsMathqtime)
	ON_COMMAND(ID_REPORTS_MATHQLENGTH, OnReportsMathqlength)
	ON_COMMAND(ID_REPORTS_MATHTHROUGHPUT, OnReportsMaththroughput)
	ON_COMMAND(ID_REPORTS_MATHPAXCOUNT, OnReportsMathpaxcount)
	ON_COMMAND(ID_REPORTS_MATHUTILIZATION, OnReportsMathutilization)
	ON_COMMAND(IDM_BAR_SHOW_RUNLOG, OnBarShowRunlog)
	ON_UPDATE_COMMAND_UI(IDM_BAR_SHOW_RUNLOG, OnUpdateBarShowRunlog)
	ON_COMMAND(ID_REPORTS_INPUTDATA, OnReportsInputdata)
	ON_COMMAND(ID_PROCESSOR_RETAILACTIVITY,OnReportRetailReport)
	ON_WM_SHOWWINDOW()
	ON_UPDATE_COMMAND_UI(ID_DATABASE_AIRPORTS, OnUpdateDatabaseMenu)
	ON_UPDATE_COMMAND_UI(ID_DATABASE_PROBDISTS, OnUpdateDatabaseMenu)
	ON_UPDATE_COMMAND_UI(ID_DATABASES_AIRLINES, OnUpdateDatabaseMenu)
	ON_COMMAND(ID_REPORTS_PASSENGER_MISSFLIGHT, OnReportsPassengerMissflight)
	ON_CBN_SELENDOK(IDC_COMBO_PROJSELECT, OnSelectSelectProject)
	ON_CBN_DROPDOWN(IDC_COMBO_PROJSELECT, OnDropDownSelectProject)
	ON_MESSAGE(WM_SLB_SELCHANGED, OnSLBSelChanged)
	ON_MESSAGE(WM_AIR_SELCHANGED, ONAIRSelChanged)
	ON_MESSAGE(WM_CABIN_SELCHANGED,OnCabinSelChanged)
	
	ON_COMMAND_RANGE(ID_ANIMATION_SET1SPEED, ID_ANIMATION_SETCUSTOMSPEED, OnAnimationSetSpeed)

	ON_COMMAND_EX_RANGE(ID_VIEW_NAMEDVIEWS1, ID_VIEW_NAMEDVIEWS8, OnViewNamedViews3D)
	ON_COMMAND_EX_RANGE(ID_VIEW_NAMEDVIEWS9, ID_VIEW_NAMEDVIEWS16, OnViewNamedViews2D)
	ON_MESSAGE(TP_TEMP_FALLBACK, OnDlgFallback)
	ON_MESSAGE(TP_ONBOARDLAYOUT_FALLBACK, OnOnboardLayoutEditDlgFallback)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ANIMPLAY, ID_ANIMPICKSPEED, OnUpdateAnimButtons)
	ON_MESSAGE(WM_SETANIMATIONSPEED, OnSetAnimationSpeed)
	ON_COMMAND(ID_SHOWSHAPESTOOLBAR, OnShowShapesToolbar)
	ON_UPDATE_COMMAND_UI(ID_SHOWSHAPESTOOLBAR, OnUpdateShowShapesToolbar)
	ON_COMMAND(ID_LAYOUT_FLOORADJUST, OnFloorAdjust)
	ON_UPDATE_COMMAND_UI(ID_LAYOUT_FLOORADJUST, OnUpdateFloorAdjust)
	ON_MESSAGE(UM_SHOW_CONTROL_BAR,OnShowControlBar)
	ON_MESSAGE(UM_CHANGE_ANIM_TOOLBTN_STATE,OnChangeAnimToolBtnState)
	ON_COMMAND_RANGE(IDM_SHAPE_LIST_DEFAULT,IDM_SHAPE_LIST_TICKETCOUNTER,OnChangeShape)
	ON_MESSAGE(UM_SETTOOLBTN_RUNDELTA_RUN, OnSetToolBtnRunDelta_Run)
	ON_MESSAGE(UM_SETTOOLBTN_RUNDELTA_DELTA, OnSetToolBtnRunDelta_Delta)
	ON_MESSAGE(WM_EXPORT_MANUAL,OnSendExportZipToARC)
	ON_MESSAGE(WM_EXPORT_EMAIL,OnSendExportEmailToARC)
	ON_COMMAND(ID_REPORT_AIRSIDE_DISTANCETRAVEL, OnRptAirsideDistTrv)
	ON_COMMAND(ID_AIRCRAFT_HOURLYDELAYS, OnRptAirsideFltDelay)
	ON_COMMAND(ID_BILLBOARD_EXPOSUREINCIDENCE, OnBillboardExposureincidence)
	ON_COMMAND(ID_BILLBOARD_INCREMENTALVISIT, OnBillboardIncrementalvisit)
	ON_COMMAND(ID_LOG_SETTING, OnLogSetting)
	ON_COMMAND(ID_LOG_VIEWSELECT, OnLogViewselect)
	ON_COMMAND(ID_LOG_CLEARLOG, OnLogClearlog)
	ON_COMMAND(ID_REPORT_AIRSIDE_ACTIVITY, OnReportAirsideActivity)
	//ON_COMMAND(ID_REPORTS_AIRSIDE_OPERATIONS, OnReportAirsideOperation)
	ON_COMMAND(ID_REPORTS_AIRSIDE_OPERATIONAL, OnReportAirsideOperational)
	ON_COMMAND(ID_REPORT_AIRSIDE_NODEDELAYS, OnReportAirsideNodeDelay)
	ON_COMMAND(ID_UNIT_DROPDOWN_ARROW,OnDropDownUnitBarArrow)
	ON_COMMAND(ID_REPORTS_OBSTRUCTIONS,OnRptAirsideObstructions)

	ON_COMMAND(ID_DIAGNOSIS_MOBILEELEMENTS, OnMobileElementsDiagnosis)
    //vehicle report 

	ON_COMMAND(ID_VEHICLES_OPERATIONS ,OnVehicleReportForOperate)
	//load and commit airport DB 
	ON_COMMAND(ID_DATABASES_LOADDEFAULTAIRPORTDB,OnLoadDefaultAirportDB)
	ON_COMMAND(ID_DATABASES_COMMITTODEFAULTAIRPORTDB,OnCommitToDefaultAirportDB)
	ON_COMMAND(ID_RUNWAYS_OPERATIONS, OnRunwaysOperationsReport)
	ON_COMMAND(ID_RUNWAYS_UTILIZATION, OnRunwaysUtilizationReport)
	ON_COMMAND(ID_AIRCRAFT_CONFLICTS, OnFlightConflictReport)

	ON_COMMAND(ID_TAXIWAYS_DELAY,OnTaxiwayReportOperate)
	ON_COMMAND(ID_TAXIWAYS_UTILIZATIONS,OnTaxiwayUtilizationReport)
	ON_COMMAND(ID_RUNWAYS_EXITS,OnRunwayExitReport)
	ON_COMMAND(ID_RUNWAYS_DELAYS34317,OnRunwaysDelayReport)
	ON_COMMAND(ID_RUNWAYS_CROSSINGS34319,OnRunwayCrossingsReport)
	ON_COMMAND(ID_AIRCRAFT_TAKEOFFPROCESS,OnTakeoffProcessReport)
	ON_COMMAND(ID_GATES_OPERATIONSREPORTS,OnStandOperationsReport)
	ON_COMMAND(ID_AIRCRAFT_FUELBURNS,OnFlightFuelBurnReport)
	ON_COMMAND(ID_DATABASES_AIRCRAFTDOORSETTING,OnAircraftDoorSetting)
//	ON_COMMAND(ID_DATABASES_AIRCRAFTTYPESRELATIONSHIP,OnAircraftTypeRelationShipSetting)
	ON_COMMAND(ID_INTERSETIONS_OPERATIONSREPORTS,OnAirsideIntersectionOperationReport)
	ON_COMMAND(ID_CONTROLLERS_WORKLOAD,OnAirsideControllerWorkloadReport)
	ON_COMMAND(ID_VIEW_RENDEROPTIONS, OnRenderOptions)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RENDEROPTIONS, OnUpdateRenderOptions)
	ON_MESSAGE(WM_CRASHCLOSE,OnSystemCrashClose)

	ON_COMMAND(ID_ONBAORD_REPORT_CARRYON, OnOnboardReportCarryonReport)
	ON_COMMAND(ID_ONBAORD_REPORT_PAX__CARRYON, OnOnboardReportPaxCarryonReport)

	ON_COMMAND(ID_ONBAORD_REPORT_PAX__CONFLICT,OnOnboardReportPaxConflictReport)

	ON_COMMAND(ID_ONBAORD_REPORT_PAX__DURATION,OnOnboardReportPaxDurationReport)


	ON_COMMAND(ID_ONBAORD_REPORT_PAX__DISTANCETRAVEL,OnOnboardReportPaxDistanceTravelReport)

	ON_COMMAND(ID_ONBAORD_REPORT_PAX_ACTIVITY,OnOnboardReportPaxActivityReport)


	ON_COMMAND(ID_LANDSIDE_VEHICLE_ACTIVITY, OnLandsideVehicleActivity)
	ON_COMMAND(ID_LANDSIDE_VEHICLE_DELAY, OnLandsideVehicleDelay)
	ON_COMMAND(ID_LANDSIDE_VEHICLE_TOTALDELAY,OnLandsideVehicleTotalDelay)

	ON_COMMAND(ID_LANDSIDE_RESOURCE_VEHICLEQUEUE, OnLandsideResourceVehicleQueueReport)
	ON_COMMAND(ID_RESOURCE_THROUGHPUT,OnLandsideResourceVehicleThroughputReport)
	END_MESSAGE_MAP()


static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,           // status line indicator
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
};

CNewMenu m_MainFrameMenu;


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	//m_menuPopupBar.Initialize(IDR_BAR_SHOW, NULL);
	m_pDlgFloorAdjust = NULL;
	m_bCanPasteProcess = FALSE;
	m_bViewOpen = true;
	m_nOnSizeType = -1;
}

CMainFrame::~CMainFrame()
{
	delete m_pDlgFloorAdjust;
}

BOOL CMainFrame::VerifyBarState(LPCTSTR lpszProfileName)
{
    CDockState state;
    state.LoadState(lpszProfileName);

    for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
    {
        CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
        ASSERT(pInfo != NULL);
        int nDockedCount = pInfo->m_arrBarID.GetSize();
        if (nDockedCount > 0)
        {
            // dockbar
            for (int j = 0; j < nDockedCount; j++)
            {
                UINT nID = (UINT) pInfo->m_arrBarID[j];
                if (nID == 0) continue; // row separator
                if (nID > 0xFFFF)
                    nID &= 0xFFFF; // placeholder - get the ID
                if (GetControlBar(nID) == NULL)
                    return FALSE;
            }
        }
        
        if (!pInfo->m_bFloating) // floating dockbars can be created later
            if (GetControlBar(pInfo->m_nBarID) == NULL)
                return FALSE; // invalid bar ID
    }

    return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	
	if (CNewMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//load unit state from profile,it loaded before create UnitBar
	UNITSMANAGER->SetLengthUnits(((CTermPlanApp*)AfxGetApp())->GetProfileInt("UnitsManager", "LengthUnits", UM_LEN_METERS)); 
	UNITSMANAGER->SetWeightUnits(((CTermPlanApp*)AfxGetApp())->GetProfileInt("UnitsManager", "WeightUnits", UM_WEIGHT_KILOS));

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetPaneInfo( 0, ID_SEPARATOR, SBPS_NORMAL, 400 );
	m_wndStatusBar.SetPaneInfo( 1, ID_SEPARATOR, SBPS_NORMAL, 400 );
	m_wndStatusBar.SetPaneInfo( 2, ID_SEPARATOR, SBPS_STRETCH, 0 );
	

    // Set caption...
	CString strCaption;
	strCaption.Format(_T("ARCport ALTOCEF (v%s)"), _T(ARCTERM_VERSION_STRING));
    SetWindowText(strCaption);

	MoveWindow(0,0,2000,1000,TRUE);
	FrameEnableDocking(this, CBRS_ALIGN_ANY);

	if (!(
		CreateMainBar() && CreateProjectBar()
		&& CreateUnitBar() && CreateAnimationBar() && CreateCameraBar() 
		&& CreateShapesBar() && CreateShapesBarPFlow() 
		&& CreateLayoutBar() && CreatePipeBar() /*&& CreateAirRouteBar()*/ && CreateCompRepLogBar() && CreateUnitTempBar()
		&& CreateAircraftModelBar() && CreateACComponentShapesBar() && CreateACFurnishingShapesBar()
		))
		return -1;
	
	m_DefaultNewMenu.LoadToolBar(IDR_MAINFRAME);
	AdjustToolbar();

	LoadState();

	ShowControlBar(&m_wndShapesBar, FALSE, FALSE);
	ShowControlBar(&m_wndACComponentShapesBar, FALSE, FALSE);
	ShowControlBar(&m_wndACFurnishingShapesBar, FALSE, FALSE);
	ShowControlBar(&m_wndShapesBarPFlow, FALSE, FALSE);
	ShowControlBar(&m_wndCompRepLogBar, FALSE, FALSE);	
	ShowControlBar(&m_wndAnimationBar, FALSE, FALSE);
	ShowControlBar(&m_wndProjectBar, FALSE, FALSE);
	ShowControlBar(&m_wndUnitBar, FALSE, FALSE);
	ShowControlBar(&m_wndUnitBarTemp, FALSE, FALSE);
	ShowControlBar(&m_wndLayoutBar, FALSE, FALSE);
	ShowControlBar(&m_wndPipeBar, FALSE, FALSE);
//	ShowControlBar(&m_wndAirRouteBar, FALSE, FALSE);
	ShowControlBar(&m_wndCameraBar, FALSE, FALSE);
	//ShowControlBar(&m_wndOnBoardLayoutBar,FALSE,FALSE);
	ShowControlBar(&m_wndAircraftModel,FALSE,FALSE);

	//chloe
	//INIT_CHLOE_SYSTEM()
	/*CString resource_path = ((CTermPlanApp*)AfxGetApp())->GetResourceDataPath();
	CHLOE_SYSTEM(ARCRSystem)->init(resource_path.GetBuffer());*/
	//
	
	// Verify the state of log switchs
	if ( ECHOLOG->IsLogSwithcesOn() )
	{
		CLogWarningDlg dlg;
		dlg.DoModal();
	}
	ShowWindow(SW_MAXIMIZE);

	return 0;
}

void CMainFrame::AdjustToolbar(void)
{
	if (!m_wndToolBar.IsVisible())
		return;

	DockControlBar(&m_wndToolBar,AFX_IDW_DOCKBAR_TOP);

	CToolBar * pLeftToolbar = 0;
	pLeftToolbar = &m_wndToolBar;

	if(pLeftToolbar->IsVisible()){
		DockControlBarLeftOf(&m_wndLayoutBar, pLeftToolbar);
		if (m_wndLayoutBar.IsVisible()) 
			pLeftToolbar = &m_wndLayoutBar;
	}
	if(pLeftToolbar->IsVisible()){
		DockControlBarLeftOf(&m_wndProjectBar, pLeftToolbar);
		if (m_wndProjectBar.IsVisible()) 
			pLeftToolbar = &m_wndProjectBar;
	}
	if(pLeftToolbar->IsVisible()){
		DockControlBarLeftOf(&m_wndUnitBarTemp, pLeftToolbar);
		if (m_wndUnitBarTemp.IsVisible()) 
			pLeftToolbar = &m_wndUnitBarTemp;
	}
	if(pLeftToolbar->IsVisible()){
		DockControlBarLeftOf(&m_wndUnitBar, pLeftToolbar);
		if (m_wndUnitBar.IsVisible()) 
			pLeftToolbar = &m_wndUnitBar;
	}
	if(pLeftToolbar->IsVisible()){
		DockControlBarLeftOf(&m_wndAnimationBar, pLeftToolbar);
		if (m_wndAnimationBar.IsVisible()) 
			pLeftToolbar = &m_wndAnimationBar;
	}
	if(pLeftToolbar->IsVisible()){
		DockControlBarLeftOf(&m_wndPipeBar, pLeftToolbar);
		if (m_wndPipeBar.IsVisible()) 
			pLeftToolbar = &m_wndPipeBar;
	}
// 	if(pLeftToolbar->IsVisible()){
// 		DockControlBarLeftOf(&m_wndAirRouteBar, pLeftToolbar);
// 		if (m_wndAirRouteBar.IsVisible()) 
// 			pLeftToolbar = &m_wndAirRouteBar;
// 	}
	if(pLeftToolbar->IsVisible()){
		DockControlBarLeftOf(&m_wndCameraBar, pLeftToolbar);
		if (m_wndCameraBar.IsVisible()) 
			pLeftToolbar = &m_wndCameraBar;
	}
	//if(pLeftToolbar->IsVisible())
	//{
	//	DockControlBarLeftOf(&m_wndOnBoardLayoutBar, pLeftToolbar);
	//	if (m_wndOnBoardLayoutBar.IsVisible()) 
	//		pLeftToolbar = &m_wndOnBoardLayoutBar;

	//}

	DockControlBar(&m_wndAircraftModel,AFX_IDW_DOCKBAR_RIGHT);
	DockControlBar(&m_wndACComponentShapesBar,AFX_IDW_DOCKBAR_RIGHT);
	DockControlBar(&m_wndACFurnishingShapesBar,AFX_IDW_DOCKBAR_RIGHT);
	DockControlBar(&m_wndShapesBar, AFX_IDW_DOCKBAR_RIGHT);
	DockControlBar(&m_wndShapesBarPFlow, AFX_IDW_DOCKBAR_RIGHT);
	DockControlBar(&m_wndCompRepLogBar, AFX_IDW_DOCKBAR_BOTTOM);
}


void CMainFrame::AdjustMenu()
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CDocument* pTermDoc = pMDIActive->GetActiveDocument();
	ASSERT(pTermDoc);

	UpdateMainframeMenu( ((CTermPlanDoc*)pTermDoc)->m_systemMode );
	

}
void CMainFrame::LoadDefaultMenu()
{
	if(m_MainFrameMenu.m_hMenu)
		m_MainFrameMenu.DestroyMenu();
	// Load the new menu.
	m_MainFrameMenu.LoadMenu(IDR_MAINFRAME);
	ASSERT(m_MainFrameMenu);
	SetMenu(&m_MainFrameMenu);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CNewMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CNewMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CNewMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::OnMobileElementsDiagnosis()
{
	/*
	Diagnosis CMD:
	CTRL + F8:	diagnosis mobElements
	*/

	CMDIChildWnd* pMDIActive = MDIGetActive();
	if(pMDIActive != NULL && pMDIActive->GetActiveDocument() != NULL)
	{
		CString strProjPath = ((CTermPlanDoc*)pMDIActive->GetActiveDocument())->m_ProjInfo.path;
		CDlgMobElementsDiagnosis dlg(strProjPath, this);
		dlg.DoModal();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

CMDIChildWnd* CMainFrame::GetNodViewFrame(CRuntimeClass *pViewClass)
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CDocument* pDoc = pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);

	CView* pView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		pView = pDoc->GetNextView(pos);
		if (pView->IsKindOf(pViewClass))
		{
			pView->GetParentFrame()->ActivateFrame();
			ASSERT(pView->GetParentFrame()->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));
			return (CMDIChildWnd*)pView->GetParentFrame();
		}
	}
	return NULL;
}

#include "LayoutView.h"
//#include "onboard/AircraftModelDatabaseView.h"
//#include "onboard/ComponentModelEditView.h"
#include "OnBoard/MeshEdit3DView.h"


CMDIChildWnd* CMainFrame::CreateOrActivateFrame(CDocTemplate *pTemplate, CRuntimeClass *pViewClass, bool* pbAlreadyExist /*= NULL*/)
{
	// activates the MDIChildWnd associated with a view of type pViewClass if it exists or creates it otherwise
	CMDIChildWnd* pPreFrame = GetNodViewFrame(pViewClass);
	if (pPreFrame)
	{
		if (pbAlreadyExist)
			*pbAlreadyExist = true;

		pPreFrame->ActivateFrame();
		return pPreFrame;

		//close 3dview for render engine conflict
		//pPreFrame->SendMessage(WM_SYSCOMMAND,SC_CLOSE,0);
	}

	if( false == m_bViewOpen)
		return NULL;


	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CDocument* pDoc = pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);

	CMDIChildWnd* pNewFrame
		= (CMDIChildWnd*)(pTemplate->CreateNewFrame(pDoc, NULL));
	if (pNewFrame == NULL)
		return NULL;     // not created
	ASSERT_KINDOF(CMDIChildWnd, pNewFrame);

	pTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	if (((CTermPlanDoc*)pDoc)->m_simType == SimType_MontCarlo)
	{
		ToolBarUpdateParameters updatePara;
		if(pViewClass == RUNTIME_CLASS(CSimpleMeshEditView))
		{
			updatePara.m_operatType = ToolBarUpdateParameters::OpenComponentEditView;
			updatePara.m_pDoc = (CTermPlanDoc*)pDoc;
		}
		if( pViewClass == RUNTIME_CLASS(C3DView) )
		{
			updatePara.m_operatType = ToolBarUpdateParameters::Open3DViewFrm;
		}
		if (pViewClass==RUNTIME_CLASS(CRender3DView))
		{
			updatePara.m_operatType = ToolBarUpdateParameters::Open3DViewFrm;
		}
		if(pViewClass == RUNTIME_CLASS(CAircraftLayout3DView))
		{
			updatePara.m_operatType = ToolBarUpdateParameters::Open3DViewFrm;
		}
		updatePara.m_nCurMode = ((CTermPlanDoc*)pDoc)->GetCurrentMode();
		updatePara.m_pDoc = (CTermPlanDoc*)pDoc;
		SendMessage(UM_SHOW_CONTROL_BAR,2,(LPARAM)&updatePara);
	}
	
	//special show Windows for Process Define Form view  
	CString strTemp;
	pNewFrame->GetActiveFrame()->GetWindowText(strTemp);
	if(strcmp("Process Define",strTemp.Right(14)) != 0)
		OnWindowSidebyside();
	pNewFrame->ActivateFrame();
	return pNewFrame;
}

/*
C3DView* CMainFrame::Get3DView()
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if(pMDIActive == NULL)
		return NULL;
	CDocument* pDoc = pMDIActive->GetActiveDocument();
	if(pDoc == NULL)
		return NULL;
	CView* pView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		pView = pDoc->GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(C3DView)))
			return (C3DView*) pView;
	}
	return NULL;
}
*/
void CMainFrame::OnView3DDefaultView() //opens or activates default 3d view (single pane)
{	

	CMDIChildWnd* pMDIChild = CreateOrActiveEnvModeView();
	if (!pMDIChild)
		return;

	CDocument* pDoc = pMDIChild->GetActiveDocument();
	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	
	CTermPlanDoc* pTPDoc = (CTermPlanDoc*)pDoc;
	pTPDoc->GetCurModeRenderCameras().m_b3DMode = true;

	if (pMDIChild->IsKindOf(RUNTIME_CLASS(CChildFrame)))
	{
		CChildFrame* pChildFrame = (CChildFrame*)pMDIChild;

		//if this is not already a 3D working view
		if(pChildFrame->GetWorkingCamera(0,0)->GetProjectionType() != C3DCamera::perspective) {
			//save current working view to 2D user view
			pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView2D));
		}

		//unsplit view if necessary
		pChildFrame->UnSplitColumn();
		pChildFrame->UnSplitRow();

		//load 3D default view to current working view
		pChildFrame->LoadWorkingViews (&(pTPDoc->GetCurModeCameras().m_defaultView3D));
		//save current working view to 3D user view
		pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView3D));

		pTPDoc->SetCameraWBS();
		pTPDoc->UpdateViewSharpTextures();
		pChildFrame->OnUpdateFrameTitle(TRUE);
		pTPDoc->UpdateAllViews(NULL);
	}
	else if (pMDIChild->IsKindOf(RUNTIME_CLASS(CRender3DFrame)))
	{
		CRender3DFrame* pRenderFrame = (CRender3DFrame*)pMDIChild;
		pRenderFrame->OnView3DDefaultView();
		pTPDoc->UpdateAllViews(NULL);
	}
	else
	{
		ASSERT(FALSE);
	}

	UpdateMainframeMenu(pTPDoc->m_systemMode);
}

void CMainFrame::OnView2DDefaultView() //opens or activates default 3d view (single pane)
{
	

	CMDIChildWnd* pMDIChild = CreateOrActiveEnvModeView();
	if (!pMDIChild)
	{
		ASSERT(FALSE);
		return;
	}

	CDocument* pDoc = pMDIChild->GetActiveDocument();
	if(!pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)))
	{
		ASSERT(FALSE);
		return;
	}
	CTermPlanDoc* pTPDoc = (CTermPlanDoc*)pDoc;
	pTPDoc->GetCurModeRenderCameras().m_b3DMode = false;

	if (pMDIChild->IsKindOf(RUNTIME_CLASS(CChildFrame)))
	{
		CChildFrame* pChildFrame = (CChildFrame*)pMDIChild;

		//if this is not already a 2D working view
		if(pChildFrame->GetWorkingCamera(0,0)->GetProjectionType() != C3DCamera::parallel) {
			//save current working view to 3D user view
			pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView3D));
		}

		//unsplit view if necessary
		pChildFrame->UnSplitColumn();
		pChildFrame->UnSplitRow();

		//load 2D default view to current working view
		pChildFrame->LoadWorkingViews (&(pTPDoc->GetCurModeCameras().m_defaultView2D));
		//save current working view to 2D user view
		pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView2D));

		pTPDoc->SetCameraWBS();
		pTPDoc->UpdateViewSharpTextures();
		pChildFrame->OnUpdateFrameTitle(TRUE);
		pTPDoc->UpdateAllViews(NULL);
	}
	else if (pMDIChild->IsKindOf(RUNTIME_CLASS(CRender3DFrame)))
	{
		CRender3DFrame* pRender3DFrame = (CRender3DFrame*)pMDIChild;
		pRender3DFrame->OnView2DDefaultView();
		pTPDoc->UpdateAllViews(NULL);
	}
	else
	{
		ASSERT(FALSE);
	}

	UpdateMainframeMenu(pTPDoc->m_systemMode );

}

void CMainFrame::OnView3dterminalview() //opens or activates working 3d view
{
	//CRender3DFrame* pRenderFrame = (CRender3DFrame*)GetNodViewFrame(RUNTIME_CLASS(CRender3DView));
	//if (RenderEngine::getSingleton().GetAlwaysInUse() && pRenderFrame)
	//{
	//	ASSERT(pRenderFrame->IsKindOf(RUNTIME_CLASS(CRender3DFrame)));
	//	pRenderFrame->OnView3dterminalview();
	//	return;
	//}
	CMDIChildWnd* pMDIChild  = CreateOrActiveEnvModeView();
	if (!pMDIChild)
		return;

	CDocument* pDoc = pMDIChild->GetActiveDocument();
	if(!pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)))
		return ;

	CTermPlanDoc* pTPDoc = (CTermPlanDoc*)pDoc;


	pTPDoc->GetCurModeRenderCameras().m_b3DMode = true;

	if (pMDIChild->IsKindOf(RUNTIME_CLASS(CChildFrame)))
	{
		CChildFrame* pChildFrame = (CChildFrame*)pMDIChild;

		if(pChildFrame->GetWorkingCamera(0,0)->GetProjectionType() != C3DCamera::perspective) 
		{
			//save current working view to 2D user view
			pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView2D));
		}

		pChildFrame->LoadWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView3D));
		pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView3D));


		pTPDoc->SetCameraWBS();
		pTPDoc->UpdateViewSharpTextures();
		pChildFrame->OnUpdateFrameTitle(TRUE);
		pTPDoc->UpdateAllViews(NULL);
	}
	else if (pMDIChild->IsKindOf(RUNTIME_CLASS(CRender3DFrame)))
	{
		pTPDoc->UpdateAllViews(NULL);
	}
	else
	{
		ASSERT(FALSE);
	}



	UpdateMainframeMenu(pTPDoc->m_systemMode );
}

void CMainFrame::AdjustMSV(CDocument* pTermDoc)
{
	using namespace MSV;
	CMDIChildWnd* pMDIChild = GetNodViewFrame(RUNTIME_CLASS(CNodeView));
	if (pMDIChild == NULL)
	{
		pMDIChild = GetNodViewFrame(RUNTIME_CLASS(CAirsideMSView));
	}

	if (pMDIChild == NULL)
	{
		pMDIChild = GetNodViewFrame(RUNTIME_CLASS(CLandsideMSView));
	}
	if (pMDIChild == NULL)
	{
		pMDIChild = GetNodViewFrame(RUNTIME_CLASS(COnBoardMSView));
	}
	if (pMDIChild == NULL)
	{
		pMDIChild = GetNodViewFrame(RUNTIME_CLASS(CCargoMSView));
	}
	if (pMDIChild == NULL)
	{
		pMDIChild = GetNodViewFrame(RUNTIME_CLASS(CEnvironmentMSView));
	}
	if (pMDIChild == NULL)
	{
		pMDIChild = GetNodViewFrame(RUNTIME_CLASS(CFinancialMSView));
	}
	if (pMDIChild == NULL)
		return;

	if(pMDIChild->IsKindOf(RUNTIME_CLASS(CNodeViewFrame)))
	{  	
		((CNodeViewFrame *)pMDIChild)->AdjustMSView(((CTermPlanDoc*)pTermDoc)->m_systemMode);
	}
	

}
void CMainFrame::UpdateMainframeMenu( EnvironmentMode envMode )
{
	if( EnvMode_AirSide == envMode )
	{
		if(m_MainFrameMenu.m_hMenu)
			m_MainFrameMenu.DestroyMenu();
		// Load the new menu.
		m_MainFrameMenu.LoadMenu(IDR_AIRSIDE);
		ASSERT(m_MainFrameMenu);
		SetMenu(&m_MainFrameMenu);
	}

	if( EnvMode_LandSide == envMode )
	{
		if(m_MainFrameMenu.m_hMenu)
			m_MainFrameMenu.DestroyMenu();
		// Load the new menu.
		m_MainFrameMenu.LoadMenu(IDR_LANDSIDE);
		ASSERT(m_MainFrameMenu);
		SetMenu(&m_MainFrameMenu);

		CMenu* pMenu =  GetMenu();
		CMenu* pSubMenu = pMenu->GetSubMenu(3);
		CMenu* pSubSubMenu = pSubMenu->GetSubMenu(0);
		
		CString strText;
		pSubSubMenu->GetMenuString(0, strText, MF_BYPOSITION );
		CString strText1;
		pSubSubMenu->GetMenuString(2, strText1, MF_BYPOSITION );

	}

	if( EnvMode_OnBoard == envMode )
	{
		if(m_MainFrameMenu.m_hMenu)
			m_MainFrameMenu.DestroyMenu();
		// Load the new menu.
		m_MainFrameMenu.LoadMenu(IDR_ONBOARD);
		ASSERT(m_MainFrameMenu);
		SetMenu(&m_MainFrameMenu);
	}
	if(  envMode == EnvMode_Cargo)
	{
		if(m_MainFrameMenu.m_hMenu)
			m_MainFrameMenu.DestroyMenu();
		// Load the new menu.
		m_MainFrameMenu.LoadMenu(IDR_MAINFRAME_CARGO);
		ASSERT(m_MainFrameMenu);
		SetMenu(&m_MainFrameMenu);
	}

	if(  envMode == EnvMode_Environment)
	{
		if(m_MainFrameMenu.m_hMenu)
			m_MainFrameMenu.DestroyMenu();
		// Load the new menu.
		m_MainFrameMenu.LoadMenu(IDR_MAINFRAME_ENVIRONMENT);
		ASSERT(m_MainFrameMenu);
		SetMenu(&m_MainFrameMenu);
	}

	if(  envMode == EnvMode_Financial)
	{
		if(m_MainFrameMenu.m_hMenu)
			m_MainFrameMenu.DestroyMenu();
		// Load the new menu.
		m_MainFrameMenu.LoadMenu(IDR_MAINFRAME_FINANCIAL);
		ASSERT(m_MainFrameMenu);
		SetMenu(&m_MainFrameMenu);
	}
}


void CMainFrame::OnMessageChangeEnvView() //opens or activates working 3d view
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CDocument* pDoc = pMDIActive->GetActiveDocument();
	ASSERT(pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	CTermPlanDoc* pTermDoc = (CTermPlanDoc*)pDoc;
	AdjustMSV(pTermDoc);

	bool bSaveWorkingCamera = false;
	CMDIChildWnd* pMDIChild = CreateOrActiveEnvModeView(&bSaveWorkingCamera);
	if(!pMDIChild)
	{
		//ASSERT(FALSE);
		return;
	}
	
	if(pMDIChild->IsKindOf(RUNTIME_CLASS(CChildFrame)))
	{
		CChildFrame* pChildFrame = (CChildFrame*)pMDIChild;
		//if this is not already a 3D working view
		if(pChildFrame->GetWorkingCamera(0,0)->GetProjectionType() != C3DCamera::perspective) 
		{
			//save current working view to 2D user view
			//		pChildFrame->SaveWorkingViews(&(pTermDoc->GetCurModeCameras().m_userView2D));
			//		OnMessageView2dterminalview();

			//save last state.
			if( pTermDoc->m_systemMode == EnvMode_Terminal)
			{
				if(bSaveWorkingCamera)
				{
					pChildFrame->SaveWorkingViews(&(pTermDoc->GetCamerasByMode(EnvMode_AirSide).m_userView2D));
					pTermDoc->GetCamerasByMode(EnvMode_AirSide).saveDataSet(pTermDoc->m_ProjInfo.path,FALSE);
				}
			}
			else
			{
				if (bSaveWorkingCamera)
				{
					pChildFrame->SaveWorkingViews(&(pTermDoc->GetCamerasByMode(EnvMode_Terminal).m_userView2D));
					pTermDoc->GetCamerasByMode(EnvMode_Terminal).saveDataSet(pTermDoc->m_ProjInfo.path,FALSE);	
				}
			}

			pChildFrame->LoadWorkingViews (&(pTermDoc->GetCurModeCameras().m_userView2D));
			//save current working view to 2D user view
			pChildFrame->SaveWorkingViews(&(pTermDoc->GetCurModeCameras().m_userView2D));

			pTermDoc->SetCameraWBS();
			pTermDoc->UpdateViewSharpTextures();
			pChildFrame->OnUpdateFrameTitle(TRUE);	
			pTermDoc->UpdateAllViews(NULL);

			return;
		}

		//save last state.
		if( pTermDoc->m_systemMode == EnvMode_Terminal)
		{
			if(bSaveWorkingCamera)
			{
				pChildFrame->SaveWorkingViews(&(pTermDoc->GetCamerasByMode(EnvMode_AirSide).m_userView3D));
				pTermDoc->GetCamerasByMode(EnvMode_AirSide).saveDataSet(pTermDoc->m_ProjInfo.path,FALSE);
			}
		}
		else
		{
			if (bSaveWorkingCamera)
			{
				pChildFrame->SaveWorkingViews(&(pTermDoc->GetCamerasByMode(EnvMode_Terminal).m_userView3D));	
				pTermDoc->GetCamerasByMode(EnvMode_Terminal).saveDataSet(pTermDoc->m_ProjInfo.path,FALSE);	
			}
		}
		//	pTermDoc->GetCurModeCameras().loadDataSet(pTermDoc->m_ProjInfo.path);
		//load 3D user view to current working view
		pChildFrame->LoadWorkingViews (&(pTermDoc->GetCurModeCameras().m_userView3D));
		//save current working view to 3D user view
		pChildFrame->SaveWorkingViews(&(pTermDoc->GetCurModeCameras().m_userView3D));


		pTermDoc->SetCameraWBS();
		pTermDoc->UpdateViewSharpTextures();
		pChildFrame->OnUpdateFrameTitle(TRUE);
		pTermDoc->UpdateAllViews(NULL);
	}
	
}


void CMainFrame::OnView2dterminalview() //opens 2D user view (working view)
{
	/*CRender3DFrame* pRenderFrame = (CRender3DFrame*)GetNodViewFrame(RUNTIME_CLASS(CRender3DView));
	if (RenderEngine::getSingleton().GetAlwaysInUse() && pRenderFrame)
	{
		ASSERT(pRenderFrame->IsKindOf(RUNTIME_CLASS(CRender3DFrame)));
		pRenderFrame->OnView2dterminalview();
		return;
	}*/

	CMDIChildWnd* pMDIChild = CreateOrActiveEnvModeView();
	if (!pMDIChild)
		return;

	CDocument* pDoc = pMDIChild->GetActiveDocument();
	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	CTermPlanDoc* pTPDoc = (CTermPlanDoc*)pDoc;
	pTPDoc->GetCurModeRenderCameras().m_b3DMode = false;

	if (pMDIChild->IsKindOf(RUNTIME_CLASS(CChildFrame)))
	{
		CChildFrame* pChildFrame = (CChildFrame*)pMDIChild;
		//split the view according to GetCurModeCameras().m_PaneInfo
	
		////if this is not already a 2D working view
		if(pChildFrame->GetWorkingCamera(0,0)->GetProjectionType() != C3DCamera::parallel) {
			//save current working view to 3D user view
			pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView3D));
		}
		
		//load & save 2D user view to current working view
		pChildFrame->LoadWorkingViews (&(pTPDoc->GetCurModeCameras().m_userView2D));
		pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView2D));
	
		pTPDoc->SetCameraWBS();
		pTPDoc->UpdateViewSharpTextures();
		pChildFrame->OnUpdateFrameTitle(TRUE);	
		pTPDoc->UpdateAllViews(NULL);
	}
	else if (pMDIChild->IsKindOf(RUNTIME_CLASS(CRender3DFrame)))
	{
		CRender3DFrame* pRender3DFrame = (CRender3DFrame*)pMDIChild;
		pRender3DFrame->OnView2dterminalview();
		pTPDoc->UpdateAllViews(NULL);
	}
	else
	{
		ASSERT(FALSE);
	}

	UpdateMainframeMenu(pTPDoc->m_systemMode );

}

void CMainFrame::OnViewDetailview() 
{
	// TODO: Add your command handler code here
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if( pMDIActive == NULL )
		return ;

	CTermPlanDoc* pDoc = NULL;
	CView* pView = pMDIActive->GetActiveView();

	if (pView->IsKindOf(RUNTIME_CLASS(CFlowChartPane)))
	{
		pDoc = (CTermPlanDoc*)((CFlowChartPane*)pView)->GetDocumentPtr();
	}
	else
	{
		pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	}
	if (pDoc == NULL)
		return;

	if (pDoc->m_simType == SimType_MontCarlo)
	{
		CreateOrActivateFrame(theApp.m_pDetailTemplate, RUNTIME_CLASS(CNodeView));
	}
	else if (pDoc->m_simType ==SimType_Mathmatical)
	{
		CMDIChildWnd *pFlowFrame = NULL;
		POSITION pos = pDoc->GetFirstViewPosition();
		while (pos != NULL)
		{
			CView* pView = pDoc->GetNextView(pos);
			if (pView->IsKindOf(RUNTIME_CLASS(CMathematicView)))
			{
				pFlowFrame = (CMDIChildWnd*)((CMathematicView*)pView)->GetParent();
				break;
			}
		}
		if (pFlowFrame == NULL)
		{
			CMDIChildWnd* pNewFrame	= (CMDIChildWnd*)(theApp.m_pMathematicTemplate->CreateNewFrame(pDoc, NULL));
			theApp.m_pMathematicTemplate->InitialUpdateFrame(pNewFrame, pDoc);
		}
	}
}


//void CMainFrame::OnViewOnBoardLayoutView()
//{
//	CMDIChildWnd* pMDIActive = MDIGetActive();
//	if (pMDIActive == NULL)
//		return;
//
//	CTermPlanDoc* pActiveDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
//	if (pActiveDoc == NULL)
//		return;
//
//	if( EnvMode_OnBoard != pActiveDoc->m_systemMode )
//		return;
//
//	CMDIChildWnd* pChildFrm = CreateOrActivateFrame(theApp.m_pOnBoardLayoutTemplate, RUNTIME_CLASS(CComponentModelEditView));
//	ASSERT(pChildFrm != NULL);
//
//	pChildFrm->ShowWindow(SW_SHOWNORMAL);
//	//UpdateMainFrameBar(pActiveDoc);
//
//	pActiveDoc->UpdateAllViews(NULL,VM_ONBOARD_LAYOUT_DEFAULT,NULL);
//}


BOOL CMainFrame::OnViewNamedViews2D(UINT nID) 
{
	/*CRender3DFrame* pRenderFrame = (CRender3DFrame*)GetNodViewFrame(RUNTIME_CLASS(CRender3DView));
	if (RenderEngine::getSingleton().GetAlwaysInUse() && pRenderFrame)
	{
		ASSERT(pRenderFrame->IsKindOf(RUNTIME_CLASS(CRender3DFrame)));
		pRenderFrame->OnViewNamedViews2D(nID);
		return TRUE;
	}*/

	CMDIChildWnd* pMDIChild = CreateOrActiveEnvModeView();
	if(!pMDIChild )
		return FALSE;

	CDocument* pDoc = pMDIChild->GetActiveDocument();
	if(!pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)))
		return FALSE;

	CTermPlanDoc* pTPDoc = (CTermPlanDoc*)pDoc;
	int nIdx = nID - ID_VIEW_NAMEDVIEWS9;
	ASSERT(nIdx >= 0);

	
	if (pMDIChild && pMDIChild->IsKindOf(RUNTIME_CLASS(CChildFrame)))
	{
		CChildFrame* pChildFrame = (CChildFrame*)pMDIChild;
		pTPDoc->GetCurModeRenderCameras().m_b3DMode = false;
		ASSERT( nIdx < static_cast<int>(pTPDoc->GetCurModeCameras().m_savedViews.size()) );

		int nSavedViewCount = pTPDoc->GetCurModeCameras().m_savedViews.size();
		int n2DCount = 0;
		for(int i=0; i<nSavedViewCount; i++) {
			if(pTPDoc->GetCurModeCameras().m_savedViews[i].panecameras[0]->GetProjectionType() == C3DCamera::parallel) {
				if(n2DCount==nIdx) {
					nIdx = i;
					break;
				}
				n2DCount++;
			}
		}	

		if(pChildFrame->GetWorkingCamera(0,0)->GetProjectionType() == C3DCamera::parallel) {
			//save current working view to 2D user view
			pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView2D));
		}
		else {
			//save current working view to 3D user view
			pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView3D));
		}

		//load saved view to current working view
		pChildFrame->LoadWorkingViews(&(pTPDoc->GetCurModeCameras().m_savedViews[nIdx]));

		if(pTPDoc->GetCurModeCameras().m_savedViews[nIdx].panecameras[0]->GetProjectionType() == C3DCamera::parallel) {		
			//save current working view to 2D user view
			pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView2D));
		}
		else {
			ASSERT(0);
			//save current working view to 3D user view
			pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView3D));
		}

		pTPDoc->SetCameraWBS();
		pTPDoc->UpdateViewSharpTextures();
		pChildFrame->OnUpdateFrameTitle(TRUE);
		pTPDoc->UpdateAllViews(NULL);
		return TRUE;
	}
	else if (pMDIChild->IsKindOf(RUNTIME_CLASS(CRender3DFrame)))
	{
		pTPDoc->UpdateAllViews(NULL);
	}
	
	return FALSE;
}

BOOL CMainFrame::OnViewNamedViews3D(UINT nID) 
{
	CRender3DFrame* pRenderFrame = (CRender3DFrame*)GetNodViewFrame(RUNTIME_CLASS(CRender3DView));
	if (RenderEngine::getSingleton().GetAlwaysInUse() && pRenderFrame)
	{
		ASSERT(pRenderFrame->IsKindOf(RUNTIME_CLASS(CRender3DFrame)));
		pRenderFrame->OnViewNamedViews3D(nID);
		return TRUE;
	}


	int nIdx = nID - ID_VIEW_NAMEDVIEWS1;
	ASSERT(nIdx >= 0);

	CMDIChildWnd* pMDIChild = CreateOrActiveEnvModeView();
	if (!pMDIChild || !pMDIChild->IsKindOf(RUNTIME_CLASS(CChildFrame)))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CChildFrame* pChildFrame = (CChildFrame*)pMDIChild;
	CDocument* pDoc = pChildFrame->GetActiveDocument();
	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	CTermPlanDoc* pTPDoc = (CTermPlanDoc*)pDoc;
	pTPDoc->GetCurModeRenderCameras().m_b3DMode = true;
	ASSERT(nIdx < static_cast<int>(pTPDoc->GetCurModeCameras().m_savedViews.size()) );

	int nSavedViewCount = pTPDoc->GetCurModeCameras().m_savedViews.size();
	int n3DCount = 0;
	for(int i=0; i<nSavedViewCount; i++) {
		if(pTPDoc->GetCurModeCameras().m_savedViews[i].panecameras[0]->GetProjectionType() == C3DCamera::perspective) {
			if(n3DCount==nIdx) {
				nIdx = i;
				break;
			}
			n3DCount++;
		}
	}	
	
	if(pChildFrame->GetWorkingCamera(0,0)->GetProjectionType() == C3DCamera::parallel) {
		//save current working view to 2D user view
		pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView2D));
	}
	else {
		//save current working view to 3D user view
		pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView3D));
	}

	//load saved view to current working view
	pChildFrame->LoadWorkingViews(&(pTPDoc->GetCurModeCameras().m_savedViews[nIdx]));

	if(pTPDoc->GetCurModeCameras().m_savedViews[nIdx].panecameras[0]->GetProjectionType() == C3DCamera::parallel) {		
		ASSERT(0);
		//save current working view to 2D user view
		pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView2D));
	}
	else {
		//save current working view to 3D user view
		pChildFrame->SaveWorkingViews(&(pTPDoc->GetCurModeCameras().m_userView3D));
	}

	pTPDoc->SetCameraWBS();
	pTPDoc->UpdateViewSharpTextures();
	pChildFrame->OnUpdateFrameTitle(TRUE);
	pTPDoc->UpdateAllViews(NULL);
	return TRUE;
}

void CMainFrame::OnClose() 
{
	//get path  //changed matt 5,12,2004
	CString  sString = PROJMANAGER->GetAppPath() + "\\Config.ini";

	//save unit state into profile
	VERIFY(((CTermPlanApp*)AfxGetApp())->WriteProfileInt("UnitsManager", "LengthUnits", UNITSMANAGER->GetLengthUnits()));
	VERIFY(((CTermPlanApp*)AfxGetApp())->WriteProfileInt("UnitsManager", "WeightUnits", UNITSMANAGER->GetWeightUnits()));

	if(AfxMessageBox("Are you sure you want to exit ARCport ALTOCEF?", MB_YESNO | MB_ICONQUESTION)==IDYES) 
	{
		CNewMDIFrameWnd::OnClose();
		//DESTROY_OPENDWGTOOLKIT;
		DESTROY_SHAPESMANAGER;
		DESTROY_ANIMTIMEMGR;
		DESTROY_UNITSMANAGER;
		DESTROY_TEXTMANAGER3D;

		DESTROYAIRPORTDBLIST;

		DESTROY_PROJMANAGER;

		DESTROY_ACALIASMANAGER;

		//close global database 
		//CGlobalAirportDataBase::CloseGoabalAirportDataBase() ;

		//write config.ini stored the Max Levels  //changed matt 5,12,2004
	    
        BOOL bRet = WritePrivateProfileString("Levels","Max_Pax_Type","16",sString.GetBuffer(0));	
	}
}

LRESULT CMainFrame::OnSystemCrashClose(WPARAM wParam,LPARAM lParam)
{
	CString  sString = PROJMANAGER->GetAppPath() + "\\Config.ini";

	//save unit state into profile
	VERIFY(((CTermPlanApp*)AfxGetApp())->WriteProfileInt("UnitsManager", "LengthUnits", UNITSMANAGER->GetLengthUnits()));
	VERIFY(((CTermPlanApp*)AfxGetApp())->WriteProfileInt("UnitsManager", "WeightUnits", UNITSMANAGER->GetWeightUnits()));

	CNewMDIFrameWnd::OnClose();
	//DESTROY_OPENDWGTOOLKIT;
	DESTROY_SHAPESMANAGER;
	DESTROY_ANIMTIMEMGR;
	DESTROY_UNITSMANAGER;
	DESTROY_TEXTMANAGER3D;

	DESTROYAIRPORTDBLIST;

	DESTROY_PROJMANAGER;

	DESTROY_ACALIASMANAGER;

	BOOL bRet = WritePrivateProfileString("Levels","Max_Pax_Type","16",sString.GetBuffer(0));

	if ((BOOL)wParam)
	{
		//CString strInputPath;
		//strInputPath.Format(_T("%s\\ARCport_ALT.exe"),PROJMANAGER->GetAppPath());

		char chPath[MAX_PATH];

		CString strPath;

		GetModuleFileName(NULL,chPath,MAX_PATH);
		strPath = chPath;
	
		::WinExec(strPath,SW_MAXIMIZE);
	}

	return TRUE;
	
}

//chloe
void CMainFrame::OnDestroy()
{
	//CHLOE_SYSTEM(ARCRSystem)->destory();
}
//

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{		

	// route to standard targets
	if(CNewMDIFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// route to inactive views as well
	// get the active MDI child
	CMDIChildWnd* pChild = (CMDIChildWnd*) GetActiveFrame();
	if(pChild != NULL)
	{
		CTermPlanDoc* pDoc = (CTermPlanDoc*) pChild->GetActiveDocument();
		
		if(pDoc != NULL)
		{
			return pDoc->RouteCmdToAllViews(GetActiveView(), nID, nCode, pExtra, pHandlerInfo);
		}
	}
	return FALSE;
}

/*
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CMDIFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
*/







BOOL CMainFrame::CreateCompRepLogBar()
{
	if(!m_wndCompRepLogBar.Create(_T("Compare Report Log"), this, IDC_COMPREPLOG)) {
        TRACE0("Failed to create log bar\n");
        return FALSE;      // fail to create
	}

    m_wndCompRepLogBar.SetBarStyle(m_wndCompRepLogBar.GetBarStyle() | CBRS_SIZE_DYNAMIC);

	m_wndCompRepLogBar.EnableDocking(CBRS_ALIGN_BOTTOM);
    return TRUE;
}

BOOL CMainFrame::CreateShapesBarPFlow()
{
	if(!m_wndShapesBarPFlow.Create(_T("ShapePFlow Templates"), this, IDC_LIST_SHAPESPFLOW)) {
        TRACE0("Failed to create shapes bar\n");
        return FALSE;      // fail to create
	}

    m_wndShapesBarPFlow.SetBarStyle(m_wndShapesBarPFlow.GetBarStyle() | CBRS_SIZE_DYNAMIC);

	m_wndShapesBarPFlow.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
    return TRUE;
}

BOOL CMainFrame::CreateShapesBar()
{
	if(!m_wndShapesBar.Create(_T("Shape Templates"), this, IDC_LIST_SHAPES)) {
        TRACE0("Failed to create shapes bar\n");
        return FALSE;      // fail to create
	}

    m_wndShapesBar.SetBarStyle(m_wndShapesBar.GetBarStyle() | CBRS_SIZE_DYNAMIC);

	m_wndShapesBar.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
    return TRUE;
}

BOOL CMainFrame::CreateACComponentShapesBar()
{
	if(!m_wndACComponentShapesBar.Create(_T("Component Templates"), this, IDC_LIST_SHAPES)) {
        TRACE0("Failed to create AC component shapes bar\n");
        return FALSE;      // fail to create
	}

    m_wndACComponentShapesBar.SetBarStyle(m_wndACComponentShapesBar.GetBarStyle() | CBRS_SIZE_DYNAMIC);

	m_wndACComponentShapesBar.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
    
	return TRUE;
}
BOOL CMainFrame::CreateACFurnishingShapesBar()
{
	if(!m_wndACFurnishingShapesBar.Create(_T("Furnishing Templates"), this, IDC_LIST_SHAPES)) {
        TRACE0("Failed to create AC furnishing shapes bar\n");
        return FALSE;      // fail to create
	}

    m_wndACFurnishingShapesBar.SetBarStyle(m_wndACFurnishingShapesBar.GetBarStyle() | CBRS_SIZE_DYNAMIC);

	m_wndACFurnishingShapesBar.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
    
	return TRUE;
}

BOOL CMainFrame::CreateAircraftModelBar()
{
	if (!m_wndAircraftModel.Create(_T("Aircraft Model Templates"),this,ID_AIRCRAFT_MODEL))
	{
		TRACE0("Failed to create shapes bar\n");
		return FALSE;      // fail to create
	}
	m_wndAircraftModel.SetBarStyle(m_wndAircraftModel.GetBarStyle() | CBRS_SIZE_DYNAMIC);
	m_wndAircraftModel.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	return TRUE;
}
void CMainFrame::DockControlBarLeftOf(CToolBar* pBar, CToolBar* pLeftOf) 
{ 
    CRect rect; 
    DWORD dw = 0; 
    UINT n = 0; 

	if(!pBar || !pLeftOf)return;

	
    // get MFC to adjust the dimensions of all docked ToolBars 
    // so that GetWindowRect will be accurate 
	CRect rectMain,rectBar;
	RecalcLayout(); 
	GetWindowRect(&rectMain);
    pLeftOf->GetWindowRect(&rect); 
	pBar->GetWindowRect(&rectBar);
    rect.OffsetRect(rect.Width()+2,0);
    dw=pLeftOf->GetBarStyle(); 
    n = 0; 
    n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n; 
    n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n; 
    n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n; 
    n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n; 
	
	if (!pLeftOf->IsVisible()){
		if (pBar->IsVisible())
			DockControlBar(pBar,n);
		return;
	}
    // When we take the default parameters on rect, DockControlBar will dock 
    // each Toolbar on a seperate line.  By calculating a rectangle, we in effect 
    // are simulating a Toolbar being dragged to that location and docked. 
	if (pBar->IsVisible()) {
		if (rect.left + rectBar.Width() <= rectMain.right){
			rect.right = rect.left + rectBar.Width();
			DockControlBar(pBar,n,&rect); 
		}else{
			rectBar.right = rectBar.Width();
			rectBar.left = rectMain.left + 2;
			rectBar.right = rectBar.left + rectBar.right;
			rectBar.bottom = rectBar.Height();
			rectBar.top = rect.bottom;
			rectBar.bottom = rectBar.top + rectBar.bottom;
			DockControlBar(pBar,n,&rectBar);
		}
	}
} 


#if 0
void CMainFrame::OnUpdateGridSpacing(CCmdUI *pCmdUI)
{
	C3DView* p3DView = Get3DView();
	if(p3DView) { // if its valid
		double ds = p3DView->GetDocument()->GetActiveFloor()->m_dGridSpacing;
		CString s(_T(""));
		CUnitsManager* pUM = CUnitsManager::GetInstance();
		s.Format("\tGrid Spacing: %5.2f %s", 
			pUM->ConvertLength(ds), 
			pUM->GetLengthUnitString(pUM->GetLengthUnits()));
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText( s );
	}
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnUpdate3dMousePos(CCmdUI *pCmdUI)
{
	C3DView* p3DView = Get3DView();
	if(p3DView) { // if its valid
		CString s(_T(""));
		CUnitsManager* pUM = CUnitsManager::GetInstance();
		s.Format("\tX: %5.2f %s | Y:%5.2f %s", 
			pUM->ConvertLength(p3DView->GetMousePos()[VX]), 
			pUM->GetLengthUnitString(pUM->GetLengthUnits()),
			pUM->ConvertLength(p3DView->GetMousePos()[VY]),
			pUM->GetLengthUnitString(pUM->GetLengthUnits()));
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText( s );
	}
	else {
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnUpdate3dMouseMode(CCmdUI* pCmdUI)
{
	C3DView* p3DView = Get3DView();
	if(p3DView) { // if its valid
		CString s(_T(""));
		pCmdUI->Enable(TRUE);
		switch(p3DView->m_eMouseState)
		{
		case C3DView::_floormanip:
			s = _T("\tFloor Manipulation Mode");
			break;
		case C3DView::_default:
			s = _T("\tNavigation Mode");
			break;
		case C3DView::_placeproc:
			s = _T("\tPlace Processor");
			break;
		case C3DView::_scaleproc:
			s = _T("\tScale Processor");
			break;
		case C3DView::_scaleprocX:
			s = _T("\tScale Processor X");
			break;
		case C3DView::_scaleprocY:
			s = _T("\tScale Processor Y");
			break;
		case C3DView::_scaleprocZ:
			s = _T("\tScale Processor Z");
			break;
		case C3DView::_rotateproc:
			s = _T("\tRotate Processor");
			break;
		case C3DView::_rotateairsideobject:
			s = _T("\tRotate Airside Object");
			break;
		case C3DView::_getonepoint:
			s = _T("\tSelect a coordinate");
			break;
		case C3DView::_getmanypoints:
			s = _T("\tSelect points, RC when done");
			break;

		}
		pCmdUI->SetText( s );
	}
	else
		pCmdUI->Enable(FALSE);
}
#endif

LRESULT CMainFrame::OnDlgFallback(WPARAM wParam, LPARAM lParam)
{	
	CMDIChildWnd* pActiveChild = MDIGetActive();
	if(!pActiveChild)
		return FALSE;
	CDocument* pActiveDoc = pActiveChild->GetActiveDocument();
	if (!pActiveDoc || !pActiveDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)))
		return FALSE;

	CTermPlanDoc* pTermDoc = (CTermPlanDoc*)pActiveDoc;
	CreateOrActiveEnvModeView();

	FallbackData* pData = (FallbackData*) wParam;
	if(pData)
	{
		IRender3DView* p3DView = pTermDoc->GetIRenderView();
		if(!p3DView)
			return FALSE;
		p3DView->m_hFallbackWnd = pData->hSourceWnd;
		p3DView->m_lFallbackParam = lParam;
		p3DView->m_eMouseState = NS3DViewCommon::GetMouseStateByFallbackReason((FallbackReason)lParam);
		p3DView->OnNewMouseState();
		/*if(pData->bCustomHeight)
		{
			pTermDoc->GetLayoutEditor()->GetCurContext()->UseTempWorkingHeight(pData->dWorkHeight);
		}*/
	
	}
	return TRUE;
}

void ClearComboBoxEntries(CComboBox* pCmb)
{
	int nCount = pCmb->GetCount();
	for(int i=0; i<nCount; i++)
		pCmb->DeleteString(0);
}

void CMainFrame::OnSelectSelectProject()
{
	CComboBox* pCmb = m_wndProjectBar.GetComboBoxProj();
	int iCurSel = pCmb->GetCurSel();
	CDocument* pDoc = (CDocument*) pCmb->GetItemDataPtr( iCurSel );
	if (pDoc == NULL)
		return;
	// TRACE("Selected proj:%s\n", pDoc->GetTitle());
	//////////////////////////////////////////////////////////////////////////
	// add by bird, 2003/6/17
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if(pMDIActive != NULL)
	{
		CDocument* pActiveDoc = pMDIActive->GetActiveDocument();
		// if is the same document and the document is play, then return!
		if( pDoc == pActiveDoc
			&& ((CTermPlanDoc*)pActiveDoc)->m_eAnimState!=0 )	// 0== anim_none
		{
			UpdateProjSelectBar();
			return;
		}
	}	
	
	if( ((CTermPlanDoc*)pDoc)->GetTerminal().getCurrentSimResult( ) >=0 )
	{
		// get the SimResult index by string
		CString strItem;
		pCmb->GetLBText( iCurSel, strItem );
		strItem = strItem.Right(5);
		strItem.Remove('(');
		int iCurIndex = atoi( strItem )-1;
		assert( iCurIndex>=0 && iCurIndex< ((CTermPlanDoc*)pDoc)->getSimResultCout() );
		
		((CTermPlanDoc*)pDoc)->GetTerminal().setCurrentSimResult( iCurIndex );
		
		// if is the same document, return
		CMDIChildWnd* pMDIActive = MDIGetActive();
		if(pMDIActive != NULL)
		{
			CDocument* pActiveDoc = pMDIActive->GetActiveDocument();
			if( pDoc == pActiveDoc )
				return;
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	
	//iterate through all views, finding views of related to pDoc
	//if possible find the modeling sequence view
	POSITION pos = pDoc->GetFirstViewPosition();
	while(pos != NULL) 
	{
		CView* pView = pDoc->GetNextView(pos);
		CFrameWnd* pParent = pView->GetParentFrame();
		//SetActiveView(pParent);
		pParent->ActivateFrame();
	}
	UpdateProjSelectBar();
}

/*
void CMainFrame::OnNotifyShapeGetObject(NMHDR* pNMHDR, LRESULT* pResult)
{
}

void CMainFrame::OnNotifyShapeLBDown(NMHDR* pNMHDR, LRESULT* pResult)
{
}
*/
void CMainFrame::DeleteItem(const CString& strName)
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if( pMDIActive == NULL )
		return ;
	CTermPlanDoc* pDoc = NULL;
	CView* pView = pMDIActive->GetActiveView();
	if(pView->IsKindOf( RUNTIME_CLASS( CFlowChartPane )))
		pDoc = (CTermPlanDoc*)((CFlowChartPane*)pView)->GetDocumentPtr();
	else
		pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();

	if( pDoc == NULL )
		return ;

	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = pDoc->GetNextView(pos);
		if(pView->IsKindOf( RUNTIME_CLASS( CMathematicView ) ) )
		{
			((CMathematicView*)pView)->DeleteItem(strName);
		}
	}
}
LRESULT CMainFrame::OnCabinSelChanged(WPARAM wParam, LPARAM lParam)
{
	// Please Modify to use SetDragDropData
// 	if (wParam != NULL)
// 	{
// 		CCrewPosition* pCrew = (CCrewPosition*)(wParam);
// 		ASSERT(pCrew != NULL);
// 
// 		OleDragDropManager::SetRawDragDropData(&pCrew, sizeof(CCrewPosition*));
// 	}
	return 0;
}

LRESULT CMainFrame::ONAIRSelChanged(WPARAM wParam,LPARAM lParam)
{
	if (wParam != NULL)
	{
		CComponentMeshModel* pModel = (CComponentMeshModel*)(wParam);
		ASSERT(pModel != NULL);

// 		CAircraftComponetNode::ComponentNODEINFO compInfo;
// 		compInfo.pModel = pModel;
// 		compInfo.bEditModel = true;
// 		OleDragDropManager::SetRawDragDropData(&compInfo, sizeof(CAircraftComponetNode::ComponentNODEINFO));
	}
	return 0;
}

LRESULT CMainFrame::OnSLBSelChanged(WPARAM wParam, LPARAM lParam)
{
	/*
	// TRACE("CMainFrame::OnSLBSelChanged()\n");
	const CShapesListBox* pSLB = m_wndShapesBar.GetShapeListBox();
	int nSelIdx = pSLB->GetCurSel();
	CShape* pSelShape = NULL;
	if(nSelIdx != LB_ERR) {
		pSelShape = (CShape*) pSLB->GetItemData(nSelIdx);
		ReleaseCapture();
	}
	else
		return 0;
	*/
	if(wParam != NULL)
	{
		CShape* pSelShape = (CShape*)(wParam);
		ASSERT(pSelShape != NULL);
		CShape::SHAPEINFO shapeInfo;
		shapeInfo.pShape = pSelShape;
		if(lParam == (LPARAM)0)
			strcpy(shapeInfo.name, pSelShape->Name());
		else
			strcpy(shapeInfo.name, (char*)lParam);

		// DragDropDataItem::Type_TerminalShape == DragDropDataItem::Type_ShapePF
		OleDragDropManager::GetInstance().SetDragDropData(DragDropDataItem::Type_TerminalShape, &shapeInfo, sizeof(CShape::SHAPEINFO));
	}
	else
	{
		CMDIChildWnd* pMDIActive = MDIGetActive();
		if( pMDIActive == NULL )
			return 0;
		CTermPlanDoc* pDoc = NULL;
		CView* pView = pMDIActive->GetActiveView();
		if(pView->IsKindOf( RUNTIME_CLASS( CFlowChartPane )))
			pDoc = (CTermPlanDoc*)((CFlowChartPane*)pView)->GetDocumentPtr();
		else
			pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();

		if( pDoc == NULL )
			return 0;

		POSITION pos = pDoc->GetFirstViewPosition();
		while (pos != NULL)
		{
			CView* pView = pDoc->GetNextView(pos);
			if(pView->IsKindOf( RUNTIME_CLASS( CMathematicView ) ) )
			{
				CString strName = "";
				strName.Format("%s", (char*)lParam);
				((CMathematicView*)pView)->SetSelectItem(strName);
//				SetActiveView(pView);
			}
		}
	}

	
	return 0;
}

void CMainFrame::OnDropDownSelectProject()
{
	CComboBox* pCmb = m_wndProjectBar.GetComboBoxProj();
	//erase all entries
	ClearComboBoxEntries(pCmb);
	//add the current proj to the list first
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if(pMDIActive == NULL)
		return;
	CDocument* pActiveDoc = pMDIActive->GetActiveDocument();
	if(pActiveDoc == NULL)
		return;
	int nCount=0;
	//////////////////////////////////////////////////////////////////////////
	// add by bird 2003/6/17
	CString strItem = pActiveDoc->GetTitle();
	int iSimResultIndex = ((CTermPlanDoc*)pActiveDoc)->GetTerminal().getCurrentSimResult();
	if( iSimResultIndex>=0 )	// valid sim_result
	{
		for( int i = 0; i<((CTermPlanDoc*)pActiveDoc)->getSimResultCout(); i++ )
		{
			CString strNum;
			strNum.Format( "%d", i+1 );
			// make string like "003"
			for( int j =0; j<=3-strNum.GetLength(); j++ )
				strNum = "0" + strNum;
			strNum = "(" +strNum+ ")";
			
			// add string to combox
			pCmb->AddString( strItem + strNum );
			pCmb->SetItemDataPtr( nCount++, (void*) pActiveDoc );
			if( iSimResultIndex == i )
				pCmb->SetCurSel( i );
		}
	}
	else					   // no valid sim_result
	{
		pCmb->AddString( strItem );
		pCmb->SetItemDataPtr( nCount, (void*) pActiveDoc );
		pCmb->SetCurSel( 0 );
	}

	//now go through the rest of the docs, adding them to the list
	POSITION pos = AfxGetApp()->m_pDocManager->GetFirstDocTemplatePosition();
	while (pos != NULL)	{
		CDocTemplate* pTemplate = (CDocTemplate*)AfxGetApp()->m_pDocManager->GetNextDocTemplate(pos);
		POSITION pos2 = pTemplate->GetFirstDocPosition();
		while (pos2) {
			CDocument* pDoc = NULL;
			if((pDoc = pTemplate->GetNextDoc(pos2)) != NULL)
				if(pDoc->GetTitle().Compare(pActiveDoc->GetTitle()) != 0) 
				{
					iSimResultIndex = ((CTermPlanDoc*)pDoc)->GetTerminal().getCurrentSimResult(); 
					if( iSimResultIndex >= 0)		// valid sim_result
					{
						for( int i = 0; i<((CTermPlanDoc*)pDoc)->getSimResultCout(); i++ )
						{
							CString strNum;
							strNum.Format( "%d", i+1 );
							// make string like "(003)"
							for( int j =0; j<=3-strNum.GetLength(); j++ )
								strNum = "0" + strNum;
							strNum = "(" + strNum + ")";

							// add string to combox
							pCmb->AddString( pDoc->GetTitle() + strNum );
							pCmb->SetItemDataPtr( nCount++, (void*) pDoc );
						}
					}
					else							// no valid sim_result
					{
						// add string to combox
						pCmb->AddString( pDoc->GetTitle());
						pCmb->SetItemDataPtr( pCmb->GetCount() -1, (void*) pDoc );
//						nCount += 1;
					}
				}
		}
	}
}

void CMainFrame::UpdateProjSelectBar()
{
	CComboBox* pCmb = m_wndProjectBar.GetComboBoxProj();
	if(pCmb->m_hWnd == NULL)
		return;
	//erase all entries
	ClearComboBoxEntries(pCmb);
	pCmb->RedrawWindow();
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if(pMDIActive == NULL)
		return;
	CDocument* pActiveDoc = pMDIActive->GetActiveDocument();
	if(pActiveDoc == NULL)
		return;
	//////////////////////////////////////////////////////////////////////////
	// modify by bird 2003/6/17
	CString strItem = pActiveDoc->GetTitle();
	int iCurSimResult =  ((CTermPlanDoc*)pActiveDoc)->GetTerminal().getCurrentSimResult();
	if( iCurSimResult>= 0 )		// is validate
	{
		CString strNum;
		strNum.Format( "%d", iCurSimResult+1 );
		// make string like "003"
		for( int j =0; j<=3-strNum.GetLength(); j++ )
			strNum = "0" + strNum;
		strNum = "("+strNum+")";
		
		strItem += strNum;
	}

	pCmb->AddString( strItem );
	pCmb->SetItemDataPtr(0, (void*) pActiveDoc);
	pCmb->SetCurSel(0);
	pCmb->RedrawWindow();


}
void CMainFrame::getCurrentEnviro(EnvironmentMode& enviro , SimulationType& sim_type)
{
  
	m_wndToolBar.getCurrentEnviro(enviro,sim_type); 
}
/*
void CMainFrame::OnSelectActiveFloor()
{

}

void CMainFrame::OnDropDownActiveFloor()
{
	
}

void CMainFrame::UpdateActiveFloorCB()
{
	CComboBox* pCmb = m_wndLayoutBar.GetActiveFloorCB();
	if(pCmb->m_hWnd == NULL)
		return;
	//erase all entries
	ClearComboBoxEntries(pCmb);
	pCmb->RedrawWindow();
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if(pMDIActive == NULL)
		return;
	CTermPlanDoc* pActiveDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	if(pActiveDoc == NULL)
		return;

	CString strItem = pActiveDoc->GetActiveFloor()->FloorName();
	pCmb->AddString( strItem );
	pCmb->SetItemDataPtr(0, (void*) pActiveDoc);
	pCmb->SetCurSel(0);
	pCmb->RedrawWindow();
}
*/

void CMainFrame::OnDatabaseShapes() 
{
	CDlgShapeSelect dlg;
	dlg.DoModal();
}

void CMainFrame::OnReportsPassengerActivitylog() 
{

	// TODO: Add your command handler code here
	LoadReport(ENUM_PAXLOG_REP);
}

void CMainFrame::OnReportsProcessorUtilization() 
{
	// TODO: Add your command handler code here	
	LoadReport(ENUM_UTILIZATION_REP);
}

void CMainFrame::OnReportsPassengerDistancetravelled() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_DISTANCE_REP);
}

void CMainFrame::OnReportsSpaceOccupancy() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_PAXCOUNT_REP);
}


void CMainFrame::OnReportsPassengerTimeinqueues() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_QUEUETIME_REP);
}

void CMainFrame::OnReportsPassengerNumber() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_PASSENGERNO_REP);
}

void CMainFrame::OnReportsProcessorThroughput() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_THROUGHPUT_REP);
}

void CMainFrame::LoadReportProcessFlow(enum ENUM_REPORT_TYPE _enumRepType)
{
	switch((UINT)_enumRepType)
	{
	case ENUM_QUEUETIME_REP:
	case ENUM_QUEUELENGTH_REP:
	case ENUM_THROUGHPUT_REP:
	case ENUM_PASSENGERNO_REP:
	case ENUM_UTILIZATION_REP:
		{
			ShowControlBar(&m_wndCompRepLogBar, FALSE, FALSE);
			ShowControlBar(&m_wndShapesBarPFlow, FALSE, FALSE);
			CMDIChildWnd* pMDIActive = MDIGetActive();
			ASSERT(pMDIActive != NULL);
			CDocument* pDoc = pMDIActive->GetActiveDocument();
			ASSERT(pDoc != NULL);

			((CTermPlanDoc*)pDoc)->LoadMathResult();
			CMathResultManager* pManager = &((CTermPlanDoc*)pDoc)->m_MathResultManager;
			CView * pTempView;
			POSITION pos = pDoc->GetFirstViewPosition();
			bool b1 = false, b2 = false, b3 = false;
			while (pos != NULL)
			{
				pTempView = pDoc->GetNextView(pos);
				if (pTempView->IsKindOf(RUNTIME_CLASS(CRepGraphView)))
				{
					((CRepGraphView *)pTempView)->ResetAllContent();
					((CRepGraphView *)pTempView)->LoadReportProcessFlow(pManager, _enumRepType);
					b1 = true;
				}
				else if(pTempView->IsKindOf(RUNTIME_CLASS(CRepListView)))
				{
					((CRepListView *)pTempView)->ResetAllContent();
					((CRepListView *)pTempView)->LoadReportProcessFlow(pManager, _enumRepType);
					b2 = true;
				}
				else if(pTempView->IsKindOf(RUNTIME_CLASS(CRepControlView)))
				{
					b3 = true;
/*					((CRepControlView *)pTempView)->bShowLoadButton = TRUE;//bExist;
					((CRepControlView *)pTempView)->ResetAllContent();
					((CRepControlView *)pTempView)->Clear();
					
*/				}
				if(b1&&b2&&b3)
				{
					b1 = false;
					CReportChildFrameSplit* pWnd = (CReportChildFrameSplit*)((CRepControlView*)pTempView)->GetParentFrame();
//					pWnd->m_wndSplitter2.DeleteRow(0);
//					CRect rc(0, 0, 0, 0);
//					pWnd->GetWindowRect(&rc);
//					pWnd->m_wndSplitter2.SetRowInfo(0, 10, rc.Height());
//					pWnd->m_wndSplitter2.UpdateWindow();
				}
			}
		}
		break;
	default:
		break;
	}
	
	return ;
}

void CMainFrame::LoadReport(enum ENUM_REPORT_TYPE _enumRepType ) 
{
	// first check if iCurrentSimIdx >= 0
	// if false, then return;
	CMDIChildWnd* pMDIActive = MDIGetActive();
	CTermPlanDoc* pActiveDoc = NULL;
	if(pMDIActive != NULL)
	{
		pActiveDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		ASSERT(pActiveDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
		if (pActiveDoc->m_simType == SimType_MontCarlo)
		{
			int iCurrentIdx = ((CTermPlanDoc*)pActiveDoc)->GetTerminal().getCurrentSimResult();
			if( iCurrentIdx <0 )
			{
				AfxMessageBox("No simulation result, so can not do report!\r\n you must first run engine. ",MB_OK|MB_ICONINFORMATION);
				return;
			}
		}
	}	
	//////////////////////////////////////////////////////////////////////////
	if (pActiveDoc == NULL)
		return;
	
	CMDIChildWnd* pChildFrm = NULL;
	if (pActiveDoc->m_simType == SimType_MontCarlo)
		pChildFrm = CreateOrActivateFrame(theApp.m_pReportTemplate, RUNTIME_CLASS(CRepControlView));
	else if (pActiveDoc->m_simType == SimType_Mathmatical)
		pChildFrm = CreateOrActivateFrame(theApp.m_pMathReportTemplate, RUNTIME_CLASS(CRepControlView));
	
	if (pChildFrm == NULL)
		return;
    
	CView* pView = pChildFrm->GetActiveView();
	ASSERT(pView->GetDocument()->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	CTermPlanDoc* pDoc = (CTermPlanDoc*)(pView->GetDocument());
	pDoc->GetARCReportManager().SetReportType(_enumRepType);

	bool bExist = TRUE;
	if (pDoc->m_simType == SimType_MontCarlo)
	{
		int iCurrentSimIdx = pDoc->GetTerminal().getCurrentSimResult();
		ASSERT( iCurrentSimIdx>=0 );
		
		pDoc->GetTerminal().GetSimReportManager()->SetCurrentSimResult( iCurrentSimIdx );
		pDoc->GetTerminal().GetSimReportManager()->SetCurrentReportType( _enumRepType );
	
		bExist = pDoc->GetTerminal().GetSimReportManager()->getSimItem( pDoc->GetTerminal().getCurrentSimResult() ) ->isReportRealExist( _enumRepType );

		CReportParameter* pPara = pDoc->GetTerminal().m_pReportParaDB->GetReportPara( _enumRepType );
		ASSERT( pPara );
		pDoc->GetARCReportManager().SetReportPara( pPara );

		pView->GetParentFrame()->OnUpdateFrameTitle(TRUE);
		pView->GetParentFrame()->ShowWindow(SW_SHOW);
		pView->Invalidate(FALSE);
	
		
		CView * pTempView;
		POSITION pos = pDoc->GetFirstViewPosition();
		while (pos != NULL)
		{
			pTempView = pDoc->GetNextView(pos);
			if (pTempView->IsKindOf(RUNTIME_CLASS(CRepGraphView)))
			{
				((CRepGraphView *)pTempView)->ResetAllContent();
			}
			else if(pTempView->IsKindOf(RUNTIME_CLASS(CRepListView)))
			{
				((CRepListView *)pTempView)->ResetAllContent();
			}
			else if(pTempView->IsKindOf(RUNTIME_CLASS(CRepControlView)))
			{
				((CRepControlView *)pTempView)->bShowLoadButton = bExist;
				((CRepControlView *)pTempView)->ResetAllContent();
				((CRepControlView *)pTempView)->Clear();
				
				CReportChildFrameSplit* pWnd = (CReportChildFrameSplit*)((CRepControlView*)pTempView)->GetParentFrame();
				CRect rc(0, 0, 0, 0);
				pWnd->GetWindowRect(&rc);
				pWnd->m_wndSplitter2.SetRowInfo(0, 350, 236);
				
				pWnd->ShowWindow(SW_SHOWMAXIMIZED);
			}
		}
	}

	if(	pDoc->m_simType == SimType_Mathmatical)
	{		
		pView->GetParentFrame()->OnUpdateFrameTitle(TRUE);
		pView->GetParentFrame()->ShowWindow(SW_SHOWMAXIMIZED);

		LoadReportProcessFlow(_enumRepType);
	}	


	if( !pDoc->GetTerminal().resultsLoaded() )
	{
		//MessageBox( "No results available to report on" );
		return;
	}
//	((CRepControlView *)pView)->Clear();

}
static CString CallbackGetAirsideLogFilePath(InputFiles enumInputFile)
{
	CString strPath = _T("");
	CMainFrame *pMainFrame = (CMainFrame *)(((CTermPlanApp *)AfxGetApp())->m_pMainWnd);
	CMDIChildWnd* pMDIActive = pMainFrame->MDIGetActive();
	CTermPlanDoc* pDoc = NULL;
	if (pMDIActive != NULL)
	{

		pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
		int iCurrentSimIdx = pDoc->GetTerminal().getCurrentSimResult();
		//ASSERT( iCurrentSimIdx>=0 );
		if (iCurrentSimIdx >= 0)
		{
			pDoc->GetAirsideSimLogs().GetSimReportManager()->SetCurrentSimResult( iCurrentSimIdx );
			strPath = pDoc->GetAirsideSimLogs().GetSimReportManager()->GetCurrentLogsFileName(enumInputFile,pDoc->m_ProjInfo.path);
		}


	}

	return strPath;
}
void CMainFrame::LoadAirsideReport_New(enum reportType airsideRpType)
{
	// first check if iCurrentSimIdx >= 0
	// if false, then return;
	CMDIChildWnd* pMDIActive = MDIGetActive();
	CTermPlanDoc* pActiveDoc = NULL;
	if(pMDIActive != NULL)
	{
		pActiveDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		ASSERT(pActiveDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
		//if (pActiveDoc->m_simType == SimType_MontCarlo)
		//{
		//	int iCurrentIdx = ((CTermPlanDoc*)pActiveDoc)->GetTerminal().getCurrentSimResult();
		//	if( iCurrentIdx <0 )
		//	{
		//		AfxMessageBox("No simulation result, so can not do report!\r\n you must first run engine. ",MB_OK|MB_ICONINFORMATION);
		//		return;
		//	}
		//}
	}	
	//////////////////////////////////////////////////////////////////////////
	if (pActiveDoc == NULL)
		return;


	CTermPlanDoc* pDoc = pActiveDoc;
	reportType type = pDoc->GetARCReportManager().GetAirsideReportManager()->GetReportType() ;
	pDoc->GetARCReportManager().GetAirsideReportManager()->SetReportType(airsideRpType);
	pDoc->GetARCReportManager().GetAirsideReportManager()->SetCBGetLogFilePath(CallbackGetAirsideLogFilePath);
    pDoc->GetARCReportManager().GetAirsideReportManager()->SetAirportDB(pDoc->GetTerminal().m_pAirportDB) ;

	//get report file directory
	int nCurSimResult = pDoc->GetTerminal().getCurrentSimResult();
	CString strSimResultFolderName;
	strSimResultFolderName.Format(_T("%s%d"),strSimResult,nCurSimResult);
	CString strReportFileDir;
	strReportFileDir.Format(_T("%s\\SimResult\\%s\\report"),pDoc->m_ProjInfo.path,strSimResultFolderName);
	pDoc->GetARCReportManager().GetAirsideReportManager()->SetReportPath(strReportFileDir);


	//if(type != airsideRpType)
      pDoc->GetARCReportManager().GetAirsideReportManager()->InitUpdate(pDoc->GetInputAirside().GetProjectID(pDoc->m_ProjInfo.name)) ;
	pDoc->GetARCReportManager().GetAirsideReportManager()->SetProjectID(pDoc->GetInputAirside().GetProjectID(pDoc->m_ProjInfo.name));


	CMDIChildWnd* pChildFrm = NULL;
	//if (pActiveDoc->m_simType == SimType_MontCarlo)
		pChildFrm = CreateOrActivateFrame(theApp.m_pAirsideReportTemplate_New, RUNTIME_CLASS(CAirsideRepControlView));
	//else if (pActiveDoc->m_simType == SimType_Mathmatical)
	//	pChildFrm = CreateOrActivateFrame(theApp.m_pMathReportTemplate, RUNTIME_CLASS(CAirsideRepControlView));

	if (pChildFrm == NULL)
		return;

	CView* pView = pChildFrm->GetActiveView();
	ASSERT(pView->GetDocument()->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	

	bool bExist = TRUE;

		//CReportParameter* pPara = pDoc->GetTerminal().m_pReportParaDB->GetReportPara( _enumRepType );
		//ASSERT( pPara );
		//pDoc->SetReportPara( pPara );

		pView->GetParentFrame()->OnUpdateFrameTitle(TRUE);
		pView->GetParentFrame()->ShowWindow(SW_SHOW);
		pView->Invalidate(FALSE);


		CView * pTempView;
		POSITION pos = pDoc->GetFirstViewPosition();
		while (pos != NULL)
		{
			pTempView = pDoc->GetNextView(pos);
			if (pTempView->IsKindOf(RUNTIME_CLASS(CAirsideReportGraphView)))
			{
				((CAirsideReportGraphView *)pTempView)->ResetAllContent();
			}
			else if(pTempView->IsKindOf(RUNTIME_CLASS(CAirsideReportListView)))
			{
				((CAirsideReportListView *)pTempView)->ResetAllContent();
			}
			else if(pTempView->IsKindOf(RUNTIME_CLASS(CAirsideRepControlView)))
			{
				//((CAirsideRepControlView *)pTempView)->bShowLoadButton = bExist;
				((CAirsideRepControlView *)pTempView)->ResetAllContent();
				//((CAirsideRepControlView *)pTempView)->Clear();

				CAirsideReportChildFrameSplit* pWnd = (CAirsideReportChildFrameSplit*)((CAirsideRepControlView*)pTempView)->GetParentFrame();
				CRect rc(0, 0, 0, 0);
				pWnd->GetWindowRect(&rc);
				pWnd->m_wndSplitter2.SetRowInfo(0, 350, 236);

				pWnd->ShowWindow(SW_SHOWMAXIMIZED);

			}
		}
	//	pDoc->UpdateAllViews(NULL,AIRSIDEREPORT_SHOWREPORT,NULL);
}

void CMainFrame::OnReportsProcessorQueuelength() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_QUEUELENGTH_REP);	
}

void CMainFrame::OnReportsProcessorCriticalqueueparameters() 
{ 
	// TODO: Add your command handler code here
	LoadReport(ENUM_AVGQUEUELENGTH_REP);			
}

void CMainFrame::OnReportsPassengerTimeInService() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_SERVICETIME_REP);
}

void CMainFrame::OnReportsPassengerTimeinTerminal() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_DURATION_REP);
}

void CMainFrame::OnReportsBaggageBaggagecount() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_BAGCOUNT_REP);				
}

void CMainFrame::OnReportsBaggageBaggagewaitingtime() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_BAGWAITTIME_REP);				
}

void CMainFrame::OnReportsSpaceDensity() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_PAXDENS_REP);					
}

void CMainFrame::OnReportsPassengerActivitybreakdown() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_ACTIVEBKDOWN_REP);					
}

void CMainFrame::OnReportsPassengerFireevacution() 
{
	LoadReport(ENUM_FIREEVACUTION_REP);
}


BOOL CMainFrame::DestroyWindow() 
{
	/*
	CString sProfile = _T("ToolBarState");
	m_wndShapesBar.SaveState(sProfile);
	SaveBarState(sProfile);
	*/
	SaveState();

	return CNewMDIFrameWnd::DestroyWindow();
}

void CMainFrame::OnAnimationSetSpeed(UINT nID)
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);
	
	TBBUTTONINFO btnInfo;
	btnInfo.cbSize = sizeof(TBBUTTONINFO);
	btnInfo.dwMask = TBIF_COMMAND | TBIF_IMAGE | TBIF_STATE | TBIF_STYLE ;
	btnInfo.idCommand = ID_ANIMPICKSPEED;
	btnInfo.fsState = TBSTATE_ENABLED;
	btnInfo.fsStyle =  TBSTYLE_TOOLTIPS | BTNS_WHOLEDROPDOWN;
	if(nID == ID_ANIMATION_SET1SPEED) {
		btnInfo.iImage = SPEEDBUTTON1;
		pDoc->m_animData.nAnimSpeed = 10;
	}
	else if(nID == ID_ANIMATION_SET5SPEED) {
		btnInfo.iImage = SPEEDBUTTON5;
		pDoc->m_animData.nAnimSpeed = 50;
	}
	else if(nID == ID_ANIMATION_SET10SPEED) {
		btnInfo.iImage = SPEEDBUTTON10;
		pDoc->m_animData.nAnimSpeed = 100;
	}
	else if(nID == ID_ANIMATION_SET25SPEED) {
		btnInfo.iImage = SPEEDBUTTON25;
		pDoc->m_animData.nAnimSpeed = 250;
	}
	else if(nID == ID_ANIMATION_SET50SPEED) {
		btnInfo.iImage = SPEEDBUTTON50;
		pDoc->m_animData.nAnimSpeed = 500;
	}
	else if(nID == ID_ANIMATION_SET100SPEED) {
		btnInfo.iImage = SPEEDBUTTON100;
		pDoc->m_animData.nAnimSpeed = 1000;
	}
	else if(nID == ID_ANIMATION_SETCUSTOMSPEED)
	{
		//btnInfo.iImage = SPEEDBUTTONCUSTOM;
		DlgAnimationSpeed dlg(pDoc->m_animData.nAnimSpeed);
		dlg.DoModal();
		/*if(dlg.DoModal() == IDOK)*/
		{
			pDoc->m_animData.nAnimSpeed = dlg.getAnimationSpeed();
			if (pDoc->m_animData.nAnimSpeed == 10)
				btnInfo.iImage = SPEEDBUTTON1;
			else if (pDoc->m_animData.nAnimSpeed == 50)
				btnInfo.iImage = SPEEDBUTTON5;
			else if (pDoc->m_animData.nAnimSpeed == 100)
				btnInfo.iImage = SPEEDBUTTON10;
			else if (pDoc->m_animData.nAnimSpeed == 250)
				btnInfo.iImage = SPEEDBUTTON25;
			else if (pDoc->m_animData.nAnimSpeed == 500)
				btnInfo.iImage = SPEEDBUTTON50;
			else if (pDoc->m_animData.nAnimSpeed == 1000)
				btnInfo.iImage = SPEEDBUTTON100;
			else
				btnInfo.iImage = SPEEDBUTTONCUSTOM;
		}
	}
	btnInfo.iImage--;
	VERIFY(m_wndAnimationBar.GetToolBarCtrl().SetButtonInfo( ID_ANIMPICKSPEED, &btnInfo ));
}

void CMainFrame::OnUpdateAnimButtons(CCmdUI* pCmdUI)
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if(pMDIActive != NULL) 
	{
		CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		if(pDoc != NULL)
		{
			if(pDoc->m_eAnimState != CTermPlanDoc::anim_none) 
			{
				if(pCmdUI->m_nID == ID_ANIMPLAY) 
				{
					if(pDoc->m_eAnimState == CTermPlanDoc::anim_playF)
						pCmdUI->SetCheck(1);
					else
						pCmdUI->SetCheck(0);
					pCmdUI->Enable(TRUE);
				}
				else if(pCmdUI->m_nID == ID_ANIMREV) 
				{
					if(pDoc->m_eAnimState == CTermPlanDoc::anim_playB)
						pCmdUI->SetCheck(1);
					else
						pCmdUI->SetCheck(0);
					pCmdUI->Enable(TRUE);
				}
				else if(pCmdUI->m_nID == ID_ANIMPAUSE)
				{
					if(pDoc->m_eAnimState == CTermPlanDoc::anim_pause)
						pCmdUI->SetCheck(1);
					else
						pCmdUI->SetCheck(0);
					pCmdUI->Enable(TRUE);
				}
				else 
				{
					pCmdUI->SetCheck(0);
					pCmdUI->Enable(TRUE);
				}
				return;
			}
			else//anim_none
			{

			}
		}
	}
	pCmdUI->SetCheck(0);
	pCmdUI->Enable(FALSE);
	return;
}

LRESULT CMainFrame::OnSetAnimationSpeed(WPARAM wParam, LPARAM lParam)
{
	OnAnimationSetSpeed((UINT) wParam);
	return TRUE;
}

void CMainFrame::OnAnimationActivitydensitydisplay() 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if(pMDIActive != NULL) {
		CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		ASSERT(pDoc!=NULL);
		pDoc->m_bActivityDensityDisplay = !(pDoc->m_bActivityDensityDisplay);
	}
}

void CMainFrame::OnAnimationMobileelementdisplay() 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if(pMDIActive != NULL) {
		CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		ASSERT(pDoc!=NULL);
		pDoc->m_bMobileElementDisplay = !(pDoc->m_bMobileElementDisplay);
	}
}

void CMainFrame::OnAnimationFlightInfoDisplay()
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if(pMDIActive != NULL) {
		CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		ASSERT(pDoc!=NULL);
		pDoc->m_bFlightInfoDisplay = !(pDoc->m_bFlightInfoDisplay);
	}
}

void CMainFrame::OnAnimationTrafficcountdisplay() 
{
	
}

void CMainFrame::OnUpdateAnimationActivitydensitydisplay(CCmdUI* pCmdUI) 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if(pMDIActive != NULL) {
		CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		ASSERT(pDoc != NULL);
		if(pDoc->m_bActivityDensityDisplay)
			pCmdUI->SetCheck(1);
		else {
			pCmdUI->SetCheck(0);
			if(pDoc->m_eAnimState != CTermPlanDoc::anim_none && pDoc->m_bMobileElementDisplay)
				pCmdUI->Enable(FALSE);
		}
	}
}

void CMainFrame::OnUpdateAnimationMobileelementdisplay(CCmdUI* pCmdUI) 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if(pMDIActive != NULL) {
		CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		ASSERT(pDoc != NULL);
		if(pDoc->m_bMobileElementDisplay)
			pCmdUI->SetCheck(1);
		else {
			pCmdUI->SetCheck(0);
			if(pDoc->m_eAnimState != CTermPlanDoc::anim_none && pDoc->m_bActivityDensityDisplay)
				pCmdUI->Enable(FALSE);
		}
	}
}

// Update flight info display menu item...
void CMainFrame::OnUpdateAnimationFlightInfoDisplay(CCmdUI *pCmdUI) 
{
	// Variables...
	CMDIChildWnd   *pMDIActive  = NULL;
    CTermPlanDoc   *pDocument   = NULL;

    // Get active MDI window...
    pMDIActive = MDIGetActive();

        // Failed, abort...
        if(!pMDIActive)
            return;

    // Get open project...
    pDocument = (CTermPlanDoc *) pMDIActive->GetActiveDocument();
        
        // "Check"
        ASSERT(pDocument != NULL);

    // Flight info display view is enabled, uncheck menu item...
    if(pDocument->m_bFlightInfoDisplay)
        pCmdUI->SetCheck(1);

    // Flight info display view is not enabled...
    else
    {
        // Uncheck menu item...
        pCmdUI->SetCheck(0);

        // Stop animation...
        if(pDocument->m_eAnimState != CTermPlanDoc::anim_none && 
           pDocument->m_bFlightInfoDisplay)
            pCmdUI->Enable(FALSE);
    }
}

void CMainFrame::OnProjectDelete() 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);

	CProjectManager* pInstance=CProjectManager::GetInstance();
	pInstance->SetProjectDeletedFlag(pDoc->m_ProjInfo.name,true);
	//Update Current Doc's ProjectInfo for rewrite
	pDoc->m_ProjInfo.isDeleted = 1;
	//Update MRU
	((CTermPlanApp*)::AfxGetApp())->UpdateMRUList(pDoc->m_ProjInfo.path);

//	pMDIActive->DestroyWindow();
//this->SendMessageToDescendants(USER_MESSAGE_CLOSE,0,1);
	
	C3DView *p3DView = NULL;
	CNodeView *pNodeView = NULL;
	p3DView = pDoc->Get3DView();
	pNodeView = pDoc->GetNodeView();
	if(p3DView && pNodeView)
	{
		p3DView->GetParentFrame()->DestroyWindow();
		pNodeView->GetParentFrame()->DestroyWindow();
	}
	else
		pMDIActive->DestroyWindow();
}

void CMainFrame::OnProjectUndelete() 
{
	// TODO: Add your command handler code here
}

void CMainFrame::OnProjectPurge() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnReportsInputdata() 
{
/*
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);

//	CInputRepDlg dlg(pDoc);
//	dlg.DoModal();
*/

	CMDIChildWnd* pMDIChild = CreateOrActivateFrame(theApp.m_pInputDataTemplate, RUNTIME_CLASS(CInputDataView));
	if( !pMDIChild )
		return;
	pMDIChild->ShowWindow(SW_HIDE);
	CInputDataView* pView=(CInputDataView*)pMDIChild->GetActiveView();
	POSITION pos=theApp.m_pDetailTemplate->GetFirstDocPosition();

	pView->SetActiveDoc(theApp.m_pDetailTemplate->GetNextDoc(pos));
	pView->InitReport();
	pMDIChild->ShowWindow(SW_SHOW);

}

void CMainFrame::OnReportsSpaceThroughput() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_SPACETHROUGHPUT_REP);
	
}

void CMainFrame::OnReportsSpaceCollisions() 
{
	// TODO: Add your command handler code here
	LoadReport( ENUM_COLLISIONS_REP );
}

void CMainFrame::OnReportsEconomic() 
{
	// TODO: Add your command handler code here
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);

	try
	{
		CEconRepDlg dlg( pDoc );
		dlg.DoModal();
	}
	catch( FileOpenError* pError )
	{
		MessageBox("No Data Available", "Error", MB_OK | MB_ICONWARNING );
		delete pError;
		return;
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
	}	
}

void CMainFrame::OnReportsBaggageDeliverytimes() 
{
	// TODO: Add your command handler code here
	LoadReport( ENUM_BAGDELIVTIME_REP );
}

void CMainFrame::OnReportsBaggageDeparturedistribution() 
{
	// TODO: Add your command handler code here
	LoadReport( ENUM_BAGDISTRIBUTION_REP );
}

void CMainFrame::OnReportsAircraftoperations() 
{
	// TODO: Add your command handler code here
	LoadReport( ENUM_ACOPERATION_REP );
}

void CMainFrame::OnWindowSidebyside() 
{
	//get the dimensions of the MainFrame client area 
	CRect rClient;
	::GetClientRect(m_hWndMDIClient,&rClient);

	//get active MDI child
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	//get the active document
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	//CDocument* pDoc = pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);
	//pDoc is active document
	//now we will go through each view of the doc and place it properly
	CView* pMSView = NULL;						//the MSV
	std::set<CMDIChildWnd*> sOtherViewFrames;	//set containing all non-MSV child frames
	
	POSITION pos = pDoc->GetFirstViewPosition();
	while(pos != NULL) {
		CView* pView = pDoc->GetNextView(pos);
		//if the view is a CNodeView, then this is the MSV
		if(pView->IsKindOf(RUNTIME_CLASS(CNodeView)) || 
			pView->IsKindOf(RUNTIME_CLASS(MSV::CAirsideMSView))||
			pView->IsKindOf(RUNTIME_CLASS(CLandsideMSView))||
			pView->IsKindOf(RUNTIME_CLASS(MSV::COnBoardMSView))||
			pView->IsKindOf(RUNTIME_CLASS(CCargoMSView))||
			pView->IsKindOf(RUNTIME_CLASS(MSV::CEnvironmentMSView))||
			pView->IsKindOf(RUNTIME_CLASS(CFinancialMSView))			
			)
			pMSView = pView;
		//otherwise add the parent frame of the view (which is an MDI child frame window) to the set
		else {
			CFrameWnd* pFrameWnd = pView->GetParentFrame();
			ASSERT(pFrameWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));

            // Add, if visible and not FIDS frame view...
            if(pFrameWnd->IsWindowVisible() && !pFrameWnd->IsKindOf(RUNTIME_CLASS(CFIDSFrameView)))
			    sOtherViewFrames.insert((CMDIChildWnd*)pFrameWnd);
		}
	}

	if(pMSView != NULL) {
		if(pMSView->GetParentFrame()->IsZoomed()) {
			//if maximized, un-maximize
			WINDOWPLACEMENT wp;
			pMSView->GetParentFrame()->GetWindowPlacement(&wp);
			wp.showCmd = SW_SHOWNORMAL;
			pMSView->GetParentFrame()->SetWindowPlacement(&wp);
		}
		//place the MSV at the left
		pMSView->GetParentFrame()->MoveWindow(
			rClient.left,
			rClient.top,
			rClient.Width()/4,
			rClient.Height());
		pMSView->GetParentFrame()->BringWindowToTop();

		//iterate through the "other views" and place them next to the MSV
		int count=sOtherViewFrames.size();
		int n=0;
		for(std::set<CMDIChildWnd*>::iterator it=sOtherViewFrames.begin();
			it!=sOtherViewFrames.end();
			it++) {
				
			if((*it)->IsZoomed()) { //if maximized, un-maximize
				WINDOWPLACEMENT wp;
				(*it)->GetWindowPlacement(&wp);
				wp.showCmd = SW_SHOWNORMAL;
				(*it)->SetWindowPlacement(&wp);
			}
			(*it)->MoveWindow(static_cast<int>(rClient.left+rClient.Width()/4),
				static_cast<int>(rClient.top+(n*rClient.Height()/count)),
				static_cast<int>(rClient.Width()*0.75),
				static_cast<int>(rClient.Height()/count));
			n++;
		}
	}

    // Flight information display is displayed...
    if(pDoc->GetFIDSView())
    {
        // Position...
        pDoc->GetFIDSView()->GetParentFrame()->MoveWindow(rClient.left,
			                                              rClient.top,
			                                              rClient.Width() / 4,
			                                              rClient.Height());

        // Bring to top...
        pDoc->GetFIDSView()->GetParentFrame()->BringWindowToTop();
    }

}

void CMainFrame::OnUpdateWindowSidebyside(CCmdUI* pCmdUI) 
{
	ASSERT(m_hWndMDIClient != NULL);
	pCmdUI->Enable(MDIGetActive() != NULL);
}

void CMainFrame::OnProjectUndo() 
{
	// TODO: Add your command handler code here
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);
	
	CUndoDialog dlg( pDoc );
	
	dlg.DoModal();
	
}

void CMainFrame::OnProjectOptions() 
{
	// TODO: Add your command handler code here
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	COptionsDlg dlg(NULL);
	dlg.DoModal();	
}



void CMainFrame::OnProjectComment() 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);
	
	CProjectCommentDlg dlg(pDoc);;
	dlg.DoModal();
	//pDoc->m_ProjInfo.

	
}

void CMainFrame::OnProjectControl() 
{

	CProjectControlDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnShowShapesToolbar()
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CDocument* pTermDoc = pMDIActive->GetActiveDocument();
	ASSERT(pTermDoc);

	if( EnvMode_OnBoard == ((CTermPlanDoc*)pTermDoc)->m_systemMode )		//will add function further
		return;

	CWnd* pWnd = &m_wndShapesBar;
	BOOL bIsShown = ((pWnd->GetStyle() & WS_VISIBLE) != 0);
	ShowControlBar((CToolBar*) pWnd, !bIsShown, FALSE);
}

void CMainFrame::OnUpdateShowShapesToolbar(CCmdUI* pCmdUI)
{
	CWnd* pWnd = &m_wndShapesBar;
	pCmdUI->SetCheck((pWnd->GetStyle() & WS_VISIBLE) != 0);
}

void CMainFrame::LoadState()
{
	CTermPlanApp* pApp = (CTermPlanApp*)AfxGetApp();
	const TCHAR szSection[]= _T("WindowPos");
	int t,l,w,h;
	l = pApp->GetProfileInt(szSection, _T("left"), 0);
	t = pApp->GetProfileInt(szSection, _T("top"), 0);
	w = pApp->GetProfileInt(szSection, _T("width"), 750);
	h = pApp->GetProfileInt(szSection, _T("height"), 550);

	MoveWindow(l,t,w,h);

	LoadBarState(_T("DockStateEx"));

	
	RenderEngine::getSingleton().SetAlwaysInUse(
		pApp->GetProfileString(_T("Render Engine"), _T("Always Use"), _T("false")) == _T("true")
		);
}
//////////////////////////////////////////////////////////////////////////
//reload LoadBarState & SaveBarState , for save data from register to INI File.
//class CArcTermDockState , inherit the class CDockState .
//
//////////////////////////////////////////////////////////////////////////
void CMainFrame::LoadBarState(LPCTSTR lpszProfileName)
{
	CArcTermDockState state;
	state.LoadState(lpszProfileName);
	SetDockState(&state);
}
void CMainFrame::SaveBarState(LPCTSTR lpszProfileName)
{
	CArcTermDockState state;
	GetDockState(&state);
	state.SaveState(lpszProfileName);
}
void CMainFrame::GetDockState(CArcTermDockState* state) 
{
	state->Clear(); //make sure dockstate is empty
	// get state info for each bar
	POSITION pos = m_listControlBars.GetHeadPosition();
	while (pos != NULL)
	{
		CControlBar* pBar = (CControlBar*)m_listControlBars.GetNext(pos);
		ASSERT(pBar != NULL);
		CArctermControlBarInfo* pInfo = new CArctermControlBarInfo;
		pBar->GetBarInfo(pInfo);
		state->m_arrBarInfo.Add(pInfo);
	}
}
void CMainFrame::SetDockState(const CArcTermDockState* state)
{
	int i;

	// first pass through barinfo's sets the m_pBar member correctly
	// creating floating frames if necessary
	for (i = 0; i < state->m_arrBarInfo.GetSize(); i++)
	{
		CArctermControlBarInfo* pInfo = (CArctermControlBarInfo*)state->m_arrBarInfo[i];
		ASSERT(pInfo != NULL);
		if (pInfo->m_bFloating)
		{
			// need to create floating frame to match
			CMiniDockFrameWnd* pDockFrame = CreateFloatingFrame(
				pInfo->m_bHorz ? CBRS_ALIGN_TOP : CBRS_ALIGN_LEFT);
			ASSERT(pDockFrame != NULL);
			CRect rect(pInfo->m_pointPos, CSize(10, 10));
			pDockFrame->CalcWindowRect(&rect);
			pDockFrame->SetWindowPos(NULL, rect.left, rect.top, 0, 0,
				SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
			CDockBar* pDockBar =
				(CDockBar*)pDockFrame->GetDlgItem(AFX_IDW_DOCKBAR_FLOAT);
			ASSERT(pDockBar != NULL);
			ASSERT_KINDOF(CDockBar, pDockBar);
			pInfo->m_pBar = pDockBar;
		}
		else // regular dock bar or toolbar
		{
			pInfo->m_pBar = GetControlBar(pInfo->m_nBarID);
			ASSERT(pInfo->m_pBar != NULL); //toolbar id's probably changed
		}
		if (pInfo->m_pBar != NULL)
			pInfo->m_pBar->m_nMRUWidth = pInfo->m_nMRUWidth;
	}

	// the second pass will actually dock all of the control bars and
	//  set everything correctly
	for (i = 0; i < state->m_arrBarInfo.GetSize(); i++)
	{
		CArctermControlBarInfo* pInfo = (CArctermControlBarInfo*)state->m_arrBarInfo[i];
		ASSERT(pInfo != NULL);
		if (pInfo->m_pBar != NULL)
			pInfo->m_pBar->SetBarInfo(pInfo, this);
	}

	// last pass shows all the floating windows that were previously shown
	for (i = 0; i < state->m_arrBarInfo.GetSize(); i++)
	{
		CArctermControlBarInfo* pInfo = (CArctermControlBarInfo*)state->m_arrBarInfo[i];
		ASSERT(pInfo != NULL);
		ASSERT(pInfo->m_pBar != NULL);
		if(pInfo->m_pBar != NULL)
		{
			if (pInfo->m_bFloating)
			{
				CFrameWnd* pFrameWnd = pInfo->m_pBar->GetParentFrame();
				CDockBar* pDockBar = (CDockBar*)pInfo->m_pBar;
				ASSERT_KINDOF(CDockBar, pDockBar);
				if (pDockBar->GetDockedVisibleCount() > 0)
				{
					pFrameWnd->RecalcLayout();
					pFrameWnd->ShowWindow(SW_SHOWNA);
				}
			}
		}
	}
	DelayRecalcLayout();
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::SaveState()
{
	CTermPlanApp* pApp = (CTermPlanApp*)AfxGetApp();

    const TCHAR szSection[]= _T("WindowPos");

	CRect r;
	this->GetWindowRect(&r);

	pApp->WriteProfileInt(szSection, _T("left"), r.left);
	pApp->WriteProfileInt(szSection, _T("top"), r.top);
	pApp->WriteProfileInt(szSection, _T("width"), r.Width());
	pApp->WriteProfileInt(szSection, _T("height"), r.Height());

	SaveBarState(_T("DockStateEx"));
}

BOOL CMainFrame::CreateMainBar()
{
	BOOL bResult=m_wndToolBar.Create(this,ID_MAIN_BAR);
	if(bResult)
	{
		m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
		//DockControlBar(&m_wndToolBar);
	}
	return bResult;

}

BOOL CMainFrame::CreateProjectBar()
{
	BOOL bResult=m_wndProjectBar.Create(this,ID_PROJECT_BAR);
	if(bResult)
	{
		m_wndProjectBar.EnableDocking(CBRS_ALIGN_TOP);
		//DockControlBar(&m_wndProjectBar);
	}
	return bResult;


}

BOOL CMainFrame::CreateAnimationBar()
{
	BOOL bResult=m_wndAnimationBar.Create(this,ID_ANIMATION_BAR);
	if(bResult)
	{
		m_wndAnimationBar.EnableDocking(CBRS_ALIGN_TOP);
		//DockControlBar(&m_wndAnimationBar);
	}
	return bResult;

}

BOOL CMainFrame::CreateUnitBar()
{
	BOOL bResult=m_wndUnitBar.Create(this,ID_UNIT_BAR);
	if(bResult)
	{
		m_wndUnitBar.EnableDocking(CBRS_ALIGN_TOP);
		//DockControlBar(&m_wndUnitBar);
	}
	/*m_wndUnitBar.MoveWindow(0,0,0,0);
	m_wndUnitBar.ShowWindow(SW_HIDE);*/
	return bResult;

}

BOOL CMainFrame::CreateUnitTempBar()
{
	BOOL bResult = m_wndUnitBarTemp.Create(this,ID_UNIT_BAR_TEMP);
	if(bResult)
	{
		m_wndUnitBarTemp.EnableDocking(CBRS_ALIGN_TOP);
		//DockControlBar(&m_wndUnitBarTemp);
	}
	return bResult;

}

BOOL CMainFrame::CreateLayoutBar()
{
	BOOL bResult=m_wndLayoutBar.Create(this,ID_LAYOUT_BAR);
	if(bResult)
	{
		m_wndLayoutBar.EnableDocking(CBRS_ALIGN_TOP);
		//DockControlBar(&m_wndLayoutBar);
	}
	return bResult;

}


BOOL CMainFrame::CreateCameraBar()
{
	BOOL bResult=m_wndCameraBar.Create(this,ID_CAMERA_BAR);
	if(bResult)
	{
		m_wndCameraBar.EnableDocking(CBRS_ALIGN_TOP);
		//DockControlBar(&m_wndCameraBar);
	}
	return bResult;

}

BOOL CMainFrame::CreatePipeBar()
{
	BOOL bResult=m_wndPipeBar.Create(this,ID_PIPE_BAR);
	if(bResult)
	{
		m_wndPipeBar.EnableDocking(CBRS_ALIGN_TOP);
		//DockControlBar(&m_wndPipeBar);
	}
	return bResult;
}

BOOL CMainFrame::CreateAirRouteBar()
{
// 	BOOL bResult = m_wndAirRouteBar.Create(this,ID_PIPE_BAR+100);
// 	if (bResult)
// 	{
// 		m_wndAirRouteBar.EnableDocking(CBRS_ALIGN_TOP);
// 		//DockControlBar(&m_wndAirRouteBar);
// 	}
// 
// 	return bResult;
	return TRUE;
}

void CMainFrame::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	CFrameWnd* pChildFrame = GetActiveFrame();
	if (pChildFrame == NULL)
		return;

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pChildFrame->GetActiveDocument();
	if (pDoc == NULL)
		return;

	if (pDoc->m_simType == SimType_MontCarlo)
	{
		if(pWnd->IsKindOf(RUNTIME_CLASS(CDockBar)))
		{
			CNewMenu menu;
			menu.LoadMenu(IDR_BAR_SHOW) ;
			CNewMenu* pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
		}
	}
}

void CMainFrame::OnBarShowMain() 
{
	// TODO: Add your command handler code here
	ShowControlBar(&m_wndToolBar,!m_wndToolBar.IsWindowVisible(),FALSE);
}

void CMainFrame::OnUpdateBarShowMain(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_wndToolBar.IsWindowVisible());
}

void CMainFrame::OnBarShowAnimation() 
{
	// TODO: Add your command handler code here
	ShowControlBar(&m_wndAnimationBar,!m_wndAnimationBar.IsWindowVisible(),FALSE);
	
}

void CMainFrame::OnUpdateBarShowAnimation(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_wndAnimationBar.IsWindowVisible());
	
}

void CMainFrame::OnBarShowProject() 
{
	// TODO: Add your command handler code here
	ShowControlBar(&m_wndProjectBar,!m_wndProjectBar.IsWindowVisible(),FALSE);

}

void CMainFrame::OnUpdateBarShowProject(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_wndProjectBar.IsWindowVisible());
	
}

void CMainFrame::OnBarShowUnit() 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	assert( pMDIActive != NULL );
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	assert( pDoc!= NULL );

	if(pDoc->m_systemMode == EnvMode_AirSide || pDoc->m_systemMode == EnvMode_LandSide)
	{
		ShowControlBar(&m_wndUnitBar,!m_wndUnitBar.IsWindowVisible(),FALSE);
		ShowControlBar(&m_wndUnitBarTemp,FALSE,FALSE);
	}
	else
	{
		ShowControlBar(&m_wndUnitBarTemp,!m_wndUnitBarTemp.IsWindowVisible(),FALSE);
		ShowControlBar(&m_wndUnitBar,FALSE,FALSE);
	}
	AdjustToolbar();
}

void CMainFrame::OnUpdateBarShowUnit(CCmdUI* pCmdUI) 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	assert( pMDIActive != NULL );
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	assert( pDoc!= NULL );

	if(pDoc->m_systemMode == EnvMode_AirSide || pDoc->m_systemMode == EnvMode_LandSide)
		pCmdUI->SetCheck(m_wndUnitBar.IsWindowVisible());
	else
		pCmdUI->SetCheck(m_wndUnitBarTemp.IsWindowVisible());	
}

void CMainFrame::OnBarShowCamera() 
{
	// TODO: Add your command handler code here
	ShowControlBar(&m_wndCameraBar,!m_wndCameraBar.IsWindowVisible(),FALSE);
	
}

void CMainFrame::OnUpdateBarShowCamera(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_wndCameraBar.IsWindowVisible());
	
}

void CMainFrame::OnBarShowShape() 
{
	// TODO: Add your command handler code here
	ShowControlBar(&m_wndShapesBar,!m_wndShapesBar.IsWindowVisible(),FALSE);	
}

void CMainFrame::OnBarShowACComponentShape() 
{
	// TODO: Add your command handler code here
	ShowControlBar(&m_wndACComponentShapesBar,!m_wndACComponentShapesBar.IsWindowVisible(),FALSE);	
}

void CMainFrame::OnBarShowACFurnishingShape() 
{
	// TODO: Add your command handler code here
	ShowControlBar(&m_wndACFurnishingShapesBar,!m_wndACFurnishingShapesBar.IsWindowVisible(),FALSE);	
}

void CMainFrame::OnUpdateBarShowShape(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_wndShapesBar.IsWindowVisible());
	
}

void CMainFrame::OnUpdateBarShowACComponentShape(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_wndACComponentShapesBar.IsWindowVisible());
	
}

void CMainFrame::OnUpdateBarShowACFurnishingShape(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_wndACFurnishingShapesBar.IsWindowVisible());
	
}

void CMainFrame::OnBarShowLayout() 
{
	// TODO: Add your command handler code here
	ShowControlBar(&m_wndLayoutBar,!m_wndLayoutBar.IsWindowVisible(),FALSE);//
	
}

void CMainFrame::OnUpdateBarShowLayout(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_wndLayoutBar.IsWindowVisible());
	
}

void CMainFrame::OnBarShowPipe() 
{
	// TODO: Add your command handler code here
	ShowControlBar(&m_wndPipeBar,!m_wndPipeBar.IsWindowVisible(),FALSE);
	
}

void CMainFrame::OnUpdateBarShowPipe(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_wndPipeBar.IsWindowVisible());
	
}

int nOpenProjectCnt = 0;
int nOpen3DViewFrameCnt = 0 ;
int nOpenComponentEditCnt = 0;


LRESULT CMainFrame::OnShowControlBar(WPARAM wParam,LPARAM lParam)
{
	ToolBarUpdateParameters& updateArg = *((ToolBarUpdateParameters*)lParam);	


	switch (updateArg.m_operatType)
	{
	case ToolBarUpdateParameters::OpenApplication:		
		break;

	case ToolBarUpdateParameters::OpenProject:
		nOpenProjectCnt++;		
		break;

	case ToolBarUpdateParameters::CloseProject:
		nOpenProjectCnt--;
		nOpenProjectCnt = max(0,nOpenProjectCnt);
		break;

	case ToolBarUpdateParameters::OpenComponentEditView:
		nOpenComponentEditCnt++;		
		break;

	case ToolBarUpdateParameters::CloseComponentEditView:
		nOpenComponentEditCnt--;		
		break;

	case ToolBarUpdateParameters::Open3DViewFrm:	
		nOpen3DViewFrameCnt++;		
		break;

	case ToolBarUpdateParameters::Close3DViewFrm:
		nOpen3DViewFrameCnt--;		
		break;

	case ToolBarUpdateParameters::ChangeEnvMode:
		//update units bar		
		break;

	default:
		break;

	}
	//ok according to the project statues ,we decide which toolbar to show
	//hide all first
	ShowControlBar(&m_wndAnimationBar, FALSE, FALSE);
	ShowControlBar(&m_wndProjectBar, FALSE, FALSE);
	ShowControlBar(&m_wndUnitBar, FALSE, FALSE);
	ShowControlBar(&m_wndUnitBarTemp, FALSE, FALSE);
	ShowControlBar(&m_wndLayoutBar, FALSE, FALSE);
	ShowControlBar(&m_wndPipeBar, FALSE, FALSE);
//	ShowControlBar(&m_wndAirRouteBar, FALSE, FALSE);
	//ShowControlBar(&m_wndOnBoardLayoutBar,FALSE,FALSE);

	//
	if(nOpenProjectCnt > 0 )
	{
		if(updateArg.m_nCurMode == EnvMode_AirSide || updateArg.m_nCurMode == EnvMode_LandSide) {
			ShowControlBar(&m_wndUnitBar, TRUE, FALSE);
			m_wndUnitBar.UpdateUnitBar(updateArg.m_pDoc); 
		}
		else
			ShowControlBar(&m_wndUnitBarTemp, TRUE, FALSE);
		ShowControlBar(&m_wndProjectBar, TRUE, FALSE);
		ShowControlBar(&m_wndLayoutBar,TRUE,FALSE);

		if(updateArg.m_nCurMode == EnvMode_OnBoard )
		{
			m_wndLayoutBar.GetActiveFloorCB()->EnableWindow(FALSE);
		}
	}

	if(nOpen3DViewFrameCnt  >0)
	{
		ShowControlBar(&m_wndAnimationBar, TRUE, FALSE);
		if(updateArg.m_nCurMode == EnvMode_AirSide){
	//		ShowControlBar(&m_wndAirRouteBar, TRUE, FALSE);
			ShowControlBar(&m_wndPipeBar,TRUE,FALSE);
		}
		if(updateArg.m_nCurMode == EnvMode_Terminal)
			ShowControlBar(&m_wndPipeBar,TRUE,FALSE);
		if(updateArg.m_nCurMode == EnvMode_LandSide)
			ShowControlBar(&m_wndPipeBar,TRUE,FALSE);
	}

	//if(updateArg.m_nCurMode == EnvMode_OnBoard && nOpenComponentEditCnt > 0)
	//{
	//	if(updateArg.m_pDoc)
	//		m_wndOnBoardLayoutBar.SetInputOnboard(updateArg.m_pDoc->GetInputOnboard());
	//	ShowControlBar(&m_wndOnBoardLayoutBar,TRUE,FALSE);			
	//}

	AdjustToolbar();
	return TRUE;
}

LRESULT CMainFrame::OnChangeAnimToolBtnState(WPARAM wParam,LPARAM lParam)
{
	UINT nID;
	UINT nStyle;
	int iImage;
	int nButtonCount = m_wndAnimationBar.GetToolBarCtrl().GetButtonCount();
	if (nButtonCount > (int)wParam)
	{
		m_wndAnimationBar.GetButtonInfo(wParam,nID,nStyle,iImage);
		m_wndAnimationBar.SetButtonInfo(wParam,lParam,nStyle,iImage);
	}

	return TRUE;
	/*
	if(lParam==ID_ANIMATION_START)
	{
		TBBUTTONINFO tbbi;
		tbbi.cbSize=sizeof(TBBUTTONINFO);
		tbbi.dwMask =TBIF_STATE;
		tbbi.fsState &=~TBSTATE_CHECKED;
		m_wndAnimationBar.GetToolBarCtrl().SetButtonInfo(lParam,&tbbi)  ;
	}
*/
}

void CMainFrame::OnUpdateFrameMenu (HMENU hMenuAlt)
{
    CNewMDIFrameWnd::OnUpdateFrameMenu (hMenuAlt);
}


void CMainFrame::OnButPipebar() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnUpdateButPipebar(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

BOOL CMainFrame::CreateTempBar()
{
	if (!m_wndTempBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD| CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndTempBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;      // fail to create
	}
	return TRUE;
}

void CMainFrame::OnChangeShape(UINT nID)
{
	int nIndex=nID-IDM_SHAPE_LIST_DEFAULT;
	
	CShape::CShapeList* pSL = SHAPESMANAGER->GetShapeList();
	ASSERT(pSL != NULL);

	CShape* pSelShape= pSL->at(nIndex);
	CMDIChildWnd* pMCW=(CMDIChildWnd*)GetActiveFrame();
	C3DView* pView=(C3DView*)(pMCW->GetActiveView());
	CTermPlanDoc* pDoc=pView->GetDocument();
	
	//std::vector<CProcessor2*>::iterator	iter;
	//for(iter =  pDoc->m_vSelectedProcessors.begin(); iter !=  pDoc->m_vSelectedProcessors.end(); iter++)
	//{
	//	(*iter)->SetShape(pSelShape);
	//}
	int nCount =  pDoc->GetSelectProcessorsCount();
	for ( int nObj = 0; nObj < nCount; ++ nObj)
	{
		CObjectDisplay *pObjDisplay  = pDoc->GetSelectedObjectDisplay(nObj);
		if (pObjDisplay&& pObjDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2* pProc2 = (CProcessor2 *)pObjDisplay;
			if(pProc2)
				pProc2->SetShape(pSelShape);
		
		}
	}

	pDoc->UpdateAllViews(NULL);
}


CComboBox* CMainFrame::GetAnimPaxCB()
{
	return m_wndAnimationBar.GetAnimPaxCB();
}

LRESULT CMainFrame::OnSetToolBtnRunDelta_Run(WPARAM wParam,LPARAM lParam)
{
	UINT nID;
	UINT nStyle;
	int iImage;
	m_wndAnimationBar.GetButtonInfo(7, nID, nStyle, iImage);
	m_wndAnimationBar.SetButtonInfo(7, nID, nStyle, 13);
	return TRUE;
}

LRESULT CMainFrame::OnSetToolBtnRunDelta_Delta(WPARAM wParam,LPARAM lParam)
{
	UINT nID;
	UINT nStyle;
	int iImage;
	m_wndAnimationBar.GetButtonInfo(7, nID, nStyle, iImage);
	m_wndAnimationBar.SetButtonInfo(7, nID, nStyle, 14);
	return TRUE;
}

void CMainFrame::OnHelp() 
{
	
    /* http://msdn.microsoft.com/library/default.asp?url=/library/en-us/htmlhelp/html/vsconovhtmlhelpapioverview.asp */

	::HtmlHelp(NULL,"ARCTerm.chm::/default.htm",HH_DISPLAY_TOPIC,0);
	
}

void CMainFrame::OnHelpIndex() 
{
	// TODO: Add your command handler code here
	::HtmlHelp(NULL,"ARCTerm.chm",HH_DISPLAY_INDEX,0);
	
}

void CMainFrame::OnHelpTechSupport() 
{
	// TODO: Add your command handler code here
	::HtmlHelp(NULL,"ARCTerm.chm::/Technical Support.htm",HH_DISPLAY_TOPIC,0);
	
}

void CMainFrame::OnCompareReport(UINT nID) 
{
	//CCompareReportDlg dlg( this );
	//dlg.DoModal();

	ENUM_REPORT_TYPE reportType;
	switch(nID)
	{
	case ID_MULTIPLERUNS_DISTANCETRAVELLED:
		reportType = ENUM_DISTANCE_REP;
		break;

	case ID_MULTIPLERUNS_TIMEINQUEUES:
		reportType = ENUM_QUEUETIME_REP;
		break;

	case ID_MULTIPLERUNS_TIMEINTERMINAL:
		reportType = ENUM_DURATION_REP;
		break;

	case ID_MULTIPLERUNS_TIMEINSERVICE:
		reportType = ENUM_SERVICETIME_REP;
		break;

	case ID_MULTIPLERUNS_ACTIVITYBREAKDOWN:
		reportType = ENUM_ACTIVEBKDOWN_REP;
		break;

	case ID_MULTIPLERUNS_OCCUPANCY:
		reportType = ENUM_PAXCOUNT_REP;
		break;
	case ID_MULTIPLERUNS_CRITICALQUEUE:
		reportType = ENUM_AVGQUEUELENGTH_REP;
		break;

	case ID_MULTIPLERUNS_DENSITY:
		reportType = ENUM_PAXDENS_REP;
		break;
	default:
		return;
	}

	CMDIChildWnd* pMDIActive = MDIGetActive();
	if (pMDIActive == NULL)
		return;

	CTermPlanDoc* pActiveDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	if (pActiveDoc == NULL)
		return;

	ASSERT(pActiveDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));

	int iCurrentIdx = pActiveDoc->GetTerminal().getCurrentSimResult();
	if (iCurrentIdx <0)
	{
		AfxMessageBox("No simulation result, so can not do report!\r\n you must first run engine. ", MB_OK|MB_ICONINFORMATION);
		return;
	}

	// create the multi runs report frame window
	pActiveDoc->GetARCReportManager().SetMultiRunsReportType(reportType);
	CMDIChildWnd* pChildFrm = CreateOrActivateFrame(theApp.m_pMultiRunReportTemplate, RUNTIME_CLASS(CMultiRunRepControlView));
	ASSERT(pChildFrm != NULL);
	
	CReportParameter* pReportParam = pActiveDoc->GetTerminal().m_pReportParaDB->GetParameterbyType(reportType);
	if (pReportParam == NULL)
		return;

	pChildFrm->ShowWindow(SW_SHOWMAXIMIZED);
	int iDetailed;
	pReportParam->GetReportType(iDetailed);

	pActiveDoc->GetARCReportManager().getMultiReport().GenerateReport(reportType,iDetailed);
	pActiveDoc->UpdateAllViews(NULL,MULTIREPORT_SHOWREPORT,NULL);


	//////////////////////////////////////////////////////////////////////////
	//CView* pTempView;
	//POSITION pos = pActiveDoc->GetFirstViewPosition();
	//while (pos != NULL)
	//{
	//	pTempView = pActiveDoc->GetNextView(pos);
	//	if (pTempView->IsKindOf(RUNTIME_CLASS(CMultiRunRepControlView)))
	//	{
	//		CMultiRunReportSplitChildFrame* pWnd = (CMultiRunReportSplitChildFrame*)((CMultiRunRepControlView*)pTempView)->GetParentFrame();
	//		CRect rc(0, 0, 0, 0);
	//		pWnd->GetWindowRect(&rc);
	//		pWnd->m_wndSplitter2.SetRowInfo(0, 350, 236);
	//		pWnd->ShowWindow(SW_SHOWMAXIMIZED);
	//	}
	//}

	//CView* pView = pChildFrm->GetActiveView();
	//CTermPlanDoc* pDoc = (CTermPlanDoc*)(pView->GetDocument());
	//ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	////pDoc->m_repManager.SetReportType(_enumRepType);

	//bool bExist = TRUE;
	//int iCurrentSimIdx = pDoc->GetTerminal().getCurrentSimResult();
	//ASSERT( iCurrentSimIdx>=0 );

	//pDoc->GetTerminal().GetSimReportManager()->SetCurrentSimResult( iCurrentSimIdx );
	//pDoc->GetTerminal().GetSimReportManager()->SetCurrentReportType( _enumRepType );

	//bExist = pDoc->GetTerminal().GetSimReportManager()->getSimItem( pDoc->GetTerminal().getCurrentSimResult() ) ->isReportRealExist( _enumRepType );

	//CReportParameter* pPara = pDoc->GetTerminal().m_pReportParaDB->GetReportPara( _enumRepType );
	//ASSERT( pPara );
	//pDoc->SetReportPara( pPara );

	//pView->GetParentFrame()->OnUpdateFrameTitle(TRUE);
	//pView->GetParentFrame()->ShowWindow(SW_SHOW);
	//pView->Invalidate(FALSE);


	//CView * pTempView;
	//POSITION pos = pDoc->GetFirstViewPosition();
	//while (pos != NULL)
	//{
	//	pTempView = pDoc->GetNextView(pos);
	//	if (pTempView->IsKindOf(RUNTIME_CLASS(CRepGraphView)))
	//	{
	//		((CRepGraphView *)pTempView)->ResetAllContent();
	//	}
	//	else if(pTempView->IsKindOf(RUNTIME_CLASS(CRepListView)))
	//	{
	//		((CRepListView *)pTempView)->ResetAllContent();
	//	}
	//	else if(pTempView->IsKindOf(RUNTIME_CLASS(CRepControlView)))
	//	{
	//		((CRepControlView *)pTempView)->bShowLoadButton = bExist;
	//		((CRepControlView *)pTempView)->ResetAllContent();
	//		((CRepControlView *)pTempView)->Clear();

	//		CReportChildFrameSplit* pWnd = (CReportChildFrameSplit*)((CRepControlView*)pTempView)->GetParentFrame();
	//		CRect rc(0, 0, 0, 0);
	//		pWnd->GetWindowRect(&rc);
	//		pWnd->m_wndSplitter2.SetRowInfo(0, 350, 236);

	//		pWnd->ShowWindow(SW_SHOWMAXIMIZED);
	//	}
	//}

}

void CMainFrame::OnFloorAdjust()
{	
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CDocument* pTermDoc = pMDIActive->GetActiveDocument();
	ASSERT(pTermDoc);

	if(pTermDoc &&  EnvMode_OnBoard == ((CTermPlanDoc*)pTermDoc)->m_systemMode )		//will add function further
	{
		CView* pView;
		POSITION pos = pTermDoc->GetFirstViewPosition();
		while (pos != NULL)
		{
			pView = pTermDoc->GetNextView(pos);
			if(pView->IsKindOf(RUNTIME_CLASS(CAircraftLayout3DView)))
			{
				CAircraftLayout3DView* pLayOutView = (CAircraftLayout3DView*)pView;
				if(pLayOutView)
					pLayOutView->ShowDeckAdjustDialog();
				return;
			}
		}
		
		return;
	}



	if(m_pDlgFloorAdjust && m_pDlgFloorAdjust->IsWindowVisible()) 
	{
		//hide (close) window
		m_pDlgFloorAdjust->DestroyWindow();
		delete m_pDlgFloorAdjust;
		m_pDlgFloorAdjust = NULL;
	}
	else 
	{
		CMDIChildWnd* pMCW=(CMDIChildWnd*)GetActiveFrame();
		CView* pView=pMCW->GetActiveView();
		ASSERT(pView);
		ASSERT(pView->GetDocument()->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
		CTermPlanDoc* pDoc=(CTermPlanDoc*)pView->GetDocument();
		C3DView* p3DView = pDoc->Get3DView();

		delete m_pDlgFloorAdjust;
		m_pDlgFloorAdjust = new CDlgFloorAdjust(pDoc, this);
		m_pDlgFloorAdjust->Create(CDlgFloorAdjust::IDD, this);
		CRect rc, rcView;
		m_pDlgFloorAdjust->GetWindowRect(&rc);
		if(p3DView)
			p3DView->GetWindowRect(&rcView);
		else
			pView->GetWindowRect(&rcView);
		rc.OffsetRect(rcView.TopLeft()-rc.TopLeft());
		m_pDlgFloorAdjust->MoveWindow(&rc);
		m_pDlgFloorAdjust->ShowWindow(SW_SHOW);
	}
}

// void CMainFrame::OnModelEdit()
// {
// 	if(m_pDlgAircraftModel && m_pDlgAircraftModel->IsWindowVisible()) {
// 		//hide (close) window
// 		m_pDlgAircraftModel->DestroyWindow();
// 		delete m_pDlgAircraftModel;
// 		m_pDlgAircraftModel = NULL;
// 	}
// 	else {
// 		CMDIChildWnd* pMCW=(CMDIChildWnd*)GetActiveFrame();
// 		CView* pView=pMCW->GetActiveView();
// 		ASSERT(pView);
// 		ASSERT(pView->GetDocument()->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
// 		CTermPlanDoc* pDoc=(CTermPlanDoc*)pView->GetDocument();
// 		CAircraftModelEditView* pModelView = pDoc->GetModelEditView();
// 
// 
// 		delete m_pDlgAircraftModel;
// 		m_pDlgAircraftModel = new CDlgAircraftModelEdit(this);
// 		m_pDlgAircraftModel->SetInputOnboard(pDoc->GetInputOnboard());
// 		m_pDlgAircraftModel->Create(CDlgAircraftModelEdit::IDD, this);
// 		CRect rc, rcView;
// 		m_pDlgAircraftModel->GetWindowRect(&rc);
// 		if(pModelView)
// 			pModelView->GetWindowRect(&rcView);
// 		else
// 			pView->GetWindowRect(&rcView);
// 		rc.OffsetRect(rcView.TopLeft()-rc.TopLeft());
// 		m_pDlgAircraftModel->MoveWindow(&rc);
// 		m_pDlgAircraftModel->ShowWindow(SW_SHOW);
// 	}
// }

// void CMainFrame::OnUpdateModelEdit(CCmdUI* pCmdUI)
// {
// 	pCmdUI->Enable(NULL!=GetActiveFrame());
// 	pCmdUI->SetCheck(m_pDlgAircraftModel && m_pDlgAircraftModel->GetSafeHwnd() && m_pDlgAircraftModel->IsWindowVisible());
// }

void CMainFrame::OnUpdateFloorAdjust(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(NULL!=GetActiveFrame());
	pCmdUI->SetCheck(m_pDlgFloorAdjust && m_pDlgFloorAdjust->GetSafeHwnd() && m_pDlgFloorAdjust->IsWindowVisible());
}

void CMainFrame::OnProjectExportAsInputzipWithoutDB() 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);

	pDoc->ShrinkDatabase(((CTermPlanApp *)AfxGetApp())->GetMasterDatabase(),pDoc->m_ProjInfo.name);
	ExportAsInputZip(pDoc->m_ProjInfo.name,pDoc->m_ProjInfo.path,true);
}

void CMainFrame::OnProjectExportAsInputzip() 
{	
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);
	pDoc->ShrinkDatabase(((CTermPlanApp *)AfxGetApp())->GetMasterDatabase(),pDoc->m_ProjInfo.name);

	ExportAsInputZip(pDoc->m_ProjInfo.name,pDoc->m_ProjInfo.path,false);
}

void CMainFrame::OnProjectExportAndEmailWithoutDB() 
{	
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);
	pDoc->ShrinkDatabase(((CTermPlanApp *)AfxGetApp())->GetMasterDatabase(),pDoc->m_ProjInfo.name);

	ExportAndEmail(pDoc->m_ProjInfo.name,pDoc->m_ProjInfo.path,true);
}

void CMainFrame::OnProjectExportAndEmail() 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);
	
	pDoc->ShrinkDatabase(((CTermPlanApp *)AfxGetApp())->GetMasterDatabase(),pDoc->m_ProjInfo.name);

	ExportAndEmail(pDoc->m_ProjInfo.name,pDoc->m_ProjInfo.path,false);
}


void CMainFrame::OnProjectImportIntoOldProject() 
{
	// check the license
	if(FALSE == CTermPlanApp::CheckLicense(TRUE))
	{
		return;
	}

	CImportIntoOldProjectDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnProjectImportIntoNewProject() 
{
	// check the license
	if(FALSE == CTermPlanApp::CheckLicense(TRUE))
	{
		return;
	}

	CImportIntoNewProject dlg;
	dlg.DoModal();	
}

void CMainFrame::DestroyFloorAdjustDlg()
{
	if(m_pDlgFloorAdjust && m_pDlgFloorAdjust->IsWindowVisible()) {
		//hide (close) window
		m_pDlgFloorAdjust->DestroyWindow();
		delete m_pDlgFloorAdjust;
		m_pDlgFloorAdjust = NULL;
		return;
	}
	delete m_pDlgFloorAdjust;
	m_pDlgFloorAdjust = NULL;
}


void CMainFrame::OnDatabaseACTypes() 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	assert( pMDIActive != NULL );
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	assert( pDoc!= NULL );

	CDlgDBAircraftTypes dlg( TRUE,&pDoc->GetTerminal() );
	dlg.setAcManager( pDoc->GetTerminal().m_pAirportDB->getAcMan() );
	dlg.DoModal();	
}

void CMainFrame::OnDatabaseACTypeModels()
{
	if (GetNodViewFrame(RUNTIME_CLASS(C3DObjModelEdit3DView)))
	{
		MessageBox(_T("Please close the aircraft model editing view first!"));
		return;
	}

	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT(pDoc);

//	if(pDoc->m_systemMode == EnvMode_OnBoard) 
	{
		CDlgACTypeModelDB dlg;
		dlg.InitOpAirpotDB(pDoc->GetTerminal().m_pAirportDB);
		if (IDOK == dlg.DoModal())
		{
			CACType* pEditACType = dlg.GetEditACType();
			CAircraftModel* pEditModel = dlg.GetEditModel();
			if (pEditACType && pEditModel)
			{
				CWaitCursor wc;
				pEditModel->SetACType(pEditACType);
				pEditModel->ReadData();
				pDoc->GetInputOnboard()->SetEditModel(pEditModel);
				CACModelEditFrame* pChildFrm = (CACModelEditFrame*)CreateOrActivateFrame(theApp.m_pAircraftModelDatabaseTermplate, RUNTIME_CLASS(CACModelEdit3DView));
				pChildFrm->ShowWindow(SW_SHOWMAXIMIZED);				
				ShowControlBar(&m_wndACComponentShapesBar, TRUE, FALSE);
			}
		}
	}
}

void CMainFrame::OnDatabaseAircraftComponents()
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	assert(pMDIActive != NULL);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	assert(pDoc != NULL);

	if(pDoc->m_systemMode == EnvMode_OnBoard) 
	{
		CAircraftComponentModelDatabase* pDB = pDoc->GetTerminal().m_pAirportDB->getACCompModelDB();
		CDlgComponentDBDefine dlg(pDB);
		if (IDOK == dlg.DoModal())
		{
			CComponentMeshModel* pModel = dlg.GetModelToEditShape();
			if (pModel)
			{
				CWaitCursor wc;
				pDoc->GetInputOnboard()->GetComponentEditContext()->SetCurrentComponent(pModel);
				CMDIChildWnd* pChildFrm = CreateOrActivateFrame(theApp.m_pOnBoardLayoutTemplate, RUNTIME_CLASS(CSimpleMeshEditView));
				ASSERT(pChildFrm != NULL);
				pChildFrm->ShowWindow(SW_SHOWMAXIMIZED);
				pDoc->UpdateAllViews(NULL,VM_ONBOARD_COMPONENT_CHANGE_MODEL);
			}
		}
		ACCOMPONENTSHAPESMANAGER.Reload(pDB);
	}
}
void CMainFrame::OnDatabasesAirlines() 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	assert( pMDIActive != NULL );
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	assert( pDoc!= NULL );

	CDlgDBAirline dlg(TRUE, &pDoc->GetTerminal() );
	dlg.setAirlinesMan( pDoc->GetTerminal().m_pAirportDB->getAirlineMan() );
	dlg.DoModal();
}

void CMainFrame::OnDatabaseAirports() 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	assert( pMDIActive != NULL );
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	assert( pDoc!= NULL );

	CDlgDBAirports dlg(TRUE,&pDoc->GetTerminal());
	dlg.setAirportMan( pDoc->GetTerminal().m_pAirportDB->getAirportMan() );
	dlg.DoModal();
}


void CMainFrame::OnDatabaseProbDists() 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	assert( pMDIActive != NULL );
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	assert( pDoc!= NULL );

	CDlgProbDist dlg(pDoc->GetTerminal().m_pAirportDB, false);
//	dlg.setProbDistMan( pDoc->GetTerminal().m_pAirportDB->getProbDistMan() );
	dlg.DoModal();
}

void CMainFrame::OnUpdateDatabaseMenu(CCmdUI* pCmdUI) 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if( pMDIActive == NULL )
	{
		pCmdUI->Enable( FALSE );
		return;
	}
	
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	if( pDoc == NULL )
	{
		pCmdUI->Enable( FALSE );
		return;
	}
	
	pCmdUI->Enable( TRUE );
}
void CMainFrame::OnDatabaseDblistman() 
{
	CGlobalDBSheet dlg("Airport DB", this );
	dlg.DoModal();
}

void CMainFrame::OnUpdateDatabaseDblistman(CCmdUI* pCmdUI) 
{
	
}

bool CMainFrame::zipProjectDB(const CString& strProjName, CString& _strTempDBZipFile )
{
	
	//const CString strProjName;
	//const CString strProjPath;
	PROJECTINFO projInfo;
	PROJMANAGER->GetProjectInfo(strProjName,&projInfo);
	CAirportDatabase* pAirportDB = AIRPORTDBLIST->getAirportDBByName(projInfo.dbname);

	if (pAirportDB == NULL)
	{

		CString _strErr;
		_strErr.Format("%s project's global DB index: %d does not exist in the DB list.", projInfo.name, projInfo.lUniqueIndex );
		AfxMessageBox( _strErr, MB_OK|MB_ICONINFORMATION );
		return false;
	}


	//assert( _pTermDoc );
	//CAirportDatabase* pAirportDB = _pTermDoc->GetTerminal().m_pAirportDB;
	//assert( pAirportDB );

	std::vector< CString > _vAllDBFile;
	AIRPORTDBLIST->getAllDBFileName( pAirportDB, _vAllDBFile );
	CString strDefaultDBPath = PROJMANAGER->GetDefaultDBPath();
	for (int j = 0; j < (int)_vAllDBFile.size(); j++)
	{
		if (PathFileExists(_vAllDBFile[j]) == FALSE)
		{
			CopyFile(strDefaultDBPath+_T("\\")+__database_file[j],_vAllDBFile[j],TRUE);
		}
	}

	int _iDBFileCount = _vAllDBFile.size();		
	char **_pDBFiles = (char **) new  int [_iDBFileCount];
	for (int i=0; i<_iDBFileCount; i++)
	{
		_pDBFiles[i] = new char[MAX_PATH+1];
		strcpy( _pDBFiles[i], _vAllDBFile[i] );
	}
	
	CZipInfo InfoZip;
// 	if (!InfoZip.InitializeZip())
// 	{
// 		AfxMessageBox("Failure to export database!", MB_OK);
// 		clear2VArray( _pDBFiles, _iDBFileCount );
// 		return false;
// 	}
	
	//InfoZip.
	CString strTempPath = getTempPath();
	strTempPath.TrimRight("\\");
	_strTempDBZipFile = strTempPath + "\\" + pAirportDB->getName() +"_DB.zip";
	DeleteFile( _strTempDBZipFile );

	if (!InfoZip.AddFiles(_strTempDBZipFile, _pDBFiles, _iDBFileCount))
	{
		AfxMessageBox("Failure to export database!", MB_OK);
		clear2VArray( _pDBFiles, _iDBFileCount );
		return false;
	}
	
// 	if (!InfoZip.Finalize())
// 	{
// 		AfxMessageBox("Failure to export database!", MB_OK);
// 		clear2VArray( _pDBFiles, _iDBFileCount );
// 		return false;
// 	}

	clear2VArray( _pDBFiles, _iDBFileCount );
	return true;
}

bool CMainFrame::zipTempZipToTermzip( const CString& strProjName, const CString& strProjPath,
									 const CString& _strTempInputZip, const CString& _strTempDBZip, 
									 const CString& _strTempMathematicZip, const CString& _strTempMathResultZip,
									 const CString& _strTerminalZip,bool bNotForceDB,CTermPlanDoc* _pDoc )
{
	CString _strTempIniFile;
	createArcExportIniFile( _strTempInputZip, _strTempDBZip, 
		_strTempMathematicZip, _strTempMathResultZip, 
		_strTempIniFile,bNotForceDB);
 //   CTermPlanDoc* pDoc;
	//if(_pDoc==NULL)
	//{
	//	CMDIChildWnd* pMDIActive = MDIGetActive();
	//    ASSERT(pMDIActive != NULL);
	//    pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	//    ASSERT(pDoc != NULL);
	//}
	//else 
	//	pDoc = _pDoc;

	//CString sProjectPath = pDoc->m_ProjInfo.path;
	CString sProjectPath = strProjPath;

/*
	CString sExportHisFileName=sProjectPath+"\\ExportHis.ini";

	CFile file;
	// Check if Exist
	CFileFind finder;
	if(!finder.FindFile(sExportHisFileName))
	{
		// Create ExportHis.ini If Not Exist----------------------------------------------------------------------
		CFile file(sExportHisFileName,CFile::modeCreate);
		file.Close();
		//--------------------------------------------------------------------------------------------------------
		
	}
	// Append Export History of Local Machine If Not Exist--------------------------------------------------------
	CIniReader iniEH;
	iniEH.setINIFileName(sExportHisFileName);
	CString strDBIndex;
	strDBIndex.Format("%d",pDoc->m_ProjInfo.lUniqueIndex);
	iniEH.setKey(strDBIndex,pDoc->m_ProjInfo.machine,"HistoryList");
	//------------------------------------------------------------------------------------------------------------
*/
	//project db info file
	//this line should be changed into pDoc->getTerminal()->m_pAirportDB, after InputTerminal be corrected.

	PROJECTINFO projInfo;
	PROJMANAGER->GetProjectInfo(strProjName,&projInfo);
	//CAirportDatabase* pAirportDB = AIRPORTDBLIST->getAirportDBByID(projInfo.lUniqueIndex);

	CAirportDatabase* pDB=AIRPORTDBLIST->getAirportDBByName(projInfo.dbname);
	ASSERT(pDB!=NULL);
	CString sFullProjInfoFile("");
	if(ExportProjDBInfo(pDB,sProjectPath,sFullProjInfoFile)==false)
		return false;
	
	int _iDBFileCount = 4;
	if (!_strTempMathematicZip.IsEmpty())
		_iDBFileCount++;
	if (!_strTempMathResultZip.IsEmpty())
		_iDBFileCount++;
		
	char **_pDBFiles ;


	_pDBFiles = (char **) new  int [_iDBFileCount];
	for (int i=0; i<_iDBFileCount; i++)
	{
		_pDBFiles[i] = new char[MAX_PATH+1];
	}
	strcpy( _pDBFiles[0], _strTempInputZip );
	strcpy( _pDBFiles[1], _strTempDBZip );
	strcpy( _pDBFiles[2], _strTempIniFile);
//	strcpy( _pDBFiles[3], sExportHisFileName );
	strcpy( _pDBFiles[3], sFullProjInfoFile);//project info file
	
	int iIndex = 4;
	if (!_strTempMathematicZip.IsEmpty())
		strcpy( _pDBFiles[iIndex++], _strTempMathematicZip );
	if (!_strTempMathResultZip.IsEmpty())
		strcpy( _pDBFiles[iIndex], _strTempMathResultZip );

	CZipInfo InfoZip;
// 	if (!InfoZip.InitializeZip())
// 	{
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		clear2VArray( _pDBFiles, _iDBFileCount );
// 		return false;
// 	}
	
	//InfoZip.
	if (!InfoZip.AddFiles(_strTerminalZip, _pDBFiles, _iDBFileCount))
	{
		AfxMessageBox("Failure to export project!", MB_OK);
		clear2VArray( _pDBFiles, _iDBFileCount );
		return false;
	}
	
// 	if (!InfoZip.Finalize())
// 	{
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		clear2VArray( _pDBFiles, _iDBFileCount );
// 		return false;
// 	}
	
	clear2VArray( _pDBFiles, _iDBFileCount );
	// delete temp file
	DeleteFile( _strTempIniFile );
	return true;
}

void CMainFrame::clear2VArray( char** _p2VArray, int _xCount )
{
	for( int i=0; i<_xCount; i++ )
	{
		delete []_p2VArray[i];
	}
	
	delete []_p2VArray;
}


CString CMainFrame::getTempPath( const CString& _strDefault )
{
	char _szTempPath[ MAX_PATH + 1];
	if( GetTempPath( MAX_PATH, _szTempPath ) == 0 )
		return  _strDefault;
	return CString( _szTempPath );
}

void CMainFrame::createArcExportIniFile( const CString& _strTempInputZip, const CString& _strTempDBZip, 
										const CString& _strTempMathematicZip, const CString& _strTempMathResultZip,
										CString& _strTmpIniFile ,bool bNotForceDB)
{
	int _iPos;
	
	CString strTempPath = getTempPath();
	strTempPath.TrimRight("\\");
	_strTmpIniFile = strTempPath + "\\" +"arcterm_export.ini";
	DeleteFile( _strTmpIniFile );

	ArctermFile inifile;
	inifile.openFile( _strTmpIniFile,WRITE );
	inifile.writeField("arcterm export ini file");
	inifile.writeLine();
	inifile.writeField("version\t inputzip\t databasezip");
	if (!_strTempMathematicZip.IsEmpty())
		inifile.writeField("\t mathematiczip");
	if (!_strTempMathResultZip.IsEmpty())
		inifile.writeField("\t mathresultzip");
	inifile.writeLine();

	inifile.writeInt(2);	// version
	inifile.writeLine();
	
	inifile.writeField("INPUTZIP");
	_iPos = _strTempInputZip.ReverseFind('\\');
	inifile.writeField( _strTempInputZip.Mid( _iPos +1) );
	inifile.writeLine();
	
	inifile.writeField("DATABASEZIP");
	_iPos = _strTempDBZip.ReverseFind('\\');
	inifile.writeField( _strTempDBZip.Mid( _iPos +1 ) );
	inifile.writeLine();

	if (!_strTempMathematicZip.IsEmpty())
	{
		inifile.writeField("MATHEMATICZIP");
		_iPos = _strTempMathematicZip.ReverseFind('\\');
		inifile.writeField( _strTempMathematicZip.Mid( _iPos +1 ) );
		inifile.writeLine();
	}

	if (!_strTempMathResultZip.IsEmpty())
	{
		inifile.writeField("MATHRESULTZIP");
		_iPos = _strTempMathResultZip.ReverseFind('\\');
		inifile.writeField( _strTempMathResultZip.Mid( _iPos +1 ) );
		inifile.writeLine();
	}

	inifile.writeField("Not Force DB");
	inifile.writeField(bNotForceDB?"1":"0");
	inifile.writeLine();

	inifile.endFile();
}


void CMainFrame::OnReportsConveyorWaitLength() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_CONVEYOR_WAIT_LENGTH_REP);
}

void CMainFrame::OnReportsPassengerConveyorWaitTime() 
{
	LoadReport(ENUM_CONVEYOR_WAIT_TIME_REP);	
}

void CMainFrame::ExportAndEmail(const CString& strProjName,const CString& strProjPath, bool bNotForceDB,bool bOpenProject)
{
	{	
		CTermPlanDoc* pDoc = NULL;
		
		CMDIChildWnd* pMDIActive = MDIGetActive();
		if( pMDIActive != NULL )
		{	

			CView* pView = pMDIActive->GetActiveView();

			if (pView->IsKindOf(RUNTIME_CLASS(CFlowChartPane)))
			{
				pDoc = (CTermPlanDoc*)((CFlowChartPane*)pView)->GetDocumentPtr();
			}
			else
			{
				pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
			}

		}

		if(pDoc){
			CChildFrame* pChildFrm  = pDoc->Get3DViewParent();
			if(pChildFrm)
				pChildFrm->StorePanInfo();
		}

		CProjectExport projExport(this,pDoc);
		projExport.ExportAndEmail(strProjName,strProjPath,bNotForceDB,bOpenProject);
		
		return;
	}
}
void CMainFrame::ZipAirsideExportFiles(const CString& strProjName,const CString& projPath, CString& strZipFilePath)
{
		//ziping 
	CString strDirPath = _T("");
	strDirPath.Format(_T("%s\\INPUT\\Airside"),projPath);
	if (!PathFileExists((LPCSTR)strDirPath))
	{
		if(!CreateDirectory(strDirPath,NULL))
			return;
	}


	CImportExportManager::ExportProject(projPath,strProjName);

	//ziping 
	if (PathFileExists((LPCSTR)strDirPath))
	{
		zipFloder(strProjName,strDirPath,strZipFilePath);
	}
}

void CMainFrame::ExportAsInputZip(const CString& strProjName, const CString& strProjPath,bool bNotForceDB,bool bOpenProject)
{
	{	
		CTermPlanDoc* pDoc = NULL;

		CMDIChildWnd* pMDIActive = MDIGetActive();
		if( pMDIActive != NULL )
		{
			CView* pView = pMDIActive->GetActiveView();

			if (pView->IsKindOf(RUNTIME_CLASS(CFlowChartPane)))
			{
				pDoc = (CTermPlanDoc*)((CFlowChartPane*)pView)->GetDocumentPtr();
			}
			else
			{
				pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
			}
		}

		
		if(pDoc){
			CChildFrame* pChildFrm  = pDoc->Get3DViewParent();
			if(pChildFrm)
				pChildFrm->StorePanInfo();
		}

		CProjectExport projExport(this,pDoc);
		projExport.ExportAsInputZip(strProjName,strProjPath,bNotForceDB,bOpenProject);

	}
	return;	
}

LRESULT CMainFrame::OnSendExportZipToARC(WPARAM wParam,LPARAM lParam)
{
	bool bNotForceDB = false;//*((bool*)(wParam));
	CTermPlanDoc* pDoc = ((CTermPlanDoc*)(wParam));
	CFileDialog dlgFile( FALSE, ".zip",pDoc->m_ProjInfo.name+(bNotForceDB?"_project Not Force DB":"_project"),OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"zip Files (*.zip)|*.zip||" );
	CString sZipFileName;
	
	if (dlgFile.DoModal() == IDOK)
	{
		sZipFileName = dlgFile.GetPathName();
	}
	else
	{
		return TRUE;
	}
	DeleteFile( sZipFileName );
	BeginWaitCursor();
	
	CString sProjectPath = pDoc->m_ProjInfo.path;
	sProjectPath += "\\INPUT";
	
	std::vector<CString>vAllInputFileName;
	TCHAR oldDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, oldDir);
	SetCurrentDirectory(sProjectPath);
	
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
	FindNextFile(hFind, &FindFileData);				// gets ".."
	while(GetLastError() != ERROR_NO_MORE_FILES) {
		if(FindNextFile(hFind, &FindFileData)) {
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				vAllInputFileName.push_back( sProjectPath + "\\" + FindFileData.cFileName );
			}
		}
		else
			break;				
	}
	FindClose(hFind);
	SetCurrentDirectory(oldDir);
	SetLastError(0);
	
	int iFileCount = vAllInputFileName.size();		
	char **pFiles = (char **) new  int [iFileCount];
	for (int i=0; i<iFileCount; i++)
	{
		pFiles[i] = new char[MAX_PATH+1];
		strcpy( pFiles[i], vAllInputFileName[i] );
	}
	
	CZipInfo InfoZip;
// 	if (!InfoZip.InitializeZip())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		return TRUE;
// 	}
	
	//InfoZip.
	CString _strTempInputZipFile = getTempPath() + CString("\\") + pDoc->m_ProjInfo.name+"_input.zip";
	DeleteFile( _strTempInputZipFile );
	
	if (!InfoZip.AddFiles(_strTempInputZipFile, pFiles, iFileCount))
	{
		EndWaitCursor();
		AfxMessageBox("Failure to export project!", MB_OK);
		return TRUE;
	}
	
	
// 	if (!InfoZip.Finalize())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		return TRUE;
// 	}

	//	zip Mathematic
	//	by Kevin
	CString _strFolder = pDoc->m_ProjInfo.path + "\\Mathematic";
	CString _strTempMathematicZipFile;
	if (!zipFloder(pDoc->m_ProjInfo.name, _strFolder, _strTempMathematicZipFile))
		return TRUE;

	//	zip mathresult
	CString _strTempMathResultZipFile;
	_strFolder = pDoc->m_ProjInfo.path + "\\MathResult";
	if (!zipFloder(pDoc->m_ProjInfo.name, _strFolder, _strTempMathResultZipFile))
		return TRUE;
	
	CString _strTempDBZipFile;
	// zip project db
	if( !zipProjectDB( pDoc->m_ProjInfo.name, _strTempDBZipFile ) )
		return TRUE;
	
	
	// zip temp input zip and temp db zip to a zip file
	if( !zipTempZipToTermzip(  pDoc->m_ProjInfo.name,pDoc->m_ProjInfo.path, _strTempInputZipFile, _strTempDBZipFile, 
		_strTempMathematicZipFile, _strTempMathResultZipFile,
		sZipFileName,bNotForceDB,pDoc ) )
		return TRUE;
	
	// delete temp file
	DeleteFile( _strTempInputZipFile );
	DeleteFile( _strTempDBZipFile );
	if (!_strTempMathematicZipFile.IsEmpty())
		DeleteFile(_strTempMathematicZipFile);
	if (!_strTempMathResultZipFile)
		DeleteFile(_strTempMathResultZipFile);
	
	EndWaitCursor();

	return TRUE;
	
}

LRESULT CMainFrame::OnSendExportEmailToARC(WPARAM wParam,LPARAM lParam)
{

	
	bool bNotForceDB = false;//*((bool*)(wParam));
	CTermPlanDoc* pDoc = ((CTermPlanDoc*)(wParam));

	// termpname == 
	//CString sZipFileName="C:\\Temp.zip";
		CString sProjectPath = pDoc->m_ProjInfo.path;
	sProjectPath += "\\INPUT";
	CString sZipFileName;
	sZipFileName.Format("%s\\%s.zip", pDoc->m_ProjInfo.path, pDoc->m_ProjInfo.name+(bNotForceDB?" Not Force DB":""));
	DeleteFile( sZipFileName );

	
	BeginWaitCursor();
	
	std::vector<CString>vAllInputFileName;
	TCHAR oldDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, oldDir);
	SetCurrentDirectory(sProjectPath);
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
	FindNextFile(hFind, &FindFileData);				// gets ".."
	while(GetLastError() != ERROR_NO_MORE_FILES) {
		if(FindNextFile(hFind, &FindFileData)) {
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				vAllInputFileName.push_back( sProjectPath + "\\" + FindFileData.cFileName );
			}
		}
		else
			break;				
	}
	FindClose(hFind);
	SetCurrentDirectory(oldDir);
	SetLastError(0);
	
	int iFileCount = vAllInputFileName.size();		
	char **pFiles = (char **) new  int [iFileCount];
	for (int i=0; i<iFileCount; i++)
	{
		pFiles[i] = new char[MAX_PATH+1];
		strcpy( pFiles[i], vAllInputFileName[i] );
	}

	CZipInfo InfoZip;
// 	if (!InfoZip.InitializeZip())
// 	{	
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project !", MB_OK);
// 		return TRUE;
// 	}


	//InfoZip.
	CString _strTempInputZipFile = getTempPath() + "\\" + pDoc->m_ProjInfo.name+"_input.zip";
	DeleteFile( _strTempInputZipFile );
	if (!InfoZip.AddFiles(_strTempInputZipFile, pFiles, iFileCount))
	{
		EndWaitCursor();
		AfxMessageBox("Failure to export project !", MB_OK);
		return TRUE;
	}

// 	if (!InfoZip.Finalize())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project !", MB_OK);
// 		return TRUE;
// 	}

	//	zip Mathematic
	//	by Kevin
	CString _strFolder = pDoc->m_ProjInfo.path + "\\Mathematic";
	CString _strTempMathematicZipFile;
	if (!zipFloder(pDoc->m_ProjInfo.name, _strFolder, _strTempMathematicZipFile))
		return TRUE;

	//	zip mathresult
	CString _strTempMathResultZipFile;
	_strFolder = pDoc->m_ProjInfo.path + "\\MathResult";
	if (!zipFloder(pDoc->m_ProjInfo.name, _strFolder, _strTempMathResultZipFile))
		return TRUE;

	// zip project db
	CString _strTempDBZipFile;
	if( !zipProjectDB( pDoc->m_ProjInfo.name, _strTempDBZipFile ) )
		return TRUE;
	
	// zip temp input zip and temp db zip to a zip file
	if( !zipTempZipToTermzip(pDoc->m_ProjInfo.name, pDoc->m_ProjInfo.path,_strTempInputZipFile, _strTempDBZipFile, 
		_strTempMathematicZipFile, _strTempMathResultZipFile,
		sZipFileName ,bNotForceDB,pDoc) )
		return TRUE;
	
	// delete temp file
	DeleteFile( _strTempInputZipFile );
	DeleteFile( _strTempDBZipFile );
	if (!_strTempMathematicZipFile.IsEmpty())
		DeleteFile(_strTempMathematicZipFile);
	if (!_strTempMathResultZipFile)
		DeleteFile(_strTempMathResultZipFile);
	
	EndWaitCursor();


	HMODULE hMod = LoadLibrary("MAPI32.DLL");

	if (hMod == NULL)	
	{
		AfxMessageBox(AFX_IDP_FAILED_MAPI_LOAD);
		return TRUE;
	}

	ULONG (PASCAL *lpfnSendMail)(ULONG, ULONG, MapiMessage*, FLAGS, ULONG);
	(FARPROC&)lpfnSendMail = GetProcAddress(hMod, "MAPISendMail");

	if (lpfnSendMail == NULL)
	{
		AfxMessageBox(AFX_IDP_INVALID_MAPI_DLL);
		return TRUE;
	}

	

	iFileCount = 1;// only one input.zip file
	MapiFileDesc* pFileDesc = (MapiFileDesc*)malloc(sizeof(MapiFileDesc) * iFileCount);
	memset(pFileDesc,0,sizeof(MapiFileDesc) * iFileCount);

	

	pFileDesc->lpszFileName = sZipFileName.GetBuffer(sZipFileName.GetLength() );
	pFileDesc->lpszPathName = sZipFileName.GetBuffer(sZipFileName.GetLength() );
	pFileDesc->nPosition = (ULONG)-1;
	

	MapiRecipDesc recip;
	memset(&recip,0,sizeof(MapiRecipDesc));
	recip.lpszAddress	= NULL;
	recip.ulRecipClass = MAPI_TO;

	
	MapiMessage message;
	memset(&message, 0, sizeof(message));
	message.nFileCount	= iFileCount;				
	message.lpFiles		= pFileDesc;				
	message.nRecipCount = 0;						
	message.lpRecips 	= &recip;					
	message.lpszSubject	= "";	
	message.lpszNoteText= "";	

	
	CWnd* pParentWnd = CWnd::GetSafeOwner(NULL, NULL);

	
	int nError = lpfnSendMail(0, 0,
					&message, MAPI_LOGON_UI|MAPI_DIALOG, 0);

	if (nError != SUCCESS_SUCCESS && nError != MAPI_USER_ABORT 
			&& nError != MAPI_E_LOGIN_FAILURE)
	{
		AfxMessageBox(AFX_IDP_FAILED_MAPI_SEND);
	}

	
	pParentWnd->SetActiveWindow();

	
	free(pFileDesc);
	
	FreeLibrary(hMod);

	return TRUE;
}


bool CMainFrame::ExportProjDBInfo(CAirportDatabase *pDB, const CString &sDestPath, CString& sFullProjInfoFile)
{
	CString sInput=pDB->getName();
	//if DB's name is "DEFAULT DB", change it .
	if(pDB->getName().CompareNoCase("DEFAULT DB")==0)
	{
		CDlgAirportDBNameInput dlg( "Please Enter The Airport Name for the Exported Project:" );//"Please Enter the Airport Name for the Exported Project"
		if(dlg.DoModal()==IDCANCEL)
		{
			return false;
		}
		sInput=AIRPORTDBLIST->getUniqueName(dlg.m_sAirportDBName);
		CMDIChildWnd* pMDIActive = MDIGetActive();
		ASSERT(pMDIActive != NULL);
		CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		if(sInput.CompareNoCase(dlg.m_sAirportDBName)==0 ) // selected  an existed db.
		{
			pDoc->m_ProjInfo.dbname=sInput;

		}
		else // input a new db.
		{
			pDoc->m_ProjInfo.lUniqueIndex = -1 ;
		}
		PROJMANAGER->UpdateProjInfoFile(pDoc->m_ProjInfo);
	}

	TCHAR sProjInfoFile[MAX_PATH];
	sprintf(sProjInfoFile,"%s\\%s",sDestPath,"ProjectDB.txt");
	fsstream fproj(sProjInfoFile,stdios::out|stdios::trunc);
	fproj<<(LPCSTR)sInput<<endl;
	fproj.close();

	sFullProjInfoFile=sProjInfoFile;
	return true;
}

void CMainFrame::OnShowProcessDefine() 
{
	CMDIChildWnd* pMDIChild = CreateOrActivateFrame(theApp.m_pProcessDefineTemplate, RUNTIME_CLASS(CProcessDefineDlg));
	if( !pMDIChild)
		return ;

	pMDIChild->ShowWindow(SW_HIDE);
	CProcessDefineDlg* pView=(CProcessDefineDlg*)pMDIChild->GetActiveView();
	pMDIChild->ShowWindow(SW_SHOW);
}

static bool bBarChanged = FALSE;       
static bool bNeedBarChanged = FALSE;  
void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CNewMDIFrameWnd::OnSize(nType, cx, cy);
	if (IsWindowVisible())
		AdjustToolbar();
	m_nOnSizeType = nType;
}



void CMainFrame::OnComparativereportopen() 
{
	// TODO: Add your command handler code here
	COpenComparativeReportsGroup dlg;
	if (dlg.DoModal() == IDOK)
	{
		CString strName = dlg.m_strName;
		CString strDesc = dlg.m_strDesc;
		CreateCompareReportAndResultView(strName, strDesc);
	}
}

void CMainFrame::OnComparativereportnew() 
{
	// TODO: Add your command handler code here
	CreateCompareReportAndResultView();
}
void CMainFrame::CreateCompareReportAndResultView(const CString& strName /* = NULL*/, const CString& strDesc /* = NULL */)
{
	CDocument * pDoc = theApp.m_pCompareReportTemplate2->CreateNewDocument();
	
	CMDIChildWnd* pNewFrame	= (CMDIChildWnd*)(theApp.m_pCompareReportTemplate2->CreateNewFrame(pDoc, NULL));
	theApp.m_pCompareReportTemplate2->InitialUpdateFrame(pNewFrame, pDoc);
	pNewFrame->ShowWindow(SW_MAXIMIZE);

	POSITION pos = pDoc->GetFirstViewPosition();
	CView * pTempView =	NULL;
	CCompareReportView* pCmpReportView = NULL;
	while (pos != NULL)
	{
		pTempView = pDoc->GetNextView(pos);

		if (pTempView->IsKindOf(RUNTIME_CLASS(CCompareReportView)))
		{
			pCmpReportView = (CCompareReportView *)pTempView;
			break;
		}
	}

	if (!strName.IsEmpty())// && !strDesc.IsEmpty())
	{
		((CCompareReportDoc*)pDoc)->GetCmpReport()->LoadProject(strName, strDesc);
	}

	pDoc->UpdateAllViews(NULL);
}

void CMainFrame::OnComparativerun() 
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if (!pMDIActive)
		return;
	assert( pMDIActive != NULL );
	CCompareReportDoc* pDoc = (CCompareReportDoc*)pMDIActive->GetActiveDocument();
	assert( pDoc!= NULL );
	if (pDoc == NULL)
		return;

	POSITION pos = pDoc->GetFirstViewPosition();

	CView * pTempView;
	while (pos != NULL)
	{
		pTempView = pDoc->GetNextView(pos);
		
		if (pTempView->IsKindOf(RUNTIME_CLASS(CCompareReportView)))
		{
//			((CCompareReportView2 *)pTempView)->RunCompReport();
		}
	}
}

void CMainFrame::ChangeSize(const CRect rcWnd, const CRect rcClient, UINT nIndex)
{
	if(nIndex == 2)
	{
		CMDIChildWnd* pMDIActive = MDIGetActive();
		if( pMDIActive == NULL )
			return ;
		CTermPlanDoc* pDoc = NULL;
		CView* pView = pMDIActive->GetActiveView();
		if( pView == NULL)
			return ;
		if(pView->IsKindOf( RUNTIME_CLASS( CFlowChartPane )))
		{
			pDoc = (CTermPlanDoc*)((CFlowChartPane*)pView)->GetDocumentPtr();
		}
		else
		{
			pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		}
		if( pDoc == NULL )
			return ;

		POSITION pos = pDoc->GetFirstViewPosition();
		static int nWidth = 0;
		while (pos != NULL)
		{
			CView* temp = pDoc->GetNextView(pos);
			if(temp->IsKindOf( RUNTIME_CLASS( CMathematicView ) ) )
			{
				CMDIChildWnd* pFrame;
				pFrame = (CMDIChildWnd*)((CMathematicView*)temp)->GetParent();
				CRect rcMainWnd, rcMainClient, rcFrameWnd, rcFrameClient;
				AfxGetMainWnd()->GetWindowRect(&rcMainWnd);
				AfxGetMainWnd()->GetClientRect(&rcMainClient);
				pFrame->GetWindowRect(&rcFrameWnd);
				pFrame->GetClientRect(&rcFrameClient);
				// TRACE("\n\n\n\n%d-%d-%d-%d", rcMainWnd.top, rcMainWnd.bottom, rcMainWnd.Height(), rcMainWnd.right);
				// TRACE("\n%d-%d-%d-%d", rcMainClient.top, rcMainClient.bottom, rcMainClient.Height(), rcMainClient.right);
				// TRACE("\n%d-%d-%d-%d", rcFrameWnd.top, rcFrameWnd.bottom, rcFrameWnd.Height(), rcFrameWnd.right);
				// TRACE("\n%d-%d-%d-%d", rcFrameClient.top, rcFrameClient.bottom, rcFrameClient.Height(), rcFrameClient.right);
				if(rcWnd.bottom != 0)
					pFrame->MoveWindow(0, 0, rcFrameWnd.Width(), rcWnd.top-rcFrameWnd.top);
				else
				{
					pFrame->MoveWindow(0, 0, rcFrameWnd.Width(), rcMainClient.bottom-rcFrameWnd.top+22);
				}
				nWidth = rcFrameWnd.Width();
			}
			else if(temp->IsKindOf(( RUNTIME_CLASS( CProcessFlowView))))
			{
				CMDIChildWnd* pFrame;
				pFrame = (CMDIChildWnd*)((CProcessFlowView*)temp)->GetParent();
				CRect rc1, rc;
				AfxGetMainWnd()->GetClientRect(&rc1);
				pFrame->GetWindowRect(&rc);
				if(rcWnd.bottom != 0)
					pFrame->MoveWindow(nWidth+1, 0, rc.Width(), rcWnd.top-rc.top);
				else
					pFrame->MoveWindow(nWidth+1, 0, rc.Width(), rc1.bottom-rc.top+22);
			}
		}
		return ;
	}
	else if(nIndex == 1)
	{
		//POSITION pos = theApp.m_pCompareReportTemplate->GetFirstDocPosition();
		//if(pos == NULL)
		//	return ;
		//CDocument* pDoc = theApp.m_pCompareReportTemplate->GetNextDoc(pos);
		//if(pDoc == NULL)
		//	return ;
		//pos = pDoc->GetFirstViewPosition();
		//static int nWidth = 0;
		//while (pos != NULL)
		//{
		//	CView* temp = pDoc->GetNextView(pos);
		//	if(temp->IsKindOf( RUNTIME_CLASS( CCompareReportView ) ) )
		//	{
		//		((CCompareReportView*)temp)->SetSizeChangedStatus(2);
		//		CFrameCompareReport* pFrame;
		//		pFrame = (CFrameCompareReport*)((CCompareReportView*)temp)->GetParent();
		//		CRect rcMainWnd, rcMainClient, rcFrameWnd, rcFrameClient;
		//		AfxGetMainWnd()->GetWindowRect(&rcMainWnd);
		//		AfxGetMainWnd()->GetClientRect(&rcMainClient);
		//		pFrame->GetWindowRect(&rcFrameWnd);
		//		pFrame->GetClientRect(&rcFrameClient);
		//		// TRACE("\n\n\n\n%d-%d-%d-%d", rcMainWnd.top, rcMainWnd.bottom, rcMainWnd.Height(), rcMainWnd.right);
		//		// TRACE("\n%d-%d-%d-%d", rcMainClient.top, rcMainClient.bottom, rcMainClient.Height(), rcMainClient.right);
		//		// TRACE("\n%d-%d-%d-%d", rcFrameWnd.top, rcFrameWnd.bottom, rcFrameWnd.Height(), rcFrameWnd.right);
		//		// TRACE("\n%d-%d-%d-%d", rcFrameClient.top, rcFrameClient.bottom, rcFrameClient.Height(), rcFrameClient.right);
		//		if(rcWnd.bottom != 0)
		//			pFrame->MoveWindow(0, 0, rcFrameWnd.Width(), rcWnd.top-rcFrameWnd.top);
		//		else
		//		{
		//			pFrame->MoveWindow(0, 0, rcFrameWnd.Width(), rcMainClient.bottom-rcFrameWnd.top+22);
		//		}
		//		nWidth = rcFrameWnd.Width();
		//		((CCompareReportView*)temp)->SetSizeChangedStatus(0);
		//	}
		//	else if(temp->IsKindOf(( RUNTIME_CLASS( CCompareReportResultView))))
		//	{
		//		((CCompareReportResultView*)temp)->SetSizeChangedStatus(2);
		//		CFrameCompareReportResult* pFrame;
		//		pFrame = (CFrameCompareReportResult*)((CCompareReportResultView*)temp)->GetParent();
		//		CRect rc1, rc;
		//		AfxGetMainWnd()->GetClientRect(&rc1);
		//		pFrame->GetWindowRect(&rc);
		//		if(rcWnd.bottom != 0)
		//			pFrame->MoveWindow(nWidth+1, 0, rc.Width(), rcWnd.top-rc.top);
		//		else
		//			pFrame->MoveWindow(nWidth+1, 0, rc.Width(), rc1.bottom-rc.top+22);
		//		((CCompareReportResultView*)temp)->SetSizeChangedStatus(0);
		//	}
		//}
	}
}

void CMainFrame::OnUpdateComparativerun(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if (pMDIActive)
	{
		CDocument* pDoc = pMDIActive->GetActiveDocument();
		if (pDoc)
		{
			if (pDoc->GetRuntimeClass()->m_lpszClassName == _T("CCompareReportDoc"))
			{
				pCmdUI->Enable();
				return;
			}
		}
	}

	pCmdUI->Enable(FALSE);
}

void CMainFrame::OnCompareWindowSidebyside() 
{
	//get the dimensions of the MainFrame client area 
	CRect rClient;
	::GetClientRect(m_hWndMDIClient,&rClient);

	//get active MDI child
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	//get the active document
	//CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	CDocument* pDoc = pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);
	//pDoc is active document
	//now we will go through each view of the doc and place it properly
	CView* pMSView = NULL;						//the MSV
	std::set<CMDIChildWnd*> sOtherViewFrames;	//set containing all non-MSV child frames
	
	POSITION pos = pDoc->GetFirstViewPosition();
	while(pos != NULL)
	{
		CView* pView = pDoc->GetNextView(pos);

		//if the view is a CNodeView, then this is the MSV
		if(pView->IsKindOf(RUNTIME_CLASS(CNodeView)))
			pMSView = pView;
		else //otherwise add the parent frame of the view (which is an MDI child frame window) to the set
		{
			CFrameWnd* pFrameWnd = pView->GetParentFrame();
			ASSERT(pFrameWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));

		    sOtherViewFrames.insert((CMDIChildWnd*)pFrameWnd);
		}
	}

	if(pMSView != NULL) {
		if(pMSView->GetParentFrame()->IsZoomed()) {
			//if maximized, un-maximize
			WINDOWPLACEMENT wp;
			pMSView->GetParentFrame()->GetWindowPlacement(&wp);
			wp.showCmd = SW_SHOWNORMAL;
			pMSView->GetParentFrame()->SetWindowPlacement(&wp);
		}
		//place the MSV at the left
		pMSView->GetParentFrame()->MoveWindow(
			rClient.left,
			rClient.top,
			rClient.Width()/4,
			rClient.Height());
		pMSView->GetParentFrame()->BringWindowToTop();

		//iterate through the "other views" and place them next to the MSV
		int count=sOtherViewFrames.size();
		int n=0;
		for(std::set<CMDIChildWnd*>::iterator it=sOtherViewFrames.begin();
			it!=sOtherViewFrames.end();
			it++) {
				
			if((*it)->IsZoomed()) { //if maximized, un-maximize
				WINDOWPLACEMENT wp;
				(*it)->GetWindowPlacement(&wp);
				wp.showCmd = SW_SHOWNORMAL;
				(*it)->SetWindowPlacement(&wp);
			}
			(*it)->MoveWindow(static_cast<int>(rClient.left+rClient.Width()/4),
				static_cast<int>(rClient.top+(n*rClient.Height()/count)),
				static_cast<int>(rClient.Width()*0.75),
				static_cast<int>(rClient.Height()/count));
			n++;
		}
	}
}

void CMainFrame::OnUpdateCompareWindowSidebyside(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	ASSERT(m_hWndMDIClient != NULL);
	pCmdUI->Enable(MDIGetActive() != NULL);
}


void CMainFrame::OnBarShowShapemath() 
{
	// TODO: Add your command handler code here
	ShowControlBar(&m_wndShapesBarPFlow,!m_wndShapesBarPFlow.IsWindowVisible(),FALSE);	
	ShapesBarPFlowMsg(m_wndShapesBarPFlow.IsWindowVisible());
}

void CMainFrame::OnUpdateBarShowShapemath(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_wndShapesBarPFlow.IsWindowVisible());
}

void CMainFrame::ShapesBarPFlowMsg(BOOL bShow)
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if( pMDIActive == NULL )
		return ;
	CTermPlanDoc* pDoc = NULL;
	CView* pView = pMDIActive->GetActiveView();
	if(pView->IsKindOf( RUNTIME_CLASS( CFlowChartPane )))
	{
		pDoc = (CTermPlanDoc*)((CFlowChartPane*)pView)->GetDocumentPtr();
	}
	else
	{
		pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	}
	if( pDoc == NULL )
		return ;

	CMDIChildWnd *pFrame1 = NULL, *pFrame2 = NULL;
	CRect rc1(0,0,0,0), rc2(0,0,0,0), rc3(0,0,0,0);
//	m_wndShapesBarPFlow.GetWindowRect(&rc1);
	m_wndAircraftModel.GetWindowRect(&rc1);
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = pDoc->GetNextView(pos);
		if(pView->IsKindOf( RUNTIME_CLASS( CMathematicView ) ) )
		{
			pFrame1 = (CMDIChildWnd*)((CMathematicView*)pView)->GetParent();
			pFrame1->GetWindowRect(&rc2);
		}
		else if(pView->IsKindOf( RUNTIME_CLASS( CProcessFlowView ) ) )
		{
			pFrame2 = (CMDIChildWnd*)((CProcessFlowView*)pView)->GetParent();
			pFrame2->GetWindowRect(&rc3);
		}
	}
	if(pFrame1)
	{
		pFrame1->MoveWindow(0, 0, rc2.Width(), rc2.Height());
	}
	if(pFrame2)
	{
		CRect rcClient(0,0,0,0);
		AfxGetMainWnd()->GetClientRect(&rcClient);

		if(bShow)
			pFrame2->MoveWindow(rc2.right, 0, rcClient.Width()-rc1.Width()-rc2.Width()-1, rc3.Height());
		else
			pFrame2->MoveWindow(rc2.right, 0, rcClient.Width()-rc2.Width()-1, rc3.Height());
	}
}

void CMainFrame::SetMainBarComboBox(bool bEnable, bool bSetContentDefault)
{
	ASSERT(::IsWindow(m_wndToolBar.m_hWnd));
	m_wndToolBar.EnableComboBox(bEnable, bSetContentDefault);

}

void CMainFrame::saveMainBarComboBoxToDB()
{
   m_wndToolBar.SaveComBoxToDB() ;
}
void CMainFrame::OnViewMathflowview() 
{
	// TODO: Add your command handler code here
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if( pMDIActive == NULL )
		return ;

	CTermPlanDoc* pDoc = NULL;
	CView* pView = pMDIActive->GetActiveView();

	if (pView->IsKindOf(RUNTIME_CLASS(CFlowChartPane)))
	{
		pDoc = (CTermPlanDoc*)((CFlowChartPane*)pView)->GetDocumentPtr();
	}
	else
	{
		pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	}

	if (pDoc == NULL)
		return;

	CMDIChildWnd *pFlowFrame = NULL;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = pDoc->GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CProcessFlowView)))
		{
			pFlowFrame = (CMDIChildWnd*)((CProcessFlowView*)pView)->GetParent();
			break;
		}
	}
	if (pFlowFrame == NULL)
	{
		CMDIChildWnd* pNewFrame	= (CMDIChildWnd*)(theApp.m_pProcessFlowTemplate->CreateNewFrame(pDoc, NULL));
		theApp.m_pProcessFlowTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	}
}

void CMainFrame::OnReportsMathqtime() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_QUEUETIME_REP);
}

void CMainFrame::OnReportsMathqlength() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_QUEUELENGTH_REP);
}

void CMainFrame::OnReportsMaththroughput() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_THROUGHPUT_REP);
}

void CMainFrame::OnReportRetailReport()
{
	LoadReport(ENUM_RETAIL_REP);
}

void CMainFrame::OnReportsMathpaxcount() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_PASSENGERNO_REP);
}

void CMainFrame::OnReportsMathutilization() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_UTILIZATION_REP);
}

void CMainFrame::OnBarShowRunlog() 
{
	// TODO: Add your command handler code here
	ASSERT(::IsWindow(m_wndCompRepLogBar.m_hWnd));
	m_wndCompRepLogBar.SetParentIndex(2);
	ShowControlBar(&m_wndCompRepLogBar, !m_wndCompRepLogBar.IsWindowVisible(), FALSE);
}

void CMainFrame::OnUpdateBarShowRunlog(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	ASSERT(::IsWindow(m_wndCompRepLogBar.m_hWnd));
	pCmdUI->SetCheck(m_wndCompRepLogBar.IsWindowVisible());
}

void CMainFrame::OnReportsPassengerMissflight() 
{
	// TODO: Add your command handler code here
	LoadReport(ENUM_MISSFLIGHT_REP);
}

BOOL CMainFrame::zipFloder(const CString& strProjName,  const CString& _strSrcFolder, CString &_strTempZipFile)
{
	std::vector<CString>	vSubFileNames;
	TCHAR oldDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, oldDir);
	if (!SetCurrentDirectory(_strSrcFolder))	//	Folder is not exist
	{
		_strTempZipFile.Empty();
		return TRUE;
	}

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
	FindNextFile(hFind, &FindFileData);				// gets ".."
	while(GetLastError() != ERROR_NO_MORE_FILES) {
		if(FindNextFile(hFind, &FindFileData)) {
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				vSubFileNames.push_back( _strSrcFolder + "\\" + FindFileData.cFileName );
			}
		}
		else
			break;				
	}
	FindClose(hFind);
	SetCurrentDirectory(oldDir);
	SetLastError(0);
	
	int iFileCount = vSubFileNames.size();
	if (iFileCount == 0)
	{
		_strTempZipFile.Empty();
		return TRUE;
	}

	char **pFiles = (char **) new  int [iFileCount];
	for (int i=0; i<iFileCount; i++)
	{
		pFiles[i] = new char[MAX_PATH+1];
		strcpy( pFiles[i], vSubFileNames[i] );
	}
	
	CZipInfo InfoZip;
// 	if (!InfoZip.InitializeZip())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		return FALSE;
// 	}
	
	//InfoZip.
	int nPos = -1;
	if ((nPos = _strSrcFolder.ReverseFind('\\')) != -1)
	{
		_strTempZipFile = getTempPath() + strProjName+"_"+_strSrcFolder.Mid(nPos + 1)+".zip";
	}
	else
	{
		EndWaitCursor();
		AfxMessageBox("Failure to export project!", MB_OK);
		return FALSE;
	}

	//CString _strTempInputZipFile = getTempPath() + CString("\\") + pDoc->m_ProjInfo.name+"_input.zip";
	DeleteFile( _strTempZipFile );
	
	if (!InfoZip.AddFiles(_strTempZipFile, pFiles, iFileCount))
	{
		EndWaitCursor();
		AfxMessageBox("Failure to export project!", MB_OK);
		return FALSE;
	}

	
// 	if (!InfoZip.Finalize())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		return FALSE;
// 	}

	return TRUE;
}

void CMainFrame::ZipAirportInfo(const CString& strProjName, CString projPath,std::vector<CString> &vAirportName)
{
	//CMDIChildWnd* pMDIActive = MDIGetActive();
	//ASSERT(pMDIActive != NULL);
	//CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	//ASSERT(pDoc != NULL);
	//now treat only one airport  in the project 

	//make sure the airport is exist ,that is make sure the airport path is valid
	int nAirport = 1;
	CString strAirportPath = projPath + _T("\\Input\\Airport1");
	while (PathFileExists((LPCSTR)strAirportPath))
	{
		
		std::vector<CString> vAddFile;
		//zip the airside files	
		CString strAirsidePath = strAirportPath + "\\Airside";
		if (PathFileExists((LPCSTR)strAirsidePath)){
			CString tempAirsidePath;
			if (!zipFloder(strProjName,strAirsidePath,tempAirsidePath))
				continue;
			
			vAddFile.push_back(tempAirsidePath);
		}

		//zip the landside files
		CString strLandsidePath = strAirportPath + "\\Landside";
		if(PathFileExists((LPCSTR)strLandsidePath)){
			CString tempLandsidePath;
			if(!zipFloder(strProjName,strLandsidePath,tempLandsidePath))
				continue;		

			vAddFile.push_back(tempLandsidePath);
		}	

		//airport
		CString tempAirportPath;
		if(!zipFloder(strProjName,strAirportPath,vAddFile,tempAirportPath))
			continue;
		vAirportName.push_back(tempAirportPath);
		
		//Next Airport
		nAirport += 1;
		strAirportPath.Empty();
		strAirportPath.Format(_T("\\Input\\Airport%d"),nAirport);
		strAirportPath = projPath + strAirportPath;
	}
}

BOOL CMainFrame::zipFloder(const CString& strProjName, const CString& _strSrcFolder,std::vector<CString> vAddFile,CString& _strTempZipFile)
{
	std::vector<CString>	vSubFileNames;
	TCHAR oldDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, oldDir);
	if (!SetCurrentDirectory(_strSrcFolder))	//	Folder is not exist
	{
		_strTempZipFile.Empty();
		return TRUE;
	}

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
	FindNextFile(hFind, &FindFileData);				// gets ".."
	while(GetLastError() != ERROR_NO_MORE_FILES) {
		if(FindNextFile(hFind, &FindFileData)) {
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				vSubFileNames.push_back( _strSrcFolder + "\\" + FindFileData.cFileName );
			}
		}
		else
			break;				
	}
	FindClose(hFind);
	SetCurrentDirectory(oldDir);
	SetLastError(0);
	
	//add others file
	std::vector<CString>::iterator it = vSubFileNames.end();
	vSubFileNames.insert(it,vAddFile.begin(),vAddFile.end());

	int iFileCount = vSubFileNames.size();
	if (iFileCount == 0)
	{
		_strTempZipFile.Empty();
		return TRUE;
	}

	char **pFiles = (char **) new  int [iFileCount];
	for (int i=0; i<iFileCount; i++)
	{
		pFiles[i] = new char[MAX_PATH+1];
		strcpy( pFiles[i], vSubFileNames[i] );
	}

	CZipInfo InfoZip;
// 	if (!InfoZip.InitializeZip())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		return FALSE;
// 	}

	//InfoZip.
	int nPos = -1;
	if ((nPos = _strSrcFolder.ReverseFind('\\')) != -1)
	{
		_strTempZipFile = getTempPath() + strProjName+"_"+_strSrcFolder.Mid(nPos + 1)+".zip";
	}
	else
	{
		EndWaitCursor();
		AfxMessageBox("Failure to export project!", MB_OK);
		return FALSE;
	}

	//CString _strTempInputZipFile = getTempPath() + CString("\\") + pDoc->m_ProjInfo.name+"_input.zip";
	DeleteFile( _strTempZipFile );

	if (!InfoZip.AddFiles(_strTempZipFile, pFiles, iFileCount))
	{
		EndWaitCursor();
		AfxMessageBox("Failure to export project!", MB_OK);
		return FALSE;
	}


// 	if (!InfoZip.Finalize())
// 	{
// 		EndWaitCursor();
// 		AfxMessageBox("Failure to export project!", MB_OK);
// 		return FALSE;
// 	}

	return TRUE;

}



static bool CallBackGetFlightConstraint(CWnd *pParent,FlightConstraint& fltCons)
{
	//FlightConstraint fltType;
	CFlightDialog flightDlg(pParent);
	if(IDOK == flightDlg.DoModal())
	{
		fltCons = flightDlg.GetFlightSelection();
		return true;
	}
	
	return false;
}
void CMainFrame::OnRptAirsideDistTrv()
{
	LoadAirsideReport_New(Airside_DistanceTravel);
	//CMDIChildWnd* pMDIActive = MDIGetActive();
	//CTermPlanDoc* pDoc = NULL;
	//if (pMDIActive == NULL)
	//	return;

	//pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	//ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));

	//CAirsideReportView* pAirsideReportView = NULL;
	//CView* pTempView = NULL;

	//POSITION pos = pDoc->GetFirstViewPosition();
	//while (pos != NULL)
	//{
	//	pTempView = pDoc->GetNextView(pos);
	//	if (pTempView->IsKindOf(RUNTIME_CLASS(CAirsideReportView)))
	//	{
	//		pAirsideReportView = (CAirsideReportView*)pTempView;
	//		break;
	//	}
	//}

	//if (pAirsideReportView == NULL)
	//{
	//	CMDIChildWnd* pNewFrame = (CMDIChildWnd*)theApp.m_pAirSideReportTemplate->CreateNewFrame(pDoc, NULL);
	//	theApp.m_pAirSideReportTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	//	pNewFrame->ShowWindow(SW_MAXIMIZE);
	//	pAirsideReportView = (CAirsideReportView*)pNewFrame->GetActiveView();
	//}
	//ASSERT(pAirsideReportView != NULL);	

	//CAirsideReportCtrl* pReport = pAirsideReportView->GetReportWnd();
	////set project id
	//pReport->SetProjID(pDoc->GetAirsieInput().GetProjID());
	//pReport->SetReportType(Airside_DistanceTravel);
	//pReport->SetCBGetLogFilePath(CallbackGetAirsideLogFilePath);
	//pReport->SetCBGetFlightConstraint(CallBackGetFlightConstraint);
	//pReport->SetLengthUnit((UM_LEN)UNITSMANAGER->GetLengthUnits());
	//pReport->UpdateReport();
}

void CMainFrame::OnRptAirsideFltDelay()
{	
	
	LoadAirsideReport_New(Airside_FlightDelay);
	//CMDIChildWnd* pMDIActive = MDIGetActive();
	//CTermPlanDoc* pDoc = NULL;
	//if (pMDIActive == NULL)
	//	return;

	//pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	//ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));

	//CAirsideReportView* pAirsideReportView = NULL;
	//CView* pTempView = NULL;

	//POSITION pos = pDoc->GetFirstViewPosition();
	//while (pos != NULL)
	//{
	//	pTempView = pDoc->GetNextView(pos);
	//	if (pTempView->IsKindOf(RUNTIME_CLASS(CAirsideReportView)))
	//	{
	//		pAirsideReportView = (CAirsideReportView*)pTempView;
	//		break;
	//	}
	//}

	//if (pAirsideReportView == NULL)
	//{
	//	CMDIChildWnd* pNewFrame = (CMDIChildWnd*)theApp.m_pAirSideReportTemplate->CreateNewFrame(pDoc, NULL);
	//	theApp.m_pAirSideReportTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	//	pNewFrame->ShowWindow(SW_MAXIMIZE);
	//	pAirsideReportView = (CAirsideReportView*)pNewFrame->GetActiveView();
	//}
	//ASSERT(pAirsideReportView != NULL);	

	//CAirsideReportCtrl* pReport = pAirsideReportView->GetReportWnd();
	////set project id
	//pReport->SetProjID(pDoc->GetInputAirside().GetProjectID(pDoc->m_ProjInfo.name));
	//pReport->SetReportType(Airside_FlightDelay);
	//pReport->SetCBGetLogFilePath(CallbackGetAirsideLogFilePath);
	//pReport->SetCBGetFlightConstraint(CallBackGetFlightConstraint);
	//pReport->SetLengthUnit((UM_LEN)UNITSMANAGER->GetLengthUnits());
	//pReport->UpdateReport();

}


void CMainFrame::OnReportsDuration()
{

	LoadAirsideReport_New(Airside_Duration);
	//CMDIChildWnd* pMDIActive = MDIGetActive();
	//CTermPlanDoc* pDoc = NULL;
	//if (pMDIActive == NULL)
	//	return;

	//pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	//ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));

	//CAirsideReportView* pAirsideReportView = NULL;
	//CView* pTempView = NULL;

	//POSITION pos = pDoc->GetFirstViewPosition();
	//while (pos != NULL)
	//{
	//	pTempView = pDoc->GetNextView(pos);
	//	if (pTempView->IsKindOf(RUNTIME_CLASS(CAirsideReportView)))
	//	{
	//		pAirsideReportView = (CAirsideReportView*)pTempView;
	//		break;
	//	}
	//}

	//if (pAirsideReportView == NULL)
	//{
	//	CMDIChildWnd* pNewFrame = (CMDIChildWnd*)theApp.m_pAirSideReportTemplate->CreateNewFrame(pDoc, NULL);
	//	theApp.m_pAirSideReportTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	//	pNewFrame->ShowWindow(SW_MAXIMIZE);
	//	pAirsideReportView = (CAirsideReportView*)pNewFrame->GetActiveView();
	//}
	//ASSERT(pAirsideReportView != NULL);

	//CAirsideReportCtrl* pReport = pAirsideReportView->GetReportWnd();
	//pReport->SetCBGetLogFilePath(CallbackGetAirsideLogFilePath);
	//pReport->SetProjID(pDoc->GetAirsieInput().GetProjID());
	//pReport->SetReportType(Airside_Duration);
	//pReport->SetLengthUnit((UM_LEN)UNITSMANAGER->GetLengthUnits());
	//pReport->UpdateReport();

}

void CMainFrame::OnBillboardExposureincidence()
{
	LoadReport(ENUM_BILLBOARD_EXPOSUREINCIDENCE_REP);
}

void CMainFrame::OnBillboardIncrementalvisit()
{
	LoadReport(ENUM_BILLBOARD_LINKEDPROCINCREMENTVISIT_REP);
}

void CMainFrame::OnLogSetting()
{
	// TODO: Add your command handler code here
	CLogSettingDlg dlg;
	dlg.DoModal();
}



void CMainFrame::OnLogViewselect()
{
	// TODO: Add your command handler code here
	CLogDisplayDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnLogClearlog()
{
	// TODO: Add your command handler code here
	CLogClearingDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnReportAirsideActivity()
{
	LoadAirsideReport_New(Airside_FlightActivity);
}
//void CMainFrame::OnReportAirsideOperation()
//{
//	LoadAirsideReport_New(Airside_AircraftOperation);
//}
void CMainFrame::OnRptAirsideObstructions()
{
// 	CMDIChildWnd* pMCW=(CMDIChildWnd*)GetActiveFrame();
// 	CView* pView=pMCW->GetActiveView();
// 	ASSERT(pView);
// 	ASSERT(pView->GetDocument()->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
// 	CTermPlanDoc* pDoc=(CTermPlanDoc*)pView->GetDocument();
// 
// 	int nProjID = -1;
// 	nProjID = pDoc->GetInputAirside().GetProjectID(pDoc->m_ProjInfo.name);
// 	std::vector<ALTAirport> vAirport;
// 	ALTAirport::GetAirportList(nProjID,vAirport);
// 	for (int i = 0; i < (int)vAirport.size(); i++)
// 	{
// 		int nAirportID = vAirport.at(i).getID();
// 		std::vector<ALTObject> vObject1;
// 		ALTObject::GetObjectList(ALT_RUNWAY,nAirportID,vObject1);
// 
// 		std::vector<ALTObject> vObject2;
// 		ALTObject::GetObjectList(ALT_TAXIWAY,nAirportID,vObject2);
// 	}
	DlgObstructionReport dlg(this);
	dlg.DoModal();
}


void CMainFrame::OnReportAirsideOperational()
{
	LoadAirsideReport_New(Airside_AircraftOperational);
}

void CMainFrame::OnReportAirsideNodeDelay()
{
	//LoadAirsideReport(Airside_NodeDelay);
	LoadAirsideReport_New(Airside_NodeDelay);
}

void CMainFrame::LoadAirsideReport(enum reportType airsideRpType)
{
	//CMDIChildWnd* pMDIActive = MDIGetActive();
	//CTermPlanDoc* pDoc = NULL;
	//if (pMDIActive == NULL)
	//	return;

	//pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	//ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));

	//CAirsideReportView* pAirsideReportView = NULL;
	//CView* pTempView = NULL;

	//POSITION pos = pDoc->GetFirstViewPosition();
	//while (pos != NULL)
	//{
	//	pTempView = pDoc->GetNextView(pos);
	//	if (pTempView->IsKindOf(RUNTIME_CLASS(CAirsideReportView)))
	//	{
	//		pAirsideReportView = (CAirsideReportView*)pTempView;
	//		break;
	//	}
	//}

	//if (pAirsideReportView == NULL)
	//{
	//	CMDIChildWnd* pNewFrame = (CMDIChildWnd*)theApp.m_pAirSideReportTemplate->CreateNewFrame(pDoc, NULL);
	//	theApp.m_pAirSideReportTemplate->InitialUpdateFrame(pNewFrame, pDoc);
	//	pNewFrame->ShowWindow(SW_MAXIMIZE);
	//	pAirsideReportView = (CAirsideReportView*)pNewFrame->GetActiveView();
	//}
	//ASSERT(pAirsideReportView != NULL);	

	//CAirsideReportCtrl* pReport = pAirsideReportView->GetReportWnd();
	////set project id
	//pReport->SetProjID(pDoc->GetInputAirside().GetProjectID(pDoc->m_ProjInfo.name));
	//pReport->SetReportType(airsideRpType);
	//pReport->SetCBGetLogFilePath(CallbackGetAirsideLogFilePath);
	//pReport->SetCBGetFlightConstraint(CallBackGetFlightConstraint);
	//pReport->SetLengthUnit((UM_LEN)UNITSMANAGER->GetLengthUnits());
	//pReport->InitReport();
}

//void CMainFrame::UpdateUnitBar(CTermPlanDoc* pDoc)
//{
//	if(!pDoc)return;
//	if(pDoc->m_systemMode == EnvMode_AirSide)
//	{
//		ShowControlBar(&m_wndUnitBar,TRUE,FALSE);
//		ShowControlBar(&m_wndUnitBarTemp,FALSE,FALSE);
//		m_wndUnitBar.UpdateUnitBar(pDoc); 
//	}
//	else
//	{
//		ShowControlBar(&m_wndUnitBarTemp,TRUE,FALSE);
//		ShowControlBar(&m_wndUnitBar,FALSE,FALSE);
//	}
//	AdjustToolbar();
//}

void CMainFrame::OnDropDownUnitBarArrow(void)
{
}
void CMainFrame::OnNoOpenProjectExportAsZipFile()
{
	CString strProjName = _T("");
	CDlgProjectSelect dlg;
	if(dlg.DoModal() == IDOK) 
	{
		strProjName = dlg.m_sSelProjName;
	}
	else
		return;

// export

	CString strProjsPath = ((CTermPlanApp *)AfxGetApp())->GetProjectsPath();
	strProjsPath.TrimRight("\\");
	CString strProjPath = strProjsPath + _T("\\") + strProjName;
	
	ExportAsInputZip(strProjName,strProjPath,false,false);

}

void CMainFrame::OnNoOpenProjectExportAsInputAndEmail()
{
	CString strProjName = _T("");
	CDlgProjectSelect dlg;
	if(dlg.DoModal() == IDOK) 
	{
		strProjName = dlg.m_sSelProjName;
	}
	else
		return;

// export

	CString strProjsPath = ((CTermPlanApp *)AfxGetApp())->GetProjectsPath();
	strProjsPath.TrimRight("\\");
	CString strProjPath = strProjsPath + _T("\\") + strProjName;
	
	ExportAndEmail(strProjName,strProjPath,false,false);

}

//BOOL CMainFrame::CreateACComponentBar()
//{
//	if(m_wndOnBoardLayoutBar.bIsCreated() )
//		return TRUE;
//	
//	BOOL bResult=m_wndOnBoardLayoutBar.Create(this,ID_ONBOARD_LAYOUTBAR);
//	if(bResult)
//	{
//		m_wndOnBoardLayoutBar.EnableDocking(CBRS_ALIGN_TOP);
//		//DockControlBar(&m_wndOnBoardLayoutBar);
//	}
//	return bResult;
//}

//void CMainFrame::UpdateMainFrameBar( CTermPlanDoc* pDoc )
//{
//	if(!pDoc)
//		return ;
//
//	if(pDoc->GetCurrentMode() ==EnvMode_OnBoard)
//	{
//		m_wndOnBoardLayoutBar.SetInputOnboard(pDoc->GetInputOnboard());
//		ShowControlBar(&m_wndOnBoardLayoutBar,TRUE,FALSE);		
//		
//		ShowControlBar(&m_wndLayoutBar, FALSE,FALSE);
//		ShowControlBar(&m_wndPipeBar,FALSE,FALSE);
//		ShowControlBar(&m_wndAnimationBar,FALSE,FALSE);
//		ShowControlBar(&m_wndAirRouteBar,FALSE,FALSE);
//		
//	} 
//	else
//	{
//		ShowControlBar(&m_wndOnBoardLayoutBar,FALSE,FALSE);
//
//		ShowControlBar(&m_wndLayoutBar, TRUE,FALSE);
//		ShowControlBar(&m_wndPipeBar,TRUE,FALSE);
//		ShowControlBar(&m_wndAnimationBar,TRUE,FALSE);
//		ShowControlBar(&m_wndAirRouteBar,TRUE,FALSE);
//	}
//	AdjustToolbar();
//}
void CMainFrame::OnLoadDefaultAirportDB() // Discarded
{

// 	if(MessageBox("Are you sure recover the AirportDB From the default AirportDB?","Warning",MB_OKCANCEL) != IDOK )
// 		return ;
// 	CMDIChildWnd* pMDIActive = MDIGetActive();
// 	if(pMDIActive == NULL)
// 		return ;
// 	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
// 	if( pDoc== NULL )
// 		return ;
//     CAirportDatabase* projectDB = pDoc->GetTerminal().m_pAirportDB ;
// 	CAirportDatabase* DefaultDB = AIRPORTDBLIST->getAirportDBByName(projectDB->getName());
// 	if(!pDoc->GetTerminal().m_pAirportDB->loadDatabase(DefaultDB))
// 	{
// 		MessageBox("Load Default AirportDB Error","Error",MB_OK);
// 		return ;
// 	}
// 	MessageBox("Load Default AirportDB Successfully","Success",MB_OK);
}

void CMainFrame::OnCommitToDefaultAirportDB() // Discarded
{
// 	if(MessageBox("Are you sure recover the Default AirportDB From the current project AirportDB?","Warning",MB_OKCANCEL) != IDOK )
// 		return ;
// 	CMDIChildWnd* pMDIActive = MDIGetActive();
// 	if(pMDIActive == NULL)
// 		return ;
// 	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
// 	if( pDoc== NULL )
// 		return ;
// 	CAirportDatabase* projectDB = pDoc->GetTerminal().m_pAirportDB ;
// 
// 	AIRPORTDBLIST->WriteAirportDBByName(projectDB->getName(),projectDB) ;
//     
// 
// 	MessageBox("Commit to Default AirportDB Successfully","Success",MB_OK);
}

void CMainFrame::OnUpdateACTypeModels(CCmdUI* pCmdUI)
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	assert(pMDIActive != NULL);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	assert(pDoc != NULL);
 	if (pDoc->m_systemMode == EnvMode_OnBoard)
 	{
 		pCmdUI->Enable(TRUE);
 	}
 	else
 	{
 		pCmdUI->Enable(FALSE);
 	}
}

void CMainFrame::OnUpdateAircraftComponents(CCmdUI* pCmdUI)
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	assert(pMDIActive != NULL);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	assert(pDoc != NULL);
	if (pDoc->m_systemMode == EnvMode_OnBoard)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

#include "ACFurnishingShapesManager.h"
void CMainFrame::OnDatabaseACFurnishings()
{	
	CMDIChildWnd* pMDIActive = MDIGetActive();
	assert( pMDIActive != NULL );
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	
	if(pDoc->m_systemMode == EnvMode_OnBoard) 
	{
		CAircraftFurnishingSectionManager* pManager = pDoc->GetTerminal().m_pAirportDB->getFurnishingMan();
		if (pManager)
		{
			CDlgAircraftFurnishingDB dlgFurnishingDB(pManager,this) ;
			if (IDOK == dlgFurnishingDB.DoModal())
			{			
				CAircraftFurnishingModel* pModel = dlgFurnishingDB.GetEidtToShape();
				if (pModel)
				{
					CWaitCursor wc;
					pDoc->GetInputOnboard()->SetEditModel(pModel);
					CACFurnishingEditFrame* pChildFrm = (CACFurnishingEditFrame*)CreateOrActivateFrame(theApp.m_pAircraftFurnishingTermplate, RUNTIME_CLASS(CACFurnishingEdit3DView));
					ASSERT(pChildFrm != NULL);
					pChildFrm->SetACFurnishingManager(pManager);
					pChildFrm->ShowWindow(SW_SHOWMAXIMIZED);
					ShowControlBar(&m_wndACComponentShapesBar, TRUE, FALSE);
				}
			}
			ACFURNISHINGSHAPESMANAGER.Reload(pManager);
		}
	}
}

void CMainFrame::OnUpdateDatabaseFurnishings( CCmdUI* pCmdUI )
{
	/*CMDIChildWnd* pMDIActive = MDIGetActive();
	assert(pMDIActive != NULL);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	if( pDoc )
	{
		if(pDoc->m_systemMode ==EnvMode_OnBoard)
		{
			pCmdUI->Enable(TRUE);
			return;
		}		
	}*/
	//pCmdUI->Enable(FALSE);
	
}

LRESULT CMainFrame::OnOnboardLayoutEditDlgFallback( WPARAM wParam, LPARAM lParam )
{
	//FallbackReason reason = (FallbackReason) lParam;

	////find and activate the 3d view

	//CMDIChildWnd* pActiveChild = MDIGetActive();
	//CDocument* pActiveDoc;	
	//CAircraftLayoutView* p3DView = NULL;

	//if(!pActiveChild)
	//	return FALSE;
	//if (!(pActiveDoc = pActiveChild->GetActiveDocument()))
	//	return FALSE;

	//POSITION pos = pActiveDoc->GetFirstViewPosition();
	//while (pos != NULL) {
	//	CView* pView = pActiveDoc->GetNextView(pos);
	//	if(pView->IsKindOf(RUNTIME_CLASS(CAircraftLayoutView))) {
	//		p3DView = (CAircraftLayoutView*) pView;
	//		break;
	//	}
	//}
	//if(!p3DView)
	//	return FALSE;

	////decode the fallback data
	//FallbackData* pData = (FallbackData*) wParam;
	//p3DView->m_hFallbackWnd = (HWND) pData->hSourceWnd;
	//p3DView->m_lFallbackParam = lParam;

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
	//default:
	//	ASSERT(FALSE);
	//	return FALSE;
	//};

	//p3DView->m_eMouseState = mousestate;
	//p3DView->m_vMousePosList.clear();	
	return TRUE;
}
void CMainFrame::OnAircraftDoorSetting()
{
// 	CMDIChildWnd* pMDIActive = MDIGetActive();
// 	ASSERT(pMDIActive != NULL);
// 	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
// 	ASSERT(pDoc != NULL);
// 	CDlgAircraftDoors dlg(pDoc->GetTerminal().m_pAirportDB->getAcMan(),pDoc->GetTerminal().m_pAirportDB,this) ;
// 	dlg.DoModal();
}
void CMainFrame::OnAircraftTypeRelationShipSetting()
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	ASSERT(pMDIActive != NULL);
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT(pDoc != NULL);
	CDlgActypeRelationShip dlg(pDoc->GetTerminal().m_pAirportDB->getAcMan(),this);
	dlg.DoModal();
}
//////////////////////////////////////////////////////////////////////////
//vehicle report 
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnVehicleReportForOperate()
{
	LoadAirsideReport_New(Airside_VehicleOperation);
};

void CMainFrame::OnRunwaysOperationsReport()
{
	// TODO: Add your command handler code here
	LoadAirsideReport_New(Airside_RunwayOperaitons);
}

void CMainFrame::OnRunwaysUtilizationReport()
{
	// TODO: Add your command handler code here
	LoadAirsideReport_New(Airside_RunwayUtilization);
}

void CMainFrame::OnFlightConflictReport()
{
	LoadAirsideReport_New(Airside_FlightConflict);

}

//taxiway 
void CMainFrame::OnTaxiwayReportOperate()
{
	LoadAirsideReport_New(Airside_TaxiwayDelay);
}

void CMainFrame::OnTaxiwayUtilizationReport()
{
	LoadAirsideReport_New(Airside_TaxiwayUtilization);
}

//runway delay
void CMainFrame::OnRunwaysDelayReport()
{
	LoadAirsideReport_New(Airside_RunwayDelay);
}
void CMainFrame::OnRunwayExitReport()
{
	LoadAirsideReport_New(Airside_RunwayExit) ;
}
void CMainFrame::OnFlightFuelBurnReport()
{
	LoadAirsideReport_New(Airside_FlightFuelBurning) ;
}

void CMainFrame::OnRunwayCrossingsReport()
{
	LoadAirsideReport_New(Airside_RunwayCrossings);
}

void CMainFrame::OnStandOperationsReport()
{
	LoadAirsideReport_New(Airside_StandOperations);
}

void CMainFrame::OnAirsideIntersectionOperationReport()
{
	LoadAirsideReport_New(Airside_IntersectionOperation);
}

void CMainFrame::OnAirsideControllerWorkloadReport()
{
	LoadAirsideReport_New(Airside_ControllerWorkload);
}

void CMainFrame::OnTakeoffProcessReport()
{
	LoadAirsideReport_New(Airside_TakeoffProcess);
}

void CMainFrame::OnRenderOptions()
{
	if (IDOK == RenderEngine::getSingleton().ShowConfigDlg())
	{
		CTermPlanApp* pApp = (CTermPlanApp*)AfxGetApp();
		pApp->WriteProfileString(_T("Render Engine"), _T("Always Use"),
			RenderEngine::getSingleton().GetAlwaysInUse() ? _T("true") : _T("false"));
	}
}

void CMainFrame::OnUpdateRenderOptions( CCmdUI* pCmdUI )
{
	CMenu* pParentMenu = pCmdUI->m_pParentMenu;
	if (pParentMenu)
	{
		pParentMenu->DeleteMenu(ID_VIEW_RENDEROPTIONS, MF_BYCOMMAND);
		pParentMenu->GetMenuItemCount();
	}
}

CMDIChildWnd* CMainFrame::CreateOrActive3DView(bool* pbAlreadyExist /*= NULL*/)
{
	// temporarily disable render 3d view - Benny Tag
	// 	if (RenderEngine::getSingleton().GetAlwaysInUse())
	// 	{
	// 		return CreateOrActiveRender3DView(pbAlreadyExist);
	// 	}
	return CreateOrActivateFrame(theApp.m_p3DTemplate, RUNTIME_CLASS(C3DView), pbAlreadyExist);
}

CMDIChildWnd* CMainFrame::CreateOrActiveRender3DView(bool* pbAlreadyExist /*= NULL*/)
{
	return CreateOrActivateFrame(theApp.m_pNewRender3DTemplate, RUNTIME_CLASS(CRender3DView), pbAlreadyExist);
}



CMDIChildWnd* CMainFrame::CreateOrActiveEnvModeView( bool* pbAlreadyExist /*= NULL*/ )
{
	CMDIChildWnd* pMDIActive = MDIGetActive();
	if(pMDIActive == NULL)
		return NULL;
	CDocument* pActiveDoc = pMDIActive->GetActiveDocument();
	if(pActiveDoc == NULL)
		return NULL;

	if(!pActiveDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)))
		return NULL;
	CTermPlanDoc* pTPDoc = (CTermPlanDoc* )pActiveDoc;

	if( pTPDoc->m_systemMode == EnvMode_LandSide )
	{
		if(CMDIChildWnd * pChildFrm = GetNodViewFrame(RUNTIME_CLASS(C3DView)))
		{
			pChildFrm->ShowWindow(SW_HIDE);
			pChildFrm->SendMessage(WM_SYSCOMMAND,SC_CLOSE,0);			
			m_bViewOpen = true;
		}		
		return CreateOrActiveRender3DView(pbAlreadyExist);
	}
	else 
	{
		if( CMDIChildWnd* pNewChildFrm = GetNodViewFrame(RUNTIME_CLASS(CRender3DView)) )
		{
			pNewChildFrm->ShowWindow(SW_HIDE);
			pNewChildFrm->SendMessage(WM_SYSCOMMAND,SC_CLOSE,0);
			m_bViewOpen = true;
			m_bViewOpen = true;
		}		
		return CreateOrActive3DView(pbAlreadyExist);
	}
	return NULL;
}

void CMainFrame::OnOnboardReportPaxActivityReport()
{
	LoadOnboardReport(ONBOARD_PAX_ACTIVITY);
}
void CMainFrame::OnOnboardReportPaxDurationReport()
{
	LoadOnboardReport(ONBOARD_PAX_DURATION);
}
void CMainFrame::OnOnboardReportPaxDistanceTravelReport()
{
	LoadOnboardReport(ONBOARD_PAX_DISTANCETRAVEL);
}
void CMainFrame::OnOnboardReportPaxCarryonReport()
{
	LoadOnboardReport(ONBOARD_PAX_CARRYON);
}
void CMainFrame::OnOnboardReportPaxConflictReport()
{
	LoadOnboardReport(ONBOARD_PAX_CONFLICT);
}
void CMainFrame::OnOnboardReportCarryonReport()
{
	LoadOnboardReport(ONBOARD_CARRYON_TIME);
}



void CMainFrame::LoadOnboardReport( enum OnboardReportType _enumRepType )
{
	// first check if iCurrentSimIdx >= 0
	// if false, then return;
	CMDIChildWnd* pMDIActive = MDIGetActive();
	CTermPlanDoc* pActiveDoc = NULL;
	if(pMDIActive != NULL)
	{
		pActiveDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		ASSERT(pActiveDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	}	
	//////////////////////////////////////////////////////////////////////////
	if (pActiveDoc == NULL)
		return;


	CTermPlanDoc* pDoc = pActiveDoc;
	//reportType type = pDoc->GetARCReportManager().GetOnboardReportManager()->GetReportType() ;
	pDoc->GetARCReportManager().GetOnboardReportManager()->SetReportType(_enumRepType);
	pDoc->GetARCReportManager().GetOnboardReportManager()->SetCBGetLogFilePath(CallbackGetAirsideLogFilePath);
	pDoc->GetARCReportManager().GetOnboardReportManager()->SetCommonData(pDoc->getARCport()->getProjectCommon()) ;

	//get report file directory
	int nCurSimResult = pDoc->GetTerminal().getCurrentSimResult();
	CString strSimResultFolderName;
	strSimResultFolderName.Format(_T("%s%d"),strSimResult,nCurSimResult);
	CString strReportFileDir;
	strReportFileDir.Format(_T("%s\\SimResult\\%s\\report"),pDoc->m_ProjInfo.path,strSimResultFolderName);
	pDoc->GetARCReportManager().GetOnboardReportManager()->SetReportPath(strReportFileDir);

	pDoc->GetARCReportManager().GetOnboardReportManager()->InitUpdate(pDoc->GetInputAirside().GetProjectID(pDoc->m_ProjInfo.name)) ;


	CMDIChildWnd* pChildFrm = CreateOrActivateFrame(theApp.m_pOnboardReportTemplate, RUNTIME_CLASS(OnboardReportControlView));
	if (pChildFrm == NULL)
		return;

	CView* pView = pChildFrm->GetActiveView();
	ASSERT(pView->GetDocument()->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));

	pView->GetParentFrame()->OnUpdateFrameTitle(TRUE);
	pView->GetParentFrame()->ShowWindow(SW_SHOW);
	pView->Invalidate(FALSE);


	CView * pTempView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		pTempView = pDoc->GetNextView(pos);
		if (pTempView->IsKindOf(RUNTIME_CLASS(OnboardReportGraphView)))
		{
			((OnboardReportGraphView *)pTempView)->ResetAllContent();
		}
		else if(pTempView->IsKindOf(RUNTIME_CLASS(OnboardReportListView)))
		{
			((OnboardReportListView *)pTempView)->ResetAllContent();
		}
		else if(pTempView->IsKindOf(RUNTIME_CLASS(OnboardReportControlView)))
		{
			//((CAirsideRepControlView *)pTempView)->bShowLoadButton = bExist;
			((OnboardReportControlView *)pTempView)->ResetAllContent();
			//((CAirsideRepControlView *)pTempView)->Clear();

			OnboardReportChildSplitFrame* pWnd = (OnboardReportChildSplitFrame*)((OnboardReportControlView*)pTempView)->GetParentFrame();
			CRect rc(0, 0, 0, 0);
			pWnd->GetWindowRect(&rc);
			pWnd->m_wndSplitter2.SetRowInfo(0, 350, 236);

			pWnd->ShowWindow(SW_SHOWMAXIMIZED);

		}
	}
}


void CMainFrame::LoadLandsideReport( enum LandsideReportType _enumRepType )
{
	// first check if iCurrentSimIdx >= 0
	// if false, then return;
	CMDIChildWnd* pMDIActive = MDIGetActive();
	CTermPlanDoc* pActiveDoc = NULL;
	if(pMDIActive != NULL)
	{
		pActiveDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		ASSERT(pActiveDoc->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	}	
	//////////////////////////////////////////////////////////////////////////
	if (pActiveDoc == NULL)
		return;


	CTermPlanDoc* pDoc = pActiveDoc;
	//reportType type = pDoc->GetARCReportManager().GetOnboardReportManager()->GetReportType() ;
	pDoc->GetARCReportManager().GetLandsideReportManager()->SetReportType(_enumRepType);
	pDoc->GetARCReportManager().GetLandsideReportManager()->SetCBGetLogFilePath(CallbackGetAirsideLogFilePath);
	pDoc->GetARCReportManager().GetLandsideReportManager()->SetCommonData(pDoc->getARCport()->getProjectCommon()) ;

	//get report file directory
	int nCurSimResult = pDoc->GetTerminal().getCurrentSimResult();
	CString strSimResultFolderName;
	strSimResultFolderName.Format(_T("%s%d"),strSimResult,nCurSimResult);
	CString strReportFileDir;
	strReportFileDir.Format(_T("%s\\SimResult\\%s\\report"),pDoc->m_ProjInfo.path,strSimResultFolderName);
	pDoc->GetARCReportManager().GetLandsideReportManager()->SetReportPath(strReportFileDir);

	pDoc->GetARCReportManager().GetLandsideReportManager()->InitUpdate(pDoc->GetInputAirside().GetProjectID(pDoc->m_ProjInfo.name)) ;


	CMDIChildWnd* pChildFrm = CreateOrActivateFrame(theApp.m_pLandsideReportTemplate, RUNTIME_CLASS(LandsideReportControlView));
	if (pChildFrm == NULL)
		return;

	CView* pView = pChildFrm->GetActiveView();
	ASSERT(pView->GetDocument()->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));

	pView->GetParentFrame()->OnUpdateFrameTitle(TRUE);
	pView->GetParentFrame()->ShowWindow(SW_SHOW);
	pView->Invalidate(FALSE);


	CView * pTempView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		pTempView = pDoc->GetNextView(pos);
		if (pTempView->IsKindOf(RUNTIME_CLASS(LandsideReportGraphView)))
		{
			((LandsideReportGraphView *)pTempView)->ResetAllContent();
		}
		else if(pTempView->IsKindOf(RUNTIME_CLASS(LandsideReportListView)))
		{
			((LandsideReportListView *)pTempView)->ResetAllContent();
		}
		else if(pTempView->IsKindOf(RUNTIME_CLASS(LandsideReportControlView)))
		{
			//((CAirsideRepControlView *)pTempView)->bShowLoadButton = bExist;
			((LandsideReportControlView *)pTempView)->ResetAllContent();
			//((CAirsideRepControlView *)pTempView)->Clear();

			LandsideReportChildSplitFrame* pWnd = (LandsideReportChildSplitFrame*)((LandsideReportControlView*)pTempView)->GetParentFrame();
			CRect rc(0, 0, 0, 0);
			pWnd->GetWindowRect(&rc);
			pWnd->m_wndSplitter2.SetRowInfo(0, 350, 236);

			pWnd->ShowWindow(SW_SHOWMAXIMIZED);

		}
	}
}






void CMainFrame::OnLandsideVehicleActivity()
{
	// TODO: Add your command handler code here

	LoadLandsideReport(LANDSIDE_VEHICLE_ACTIVITY);
}

void CMainFrame::OnLandsideVehicleDelay()
{
	LoadLandsideReport(LANDSIDE_VEHICLE_DELAY);
}

void CMainFrame::OnLandsideVehicleTotalDelay()
{
	LoadLandsideReport(LANDSIDE_VEHICLE_TOTALDELAY);
}

void CMainFrame::OnLandsideResourceVehicleQueueReport()
{
	// TODO: Add your command handler code here
	LoadLandsideReport(LANDSIDE_RESOURCE_QUEUELEN);
}

void CMainFrame::OnLandsideResourceVehicleThroughputReport()
{
	LoadLandsideReport(LANDSIDE_RESOURCE_THROUGHPUT);
}



