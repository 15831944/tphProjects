// AirsideMSView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include <Inputs/InputsDll.h>
#include <MFCExControl/ARCBaseTree.h>
#include "AirsideMSView.h"

#include <Common/WinMsg.h>
#include <common/AirportDatabase.h>
#include <common/AirlineManager.h>
#include <Inputs/WallShape.h>
#include <Inputs/WallShapeList.h>
#include <InputAirside/ARCUnitManagerOwnerEnum.h>
#include <InputAirside/Contour.h>

#include <AirsideGUI/NodeViewDbClickHandler.h>

#include "ViewMsg.h"

#include "Airside3D.h"
#include "Topograph3D.h"

#include "AirsideLevelList.h"

#include "FlightScheduleDlg.h"
#include "AirsideGroundPropDlg.h"
#include "AirsideRunwayDlg.h"
#include "AirsideTaxiwayDlg.h"
#include "AirsideGateDlg.h"
#include "AirsideDeicePadDlg.h"
#include "AirsideWaypointDlg.h"
#include "AirsideSectorDlg.h"
#include "AirsideContourDlg.h"
#include "AirsideHoldDlg.h"
#include "AirsideSurfaceDlg.h"
#include "AirsideStructureDlg.h"
#include "SimEngSettingDlg.h"
#include "AircraftDispPropDlg.h"
#include "CalloutDispSpecDlg.h"
#include "GateAssignDlg.h"
#include "VehicleDispPropDlg.h"

#include "FlightDialog.h"

#include "DlgEditARP.h"
#include "DlgFlightPlan.h"
#include "DlgAirsideObjectDisplayProperties.h"
#include "DlgAircraftTags.h"
#include "DlgArrivalDelayTriggers.h"
#include "DlgProbDist.h"
#include "DlgFlightDB.h"
#include "DlgDBAirports.h"
#include "DlgDBAirline.h" 
#include "DlgDBAircraftTypes.h"
#include "DlgGateAssignConstraints.h"
#include "DlgVehicleTags.h"
#include "DlgWallShapeProp.h"
#include "DlgAircraftAlias.h"
#include "DlgBaggageCartParking.h"
#include "DlgPaxBusParking.h"
#include "DlgStandAssignment.h"
//#include "GateAssignDlg.h"
#include "DlgFloorPictureReference.h"
#include "DlgHoldShortLines.h"
#include "DlgWaveCrossRunwaySpecification.h"
#include "DlgAirsideObjectDisplayProperties.h"
#include "DlgFollowMeConnections.h"
#include "DlgNewFlightType.h"
#include "DlgScheduleAndRostContent.h"
#include "AirsideGUI/NodeViewDbClickHandler.h"

// CAirsideMSView

DECLARE_NEWFLIGHTTYPE_SELECT_CALLBACK()

#include "MainFrm.h"
#include "3DView.h"
#include "TermPlanDoc.h"
#include "ChildFrm.h"
#include "boost/array.hpp"

using namespace MSV;
IMPLEMENT_DYNCREATE(CAirsideMSView, CView)
#define IDC_AIRSIDE_TREE 1
LPCSTR strType[] = {_T("Offset from centreline"),_T("Angle at offset line"),_T("Distance")};
CAirsideMSView::CAirsideMSView()
: m_pAirsideNodeHandler(NULL),use_Marker(FALSE),use_Marker_line(FALSE)
{
	m_pDlgScheduleRosterContent = NULL;
	m_hRightClkItem = NULL;
	m_nProjID = -1;
	m_nType = -1;
	mpNodeDataAlloc = new MSV::CNodeDataAllocator();
}

CAirsideMSView::~CAirsideMSView()
{
	if (NULL != m_pAirsideNodeHandler)
	{
		delete m_pAirsideNodeHandler;
	}

	if (m_pDlgScheduleRosterContent)
	{
		delete m_pDlgScheduleRosterContent;
		m_pDlgScheduleRosterContent = NULL;
	}
	delete mpNodeDataAlloc;
}

BEGIN_MESSAGE_MAP(CAirsideMSView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_CONTEXTMENU()
//	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
//	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndLabelEdit)
//	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
//	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRClick)
	ON_MESSAGE(ITEMEXPANDING,OnItemExpanding)

	ON_COMMAND_RANGE(ID_MENUITEM_OBJECTROOT+1, ID_MENUITEM_OBJECTROOT + 32, OnNewAirsideObject)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
//	ON_WM_LBUTTONDBLCLK()


	ON_COMMAND(ID_MENUITEM_LOCK_ALTOBJECT,OnLockALTObject)
	ON_COMMAND(ID_MENUITEM_UNLOCK_ALTOBJECT,OnUnlockALTObject)
    ON_COMMAND(ID_MENUITEM_NOTOBJECTITEM + 10,OnStretchOption)
	ON_COMMAND(ID_AIRPORT_NEWLEVELABOVEARP, OnNewLevelAboveARP)
	ON_COMMAND(ID_AIRPORT_NEWLEVELBELOWARP, OnNewLevelBelowARP)
	ON_COMMAND(ID_CTX_DELETEFLOOR_ASLEVEL, OnDeleteAirsideLevel)
	ON_COMMAND(ID_CTX_RENAME_ASLEVEL, OnRenameAirsideLevel)
	ON_COMMAND(ID_CTX_LATLONG, OnCtxIputLatlong)
	ON_COMMAND(ID_CTX_HIDE, OnHideControl)
	ON_COMMAND(ID_CTX_FLOORPROPERTIES_ASLEVEL, OnFloorProperties)

	ON_COMMAND(ID_CTX_FLOORPICTURE_INSERT, OnFloorPictureInsert)
	ON_COMMAND(ID_CTX_FLOORPICTURE_HIDE, OnFloorPictureHide)
	ON_COMMAND(ID_CTX_FLOORPICTURE_SHOW, OnFloorPictureShow)
	ON_COMMAND(ID_CTX_FLOORPICTURE_REMOVE,OnFloorPictureRemove)
	ON_COMMAND(ID_CTX_FLOORPICTURE_SAVE,OnFloorPictureSave)
	ON_COMMAND(ID_CTX_FLOORPICTURE_LOAD,OnFloorPictureLoad)

	ON_COMMAND(ID_AIRSIDEOBJECT_OBJECTPROPERTIES, OnObjectProperties)
	ON_UPDATE_COMMAND_UI(ID_AIRSIDEOBJECT_DELETE,OnUpdateObjectDelete)
	ON_COMMAND(ID_AIRSIDEOBJECT_DELETE, OnObjectDelete)
	ON_COMMAND(ID_AIRPORTS_ADDAIRPORT, OnNewAirport)
	ON_COMMAND(ID_AIRPORT_DELETE, OnDeleteAirport)
	ON_COMMAND(ID_AIRPORT_RENAME, OnRenameAirport)
	ON_COMMAND(ID_VEHICLE_ROUTES_SPECIFICATION,OnSetVehicleRoutes)
	ON_COMMAND(ID_AIR_ROUTES_DEFINE,OnDefineAirRoutes)
	ON_COMMAND(ID_FLIGHT_SCHEDULE,OnFlightSchedule)
	ON_COMMAND(ID_ITINERANT_TRAFFIC,OnSetItinerantTraffic)
	ON_COMMAND(ID_AIRPORT_AND_SECTORS_DB,OnEditAirportSectorsDB)
	ON_COMMAND(ID_FLIGHT_GROUPS_DB,OnEditFlightGroupsDB)
	ON_COMMAND(ID_AIRLINE_GROUP_DB,OnEditAirlineGroupDB)
	ON_COMMAND(ID_FLITYPE_CATEGORY,OnEditAircraftTypeAndCategory)
	ON_COMMAND(ID_FLITYPE_DIMENSION,OnEditFliTypeDimension)
	ON_COMMAND(ID_WINGSPAN_CATEGORY,OnEditWingspanCategory)
	ON_COMMAND(ID_SURFACE_BEARING_CATEGORY,OnEditSurfaceBearingCategory)
	ON_COMMAND(ID_WAKE_VORTEX_CATEGORY,OnEditWakeVortexCategory)
	ON_COMMAND(ID_APPRORACH_SPEED_CATEGORY,OnEditApproachSpeedCategory)
	ON_COMMAND(ID_CRUISE_SPE,OnEditCruise)
	ON_COMMAND(ID_TERMINAL_MANEUVER,OnEditTerminalManeuver)
	ON_COMMAND(ID_APPROACH_TO_LAND,OnEditApproachToLand)
	ON_COMMAND(ID_LANDING_PERFORMANCE,OnEditLandingPerformance)
	ON_COMMAND(ID_TAXI_INBOUND,OnEditTaxiInbound)
	ON_COMMAND(ID_PUSHBACK_PERFORMANCE,OnEditPushBackPerformance)
	ON_COMMAND(ID_STANDSERVICE_PERFORMANCE,OnEditStandServicePerformance)
	ON_COMMAND(ID_TAXIOUTBOUND_PERFORMANCE,OnEditTaxiOutboundPerformance)
	ON_COMMAND(ID_TAKEOFF_PERFORMANCE,OnEditTakeoffPerformance)
	ON_COMMAND(ID_DEPARTURE_CLIMB_OUT,OnEditDepartureClimbOut)
	ON_COMMAND(ID_FLIGHT_PLAN,OnEditFlightPlan)
	ON_COMMAND(ID_DEPARTURE_SLOT,OnEditDepartureSlot)
	ON_COMMAND(ID_VEHICLE_SPE,OnEditVehicle)
	ON_COMMAND(ID_FLITYPE_RELATIVE_SERVICE_LOCATION,OnEditFliTypeLocation)
	ON_COMMAND(ID_SERVICE_REQUIREMENT,OnEditServiceRequirement)
	ON_COMMAND(ID_WEATHER_IMPACT,OnEditWeatherImpact)
	ON_COMMAND(ID_IN_TRAIL,OnEditInTrail)
	ON_COMMAND(ID_DIRECT_ROUTING,OnEditDirectRouting)
	ON_COMMAND(ID_APPROACH_SEPARATION,OnEditApproachSeparation)
	ON_COMMAND(ID_CONTROLLER_INTERVENTION,OnControllerIntervention)
	ON_COMMAND(ID_SIDE_STEP,OnEditSideStep)
	ON_COMMAND(ID_GO_AROUND,OnEditGoAround)
	ON_COMMAND(ID_STAR_ASSIGNMENT,OnEditSTARAssignment)
	ON_COMMAND(ID_LANDING_RUNWAY_ASSIGNMENT,OnEditLandingRunwayAssignment)
	ON_COMMAND(ID_LANDING_RUNWAY_EXIT_STRATEGY,OnEditLandingRunwayExitStrategy)
	ON_COMMAND(ID_TAXIWAY_WEIGHT,OnEditTaxiwayWeight)
	ON_COMMAND(ID_TAXIWAY_WINGSPAN_CONSTRAINT,OnEditTaxiwayWingspanConstraint)
	ON_COMMAND(ID_TAXIWAY_FLITYPE_RESTRICTION,OnEditTaxiwayFliTypeRestriction)
	ON_COMMAND(ID_DIRECTIONALITY_CONSTRAINT,OnEditDirectionalityConstraint)
	ON_COMMAND(ID_TAXISPEED_CONSTRAINT,OnEditTaxiSpeedConstraint)
	ON_COMMAND(ID_INBOUND_ROUTE_ASSIGNMENT,OnEditInboundRouteAssignment)
	ON_COMMAND(ID_OUTBOUND_ROUTE_ASSIGNMENT,OnEditOutboundRouteAssignment)
	ON_COMMAND(ID_TEMPORARY_PARKING,OnEditTemporaryParking)
	ON_COMMAND(ID_CONFLICT_RESOLUTION,OnEditConflictResolution)
	ON_COMMAND(ID_PARKING_STAND_BUFFER,OnEditParkingStandBuffer)
	ON_COMMAND(ID_STAND_ASSIGNMENT,OnEditStandAssignment)
	ON_COMMAND(ID_OCCUPY_STAND_ACTION,OnOccupyStandAction)
	ON_COMMAND(ID_TAXIOUT_PUSHBACK,OnTaxiOutPushBack)
	ON_COMMAND(ID_TAKEOFF_RUNWAY_ASSIGNMENT,OnEditTakeoffRunwayAssignment)
	ON_COMMAND(ID_SID_ASSIGNMENT,OnEditSIDAssignment)
	ON_COMMAND(ID_TAKEOFF_QUEUE,OnEditTakeoffQueue)
	ON_COMMAND(ID_RUNWAY_TAKEOFF_POSITION,OnEditRunwayTakeoffPosition)
	ON_COMMAND(ID_TAKEOFF_SEQUENCE,OnEditTakeoffSequence)
	ON_COMMAND(ID_TAKEOFF_CLEARANCE,OnEditTakeoffClearance)
	ON_COMMAND(ID_ARRIVAL_DELAY_TRIGGER,OnEditArrivalDelayTrigger)
	ON_COMMAND(ID_VEHICLE_POOLS,OnEditVehiclePools)
	ON_COMMAND(ID_SIMULATION_ENGINE_SETTING,OnSetSimulationEngine)
	ON_COMMAND(ID_RUN_SIMULATION,OnRunSimulation)
	ON_COMMAND(ID_AIRCRAFT_DISPLAY,OnDisplayAircraft)
	ON_COMMAND(ID_VEHICLE_DISPLAY,OnDisplayVehicle)
	ON_COMMAND(ID_AIRCRAFT_TAGS,OnSetAircraftTags)
	ON_COMMAND(ID_VEHICLE_TAGS,OnSetVehicleTags)
	ON_COMMAND(ID_CALLOUT_DISP_SPEC,OnCalloutDispSpec)
	ON_COMMAND(ID_TRAINING_FLIGHTSMANAGEMENT,OnEditTrainFlightsManagement)
	ON_COMMAND(ID_AIRROUTE_EDIT,OnEidtAirRoute)
	ON_COMMAND(ID_AIRROUTE_DELETE,OnDeleteAirRoute)
	ON_COMMAND(ID_AIRROUTE_COPY,OnCopyAirRoute)
	ON_COMMAND(ID_AIRROUTE_DISPLAY,OnDisplayAirRouteProperties)
	ON_COMMAND(ID_ALLAIRROUTE_DISPLAY,OnDisplayAllAirRouteProperties)
	ON_COMMAND(ID_SECTORFLIGHTMIXSPECIFICATION,OnEditSectorFlightMixSpec)
	ON_NOTIFY_REFLECT(NM_THEMECHANGED, OnNMThemeChanged)

	ON_COMMAND(ID_ADDCONTOUR_EDITCONTROLPOINT,OnEditControlPoint)
	ON_UPDATE_COMMAND_UI(ID_ADDCONTOUR_EDITCONTROLPOINT,OnUpdateEditControlPoint)
	ON_COMMAND(ID_DISABLE_EIDTPOINT,OnDisableControlPoint)
	ON_UPDATE_COMMAND_UI(ID_DISABLE_EIDTPOINT,OnUpdateDisableControlPoint)
	ON_UPDATE_COMMAND_UI(ID_CTX_FLOORVISIBLE_ASLEVEL, OnUpdateFloorVisible)
	ON_UPDATE_COMMAND_UI(ID_CTX_GRIDVISIBLE_ASLEVEL, OnUpdateGridVisible)
	ON_UPDATE_COMMAND_UI(ID_CTX_HIDE, OnUpdateHideControl)
	ON_UPDATE_COMMAND_UI(ID_CTX_MONOCHROME_ASLEVEL, OnUpdateFloorMonochrome)
	ON_UPDATE_COMMAND_UI(ID_CTX_SHOWHIDEMAP_ASLEVEL, OnUpdateShowHideMap)
	ON_UPDATE_COMMAND_UI(ID_CTX_OPAQUEFLOOR_ASLEVEL, OnUpdateOpaqueFloor)
	ON_UPDATE_COMMAND_UI(ID_CTX_DELETEFLOOR_ASLEVEL, OnUpdateFloorsDelete)

	ON_UPDATE_COMMAND_UI(ID_CTX_PLACEMARKER_ASLEVEL, OnUpdatePlaceMarker)
	ON_UPDATE_COMMAND_UI(ID_CTX_PLACELINE_ASLEVEL, OnUpdatePlaceMarkerLine)
////

   ON_COMMAND(ID_CTX_PLACEMARKER_ASLEVEL,OnsetMarker) 

   ON_COMMAND(ID_CTX_PLACELINE_ASLEVEL,OnsetAlignLine) 

   ON_BN_CLICKED(ID_CTX_PLACELINE_ASLEVEL,OnsetAlignLine)
   ON_MESSAGE(TP_DATA_BACK, SetActiveFloorMarkerLine)
 
   ON_UPDATE_COMMAND_UI(ID_CTX_ALIGNFLOORS_ASLEVEL,OnUpdateAlignMarker)
  ON_COMMAND(ID_CTX_ALIGNFLOORS_ASLEVEL,OnAlignMarker)
   ON_UPDATE_COMMAND_UI(ID_CTX_REMOVELINE_ASLEVEL,OnUpdateRemoveAlignLine)
   ON_COMMAND(ID_CTX_REMOVELINE_ASLEVEL,OnRemoveAlignLine)
   ON_UPDATE_COMMAND_UI(ID_CTX_REMOVEMARKER_ASLEVEL,OnUpdateRemoveMarker)
   ON_COMMAND(ID_CTX_REMOVEMARKER_ASLEVEL,OnRemoveMarker) 
///////
	//ON_UPDATE_COMMAND_UI(ID_CTX_FLOORTHICKNESS_VALUE, OnUpdateThicknessValue)
	//ON_UPDATE_COMMAND_UI(ID_CTX_VISIBLEREGIONS, OnUpdateVisibleRegins)
	//ON_UPDATE_COMMAND_UI(ID_CTX_INVISIBLEREGIONS, OnUpdateInvisibleRegins)

	ON_UPDATE_COMMAND_UI(ID_CTX_FLOORPICTURE_HIDE, OnUpdateFloorPictureHide)
	ON_UPDATE_COMMAND_UI(ID_CTX_FLOORPICTURE_SHOW, OnUpdateFloorPictureShow)
	ON_UPDATE_COMMAND_UI(ID_CTX_FLOORPICTURE_REMOVE,OnUpdateFloorPictureRemove)

   
	ON_COMMAND(ID_CTX_GRIDVISIBLE_ASLEVEL, OnGridVisible)
	ON_COMMAND(ID_CTX_MONOCHROME_ASLEVEL, OnFloorMonochrome)
	ON_COMMAND(ID_CTX_FLOORVISIBLE_ASLEVEL, OnFloorVisible)
	ON_COMMAND(ID_CTX_SHOWHIDEMAP_ASLEVEL, OnShowHideMap)

	ON_COMMAND(ID_HOLD_SHORT_LINES,OnHoldShortLines)
	ON_COMMAND(ID_SURFACE_ADD,OnAddSurface)
	ON_COMMAND(ID_SURFACE_DEL,OnRemoveSurface)
	ON_COMMAND(ID_SURFACE_SHOW,OnShowSurface)
	ON_COMMAND(ID_SURFACE_HIDE,OnHideSurface)

	ON_UPDATE_COMMAND_UI(ID_SURFACE_HIDE,OnUpdateHideSurface)
	ON_UPDATE_COMMAND_UI(ID_SURFACE_SHOW,OnUpdateShowSurface)

	ON_COMMAND(ID_MENUITEM_OBJECTROOT + 30,OnReportingAreaDisplayProperty)
	ON_UPDATE_COMMAND_UI(ID_MENUITEM_OBJECTROOT + 7,OnUpdateAddContour)


END_MESSAGE_MAP() 


// CAirsideMSView diagnostics

#ifdef _DEBUG
void CAirsideMSView::AssertValid() const
{
	CView::AssertValid();
}

void CAirsideMSView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CAirsideMSView message handlers

void MSV::CAirsideMSView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	//GetTreeCtrl().InsertItem("Airside Node");
	if (m_imgList.m_hImageList == NULL)
	{
		m_imgList.Create(16,16,ILC_COLOR8|ILC_MASK,0,1);
		//CBitmap bmp;
		//bmp.LoadBitmap(IDB_COOLTREE);
		//m_imgList.Add(&bmp,RGB(255,0,255));
	}
	CBitmap bmL;

	bmL.LoadBitmap(IDB_SMALLNODEIMAGES);
	int nIndex = m_imgList.Add(&bmL, RGB(255,0,255));
	bmL.DeleteObject();
	bmL.LoadBitmap(IDB_NODESMALLIMG);
	nIndex = m_imgList.Add(&bmL, RGB(255,0,255));
	bmL.DeleteObject();
	bmL.LoadBitmap(IDB_FLOORSMALLIMG);
	nIndex = m_imgList.Add(&bmL, RGB(255,0,255));
	bmL.DeleteObject();
	bmL.LoadBitmap(IDB_AREASSMALLIMG);
	nIndex = m_imgList.Add(&bmL, RGB(255,0,255));
	bmL.DeleteObject();
	bmL.LoadBitmap(IDB_PORTALSSMALLIMG);
	nIndex = m_imgList.Add(&bmL, RGB(255,0,255));

	GetTreeCtrl().SetImageList(&m_imgList,TVSIL_NORMAL);
	GetTreeCtrl().SetImageList(&m_imgList,TVSIL_STATE);
	
	InitTree();
	SetScrollPos(SB_VERT,0);
	
}
/*
Layout				
	Airports	

	Airspace				
		Way points			
		Holds			
		Air routes 			
		Airspace sectors 	
	Topography		
		Flat surface areas	
		Contoured surface areas	
		Structures	
		Walls	


Facilities requirement generators			
	Flights		
		Schedule 	
		Itinerant Flight Generator 	
		Flight type specification
			Airport/Sectors
			Airline/Groups
			Aircraft/categories
			Dimensions
		Aircraft classifications	
			Wingspan based
			Surface bearing based
			Wake vortex weight based
			Approach speed based
		Performance 	
			Cruise
			Terminal
			Approach to land
			Landing
			Taxi inbound
			Push Back
			Taxi Outbound
			Takeoff
			Departure / climbout
		Flight Plans 	
		Departure Slots
		Noise data
			Approach and landing
			Takeoff and climbout
			Cruise
	Vehicles (GSE		
		Vehicle Data Specification	
		Service locations specification
		Servicing requirements	
	Weather Impacts
	
Resource Management			
	Airspace		
		Sector Flight Mix Limitation 	
		In trail separations 	
		Controller interventions (airspace)	
		Vectoring criteria 	
		Direct routing criteria 	
		Controller workload
	Approaches	
		Approach Separation 
		Controller interventions (approach)
		Side Step criteria  
		Go Around Criteria 
		Controller workload
	Landings	
		STAR Assignment
		Landing runway assignment 
		Landing runway exit strategies 
		Controller workload
	Taxiways	
	 	Weight constraints  
		Wingspan constraints 
		Fight Type restrictions 
		Directionality Constraints 
		Inbound route assignment 
		Outbound route assignment 
		Temporary parking criteria 	
		Towing routes 	
		Conflict resolution criteria (taxiing
		Controller workload
	Parking stands (gates)		
		Buffers
		Stand assignment
			Airport 1
			Airport 2
			Airport N
		Occupied assigned stand action criteria
		Push back (taxi out) clearance criteria
		Controller workload
	Deicing pads		
		Deicing queue specification
		Deicing assignment specification
		Deicing routes Specification	
		Controller workload
	Departures		
		Take off runway assignment
		SID assignment
		Take off queues 
		Runway take off positions
		Take off sequencing
		Clearance criteria 
		Arrival Delay triggers 
		Controller workload
	Vehicles (GSE) Pools
Simulation		
	Initial conditions
	Weather script	
	Settings	
	Run	
Analysis Parameters	
	Aircraft Display	
	Vehicle Display	
	Tags
	Processor Information
	Auto Reports
*/

void CAirsideMSView::InitTree()
{
	CString strNode;
	HTREEITEM hTreeItem = NULL;
	CNodeData *pNodeData = NULL;
	int nProjID = -1;


	HTREEITEM hDefaultSelected;
	//----------------------------------------------------------------------------
	//Layout
	{
		strNode.LoadString(IDS_TVNN_LAYOUT);
		pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
		mpNodeDataAlloc->allocateNodeData(NodeType_Normal);

		HTREEITEM hLayout =  AddItem(strNode,TVI_ROOT,TVI_LAST,ID_NODEIMG_DEFAULT,ID_NODEIMG_DEFAULT);
		hDefaultSelected = hLayout;
		GetTreeCtrl().SetItemData(hLayout,(DWORD_PTR)pNodeData);


		{
			//-----------------------------------------------------------------------
			//Airports
			{
				strNode.LoadString(IDS_TVNN_AIRPORTS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nIDResource = IDR_MENU_AIRSIDEAIRPORTS;
				HTREEITEM hAirports =  AddItem(strNode,hLayout);
				m_hItemAirports = hAirports;
				GetTreeCtrl().SetItemData(hAirports,(DWORD_PTR)pNodeData);


				std::vector<ALTAirport> vAirport;
				try
				{
					nProjID = ALTAirport::GetProjectIDByName(GetARCDocument()->m_ProjInfo.name);
					ALTAirport::GetAirportList(nProjID,vAirport);
					//ALTAirport airport;
					//vAirport.push_back(airport);
					//return;
					m_nProjID = nProjID;
				}
				catch (CADOException &e)
				{
					e.ErrorMessage();
					MessageBox(_T("Cann't read the airport information."));
					return;
				}

				for (int i = 0; i < static_cast<int>(vAirport.size());++i)
				{	
					AddAirportToTree(hAirports,vAirport[i]);
				}


				GetTreeCtrl().Expand(hAirports,TVE_EXPAND);
			}


			//-----------------------------------------------------------------------
			//Airspace
			{
				int  nAirspaceID = nProjID;

				strNode.LoadString(IDS_TVNN_AIRSPACE);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				HTREEITEM hAirspace =  AddItem(strNode,hLayout);
				m_hItemAirspace = hAirspace;
				GetTreeCtrl().SetItemData(hAirspace,(DWORD_PTR)pNodeData);


				//Waypoints
				strNode.LoadString(IDS_TVNN_WAYPOINTS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_WAYPOINT;
				pNodeData->bHasExpand = false;
				pNodeData->nOtherData = nAirspaceID;
				HTREEITEM hAirFix =  AddItem(strNode,hAirspace);
				GetTreeCtrl().SetItemData(hAirFix,(DWORD_PTR)pNodeData);	
				AddItem("Loading...",hAirFix);
				GetTreeCtrl().Expand(hAirFix,TVE_EXPAND);
				m_hWaypoint = hAirFix;

				//Holds
				strNode.LoadString(IDS_TVNN_HOLDS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_HOLD;
				pNodeData->bHasExpand = false;
				pNodeData->nOtherData = nAirspaceID;
				HTREEITEM hAirHold= AddItem(strNode,hAirspace);
				GetTreeCtrl().SetItemData(hAirHold,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hAirHold);
				GetTreeCtrl().Expand(hAirHold,TVE_EXPAND);

				//Air routes
				strNode.LoadString(IDS_TVNN_AIRROUTS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_Dlg;
				pNodeData->nSubType = Dlg_AirRoute;
				pNodeData->nIDResource = IDR_MENU_AIRROUTES;
				pNodeData->nOtherData = nAirspaceID;
				pNodeData->dwData = nAirspaceID;
				pNodeData->bHasExpand = false;
				HTREEITEM hAirRoute= AddItem(strNode,hAirspace);
				GetTreeCtrl().SetItemData(hAirRoute,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hAirRoute);
				GetTreeCtrl().Expand(hAirRoute,TVE_EXPAND);
// 				AirRouteList m_airRouteList;
// 				m_airRouteList.ReadData(pNodeData->dwData);
// 				int nAirRouteCount = m_airRouteList.GetAirRouteCount();
// 
// 				for (int i =0; i < nAirRouteCount; i++)
// 				{
// 					CAirRoute *pAirRoute = m_airRouteList.GetAirRoute(i);
// 
// 					ASSERT(pAirRoute != NULL);
// 					HTREEITEM hItem = AddItem(pAirRoute->getName(),hAirRoute);
// 					GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)pAirRoute);
// 				}
			

				//Airspace sectors
				strNode.LoadString(IDS_TVNN_AIRPLACESECTORS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_SECTOR;
				pNodeData->nOtherData = nAirspaceID;
				pNodeData->bHasExpand = false;
				HTREEITEM hAirSector= AddItem(strNode,hAirspace);
				GetTreeCtrl().SetItemData(hAirSector,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hAirSector);
				GetTreeCtrl().Expand(hAirSector,TVE_EXPAND);
				
				GetTreeCtrl().Expand(hAirspace,TVE_EXPAND);
			}
			

			//-----------------------------------------------------------------------
			//Topography
			{
				int nTopographyID = nProjID;

				strNode.LoadString(IDS_TVNN_TOPOGRAPHY);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				HTREEITEM hTopography =  AddItem(strNode,hLayout);
				m_hItemTopography = hTopography;
				GetTreeCtrl().SetItemData(hTopography,(DWORD_PTR)pNodeData);

			
				//Flat surface areas
				strNode.LoadString(IDS_TVNN_FLATSURFACEAREAS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				//pNodeData->nIDResource = IDR_CTX_SURFACEAREAS;
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_SURFACE;
				pNodeData->nOtherData = nTopographyID;
				pNodeData->bHasExpand = false;
				HTREEITEM hSurface= AddItem(strNode,hTopography);
				GetTreeCtrl().SetItemData(hSurface,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hSurface);
				GetTreeCtrl().Expand(hSurface,TVE_EXPAND);

				//Contoured surface areas
				strNode.LoadString(IDS_TVNN_CONTOUREDSURFACEAREAS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_CONTOUR;
				pNodeData->nOtherData = nTopographyID;
				pNodeData->bHasExpand = false;
				HTREEITEM hContour= AddItem(strNode,hTopography);
				GetTreeCtrl().SetItemData(hContour,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hContour);
				GetTreeCtrl().Expand(hContour,TVE_EXPAND);

				//Structures
				strNode.LoadString(IDS_TVNN_STRUCTURES);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				//pNodeData->nIDResource = IDR_CTX_STRUCTURES;
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_STRUCTURE;
				pNodeData->nOtherData = nTopographyID;
				pNodeData->bHasExpand = false;
				HTREEITEM hStructure= AddItem(strNode,hTopography);
				GetTreeCtrl().SetItemData(hStructure,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hStructure);
				GetTreeCtrl().Expand(hStructure,TVE_EXPAND);

				//Walls
				strNode.LoadString(IDS_TVNN_WALLS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nIDResource = IDR_CTX_WALLSHAPES;
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_WALLSHAPE;
				pNodeData->nOtherData = nTopographyID;
				pNodeData->bHasExpand = false;
				HTREEITEM hWallShape= AddItem(strNode,hTopography);
				GetTreeCtrl().SetItemData(hWallShape,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hWallShape);
				GetTreeCtrl().Expand(hWallShape,TVE_EXPAND);

				//Surface
				strNode.LoadString(IDS_TVNN_SURFACE);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->bHasExpand = false;
				HTREEITEM hObjSurface = AddItem(strNode,hTopography);
				m_hObjSurface = hObjSurface;
				GetTreeCtrl().SetItemData(hObjSurface,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hObjSurface);
				GetTreeCtrl().Expand(hObjSurface,TVE_EXPAND);

				//Reporting Area
				strNode.LoadString(IDS_TVNN_REPORTINGAREA);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_REPORTINGAREA;
				pNodeData->nOtherData = nTopographyID;
				pNodeData->bHasExpand = false;
				HTREEITEM hObjArea = AddItem(strNode,hTopography);
				GetTreeCtrl().SetItemData(hObjArea,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hObjArea);
				GetTreeCtrl().Expand(hObjArea,TVE_EXPAND);
				
				GetTreeCtrl().Expand(hTopography,TVE_EXPAND);
			}
			
		}
		GetTreeCtrl().Expand(hLayout,TVE_EXPAND);
	}

	//----------------------------------------------------------------------------
	//add extend data
	{
		strNode.LoadString(IDS_TVNN_DATA);
		pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
		HTREEITEM hExtelData = AddItem(strNode,TVI_ROOT);
		GetTreeCtrl().SetItemData(hExtelData,(DWORD_PTR)pNodeData);

		//real time radar
		strNode.LoadString(IDS_TVNN_RADAR);
		pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
		HTREEITEM hRadarNode = AddItem(strNode,hExtelData);
		GetTreeCtrl().SetItemData(hRadarNode,(DWORD_PTR)pNodeData);

		//real time noise
		strNode.LoadString(IDS_TVNN_NOISE);
		pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
		HTREEITEM hNoiseNode = AddItem(strNode,hExtelData);
		GetTreeCtrl().SetItemData(hNoiseNode,(DWORD_PTR)pNodeData);
		
		//aodb
		strNode.LoadString(IDS_TVNN_AODB);
		pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
		HTREEITEM hAodbNode = AddItem(strNode,hExtelData);
		GetTreeCtrl().SetItemData(hAodbNode,(DWORD_PTR)pNodeData);
	
		//auto transfer
		strNode.LoadString(IDS_TVNN_AUTO);
		pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
		HTREEITEM hAutoNode = AddItem(strNode,hAodbNode);
		GetTreeCtrl().SetItemData(hAutoNode,(DWORD_PTR)pNodeData);
		
		//manual transfer
		strNode.LoadString(IDS_TVNN_MANUAL);
		pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
		pNodeData->nSubType = Dlg_ManualData;
		HTREEITEM hManualNode = AddItem(strNode,hAodbNode);
		GetTreeCtrl().SetItemData(hManualNode,(DWORD_PTR)pNodeData);
	}

	//Facilities requirement generators
	{
		strNode.LoadString(LDS_TVNN_FACILITIES);
		pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
		HTREEITEM hFacilities = AddItem(strNode, TVI_ROOT);
		GetTreeCtrl().SetItemData(hFacilities,(DWORD_PTR)pNodeData);

		{
			//-----------------------------------------------------------------------
			//Flight
			{
				strNode.LoadString(IDS_TVNN_FLIGHTS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				HTREEITEM hFlights =  AddItem(strNode,hFacilities);
				GetTreeCtrl().SetItemData(hFlights,(DWORD_PTR)pNodeData);
			
				//Schedule 
				strNode.LoadString(IDS_TVNN_SCHEDULE);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_FlightSchedule;
				pNodeData->nIDResource = IDR_MENU_FLIGHT_SCHEDULE;
				pNodeData->bHasExpand = false;
				HTREEITEM hSchedule =  AddItem(strNode,hFlights);
				GetTreeCtrl().SetItemData(hSchedule,(DWORD_PTR)pNodeData);


				//Itinerant Flight Generator
				strNode.LoadString(IDS_TVNN_ITINEANTFLIGHTSGENERATOR);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_ItinerantTraffic;
				pNodeData->nIDResource = IDR_MENU_ITINERANT_TRAFFIC_SPECIFICATTION ;
				pNodeData->bHasExpand = false;
				HTREEITEM hItinerantflight =  AddItem(strNode,hFlights);
				GetTreeCtrl().SetItemData(hItinerantflight,(DWORD_PTR)pNodeData);

				//Training flight specification
				strNode = _T("Training Flight Specification");
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_TrainingFlightSpec;
				pNodeData->bHasExpand = false;
				HTREEITEM hTrainFlgihtItem =  AddItem(strNode,hFlights);
				GetTreeCtrl().SetItemData(hTrainFlgihtItem,(DWORD_PTR)pNodeData);
				//-------------------------------------------------------------------
				//Flight type specification
				{
					strNode.LoadString(IDS_TVNN_FLIGHTTYPESPECIFICATION);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
					HTREEITEM hFltTypeSpecification =  AddItem(strNode,hFlights);
					GetTreeCtrl().SetItemData(hFltTypeSpecification,(DWORD_PTR)pNodeData);
 
				
					//Flight/Groups
					strNode.LoadString(IDS_TVNN_FLIGHT_GROUPS);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_DatabaseFlight;
					pNodeData->nIDResource = IDR_MENU_FLIGHT_GROUPS;
					pNodeData->bHasExpand = false;
					hTreeItem = AddItem(strNode,hFltTypeSpecification);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

					//Airport/Sectors
					strNode.LoadString(IDS_TVNN_AIRPORTSECTORS);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_DatabaseAirportSector;
					pNodeData->nIDResource = IDR_MENU_AIRPORT_AND_SECTORS;
					pNodeData->bHasExpand = false;
					hTreeItem =  AddItem(strNode,hFltTypeSpecification);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

					//Airline/Groups
					strNode.LoadString(IDS_TVNN_AIRLINEGROUPS);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_DatabaseAirline;
					pNodeData->nIDResource = IDR_MENU_AIRLINE_GROUP;
					pNodeData->bHasExpand = false;
					hTreeItem =  AddItem(strNode,hFltTypeSpecification);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

					//Aircraft/categories
					strNode.LoadString(IDS_TVNN_AIRCRAFTCATEGORIES);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_DatabaseAircraft;
					pNodeData->nIDResource = IDR_MENU_FLITYPE_CATEGORY;
					pNodeData->bHasExpand = false;
					hTreeItem =  AddItem(strNode,hFltTypeSpecification);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

					//Dimensions
					strNode.LoadString(IDS_TVNN_DIMENSIONS);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_FlightTypeDimensions;
					pNodeData->nIDResource = IDR_MENU_FLITYPE_DIMENSION;
					pNodeData->bHasExpand = false;
					hTreeItem =  AddItem(strNode,hFltTypeSpecification);
					GetTreeCtrl().SetItemData(hTreeItem, (DWORD_PTR)pNodeData);
					
					GetTreeCtrl().Expand(hFltTypeSpecification,TVE_EXPAND);
				}
				

				//-------------------------------------------------------------------
				//Aircraft classifications 
				{
					strNode.LoadString(IDS_TVNN_AIRCRAFTCLASSIFICATIONS);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
					HTREEITEM hAircraftRelate =  AddItem(strNode,hFlights);
					GetTreeCtrl().SetItemData(hAircraftRelate,(DWORD_PTR)pNodeData);


					//Wingspan based
					strNode.LoadString(IDS_TVNN_WINGSPANBASED);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_WingSpan;
					pNodeData->nIDResource = IDR_MENU_WINGSPAN_CATEGORY;
					pNodeData->bHasExpand = false;
					hTreeItem =  AddItem(strNode,hAircraftRelate);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);	


					//Surface bearing based
					strNode.LoadString(IDS_TVNN_SURFACEBEARINGWEIGHTBASED);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_SurfaceBearingWeight;
					pNodeData->nIDResource = IDR_MENU_SURFACE_BEARING;
					pNodeData->bHasExpand = false;
					hTreeItem =  AddItem(strNode,hAircraftRelate);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


					//Wake vortex weight based
					strNode.LoadString(IDS_TVNN_WAKEVORTEXWEIGHTBASED);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_WakeVortexWeight;
					pNodeData->nIDResource = IDR_MENU_WAKE_VORTEX;
					pNodeData->bHasExpand = false;
					hTreeItem =  AddItem(strNode,hAircraftRelate);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

					//Approach speed based
					strNode.LoadString(IDS_TVNN_APPROACHSPEEDBASED);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_ApproachSpeed;
					pNodeData->nIDResource = IDR_MENU_APPRORACH_SPEED;
					pNodeData->bHasExpand = false;
					hTreeItem =  AddItem(strNode,hAircraftRelate);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
					
					GetTreeCtrl().Expand(hAircraftRelate,TVE_EXPAND);
				}
				

				//-------------------------------------------------------------------
				//Performance 
				{
					strNode.LoadString(IDS_TVNN_PERFORMANCE);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
					HTREEITEM hPerformance =  AddItem(strNode,hFlights);
					GetTreeCtrl().SetItemData(hPerformance,(DWORD_PTR)pNodeData);

				
					//Cruise
					strNode.LoadString(IDS_TVNN_CRUISE);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_PerformanceCruise;
					pNodeData->nIDResource = IDR_MENU_CRUISE;
					pNodeData->bHasExpand = false;
                    hTreeItem =  AddItem(strNode,hPerformance);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


					//Terminal
					strNode.LoadString(IDS_TVNN_TERMINAL);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_PerformanceTerminal ;
					pNodeData->nIDResource = IDR_MENU_TERMINAL_MANEUVER;
					pNodeData->bHasExpand = false;
					hTreeItem =  AddItem(strNode,hPerformance);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
						

					//Approach to land
					strNode.LoadString(IDS_TVNN_APPROACHTOLAND);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_PerformanceApproach;
					pNodeData->nIDResource = IDR_MENU_APPROACH_TO_LAND;
					pNodeData->bHasExpand = false;
					hTreeItem = AddItem(strNode,hPerformance);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


					//Landing
					strNode.LoadString(IDS_TVNN_LANDING);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_PerformanceLanding;
					pNodeData->nIDResource = IDR_MENU_LANDING;
					pNodeData->bHasExpand = false;
					hTreeItem = AddItem(strNode,hPerformance);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


					//Taxi inbound
					strNode.LoadString(IDS_TVNN_TAXIINBOUND);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_PerformanceTaxiInBound;
					pNodeData->nIDResource = IDR_MENU_TAXIINBOUND;
					pNodeData->bHasExpand = false;
					hTreeItem = AddItem(strNode,hPerformance);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


					//Stand service
					strNode.LoadString(IDS_TVNN_STANDSERVICE);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_PerformanceStandService;
					pNodeData->nIDResource = IDR_MENU_STANDSERVICE;
					pNodeData->bHasExpand = false;
					hTreeItem = AddItem(strNode,hPerformance);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


					//Push Back
					strNode.LoadString(IDS_TVNN_PUSHBACK);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_PerformancePushBack;
					pNodeData->nIDResource = IDR_MENU_PUSHBACK;
					pNodeData->bHasExpand = false;
					hTreeItem = AddItem(strNode,hPerformance);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

					//Engine start and parameters
					strNode.LoadString(IDS_TVNN_ENGINE_PARAMETERS);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_PerformanceEngineStartAndParameters;
					pNodeData->nIDResource = IDR_CTX_DEFAULT;
					pNodeData->bHasExpand = false;
					hTreeItem = AddItem(strNode,hPerformance);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

					//Taxi Outbound
					strNode.LoadString(IDS_TVNN_TAXIOUTBOUND);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_PerformanceTaxiOutBound;
					pNodeData->nIDResource = IDR_MENU_TAXIOUTBOUND;
					pNodeData->bHasExpand = false;
					hTreeItem = AddItem(strNode,hPerformance);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


					//Takeoff
					strNode.LoadString(IDS_TVNN_TAKEOFF);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_PerformanceTakeOff;
					pNodeData->nIDResource = IDR_MENU_TAKEOFF_PERFORMANCE;
					pNodeData->bHasExpand = false;
					hTreeItem = AddItem(strNode,hPerformance);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


					//Departure / climbout  
					strNode.LoadString(IDS_TVNN_DEPARTURECLIMBOUT);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
					pNodeData->nSubType = Dlg_PerformanceDepartureClimbout;
					pNodeData->nIDResource = IDR_MENU_DEPARTURE_CLIMBOUT;
					pNodeData->bHasExpand = false;
					hTreeItem = AddItem(strNode,hPerformance);
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


					GetTreeCtrl().Expand(hPerformance,TVE_EXPAND);
				}
				

				//Flight Plans 
				strNode.LoadString(IDS_TVNN_FLIGHTPLANS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_FlightPlans;
				pNodeData->nIDResource = IDR_MENU_FLIGHT_PLAN;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hFlights);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Arrivals ETA Offset
				strNode.LoadString(IDS_TVNN_ARRIVALSETAOFFSET);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_ArrivalETAOffset;
				//pNodeData->nIDResource = IDR_MENU_FLIGHT_PLAN;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hFlights);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


				//Departure Slots
				strNode.LoadString(IDS_TVNN_DEPARTURESLOTS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_DepartureSlots;
				pNodeData->nIDResource = IDR_MENU_DEPARTURE_SLOT;
				pNodeData->bHasExpand = false;
                hTreeItem =  AddItem(strNode,hFlights);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//-------------------------------------------------------------------
				//Noise data
				{
					strNode.LoadString(IDS_TVNN_NOISEDATA);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
					HTREEITEM hNoiseData =  AddItem(strNode,hFlights);
					GetTreeCtrl().SetItemData(hNoiseData,(DWORD_PTR)pNodeData);


				
					//Approach and landing
					strNode.LoadString(IDS_TVNN_APPROACHANDLANDING);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
					hTreeItem =  AddItem(strNode,hNoiseData);

					//Takeoff and climbout 
					strNode.LoadString(IDS_TVNN_TAKEOFFCLIMBOUT);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
					hTreeItem =  AddItem(strNode,hNoiseData);

					//Cruise
					strNode.LoadString(IDS_TVNN_CRUISE);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
					hTreeItem =  AddItem(strNode,hNoiseData);
					
					GetTreeCtrl().Expand(hNoiseData,TVE_EXPAND);
				}
				
				
				GetTreeCtrl().Expand(hFlights,TVE_EXPAND);
			}
			

			//-----------------------------------------------------------------------
			//Vehicles (GSE)
			{
				strNode.LoadString(IDS_TVNN_VEHICLES);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				HTREEITEM hVehicles =  AddItem(strNode,hFacilities);

			
				//Vehicle Data Specification
				strNode.LoadString(IDS_TVNN_VEHICLEDATASPECIFICATION);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_VehicleSpecification;
				pNodeData->nIDResource = IDR_MENU_VEHICLE;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hVehicles);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Service locations specification
				strNode.LoadString(IDS_TVNN_SERVICELOCATIONSPECIFICATION);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_FlightTypeRelativeServiceLocations;
				pNodeData->nIDResource = IDR_MENU_FLITYPE_RELATIVE_SERVICE_LOCATION;
				pNodeData->bHasExpand = false;
                hTreeItem =  AddItem(strNode,hVehicles);
				GetTreeCtrl().SetItemData(hTreeItem, (DWORD_PTR)pNodeData);

				//Servicing requirements 
				strNode.LoadString(IDS_TVNN_SERVICINGREQUIREMENTS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_ServiceRequirement;
				pNodeData->nIDResource = IDR_MENU_SERVICE_REQUIREMENT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hVehicles);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
				
				//Vehicular Movement Allowed
				strNode.LoadString(ID_TVNN_VEHICULARMOVEMENTALLOWED);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_VehicularMovementAllowed;
				pNodeData->nIDResource = IDR_MENU_SERVICE_REQUIREMENT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hVehicles);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				GetTreeCtrl().Expand(hVehicles,TVE_EXPAND);
			}
			//Deicing and anti-icings demand
			strNode.Format(_T("De-icing/Anti-icing Demand")) ;
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_Deicingandanti_demand;
			pNodeData->bHasExpand = false;
			hTreeItem =  AddItem(strNode,hFacilities);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
			//-----------------------------------------------------------------------
			//Weather Impacts
			strNode.LoadString(IDS_TVNN_WEATHERIMPACTS);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_WeatherImpacts;
			pNodeData->nIDResource = IDR_MENU_WEATHER_IMPACT;
			pNodeData->bHasExpand = false;
			hTreeItem =  AddItem(strNode,hFacilities);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
			
			GetTreeCtrl().Expand(hFacilities,TVE_EXPAND);
			
		}
		
	}


	//----------------------------------------------------------------------------
	//Resource Management 
	{
		strNode.LoadString(IDS_TVNN_RESOURCEMANAGEMENT);
		pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
		HTREEITEM hResource = AddItem(strNode, TVI_ROOT);
		GetTreeCtrl().SetItemData(hResource,(DWORD_PTR)pNodeData);

		{
			//-----------------------------------------------------------------------
			//Airspace
			{
				strNode.LoadString(IDS_TVNN_AIRSPACE);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				HTREEITEM hAirspace =  AddItem(strNode,hResource);
				GetTreeCtrl().SetItemData(hAirspace,(DWORD_PTR)pNodeData);

			
			
				//Sector Flight Mix Limitation
				strNode.LoadString(IDS_TVNN_SECTORFLIGHTMIXLIMITATION);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType =Dlg_SectorFlightMixSpecification;
				pNodeData->nIDResource = IDR_MENU_SECTORLIMITATION;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hAirspace);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


				//In trail separations 
				strNode.LoadString(IDS_TVNN_INTRAILSEPARATIONS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_AirspaceInTrailSeparation;
				pNodeData->nIDResource = IDR_MENU_INTRAIL;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hAirspace);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);



				//Controller interventions (airspace)
				strNode.LoadString(IDS_TVNN_CONTROLLERINTERVENTION_AIRSPACE);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nSubType = Dlg_ControllerIntervention ;
				pNodeData->nodeType = NodeType_Dlg ;
                pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hAirspace);
                GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Vectoring criteria
				strNode.LoadString(IDS_TVNN_VECTORINGCRITERIA);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				hTreeItem =  AddItem(strNode,hAirspace);

				//Direct routing criteria
				strNode.LoadString(IDS_TVNN_DIRECTROUTINGCRITERIA);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_DirectRoutingCriteria;
				pNodeData->nIDResource = IDR_MENU_DIRECT_ROUTING;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hAirspace);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
				
				// Controller workload
				strNode.LoadString(IDS_TVNN_CONTROLLERWORKLOAD);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				hTreeItem =  AddItem(strNode,hAirspace);

				GetTreeCtrl().Expand(hAirspace,TVE_EXPAND);

			}
			

			//-----------------------------------------------------------------------
			//Approaches
			{
				strNode.LoadString(IDS_TVNN_APPROACH);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				HTREEITEM hApproaches =  AddItem(strNode,hResource);
				GetTreeCtrl().SetItemData(hApproaches,(DWORD_PTR)pNodeData);
			
				//approach separation
				strNode.LoadString(IDS_TVNN_APPROACHSEPARATIONS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_ApprochSeparation;
				pNodeData->nIDResource = IDR_MENU_APPROACH_SEPARATION;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hApproaches);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Controller intervention(Approach)
				strNode.LoadString(IDS_TVNN_CONTROLLERINTERVENTION_APPROACH);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_CtrlInterventionSpec;
				pNodeData->nIDResource = IDR_MENU_CONTROLLER_INTERVENTION;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hApproaches);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Side step criteria
				strNode.LoadString(IDS_TVNN_SIDESTEPCRITERIA);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_SideStepSpecification;
				pNodeData->nIDResource = IDR_MENU_SIDE_STEP ;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hApproaches);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Go around criteria
				strNode.LoadString(IDS_TVNN_GOAROUNDCRITERIA);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_GoAroundCriteriaCustomize;
				pNodeData->nIDResource =  IDR_MENU_GOAROUND;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hApproaches);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
				
				// Controller workload
				strNode.LoadString(IDS_TVNN_CONTROLLERWORKLOAD);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				hTreeItem =  AddItem(strNode,hApproaches);

				GetTreeCtrl().Expand(hApproaches,TVE_EXPAND);
			}
			

			//-----------------------------------------------------------------------
			//Landings
			{
				strNode.LoadString(IDS_TVNN_LANDING);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_PerformanceLanding;
				HTREEITEM hLanding =  AddItem(strNode,hResource);
				GetTreeCtrl().SetItemData(hLanding,(DWORD_PTR)pNodeData);

				//STAR Assignment
				strNode.LoadString(IDS_TVNN_STARRUNWAYASSIGNMENT);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_StarAssignment;
				pNodeData->nIDResource =  IDR_MENU_STAR_ASSIGNMENT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hLanding);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Landing runway assignment 
				strNode.LoadString(IDS_TVNN_LANDINGRUNWAYASSIGNMENT);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_LandingRunwayAssignment;
				pNodeData->nIDResource =  IDR_MENU_LANDING_RUNWAY_ASSIGNMENT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hLanding);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Landing runway exit strategies
				strNode.LoadString(IDS_TVNN_LANDINGRUNWAYEXITSTRATEGIES);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_LandingRunwayExitStrategies;
				pNodeData->nIDResource = IDR_MENU_RUNWAYEXIT_STRATEGY;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hLanding);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
				
				//follow me connection
				strNode.Format(_T("Follow Me Connection")) ;
				pNodeData  =mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_FollowMeConnection ;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hLanding);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


				// Controller workload
				strNode.LoadString(IDS_TVNN_CONTROLLERWORKLOAD);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				hTreeItem =  AddItem(strNode,hLanding);

				GetTreeCtrl().Expand(hLanding,TVE_EXPAND);
			}
			

			//-----------------------------------------------------------------------
			//Taxiways (inbound)
			{
				strNode = "Taxiways(inbound)";//strNode.LoadString(IDS_TVNN_TAXIWAYS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				HTREEITEM hTaxiways =  AddItem(strNode,hResource);
			
			
				//Weight constraints
				strNode.LoadString(IDS_TVNN_WEIGHTCONSTRAINTS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_TaxiwayWeightConstraints;
				pNodeData->nIDResource = IDR_MENU_TAXIWAY_WEIGHT ;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Wingspan constraints 
				strNode.LoadString(IDS_TVNN_WINGSPANCONSTRAINTS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_TaxiwayWingspanConstraints;
				pNodeData->nIDResource = IDR_MENU_TAXIWAY_WINGSPAN_CONSTRAINT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Flight Type restrictions
				strNode.LoadString(IDS_TVNN_FLIGHTTYPERESTRICTIONS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_TaxiwayFlightTyperestrictions;
				pNodeData->nIDResource = IDR_MENU_TAXIWAY_FLITYPE_RESTRICTION;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Directionality Constraints
				strNode.LoadString(IDS_TVNN_DIRECTIONALITYCONSTRAINTS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_DirectionalityConstraints;
				pNodeData->nIDResource = IDR_MENU_DIRECTIONALITY_CONSTRAINT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Taxi Speed Constraints
				strNode.LoadString(IDS_TVNN_TAXISPEEDCONSTRAINTS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_TaxiSpeedConstraints;
				pNodeData->nIDResource = IDR_MENU_TAXISPEED_CONSTRAINT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Intersection node option
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_IntersectionNodeOption;
				//pNodeData->nIDResource = IDR_MENU_TAXISPEED_CONSTRAINT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem("Intersection Node Option",hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Inbound route assignment
				strNode.LoadString(IDS_TVNN_INBOUNDROUTEASSIGNMENT);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_InboundRouteAssignment;
				pNodeData->nIDResource = IDR_MENU_INBOUND_ROUTE_ASSIGNMENT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


				//Temporary parking criteria
				strNode.LoadString(IDS_TVNN_TEMPORARAYPARKINGCRITERIA);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_TaxiwayTemporaryParkingCriteria;
				pNodeData->nIDResource = IDR_MENU_TEMPORARY_PARKING;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Towing routes
				strNode.LoadString(IDS_TVNN_TOWINGROUTES);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_TowingRoute;
		//		pNodeData->nIDResource = ;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Conflict resolution criteria (taxiing)
				strNode.LoadString(IDS_TVNN_CONFLICTRESOLUTIONCRITERIA);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_ConflictResolutionCriteria;
				pNodeData->nIDResource = IDR_MENU_CONFLICT_RESOLUTION;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//strNode.LoadString(IDS_TVNN_CONFLICTRESOLUTIONCRITERIA);
				strNode = _T("Wave runway crossing");
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_WaveCrossRunwaySpecification;
				//pNodeData->nIDResource = IDR_MENU_CONFLICT_RESOLUTION;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
	
				//Taxi interrupt time
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_TaxiInterruptTime;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(_T("Taxi Interrupt Times"),hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				// Controller workload
				strNode.LoadString(IDS_TVNN_CONTROLLERWORKLOAD);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				hTreeItem =  AddItem(strNode,hTaxiways);

				GetTreeCtrl().Expand(hTaxiways,TVE_EXPAND);
			}
			

			//-----------------------------------------------------------------------
			//Parking stands (gates)
			{
				strNode.LoadString(IDS_TVNN_PARKINGSTANDS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				HTREEITEM hParkingStands =  AddItem(strNode,hResource);
			
			
				//Buffers
				strNode.LoadString(IDS_TVNN_STANDBUFFERS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_StandBuffers;
				pNodeData->nIDResource = IDR_MENU_PARKING_STANDBUFFER;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hParkingStands);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//stand constraints
				strNode.LoadString(IDS_TVNN_STANDCON);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_StandConstraints;
				pNodeData->bHasExpand = false;
				hTreeItem = AddItem(strNode,hParkingStands);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Tow Off Stand Assignment
				strNode.LoadString(IDS_TVNN_TOWOFFSTAND);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_TowOffStandAssignment;
				//pNodeData->nIDResource = ;
				pNodeData->bHasExpand = false;
				hTreeItem = AddItem(strNode, hParkingStands);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Stand Entry/Exit Criteria
				strNode.LoadString(IDS_TVNN_STANDCRITERIA);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_StandCriteriaAssignment;
				pNodeData->bHasExpand = false;
				hTreeItem = AddItem(strNode,hParkingStands);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Stand Assignment
				strNode.LoadString(IDS_TVNN_STANDASSIGNMENT);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_StandAssignment;
				pNodeData->nIDResource = IDR_MENU_STAND_ASSIGNMENT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hParkingStands);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
				//HTREEITEM hStandAssignment =  AddItem(strNode,hParkingStands);
				//{

				//}

				//Occupied assigned stand action criteria
				strNode.LoadString(IDS_TVNN_OCCUPIEDASSIGNED);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_OccupiedAssignedStandAction;
				pNodeData->nIDResource = IDR_MENU_OCCUPY_STAND_ACTION;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hParkingStands);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);	

				//Push back (taxi out) clearance criteria
				strNode.LoadString(IDS_TVNN_PUSHBACKCLEARANCE);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_PushBackClearanceCriteria;
				pNodeData->nIDResource = IDR_MENU_TAXIOUT_PUSHBACK;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hParkingStands);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);	

				//Ignition Specification
				strNode.LoadString(IDS_TVNN_IGNITION);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_Ignition;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hParkingStands);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);	


				// Controller workload
				strNode.LoadString(IDS_TVNN_CONTROLLERWORKLOAD);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				hTreeItem =  AddItem(strNode,hParkingStands);
				
				
				GetTreeCtrl().Expand(hParkingStands,TVE_EXPAND);
			}
			

			//-----------------------------------------------------------------------
			//Deicing pads
			{
				strNode.LoadString(IDS_TVNN_DEICEANTI_STRATEGIES);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_DeiceAntiStrategies;
				pNodeData->nOtherData = nProjID;
				HTREEITEM hDeiceItem = AddItem(strNode,hResource);
				GetTreeCtrl().SetItemData(hDeiceItem,(DWORD_PTR)pNodeData);

				strNode.LoadString(IDS_TVNN_DEUCINGPADS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				HTREEITEM hDeicingPads =  AddItem(strNode,hDeiceItem);
			
			
				//Deicing queue specification
				strNode.LoadString(IDS_TVNN_DEICINGQUEUE);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_DeiceQueue;
				pNodeData->nOtherData = nProjID;
				hTreeItem =  AddItem(strNode,hDeicingPads);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Deicing assignment specification
// 				strNode.LoadString(IDS_TVNN_DEICINGASSIGNMENT);
// 				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
// 				pNodeData->nSubType = Dlg_PadAssignment;
// 				hTreeItem =  AddItem(strNode,hDeicingPads);
// 				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Deicing routes specification
				strNode.LoadString(IDS_TVNN_DEICINGROUTES);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_DeiceRoute;
				pNodeData->nOtherData = nProjID;
				hTreeItem =  AddItem(strNode,hDeicingPads);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Post deicing route specification
				strNode.LoadString(IDS_TVNN_POSTDEICINGROUTES);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_PostDeiceRoute;
				pNodeData->nOtherData = nProjID;
				hTreeItem =  AddItem(strNode,hDeicingPads);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
				
				// Controller workload
				strNode.LoadString(IDS_TVNN_CONTROLLERWORKLOAD);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				hTreeItem =  AddItem(strNode,hDeiceItem);
				
				GetTreeCtrl().Expand(hDeicingPads,TVE_EXPAND);
				GetTreeCtrl().Expand(hDeiceItem,TVE_EXPAND);
			}
			

			//-----------------------------------------------------------------------
			//Departures
			{
				strNode.LoadString(IDS_TVNN_DEPARTURES);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				HTREEITEM hDepartures =  AddItem(strNode,hResource);
			
// 				//Runway take off positions
// 				strNode.LoadString(IDS_TVNN_RUNWAYTAKEOFFPOSITIONS);
// 				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
// 				pNodeData->nSubType = Dlg_RunwayTakeOffPositions;
// 				pNodeData->nIDResource = IDR_MENU_RUNWAY_TAKEOFF_POSITION;
// 				pNodeData->bHasExpand = false;
// 				hTreeItem =  AddItem(strNode,hDepartures);
// 				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Runway take off positions
				strNode=_T("Runway Takeoff Assignment");
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_RunwayTakeOffAssignment;
				pNodeData->nIDResource = IDR_MENU_TAKEOFF_RUNWAY_ASSIGNMENT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hDepartures);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


				//Take off queues
				strNode.LoadString(IDS_TVNN_TAKEOFFQUEUES);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_RunwayTakeOffQueues;
				pNodeData->nIDResource = IDR_MENU_TAKEOFF_QUEUE;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hDepartures);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


				//Take off sequencing
				strNode.LoadString(IDS_TVNN_TAKEOFFSEQUENCING);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_RunwayTakeOffSequencing;
				pNodeData->nIDResource = IDR_MENU_TAKEOFF_SEQUENCE;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hDepartures);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


				//Take off runway assignment
				//strNode.LoadString(IDS_TVNN_TAKEOFFRUNWAYASSIGNMENT);
				//pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				//pNodeData->nSubType = Dlg_RunwayTakeOffAssignment;
				//pNodeData->nIDResource = IDR_MENU_TAKEOFF_RUNWAY_ASSIGNMENT;
				//pNodeData->bHasExpand = false;
				//hTreeItem =  AddItem(strNode,hDepartures);
				//GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


				//SID assignment
				strNode.LoadString(IDS_TVNN_SIDASSIGNMENT);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_SIDAssignment;
				pNodeData->nIDResource = IDR_MENU_SID_ASSIGNMENT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hDepartures);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				
				//Clearance criteria
				strNode.LoadString(IDS_TVNN_CLEARANCECRITERIA);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_RunwayClearanceCriteria;
				pNodeData->nIDResource = IDR_MENU_TAKEOFF_CLEARENCE;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hDepartures);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


				//Arrival Delay triggers
				strNode.LoadString(IDS_TVNN_ARRIAVLDELAYTRIGGERS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_RunwayArrivalDelayTriggers;
				pNodeData->nIDResource = IDR_MENU_ARRIVAL_DELAY_TRIGGER;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hDepartures);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
				
				// Controller workload
				strNode.LoadString(IDS_TVNN_CONTROLLERWORKLOAD);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				hTreeItem =  AddItem(strNode,hDepartures);
				
				GetTreeCtrl().Expand(hDepartures,TVE_EXPAND);
			}

			//-----------------------------------------------------------------------
			//Taxiways (outbound)
			{
				strNode = "Taxiways(outbound)";//strNode.LoadString(IDS_TVNN_TAXIWAYS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				HTREEITEM hTaxiways =  AddItem(strNode,hResource);

				//Weight constraints
				strNode.LoadString(IDS_TVNN_WEIGHTCONSTRAINTS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_TaxiwayWeightConstraints;
				pNodeData->nIDResource = IDR_MENU_TAXIWAY_WEIGHT ;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Wingspan constraints 
				strNode.LoadString(IDS_TVNN_WINGSPANCONSTRAINTS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_TaxiwayWingspanConstraints;
				pNodeData->nIDResource = IDR_MENU_TAXIWAY_WINGSPAN_CONSTRAINT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Flight Type restrictions
				strNode.LoadString(IDS_TVNN_FLIGHTTYPERESTRICTIONS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_TaxiwayFlightTyperestrictions;
				pNodeData->nIDResource = IDR_MENU_TAXIWAY_FLITYPE_RESTRICTION;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Directionality Constraints
				strNode.LoadString(IDS_TVNN_DIRECTIONALITYCONSTRAINTS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_DirectionalityConstraints;
				pNodeData->nIDResource = IDR_MENU_DIRECTIONALITY_CONSTRAINT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Taxi Speed Constraints
				strNode.LoadString(IDS_TVNN_TAXISPEEDCONSTRAINTS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_TaxiSpeedConstraints;
				pNodeData->nIDResource = IDR_MENU_TAXISPEED_CONSTRAINT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Intersection node option
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_IntersectionNodeOption;
				//pNodeData->nIDResource = IDR_MENU_TAXISPEED_CONSTRAINT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem("Intersection Node Option",hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

			

				//Outbound route assignment
				strNode.LoadString(IDS_TVNN_OUTBOUNDROUTEASSIGNMENT);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_OutboundRouteAssignment;
				pNodeData->nIDResource = IDR_MENU_OUTBOUND_ROUTE_ASSIGNMENT;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

			

				//Towing routes
				strNode.LoadString(IDS_TVNN_TOWINGROUTES);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_TowingRoute;
				//		pNodeData->nIDResource = ;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Conflict resolution criteria (taxiing)
				strNode.LoadString(IDS_TVNN_CONFLICTRESOLUTIONCRITERIA);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_ConflictResolutionCriteria;
				pNodeData->nIDResource = IDR_MENU_CONFLICT_RESOLUTION;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//strNode.LoadString(IDS_TVNN_CONFLICTRESOLUTIONCRITERIA);
				strNode = _T("Wave runway crossing");
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_WaveCrossRunwaySpecification;
				//pNodeData->nIDResource = IDR_MENU_CONFLICT_RESOLUTION;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(strNode,hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				//Taxi interrupt time
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				pNodeData->nSubType = Dlg_TaxiInterruptTime;
				pNodeData->bHasExpand = false;
				hTreeItem =  AddItem(_T("Taxi Interrupt Times"),hTaxiways);
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

				// Controller workload
				strNode.LoadString(IDS_TVNN_CONTROLLERWORKLOAD);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
				hTreeItem =  AddItem(strNode,hTaxiways);

				GetTreeCtrl().Expand(hTaxiways,TVE_EXPAND);
			}


			//-----------------------------------------------------------------------
			//Vehicles (GSE) Pools 
			strNode.LoadString(IDS_TVNN_VEHICLESPOOLS);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_VehiclePoolsAndDeployment;
			pNodeData->nIDResource = IDR_MENU_VEHICLE_POOLS;
			pNodeData->bHasExpand = false;
			HTREEITEM hVehiclesPools =  AddItem(strNode,hResource);
			GetTreeCtrl().SetItemData(hVehiclesPools,(DWORD_PTR)pNodeData);

			//push back and tow operation
			//strNode.LoadString(IDS_TVNN_VEHICLESPOOLS);
			strNode.Format(_T("Push back & tow operation specification"));
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_PushBackAndTowOperationSpec;
			pNodeData->bHasExpand = false;
			HTREEITEM hPushBackAndTowItem =  AddItem(strNode,hResource);
			GetTreeCtrl().SetItemData(hPushBackAndTowItem,(DWORD_PTR)pNodeData);		

			//Training Flights Management
			strNode.LoadString(IDS_TVNN_TRAININGFLIGHTSMANAGEMENT);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_TrainingFlightsManagement;
			pNodeData->nIDResource = IDR_MENU_TRAININGFLIGHTSMANGEMENT;
			pNodeData->bHasExpand = false;
			hTreeItem =  AddItem(strNode,hResource);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

			GetTreeCtrl().Expand(hResource,TVE_EXPAND);
		}
		
	}


	//----------------------------------------------------------------------------
	//Simulation
	{
		strNode.LoadString(IDS_TVNN_SIMULATION);
		pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
		HTREEITEM hSimulation =  AddItem(strNode,TVI_ROOT);
		{
			//Initial conditions
			strNode.LoadString(IDS_TVNN_INITIALCONDITIONS);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
			HTREEITEM ConditionItem =  AddItem(strNode,hSimulation);
			GetTreeCtrl().Expand(ConditionItem,TVE_EXPAND);

			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_AircraftSurfaceCondition;
			hTreeItem =  AddItem(_T("Aircraft Surfaces"),ConditionItem);
            GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData) ;

			//Weather script
			strNode.LoadString(IDS_TVNN_WEATHERSCRIPT);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_WeatherScript ;
			hTreeItem =  AddItem(strNode,hSimulation);
            GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData) ;

			//Settings
			strNode.LoadString(IDS_TVNN_SETTINGS);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_SimSetting;
			pNodeData->nIDResource = IDR_MENU_SIMULATION_ENGINE_SETTING;
			pNodeData->bHasExpand = false;
			hTreeItem =  AddItem(strNode,hSimulation);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);


			
			//Run
			strNode.LoadString(IDS_TVNN_RUN);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_SimRun;
			pNodeData->nIDResource = IDR_MENU_RUN_SIMULATION;
			pNodeData->bHasExpand = false;
			hTreeItem =  AddItem(strNode,hSimulation);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

		}
		GetTreeCtrl().Expand(hSimulation,TVE_EXPAND);
	}

	//----------------------------------------------------------------------------
	//Analysis Parameters
	{
		strNode.LoadString(IDS_TVNN_ANALYSISPARAMETERS);
		pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
		HTREEITEM hAnalysisParam =  AddItem(strNode,TVI_ROOT);
		{


			//Aircraft Display
			strNode.LoadString(IDS_TVNN_AIRCRAFTDISPLAY);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_AircraftDisplay;
			pNodeData->nIDResource = IDR_MENU_AIRCRAFT_DISPLAY;
			pNodeData->bHasExpand = false;
			hTreeItem = AddItem(strNode,hAnalysisParam);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);




			//Vehicle Display
			strNode.LoadString(IDS_TVNN_VEHICLEDISPLAY);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_VehicleDisplay;
			pNodeData->nIDResource = IDR_MENU_VEHICLE_DISPLAY;
			pNodeData->bHasExpand = false;
			hTreeItem =  AddItem(strNode,hAnalysisParam);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

			//Aircraft Alias
			strNode.LoadString(IDS_TVNN_AIRCRAFTALIAS);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_AircraftAlias;
			//pNodeData->nIDResource = IDR_MENU_AIRCRAFT_TAGS;
			pNodeData->bHasExpand = false;
			hTreeItem =  AddItem(strNode,hAnalysisParam);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

			//Aircraft Tags
			strNode.LoadString(IDS_TVNN_AIRCRAFTTAGS);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_AircraftTags;
			pNodeData->nIDResource = IDR_MENU_AIRCRAFT_TAGS;
			pNodeData->bHasExpand = false;
			hTreeItem =  AddItem(strNode,hAnalysisParam);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

			//Vehicle Tags
			strNode.LoadString(IDS_TVNN_VEHICLETAGS);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_VehicleTags;
			pNodeData->nIDResource = IDR_MENU_VEHICLE_TAGS;
			pNodeData->bHasExpand = false;
			hTreeItem =  AddItem(strNode,hAnalysisParam);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

			//--------------------------------------------------------------------------------------
			//Processor Information
			/*strNode.LoadString(IDS_TVNN_PROCESSORINFORMATION);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
			hTreeItem=  AddItem(strNode,hAnalysisParam);*/

			//Auto Reports
			strNode.LoadString(IDS_TVNN_AUTOREPORTS);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
			hTreeItem =  AddItem(strNode,hAnalysisParam);

			//Callout Display Specification
			strNode.LoadString(IDS_TVNN_CALLOUT_DISP_SPEC);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_CalloutDispSpec;
			pNodeData->nIDResource = IDR_MENU_CALLOUT_DISP_SPEC;
			pNodeData->bHasExpand = false;
			hTreeItem = AddItem(strNode,hAnalysisParam);
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
		}
		GetTreeCtrl().Expand(hAnalysisParam,TVE_EXPAND);
	}


	GetTreeCtrl().SelectItem(hDefaultSelected);

}
void CAirsideMSView::AddLevelToTree(HTREEITEM hAirports,const ALTAirport& airport)
{


}

void CAirsideMSView::AddAirportToTree(HTREEITEM hAirports,const ALTAirport& airport)
{
	int nAirport = airport.getID();

	//add airport
	CString strNode = airport.getName();
	CNodeData *pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Airport);
	pNodeData->dwData = nAirport;
	pNodeData->nIDResource = IDR_MENU_AIRSIDEAIRPORT;
	HTREEITEM hAirport = AddItem(strNode,hAirports,TVI_LAST,ID_NODEIMG_DEFAULT,ID_NODEIMG_DEFAULT);
	GetTreeCtrl().SetItemData(hAirport,(DWORD_PTR)pNodeData);

	{
		//------------------------------------------------------------------------------------
		//Levels

		{
			strNode.LoadString(IDS_TVNN_LEVELS);	
// 			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
// 			pNodeData->dwData = nAirport;
// 			pNodeData->nIDResource = IDR_MENU_AIRPORTLEVELS;
			HTREEITEM hLevels = AddItem(strNode,hAirport);
			m_hItemLevels = hLevels;
		//	GetTreeCtrl().SetItemData(hLevels,(DWORD_PTR)pNodeData);


			//Base Level
			CAirsideLevelList levelList(nAirport);
			levelList.ReadLevelList();
			ASSERT(levelList.GetCount()>0);

			for (int i=0; i<levelList.GetCount(); i++)
			{
				CString strNode = levelList.GetItem(i).strName;
				int nLevelID = levelList.GetItem(i).nLevelID;
				int bMainLevel = levelList.GetItem(i).bMainLevel;

				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Level);
				pNodeData->nIDResource = IDR_CTX_LEVEL_AIRSIDE;
				pNodeData->nSubType = bMainLevel;//Base Level
				pNodeData->dwData = nLevelID;
				pNodeData->nOtherData = nAirport;
				HTREEITEM hBaseLevel = AddItem(strNode,hLevels);
				if(bMainLevel)
					m_hItemBaseLevel = hBaseLevel;
				GetTreeCtrl().SetItemData(hBaseLevel,(DWORD_PTR)pNodeData);
			}

			GetTreeCtrl().Expand(hLevels,TVE_EXPAND);
		}

		//Airport reference data 
		{
			strNode.LoadString(IDS_TVNN_AIRPORTREFERENCEPOINTS);	
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_AirportReferencePoints;
			pNodeData->nIDResource = IDR_CTX_LATLONG;
			pNodeData->nOtherData = nAirport;
			HTREEITEM hAirportReferPt = AddItem(strNode,hAirport);
			GetTreeCtrl().SetItemData(hAirportReferPt,(DWORD_PTR)pNodeData);

			//Lat/Long
			CString strLL;
			strLL.Format("LAT:%s LONG:%s",airport.getLatitude(),airport.getLongtitude());
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
			HTREEITEM hLatLong = AddItem(strLL,hAirportReferPt);
			GetTreeCtrl().SetItemData(hLatLong,(DWORD_PTR)pNodeData);

			//X/Y
			CString strXY;
			double dx=	airport.getRefPoint().getX();
			double dy=	airport.getRefPoint().getY();

			strXY.Format("X:%.2f Y:%.2f",dx,dy);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
			HTREEITEM hXY = AddItem(strXY,hAirportReferPt);
			GetTreeCtrl().SetItemData(hXY,(DWORD_PTR)pNodeData);

			//Elevation (ASL)
			CString strElevation;
			CDlgEditARP dlg(m_nProjID,nAirport,this);
			dlg.InitializeUnitPiece(m_nProjID,UM_ID_24,this);
			strElevation.Format("Elevation(%s): %.0f",CARCLengthUnit::GetLengthUnitString(dlg.GetCurSelLengthUnit()),\
				CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,dlg.GetCurSelLengthUnit(),airport.getElevation()));
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
			HTREEITEM hElevate = AddItem(strElevation,hAirportReferPt);
			GetTreeCtrl().SetItemData(hElevate,(DWORD_PTR)pNodeData);


			//Magnetic Variation
			CString strDegree;
			strDegree = (airport.GetMagnectVariation().m_iEast_West ==0) ? _T("E") : _T("W");//Read From database
			CString strMagneticVariation;
			strMagneticVariation.Format("Magnetic Variation: %d %s", (int)(airport.GetMagnectVariation().m_dval), strDegree);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
			HTREEITEM hMagVar = AddItem(strMagneticVariation,hAirportReferPt);
			GetTreeCtrl().SetItemData(hMagVar,(DWORD_PTR)pNodeData);

			GetTreeCtrl().Expand(hAirportReferPt,TVE_EXPAND);
		}
		//------------------------------------------------------------------------------------
		

		//------------------------------------------------------------------------------------
		//Aircraft Facilities
		{
			strNode.LoadString(IDS_TVNN_AIRCRAFTFACI);	
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_AircraftFacility);
			HTREEITEM hAircraftFaci = AddItem(strNode,hAirport);
			GetTreeCtrl().SetItemData(hAircraftFaci,(DWORD_PTR)pNodeData);

			//Maneuvering Surfaces
			{
				strNode.LoadString(IDS_TVNN_MANEUVERINGSURFACES);	
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_ManeuverSurface);
				HTREEITEM hManuSurfaces = AddItem(strNode,hAircraftFaci);
				GetTreeCtrl().SetItemData(hManuSurfaces,(DWORD_PTR)pNodeData); 

				//Runway
				strNode.LoadString(IDS_TVNN_RUNWAYS);	
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_RUNWAY;
				pNodeData->nOtherData = nAirport;
				pNodeData->bHasExpand = false;
				HTREEITEM hRunway = AddItem(strNode,hManuSurfaces);
				GetTreeCtrl().SetItemData(hRunway,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hRunway);
				GetTreeCtrl().Expand(hRunway,TVE_EXPAND);

				//Heliport
				strNode.LoadString(IDS_TVNN_HELIPORT);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_HELIPORT;
				pNodeData->nOtherData = nAirport;
				pNodeData->bHasExpand = false;
				HTREEITEM hHeliport = AddItem(strNode,hManuSurfaces);
				GetTreeCtrl().SetItemData(hHeliport,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hHeliport);
				GetTreeCtrl().Expand(hHeliport,TVE_EXPAND);

				//TaxiWays
				strNode.LoadString(IDS_TVNN_TAIWAYS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_TAXIWAY;
				pNodeData->nOtherData = nAirport;
				pNodeData->bHasExpand = false;
				pNodeData->nOtherData = nAirport;
				HTREEITEM hTaxiway= AddItem(strNode,hManuSurfaces);
				GetTreeCtrl().SetItemData(hTaxiway,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hTaxiway);
				GetTreeCtrl().Expand(hTaxiway,TVE_EXPAND);

				//Stand
				strNode.LoadString(IDS_TVNN_GATES);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_STAND;
				pNodeData->bHasExpand = false;
				pNodeData->nOtherData = nAirport;
				HTREEITEM hStand= AddItem(strNode,hManuSurfaces);
				GetTreeCtrl().SetItemData(hStand,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hStand);
				GetTreeCtrl().Expand(hStand,TVE_EXPAND);

				//Deice pad
				strNode.LoadString(IDS_TVNN_DEICEPADS);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_DEICEBAY;
				pNodeData->bHasExpand = false;
				pNodeData->nOtherData = nAirport;
				HTREEITEM hDeicePad= AddItem(strNode,hManuSurfaces);
				GetTreeCtrl().SetItemData(hDeicePad,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hDeicePad);
				GetTreeCtrl().Expand(hDeicePad,TVE_EXPAND);

				GetTreeCtrl().Expand(hManuSurfaces,TVE_EXPAND);
			}

			//Hold Short Lines
			strNode.LoadString(IDS_TVNN_HOLDSHORTLINES);	
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
			pNodeData->nodeType = NodeType_Dlg;
			pNodeData->nSubType = Dlg_HoldShortLines;
			pNodeData->nIDResource = IDR_MENU_HOLDSHORTLINES;
			pNodeData->nOtherData = nAirport;
			pNodeData->bHasExpand = false;
			HTREEITEM hHoldShortLines = AddItem(strNode,hAircraftFaci);
			GetTreeCtrl().SetItemData(hHoldShortLines,(DWORD_PTR)pNodeData);

			//Taxi routes
			strNode.LoadString(IDS_TVNN_TAXIROUTES);	
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
			pNodeData->nodeType = NodeType_ObjectRoot;
			pNodeData->nSubType = ALT_GROUNDROUTE; 
			pNodeData->bHasExpand = false;
			pNodeData->nOtherData = nAirport;
			HTREEITEM hTaxiRoutes = AddItem(strNode,hAircraftFaci);
			GetTreeCtrl().SetItemData(hTaxiRoutes,(DWORD_PTR)pNodeData);

			//Start position
			strNode.LoadString(IDS_TVNN_STARTPOSITIONS);	
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
			pNodeData->nodeType = NodeType_ObjectRoot;
			pNodeData->nSubType = ALT_STARTPOSITION;
			pNodeData->nOtherData = nAirport;
			pNodeData->bHasExpand = false;
			HTREEITEM hStartPositions = AddItem(strNode,hAircraftFaci);
			GetTreeCtrl().SetItemData(hStartPositions,(DWORD_PTR)pNodeData);
			AddItem("Loading...",hStartPositions);
			GetTreeCtrl().Expand(hStartPositions,TVE_EXPAND);

			//Meeting points
			strNode.LoadString(IDS_TVNN_MEETINGPOINTS);
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
			pNodeData->nodeType = NodeType_ObjectRoot;
			pNodeData->nSubType = ALT_MEETINGPOINT;
			pNodeData->nOtherData = nAirport;
			pNodeData->bHasExpand = false;
			HTREEITEM hMeetingPoints = AddItem(strNode,hAircraftFaci);
			GetTreeCtrl().SetItemData(hMeetingPoints,(DWORD_PTR)pNodeData);
			AddItem("Loading...",hMeetingPoints);
			GetTreeCtrl().Expand(hMeetingPoints,TVE_EXPAND);
			
			GetTreeCtrl().Expand(hAircraftFaci,TVE_EXPAND);
		}


		//------------------------------------------------------------------------------------
		//Vehicular facilities 
		{
			strNode.LoadString(IDS_TVNN_VEHICULARFACI);	
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_VehicleFacility);
			HTREEITEM hVahicularFaci = AddItem(strNode,hAirport);
			GetTreeCtrl().SetItemData(hVahicularFaci,(DWORD_PTR)pNodeData);

			//Maneuvering Surfaces
			{
				strNode.LoadString(IDS_TVNN_MANEUVERINGSURFACES);	
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_ManeuverSurface);
				HTREEITEM hManuSurfaces = AddItem(strNode,hVahicularFaci);
				GetTreeCtrl().SetItemData(hManuSurfaces,(DWORD_PTR)pNodeData);

				//Stretch
				strNode.LoadString(IDS_TVNN_STRETCHS);	
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_STRETCH;
				pNodeData->nOtherData = nAirport;
				pNodeData->bHasExpand = false;
				HTREEITEM hStretch = AddItem(strNode,hManuSurfaces);
				GetTreeCtrl().SetItemData(hStretch,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hStretch);
				GetTreeCtrl().Expand(hStretch,TVE_EXPAND);

				//Intersections
				strNode.LoadString(IDS_TVNN_INTERSECTIONS);	
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_INTERSECTIONS;
				pNodeData->nOtherData = nAirport;
				pNodeData->bHasExpand = false;
				HTREEITEM hIntersections = AddItem(strNode,hManuSurfaces);
				GetTreeCtrl().SetItemData(hIntersections,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hIntersections);
				GetTreeCtrl().Expand(hIntersections,TVE_EXPAND);

				//RoundAbout
				strNode.LoadString(IDS_TVNN_ROUNDABOUT);	
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_ROUNDABOUT;
				pNodeData->nOtherData = nAirport;
				pNodeData->bHasExpand = false;
				HTREEITEM hRoundAbout = AddItem(strNode,hManuSurfaces);
				GetTreeCtrl().SetItemData(hRoundAbout,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hRoundAbout);
				GetTreeCtrl().Expand(hRoundAbout,TVE_EXPAND);

				//TurnOff
				/*strNode.LoadString(IDS_TVNN_TURNOFFS);	
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_TURNOFF;
				pNodeData->nOtherData = nAirport;
				pNodeData->bHasExpand = false;
				HTREEITEM hTurnOff = AddItem(strNode,hManuSurfaces);
				GetTreeCtrl().SetItemData(hTurnOff,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hTurnOff);*/

				//LaneAdapter
			/*	strNode.LoadString(IDS_TVNN_LANEADAPTER);	
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_LANEADAPTER;
				pNodeData->nOtherData = nAirport;
				pNodeData->bHasExpand = false;
				HTREEITEM hLaneAdapter = AddItem(strNode,hManuSurfaces);
				GetTreeCtrl().SetItemData(hLaneAdapter,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hLaneAdapter);*/

				//vehicle pool parking
				//strNode.LoadString(IDS_TVNN_LINEPARKING);	
				strNode = _T("Pool Parkings");
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_VEHICLEPOOLPARKING;
				pNodeData->nOtherData = nAirport;
				pNodeData->bHasExpand = false;
				HTREEITEM hVehiclePoolParking = AddItem(strNode,hManuSurfaces);
				GetTreeCtrl().SetItemData(hVehiclePoolParking,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hVehiclePoolParking);
				GetTreeCtrl().Expand(hVehiclePoolParking,TVE_EXPAND);
				//--------------------------------------------------------------------------------/
				//NoseInParking
				/*strNode.LoadString(IDS_TVNN_NOSEINPARKING);	
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_NOSEINPARKING;
				pNodeData->nOtherData = nAirport;
				pNodeData->bHasExpand = false;
				HTREEITEM hNoseInParking = AddItem(strNode,hManuSurfaces);
				GetTreeCtrl().SetItemData(hNoseInParking,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hNoseInParking);*/

				GetTreeCtrl().Expand(hManuSurfaces,TVE_EXPAND);
			}

			//Control devices
			{
				strNode.LoadString(IDS_TVNN_CONTROLDEVICES);	
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_ControlDevice);
				HTREEITEM hControlDevices = AddItem(strNode,hVahicularFaci);
				GetTreeCtrl().SetItemData(hControlDevices,(DWORD_PTR)pNodeData);

				//TrafficLight
				strNode.LoadString(IDS_TVNN_TRAFFICLIGHT);	
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_TRAFFICLIGHT;
				pNodeData->nOtherData = nAirport;
				pNodeData->bHasExpand = false;
				HTREEITEM hTrafficLight = AddItem(strNode,hControlDevices);
				GetTreeCtrl().SetItemData(hTrafficLight,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hTrafficLight);
				GetTreeCtrl().Expand(hTrafficLight,TVE_EXPAND);

				//TollGate
				strNode.LoadString(IDS_TVNN_TOLLGATE);	
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_TOLLGATE;
				pNodeData->nOtherData = nAirport;
				pNodeData->bHasExpand = false;
				HTREEITEM hTollGate = AddItem(strNode,hControlDevices);
				GetTreeCtrl().SetItemData(hTollGate,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hTollGate);
				GetTreeCtrl().Expand(hTollGate,TVE_EXPAND);

				//StopSign
				strNode.LoadString(IDS_TVNN_STOPSIGN);	
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_STOPSIGN;
				pNodeData->nOtherData = nAirport;
				pNodeData->bHasExpand = false;
				HTREEITEM hStopSign = AddItem(strNode,hControlDevices);
				GetTreeCtrl().SetItemData(hStopSign,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hStopSign);
				GetTreeCtrl().Expand(hStopSign,TVE_EXPAND);

				//YieldSign
				strNode.LoadString(IDS_TVNN_YIELDSIGN);	
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nodeType = NodeType_ObjectRoot;
				pNodeData->nSubType = ALT_YIELDSIGN;
				pNodeData->nOtherData = nAirport;
				pNodeData->bHasExpand = false;
				HTREEITEM hYieldSign = AddItem(strNode,hControlDevices);
				GetTreeCtrl().SetItemData(hYieldSign,(DWORD_PTR)pNodeData);
				AddItem("Loading...",hYieldSign);
				GetTreeCtrl().Expand(hYieldSign,TVE_EXPAND);

				GetTreeCtrl().Expand(hControlDevices,TVE_EXPAND);
			}

			//Pax bus parking 
			strNode.LoadString(IDS_TVNN_PaxBusParking);	
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_Paxbusparking;
			HTREEITEM hPaxBusParking = AddItem(strNode,hVahicularFaci);
			GetTreeCtrl().SetItemData(hPaxBusParking,(DWORD_PTR)pNodeData);

			//Bag carts loading areas
			strNode.LoadString(IDS_TVNN_BagCartsLoadingAreas);	
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
			pNodeData->nodeType = NodeType_Dlg;
			pNodeData->nSubType = Dlg_BagCartsLoadingAreas;
			pNodeData->nOtherData = nAirport;
			pNodeData->bHasExpand = false;
			HTREEITEM hBagCartsLoadingAreas = AddItem(strNode,hVahicularFaci);
			GetTreeCtrl().SetItemData(hBagCartsLoadingAreas,(DWORD_PTR)pNodeData);

			//Routes specification
			strNode.LoadString(IDS_TVNN_ROUTESSPECIFIC);	
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Dlg);
			pNodeData->nSubType = Dlg_VehicleRoute;
			pNodeData->nIDResource = IDR_MENU_ROUTESPE;
			pNodeData->nOtherData = nAirport;
			pNodeData->bHasExpand = false;
			HTREEITEM hRoutesSpecific = AddItem(strNode,hVahicularFaci);
			GetTreeCtrl().SetItemData(hRoutesSpecific,(DWORD_PTR)pNodeData);

			GetTreeCtrl().Expand(hVahicularFaci,TVE_EXPAND);
		}
	}


	GetTreeCtrl().Expand(hAirport,TVE_EXPAND);

}

CTermPlanDoc *CAirsideMSView::GetARCDocument()
{
	return (CTermPlanDoc *)GetDocument();
}
HTREEITEM CAirsideMSView::AddItem(const CString& strNode,HTREEITEM hParent,HTREEITEM hInsertAfter,int nImage ,int nSelectImage )
{
	COOLTREE_NODE_INFO cni;
	ALTObjectList vObject;
	CARCBaseTree::InitNodeInfo(this,cni);
	cni.nImage = nImage;
	cni.nImageSeled = nSelectImage;
	return GetTreeCtrl().InsertItem(strNode,cni,FALSE,FALSE,hParent,TVI_LAST);
//	return GetTreeCtrl().InsertItem(strNode,nImage,nSelectImage,hParent,hInsertAfter);
}
HTREEITEM CAirsideMSView::AddARCBaseTreeItem(const CString& strNode,HTREEITEM hParent /* = TVI_ROOT */,NODE_EDIT_TYPE net,NODE_TYPE nt,HTREEITEM hInsertAfter /* = TVI_LAST */,int nImage /* = ID_NODEIMG_DEFAULT */,int nSelectImage /* = ID_NODEIMG_DEFAULT */)
{
	COOLTREE_NODE_INFO cni;
	ALTObjectList vObject;
	CARCBaseTree::InitNodeInfo(this,cni);
	cni.nt=nt;
	cni.net=net;
	cni.nImage = nImage;
	cni.nImageSeled = nSelectImage;
	return GetTreeCtrl().InsertItem(strNode,cni,FALSE,FALSE,hParent,TVI_LAST);
}

LRESULT CAirsideMSView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case WM_LBUTTONDBLCLK:
		OnLButtonDblClk(wParam,lParam);
		break;
	case ENDLABELEDIT:
		OnEndLabelEdit(wParam,lParam);
		break;
	case BEGINLABELEDIT:
		OnBeginLabelEdit(wParam,lParam);
		break;
	case SELCHANGED:
		OnSelChanged(wParam,lParam);
		break;
	case UM_CEW_EDITSPIN_END:
		{
			CString strTemp = _T("");
			CNodeData* pNodeData = NULL;
			SurfaceType emSurType = No_Type; 
			HTREEITEM hItem = (HTREEITEM)wParam;
			HTREEITEM hParent = NULL;
			HTREEITEM hLevel = NULL;
			int nIndex = -1;
			CString strValue=*((CString*)lParam);
			if (m_nType == 0)
			{
				hParent = GetTreeCtrl().GetParentItem(hItem);
				pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hParent);
			}
			else
			{
				hLevel = GetTreeCtrl().GetParentItem(hItem);
				hParent = getLevelItem(hItem,2);
				pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hParent);
			}
			emSurType = (SurfaceType)pNodeData->nOtherData;
			switch(emSurType)
			{
			case No_Type: 
				{
					Taxiway taxiObj;
					taxiObj.ReadObject(pNodeData->dwData);
					if (m_nType == 0)
					{
						taxiObj.SetOffset(CARCLengthUnit::ConvertLength(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,atof(strValue)));
						GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(atof(strValue)*100));
						strTemp.Format(_T("%s %.2f"),m_strData,atof(strValue));
					}
					else if(m_nType == 1)
					{
						pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel);
						nIndex = taxiObj.getSurface().findIndex(pNodeData->dwData);
						taxiObj.SetAngle(nIndex,atoi(strValue));
						GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(taxiObj.GetAngle(nIndex)*100));
						strTemp.Format(_T("%s %d"),m_strData,atoi(strValue));
					}
					else if(m_nType == 2)
					{
						pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel);
						nIndex = taxiObj.getSurface().findIndex(pNodeData->dwData);
						taxiObj.SetDistance(nIndex,CARCLengthUnit::ConvertLength(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,atof(strValue)));
						GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(atof(strValue)*100));
						strTemp.Format(_T("%s %.2f"),m_strData,atof(strValue));
					}
					taxiObj.UpdateSurfaceData();
					GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&taxiObj );
				}
				break;
			case ThresHold_Type:
				{
					Runway runObj;
					HTREEITEM hLevel2 = GetTreeCtrl().GetParentItem(hParent);
					pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel2);
					runObj.ReadObject(pNodeData->dwData);
					if (pNodeData->nOtherData == 0)
					{
						if (m_nType == 0)
						{
							runObj.SetMark1ThresholdOffset(CARCLengthUnit::ConvertLength(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,atof(strValue)));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(atof(strValue)*100));
							strTemp.Format(_T("%s %.2f"),m_strData,atof(strValue));
						}
						else if(m_nType == 1)
						{
							pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel);
							nIndex = runObj.GetMark1Threshold().findIndex(pNodeData->dwData);
							runObj.SetMark1ThresholdAngle(nIndex,atoi(strValue));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(runObj.GetMark1ThresholdAngle(nIndex)*100));
							strTemp.Format(_T("%s %d"),m_strData,atoi(strValue));
						}
						else if (m_nType == 2)
						{
							pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel);
							nIndex = runObj.GetMark1Threshold().findIndex(pNodeData->dwData);
							runObj.SetMark1ThresholdDistance(nIndex,CARCLengthUnit::ConvertLength(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,atof(strValue)));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(atof(strValue)*100));
							strTemp.Format(_T("%s %.2f"),m_strData,atof(strValue));
						}
					}
					else
					{
						if (m_nType == 0)
						{
							runObj.SetMark2ThresholdOffset(CARCLengthUnit::ConvertLength(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,atof(strValue)));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(atof(strValue)*100));
							strTemp.Format(_T("%s %.2f"),m_strData,atof(strValue));
						}
						else if(m_nType == 1)
						{
							pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel);
							nIndex = runObj.GetMark2Threshold().findIndex(pNodeData->dwData);
							runObj.SetMark2ThresholdAngle(nIndex,atoi(strValue));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(runObj.GetMark2ThresholdAngle(nIndex)*100));
							strTemp.Format(_T("%s %d"),m_strData,atoi(strValue));
						}
						else if (m_nType == 2)
						{
							pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel);
							nIndex = runObj.GetMark2Threshold().findIndex(pNodeData->dwData);
							runObj.SetMark2ThresholdDistance(nIndex,CARCLengthUnit::ConvertLength(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,atof(strValue)));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(atof(strValue)*100));
							strTemp.Format(_T("%s %.2f"),m_strData,atof(strValue));
						}
					}
					runObj.UpdataSurfaceData();
					GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&runObj );
				}
				break;
			case Lateral_Type:
				{
					Runway runObj;
					HTREEITEM hLevel2 = GetTreeCtrl().GetParentItem(hParent);
					pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel2);
					runObj.ReadObject(pNodeData->dwData);
					if (pNodeData->nOtherData == 0)
					{
						if (m_nType == 0)
						{
							runObj.SetMark1LateralOffset(CARCLengthUnit::ConvertLength(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,atof(strValue)));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(atof(strValue)*100));
							strTemp.Format(_T("%s %.2f"),m_strData,atof(strValue));
						}
						else if(m_nType == 1)
						{
							pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel);
							nIndex = runObj.GetMark1Lateral().findIndex(pNodeData->dwData);
							runObj.SetMark1LateralAngle(nIndex,atoi(strValue));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(runObj.GetMark1LateralAngle(nIndex)*100));
							strTemp.Format(_T("%s %d"),m_strData,atoi(strValue));
						}
						else if (m_nType == 2)
						{
							pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel);
							nIndex = runObj.GetMark1Lateral().findIndex(pNodeData->dwData);
							runObj.SetMark1LateralDistance(nIndex,CARCLengthUnit::ConvertLength(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,atof(strValue)));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(atof(strValue)*100));
							strTemp.Format(_T("%s %.2f"),m_strData,atof(strValue));
						}
					}
					else
					{
						if (m_nType == 0)
						{
							runObj.SetMark2LateralOffset(CARCLengthUnit::ConvertLength(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,atof(strValue)));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(atof(strValue)*100));
							strTemp.Format(_T("%s %.2f"),m_strData,atof(strValue));
						}
						else if(m_nType == 1)
						{	
							pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel);
							nIndex = runObj.GetMark2Lateral().findIndex(pNodeData->dwData);
							runObj.SetMark2LateralAngle(nIndex,atoi(strValue));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(runObj.GetMark2LateralAngle(nIndex)*100));
							strTemp.Format(_T("%s %d"),m_strData,atoi(strValue));
						}
						else if (m_nType == 2)
						{
							pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel);
							nIndex = runObj.GetMark2Lateral().findIndex(pNodeData->dwData);
							runObj.SetMark2LateralDistance(nIndex,CARCLengthUnit::ConvertLength(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,atof(strValue)));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(atof(strValue)*100));
							strTemp.Format(_T("%s %.2f"),m_strData,atof(strValue));
						}
					}
					runObj.UpdataSurfaceData();
					GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&runObj );
				}
				break;
			case EndOfRunway_Type:
				{	
					Runway runObj;
					HTREEITEM hLevel2 = GetTreeCtrl().GetParentItem(hParent);
					pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel2);
					runObj.ReadObject(pNodeData->dwData);
					if (pNodeData->nOtherData == 0)
					{
						if (m_nType == 0)
						{
							runObj.SetMark1EndOfRunwayOffset(CARCLengthUnit::ConvertLength(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,atof(strValue)));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(atof(strValue)*100));
							strTemp.Format(_T("%s %.2f"),m_strData,atof(strValue));
						}
						else if(m_nType == 1)
						{
							pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel);
							nIndex = runObj.GetMark1EnOfRunway().findIndex(pNodeData->dwData);
							runObj.SetMark1EndOfRunwayAngle(nIndex,atoi(strValue));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(runObj.GetMark1EndOfRunwayAngle(nIndex)*100));
							strTemp.Format(_T("%s %d"),m_strData,atoi(strValue));
						}
						else if (m_nType == 2)
						{
							pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel);
							nIndex = runObj.GetMark1EnOfRunway().findIndex(pNodeData->dwData);
							runObj.SetMark1EndOfRunwayDistance(nIndex,CARCLengthUnit::ConvertLength(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,atof(strValue)));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(atof(strValue)*100));
							strTemp.Format(_T("%s %.2f"),m_strData,atof(strValue));
						}
					}
					else
					{
						if (m_nType == 0)
						{
							runObj.SetMark2EndOfRunwayOffset(CARCLengthUnit::ConvertLength(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,atof(strValue)));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(atof(strValue)*100));
							strTemp.Format(_T("%s %.2f"),m_strData,atof(strValue));
						}
						else if(m_nType == 1)
						{
							pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel);
							nIndex = runObj.GetMark2EndOfRunway().findIndex(pNodeData->dwData);
							runObj.SetMark2EndOfRunwayAngle(nIndex,atoi(strValue));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(runObj.GetMark2EndOfRunwayAngle(nIndex)*100));
							strTemp.Format(_T("%s %d"),m_strData,atoi(strValue));
						}
						else if (m_nType == 2)
						{
							pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hLevel);
							nIndex = runObj.GetMark2EndOfRunway().findIndex(pNodeData->dwData);
							runObj.SetMark2EndOfRunwayDistance(nIndex,CARCLengthUnit::ConvertLength(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,atof(strValue)));
							GetTreeCtrl().SetItemData(hItem,(DWORD_PTR)(atof(strValue)*100));
							strTemp.Format(_T("%s %.2f"),m_strData,atof(strValue));
						}
					}
					runObj.UpdataSurfaceData();
					GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&runObj );
				}
				break;
			default:
				break;
			}
			GetTreeCtrl().SetItemText(hItem,strTemp);
		}
	default:
		break;
	}
	return CView::WindowProc(message, wParam, lParam);
}

HTREEITEM CAirsideMSView::getLevelItem(HTREEITEM hItem,int nLevel)
{
	while (hItem && nLevel)
	{
		hItem = GetTreeCtrl().GetParentItem(hItem);
		nLevel--;
	}
	return hItem;
}

void CAirsideMSView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CView::OnRButtonDown(nFlags, point);
}

void CAirsideMSView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CPoint cliPt = point;
	ScreenToClient(&cliPt);
	CNewMenu menu;
	CMenu *pCtxMenu = NULL;
	UINT uFlags;
	HTREEITEM hItem = GetTreeCtrl().HitTest(cliPt, &uFlags);
	m_hRightClkItem = hItem;
	if((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		if(GetTreeCtrl().GetItemText(hItem).Find(':') != -1)
			return;
		CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( hItem );

		if(pNodeData != NULL)
		{
			//object root menu
			
			if(pNodeData->nIDResource != NULL )
			{				
				menu.LoadMenu(pNodeData->nIDResource);
//				pCtxMenu = menu.GetSubMenu(0);
				pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));

				UINT nMenuID = 0;
				CString strMenuCaption = _T("");

				if (pNodeData->nodeType == NodeType_ObjectRoot)
				{	
					switch(pNodeData->nSubType)
					{
					case ALT_RUNWAY:
						nMenuID = ID_MENUITEM_OBJECTROOT + 1;
						strMenuCaption = _T("Add Runway...");
						break;

					case ALT_TAXIWAY:
						nMenuID = ID_MENUITEM_OBJECTROOT + 2;
						strMenuCaption = _T("Add Taxiway...");
						break;

					case ALT_STAND:
						nMenuID = ID_MENUITEM_OBJECTROOT + 3;
						strMenuCaption = _T("Add Stand...");
						break;

					case ALT_GROUNDROUTE:
						nMenuID = ID_MENUITEM_OBJECTROOT + 4;
						strMenuCaption = _T("Taxi Routes...");
						break;

					case ALT_DEICEBAY:
						nMenuID = ID_MENUITEM_OBJECTROOT + 5;
						strMenuCaption = _T("Add Deice Station...");
						break;

					case ALT_WAYPOINT:
						nMenuID = ID_MENUITEM_OBJECTROOT + 6;
						strMenuCaption = _T("Add Waypoint...");
						break;

					case ALT_CONTOUR:
						nMenuID = ID_MENUITEM_OBJECTROOT + 7;
						strMenuCaption = _T("Add Contour...");
						break;

					//case ObjectRoot_Aprons:
					//	nMenuID=ID_MENUITEM_OBJECTROOT+8;
					//	strMenuCaption=_T("Add Apron");
					//	break;

					case ALT_HOLD:
						nMenuID = ID_MENUITEM_OBJECTROOT +9;
						strMenuCaption = _T("Add Hold...");
						break;
					case ALT_SECTOR:
						nMenuID = ID_MENUITEM_OBJECTROOT + 10;
						strMenuCaption = _T("Add Sector...");
						break;
					case ALT_SURFACE:	
						nMenuID = ID_MENUITEM_OBJECTROOT + 11;
						strMenuCaption = _T("Add Surface Area...");
						break;
					case ALT_STRUCTURE:
						nMenuID = ID_MENUITEM_OBJECTROOT + 12;
						strMenuCaption = _T("Add Structure...");
						break;

					case ALT_STRETCH:
						nMenuID = ID_MENUITEM_OBJECTROOT + 13;
						strMenuCaption = _T("Add Stretch...");
						break;

					case ALT_INTERSECTIONS:
						nMenuID = ID_MENUITEM_OBJECTROOT + 14;
						strMenuCaption = _T("Add Intersections...");
						break;

					case ALT_ROUNDABOUT:
						nMenuID = ID_MENUITEM_OBJECTROOT + 15;
						strMenuCaption = _T("Add Roundabout...");
						break;

					case ALT_TURNOFF:
						nMenuID = ID_MENUITEM_OBJECTROOT + 16;
						strMenuCaption = _T("Add TurnOff...");
						break;

					case ALT_LANEADAPTER:
						nMenuID = ID_MENUITEM_OBJECTROOT + 17;
						strMenuCaption = _T("Add LaneAdapter...");
						break;

					case ALT_VEHICLEPOOLPARKING:
						nMenuID = ID_MENUITEM_OBJECTROOT + 18;
						strMenuCaption = _T("Add PoolParking...");
						break;

					case ALT_NOSEINPARKING:
						nMenuID = ID_MENUITEM_OBJECTROOT + 19;
						strMenuCaption = _T("Add NoseInParking...");
						break;

					case ALT_TRAFFICLIGHT:
						nMenuID = ID_MENUITEM_OBJECTROOT + 20;
						strMenuCaption = _T("Add TrafficLight...");
						break;

					case ALT_TOLLGATE:
						nMenuID = ID_MENUITEM_OBJECTROOT + 21;
						strMenuCaption = _T("Add TollGate...");
						break;

					case ALT_STOPSIGN:
						nMenuID = ID_MENUITEM_OBJECTROOT + 22;
						strMenuCaption = _T("Add StopSign...");
						break;

					case ALT_YIELDSIGN:
						nMenuID = ID_MENUITEM_OBJECTROOT + 23;
						strMenuCaption = _T("Add YieldSign...");
						break;

					case ALT_HELIPORT:
						nMenuID = ID_MENUITEM_OBJECTROOT + 24;
						strMenuCaption = _T("Add Heliport...");
						break;

					case ALT_PARKINGPLACE:
						nMenuID = ID_MENUITEM_OBJECTROOT + 27;
						strMenuCaption = _T("Add Parking Place...");
						break;

					case ALT_DRIVEROAD:
						nMenuID = ID_MENUITEM_OBJECTROOT + 28;
						strMenuCaption = _T("Add Drive Road...");
						break;

					case ALT_REPORTINGAREA:
						nMenuID = ID_MENUITEM_OBJECTROOT + 29;
						strMenuCaption = _T("Add Reporting Area...");
						break;
					case ALT_AIRROUTE:
					//case ALT_WALLSHAPE:
					//	strMenuCaption = _T("Add Wallshape");
					//	break;

					//Start position
					case ALT_STARTPOSITION:
						nMenuID = ID_MENUITEM_OBJECTROOT + 31;
						strMenuCaption = _T("Add Start Position...");
						break;
					// Meeting Point
					case ALT_MEETINGPOINT:
						nMenuID = ID_MENUITEM_OBJECTROOT + 32;
						strMenuCaption = _T("Add Meeting Point...");
						break;
					default:
						break;
					} 
				
				
					if (nMenuID !=0 &&!strMenuCaption.IsEmpty())
					{
						pCtxMenu->InsertMenu(0, MF_STRING | MF_BYPOSITION, nMenuID, strMenuCaption);
					}
					//--------------------------------------------------------------------------------------------------------
					if (pNodeData->nSubType ==ALT_WAYPOINT)
					{
						UINT nNewMenuID = ID_MENUITEM_OBJECTROOT + 25;
						CString strNewMenuCaption = _T("Import Waypoints");
						pCtxMenu->InsertMenu(1, MF_STRING | MF_BYPOSITION, nNewMenuID, strNewMenuCaption);
					}
					if (pNodeData->nSubType == ALT_STRETCH)
					{
						UINT nMenuID = ID_MENUITEM_NOTOBJECTITEM + 10;
						CString strNewMenuCaption = _T("Set Options...");
						pCtxMenu->InsertMenu(1, MF_STRING | MF_BYPOSITION, nMenuID, strNewMenuCaption);

					}
					if(pNodeData->nSubType == ALT_REPORTINGAREA)
					{
						UINT nNewMenuID = ID_MENUITEM_OBJECTROOT + 30;
						CString strNewMenuCaption = _T("Display property...");
						pCtxMenu->InsertMenu(1, MF_STRING | MF_BYPOSITION, nNewMenuID, strNewMenuCaption);
					}

 					pCtxMenu->InsertMenu(1, MF_STRING | MF_BYPOSITION, ID_MENUITEM_LOCK_ALTOBJECT, _T("Lock All"));
					pCtxMenu->InsertMenu(2, MF_STRING | MF_BYPOSITION, ID_MENUITEM_UNLOCK_ALTOBJECT, _T("Unlock All"));
					//--------------------------------------------------------------------------------------------------------
				}
				else if (pNodeData->nodeType == NodeType_ObjectGroup)
				{
 					pCtxMenu->InsertMenu(0, MF_STRING | MF_BYPOSITION, ID_MENUITEM_LOCK_ALTOBJECT, _T("Lock Group"));
					pCtxMenu->InsertMenu(1, MF_STRING | MF_BYPOSITION, ID_MENUITEM_UNLOCK_ALTOBJECT, _T("Unlock Group"));
				}
				else if (pNodeData->nodeType == NodeType_Object)
				{
					// read lock status
					bool bFailed = true;
					bool bNowLocked = ALTObject::GetLockedByeID(pNodeData->dwData, bFailed);
					UINT nEnableCode = bNowLocked ? MF_BYCOMMAND | MF_DISABLED | MF_GRAYED : MF_BYCOMMAND | MF_ENABLED;

					UINT nLockInsertPosition = 1;
					if (pNodeData->nSubType == ALT_CONTOUR)
					{
						nLockInsertPosition = 2;
						nMenuID = ID_MENUITEM_OBJECTROOT + 7;
						strMenuCaption = _T("Add Contour...");

						pCtxMenu->InsertMenu(0, MF_STRING | MF_BYPOSITION, nMenuID, strMenuCaption);
						pCtxMenu->EnableMenuItem(nMenuID, nEnableCode);
						pCtxMenu->EnableMenuItem(ID_ADDCONTOUR_EDITCONTROLPOINT, nEnableCode);
						pCtxMenu->EnableMenuItem(ID_DISABLE_EIDTPOINT, nEnableCode);
					}

					// add the lock menu
					pCtxMenu->InsertMenu(nLockInsertPosition, MF_STRING | MF_BYPOSITION,
						bNowLocked ? ID_MENUITEM_UNLOCK_ALTOBJECT : ID_MENUITEM_LOCK_ALTOBJECT,
						bNowLocked ? _T("Unlock") : _T("Lock"));
					pCtxMenu->EnableMenuItem(ID_AIRSIDEOBJECT_DELETE, nEnableCode);
				}
			}
		}
	}
	if(pCtxMenu != NULL)
	{
		//UpdatePopMenu(this, pCtxMenu);
		pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON , point.x, point.y, AfxGetMainWnd());
	}

}
void CAirsideMSView::OnNewAirsideObject(UINT nID)
{
	HTREEITEM hItem = m_hRightClkItem;
	if(hItem != NULL)
	{
		CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( hItem );

		if (nID == ID_MENUITEM_OBJECTROOT + 4) // Ground routes
		{
			if (NULL == m_pAirsideNodeHandler)
			{
				m_pAirsideNodeHandler = new AirsideGUI::NodeViewDbClickHandler( 
					GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
			}			
			
			m_pAirsideNodeHandler->GroundRoutesDlg(GetARCDocument()->GetProjectID(), pNodeData->nOtherData, this);
			
			//CDlgGroundRoutes dlg(GetARCDocument()->GetProjectID(), pNodeData->nOtherData);
			//dlg.DoModal();
			return;
		}
		//----------------------------------------------------------------------------------------------------------------------------------------------
		if (nID == ID_MENUITEM_OBJECTROOT + 25) // Ground routes
		{
			AirsideGUI::NodeViewDbClickHandler AirsideNodeHandler( 
				GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
			AirsideNodeHandler.WaypointImport(m_nProjID);
			
			GetDocument()->UpdateAllViews(this,VM_NEW_SINGLE_ALTOBJECT,NULL);
			CNodeData* pNodeData = reinterpret_cast<CNodeData*>(GetTreeCtrl().GetItemData( m_hWaypoint ));
			pNodeData->bHasExpand = false;
			LoadObjects(m_hWaypoint);		
			return;

		}
		//----------------------------------------------------------------------------------------------------------------------------------------------
		if(pNodeData == NULL)
		 return;

		CDialog *pDlg = NULL;
		pDlg = GetObjectDefineDlg(pNodeData,-1);
		if(pDlg)
		{

			if(pDlg->DoModal() == IDOK)
			{
				//if( GetARCDocument() && GetARCDocument()->Get3DViewParent() )
				//	GetARCDocument()->Get3DViewParent()->GetAirside3D()->AddObject(((CAirsideObjectBaseDlg *)pDlg)->GetObject());

				GetARCDocument()->UpdateAllViews(this,VM_NEW_SINGLE_ALTOBJECT,(CObject*)((CAirsideObjectBaseDlg *)pDlg)->GetObject());		
				AddObjectToTree(m_hRightClkItem,((CAirsideObjectBaseDlg *)pDlg)->GetObject());
				HTREEITEM hChildItem = NULL;
				while( (hChildItem = GetTreeCtrl().GetChildItem(m_hObjSurface)) != NULL)
				{
					GetTreeCtrl().DeleteItem(hChildItem);		
				}
				pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hObjSurface);
				pNodeData->bHasExpand = false;
				AddRunwayandTaxiwaytoTree(m_hObjSurface);
				GetTreeCtrl().Expand(m_hObjSurface,TVE_EXPAND);
			}else
			{
				/*if( GetARCDocument() && GetARCDocument()->Get3DViewParent() )
					GetARCDocument()->Get3DViewParent()->GetAirside3D()->AddObject(((CAirsideObjectBaseDlg *)pDlg)->GetObject());

				if( GetARCDocument() && GetARCDocument()->Get3DViewParent() )
					GetARCDocument()->Get3DViewParent()->GetAirside3D()->DeleteObject(-1);*/
				GetARCDocument()->UpdateAllViews(this, VM_DELECT_ALTOBJECT);
			}
			delete pDlg;
		}
	}
}

void CAirsideMSView::AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject)
{
	if(LoadObjects(hObjRoot))
		return;

	ALTObjectID objName;
	pObject->getObjName(objName);
	HTREEITEM hParentItem = hObjRoot;
	bool bObjNode = false;
	CString strNodeName = _T("");

	//level1
	//int nLevel = 0;
	//strNodeName = objName.at(nLevel);
	//if (objName.at(nLevel+1) == _T("")) //object node
	//{
	//	//FindObjNode();
	//}
	CNodeData *pParentData =(CNodeData *)GetTreeCtrl().GetItemData(hObjRoot);
	if (pParentData == NULL)
		return;

	CNodeData *pNodeData = NULL;

	if (pObject->GetType() == ALT_CONTOUR)
	{
		HTREEITEM hTreeItem = AddItem(objName.GetIDString(), hParentItem);

		pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Object);
		pNodeData->dwData = pObject->getID();
		pNodeData->nSubType = pObject->GetType();
		
		pNodeData->nOtherData = pParentData->nOtherData;
		GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
		GetTreeCtrl().SelectItem(hTreeItem);

		Contour* pContour = (Contour*)pObject;
		if (pContour->GetParentID() == -1)
		{
			pNodeData->nIDResource = IDR_MENU_COUNT_PROPERTY;
		}
		else
		{
			pNodeData->nIDResource = IDR_MENU_AIARSIDEOBJECT;
		}
		for (int i=0; i<pContour->GetChildCount(); i++)
		{
			Contour* pChildContour = pContour->GetChildItem(i);
			ASSERT(pChildContour);

			LoadChildContour(hTreeItem, pChildContour);
		}
		GetTreeCtrl().Expand(hTreeItem, TVE_EXPAND);
		GetTreeCtrl().Expand(hTreeItem, TVE_COLLAPSE);		   
	}
	else if (pObject->GetType() == ALT_VEHICLEPOOLPARKING || pObject->GetType() == ALT_PARKINGLOT)
	{

	}
	else
	{
		for (int nLevel =0; nLevel< OBJECT_STRING_LEVEL; ++nLevel)
		{
			if (nLevel != OBJECT_STRING_LEVEL -1 )
			{
				HTREEITEM hItem = FindObjNode(hParentItem,objName.m_val[nLevel].c_str());
				if (hItem != NULL)
				{
					hParentItem = hItem;
					continue;
				}
				if (objName.m_val[nLevel+1] != _T(""))
				{			
					HTREEITEM hTreeItem = AddItem(objName.m_val[nLevel].c_str(),hParentItem);	
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_ObjectGroup);
					pNodeData->nSubType = pObject->GetType();
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
					hParentItem = hTreeItem;

					continue;
				}
				else //objNode
				{
					HTREEITEM hTreeItem = AddItem(objName.m_val[nLevel].c_str(),hParentItem);
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Object);
					pNodeData->dwData = pObject->getID();
					pNodeData->nSubType = pObject->GetType();
					pNodeData->nIDResource = IDR_MENU_AIARSIDEOBJECT;
					pNodeData->nOtherData = pParentData->nOtherData;
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
					GetTreeCtrl().SelectItem(hTreeItem);
					break;
				}

			}
			else
			{
				HTREEITEM hTreeItem = AddItem(objName.m_val[nLevel].c_str(),hParentItem);
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Object);
				pNodeData->dwData = pObject->getID();
				pNodeData->nSubType = pObject->GetType();
				pNodeData->nIDResource = IDR_MENU_AIARSIDEOBJECT;
				pNodeData->nOtherData = pParentData->nOtherData;
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
				GetTreeCtrl().SelectItem(hTreeItem);
				break;
			}
		}
	}	
}

HTREEITEM CAirsideMSView::FindObjNode(HTREEITEM hParentItem,const CString& strNodeText)
{
	HTREEITEM hRetItem = NULL;

	HTREEITEM hChildItem = GetTreeCtrl().GetChildItem(hParentItem);

	//find
	while (hChildItem)
	{
		CString strItemText = GetTreeCtrl().GetItemText(hChildItem);
		if(strItemText.CompareNoCase(strNodeText) == 0)
		{
			hRetItem = hChildItem;
			break;
		}
		hChildItem = GetTreeCtrl().GetNextSiblingItem(hChildItem);
	}
	return hRetItem;
}

CDialog* CAirsideMSView::GetObjectDefineDlg(CNodeData* pNodeData,int nObjID)
{
	if (pNodeData->nodeType == NodeType_Object)
	{
		return CAirsideObjectBaseDlg::NewObjectDlg(nObjID,(ALTOBJECT_TYPE)pNodeData->nSubType, (int)pNodeData->dwData, pNodeData->nOtherData ,m_nProjID, this);
	}

	return CAirsideObjectBaseDlg::NewObjectDlg(nObjID,(ALTOBJECT_TYPE)pNodeData->nSubType, -1, pNodeData->nOtherData ,m_nProjID, this);
}

LRESULT CAirsideMSView::OnSelChanged(WPARAM wParam, LPARAM lParam)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)wParam;
	HTREEITEM hItem = NULL;
	CNodeData *pNodeData = NULL;
	CAirRoute* pAirRoute = NULL;
	CString strNode;
	strNode.LoadString(IDS_TVNN_AIRROUTS);
	hItem = pNMTreeView->itemNew.hItem;
	if (hItem == NULL) 
		return 0;

	m_strData = GetTreeCtrl().GetItemText(hItem);
	int nPos = m_strData.Find(':');
	CString strData = _T("");
	if (nPos != -1)
	{
		m_strData = m_strData.Left(nPos + 1);
		nPos = m_strData.Find('(');
		strData = m_strData.Left(nPos - 1);
		for (int i = 0; i < 3; i++)
		{
			if (strData.CompareNoCase(strType[i]) == 0)
			{
				m_nType = i;
				return 0;
			}
		}
	}

	pNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hItem));

	if (pNodeData == NULL)
		return 0;
	if(pNodeData->nodeType != NodeType_Object)
		return 0;	

	GetDocument()->UpdateAllViews(this,VM_SELECT_ALTOBJECT,(CObject *)pNodeData->dwData);

	return 0;
//	*pResult = 0;
}
LRESULT CAirsideMSView::OnItemExpanding(WPARAM wParam, LPARAM lParam)
{
	//*pResult = FALSE;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)wParam;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;

	CString strText = GetTreeCtrl().GetItemText(hItem);
	if(strText.IsEmpty())
		return 0;
	CNodeData* pNodeData = reinterpret_cast<CNodeData*>(GetTreeCtrl().GetItemData( hItem ));
	if (pNodeData == NULL)
	{
		return 0;
	}
	if (pNodeData->bHasExpand == false)
	{
		HTREEITEM hChildItem = NULL;
		if (pNodeData->nodeType == NodeType_Dlg && pNodeData->nSubType == Dlg_AirRoute)
		{
			pNodeData->bHasExpand = true;
			while( (hChildItem = GetTreeCtrl().GetChildItem(hItem)) != NULL)
			{
				GetTreeCtrl().DeleteItem(hChildItem);		
			}
			AirRouteList m_airRouteList;
			m_airRouteList.ReadData(pNodeData->dwData);
			int nAirRouteCount = m_airRouteList.GetAirRouteCount();

			for (int i =0; i < nAirRouteCount; i++)
			{
				CAirRoute *pAirRoute = m_airRouteList.GetAirRoute(i);

				ASSERT(pAirRoute != NULL);
				AddAirRouteToTree(hItem,pAirRoute);
			}
		}

		CString strNode = _T("");
		strNode.LoadString(IDS_TVNN_SURFACE);
		if (!strText.CompareNoCase(strNode))
		{
			while( (hChildItem = GetTreeCtrl().GetChildItem(hItem)) != NULL)
			{
				GetTreeCtrl().DeleteItem(hChildItem);		
			}
			pNodeData->bHasExpand = true;
			AddRunwayandTaxiwaytoTree(hItem);
		}
	}
	//CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( hItem );

	//if(pNodeData == NULL)
	//	return;
	//if (pNodeData->nodeType != NodeType_ObjectRoot)
	//	return;

	//if(pNodeData->bHasExpand == true)
	//	return;

	//pNodeData->bHasExpand = true;
	LoadObjects(hItem);

	return 0;



}
void CAirsideMSView::OnRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
	*pResult = 1;
}
LRESULT CAirsideMSView::OnBeginLabelEdit(WPARAM wParam, LPARAM lParam) 
{
//	*pResult = TRUE;
	NMTVDISPINFO* pDispInfo = (NMTVDISPINFO*)wParam;
	LRESULT* pResult = (LRESULT*)lParam;

	if(GetTreeCtrl().GetItemText(pDispInfo->item.hItem).Find(':') != -1) 
	{
		*pResult = TRUE;
		return FALSE;
	}
	CNodeData *pNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(pDispInfo->item.hItem));
	if (pNodeData == NULL)
	{
		*pResult = FALSE;
		return FALSE;
	}
	if(pNodeData->nodeType != NodeType_Airport && pNodeData->nodeType != NodeType_Level)
	{
		*pResult = TRUE;
		return FALSE;	
	}
	*pResult = FALSE;
	return FALSE;
}

LRESULT CAirsideMSView::OnEndLabelEdit(WPARAM wParam, LPARAM lParam) 
{
	NMTVDISPINFO* pDispInfo = (NMTVDISPINFO*)wParam;
	LRESULT* pResult = (LRESULT*)lParam;

	CNodeData *pNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(pDispInfo->item.hItem));
	if (pNodeData == NULL)
		return 0;
	if(pNodeData->nodeType != NodeType_Airport && pNodeData->nodeType != NodeType_Level)
		return 0;	

	switch(pNodeData->nodeType) {
	case NodeType_Airport:
		{
			try
			{
				ALTAirport::UpdateName(pDispInfo->item.pszText,pNodeData->dwData);
			}
			catch (CADOException& e)
			{
				e.ErrorMessage();
				MessageBox(_T("Unable to modify airport name."));
				*pResult = FALSE;
			}
		}
		break;
	case NodeType_Level:
		{
			try
			{
				CAirsideGround airsideGround;
				airsideGround.ReadData(pNodeData->dwData);
				CString strName(pDispInfo->item.pszText);
				strName.Trim();
				if( !strName.IsEmpty() )
					airsideGround.Rename(pDispInfo->item.pszText);
			}

			catch (CADOException& e)
			{
				e.ErrorMessage();
				MessageBox(_T("Unable to modify airport name."));
				*pResult = FALSE;
			}

		}
		break;
	default:
		break;
	}

	*pResult = TRUE;  // return FALSE to cancel edit, TRUE to accept it.
	return 0;
}

BOOL CAirsideMSView::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CView::PreCreateWindow(cs))
		return FALSE;

	cs.style |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS|WS_CLIPCHILDREN;

	return TRUE;
}

void  CAirsideMSView::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 1;

	//after the double click message
	//it will send another message --WM_ITEMEXPLANDING-- at the time
	//and it maybe lead to an debug error,
	//because the item selected has been removed ,before the message

	HTREEITEM hSelectedItem = GetTreeCtrl().GetSelectedItem();
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( hSelectedItem );
	if (pNodeData == NULL)
		return;
	if (pNodeData->nodeType != NodeType_Object)
		return;
	

	TRACE(_T("the selected Tree Item: ") + GetTreeCtrl().GetItemText(hSelectedItem));
	int nObjID = pNodeData->dwData;

	/*ALTObject * pObj = NULL;

	if(GetARCDocument()->Get3DViewParent() && GetARCDocument()->Get3DViewParent()->GetAirside3D() )
	{
		ALTObject3D * pObj3D = GetARCDocument()->Get3DViewParent()->GetAirside3D()->GetObject3D(nObjID);
		if(pObj3D)
		{
			pObj = pObj3D->GetObject();
		}
	}*/

//	CDialog *pDlg = GetObjectDefineDlg(pNodeData,nObjID);
	CDialog *pDlg = /*pObj?GetObjectDefineDlg(pNodeData,pObj) :*/ GetObjectDefineDlg(pNodeData,nObjID);
	if (pDlg)
	{
		if(pDlg->DoModal() == IDOK)
		{
			if(((CAirsideObjectBaseDlg*)pDlg)->m_bNameModified
				|| ((CAirsideObjectBaseDlg*)pDlg)->m_bPathModified )
			{
				HTREEITEM hObjRoot = GetObjRootNode(hSelectedItem);
				DeleteObjectFromTree(hSelectedItem);
				AddObjectToTree(hObjRoot,((CAirsideObjectBaseDlg*)pDlg)->GetObject());
				
			}
			//GetDocument()->UpdateAllViews(this,VM_MODIFY_ALTOBJECT,(CObject*)((CAirsideObjectBaseDlg*)pDlg)->GetObject());
		}
		delete pDlg;
	}

}

DECLARE_FLIGHTTYPE_SELECT_CALLBACK()


static CProbDistEntry*  SelectProbDistEntry(CWnd* pParent, InputAirside* pInputAirside)
{
	CDlgProbDist dlg( pInputAirside->m_pAirportDB, true );
	CProbDistEntry* pPDEntry;
	if(dlg.DoModal() == IDOK)
		pPDEntry = dlg.GetSelectedPD();
	else
		pPDEntry = NULL;
	return pPDEntry;
}

void CAirsideMSView::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	UINT uFlags;
	CPoint point;
	point = *(CPoint*)lParam;
	HTREEITEM hItem = GetTreeCtrl().HitTest(point, &uFlags);
	if((hItem == NULL) || !(TVHT_ONITEM & uFlags))
		return;

	m_hRightClkItem = hItem;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( hItem );

	if(GetTreeCtrl().GetItemText(hItem).Find(':') != -1)
		return;
	if(pNodeData != NULL && pNodeData->nodeType == NodeType_ObjectRoot)
	{
		if(pNodeData->nSubType == ALT_GROUNDROUTE)//ground routes.
		{ 
			if (NULL == m_pAirsideNodeHandler)
			{
				m_pAirsideNodeHandler = new AirsideGUI::NodeViewDbClickHandler( 
					GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
			}		
			m_pAirsideNodeHandler->GroundRoutesDlg(GetARCDocument()->GetProjectID(), pNodeData->nOtherData, this);
			return;
		}
		else if(pNodeData->nSubType == ALT_WALLSHAPE)
		{
			WallShape *pNewwallsh=new WallShape();
			pNewwallsh->SetTerminal(&(GetARCDocument()->GetTerminal()));
			DlgWallShapeProp dlg(pNewwallsh,this);
			if (IDOK == dlg.DoFakeModal())
			{
				//should new a surface here
				CTermPlanDoc* pDoc	= GetARCDocument(); 
				CString strProcName =dlg.m_csLevel1+ "-" + dlg.m_csLevel2 + "-" + dlg.m_csLevel3 + "-" +dlg.m_csLevel4;
				//		pNewwallsh->SetNameStr(strProcName);
				pNewwallsh->SetFloorNum(pDoc->m_nActiveFloor);
				pNewwallsh->SetPath(dlg.m_Wallpath);
				pDoc->GetCurWallShapeList().addShape( pNewwallsh);

				pDoc->GetCurWallShapeList().saveDataSet(pDoc->m_ProjInfo.path, false);
				pDoc->m_msManager.RefreshWallShape(pDoc->m_systemMode);

				LPARAM lHint = 0L;
				if (pDoc->m_systemMode == EnvMode_Terminal)
					lHint = UPDATETREE_TERMINAL;
				else if (pDoc->m_systemMode == EnvMode_AirSide)
					lHint = UPDATETREE_AIRSIDE;
				else
					ASSERT(0);

				m_pDocument->UpdateAllViews(NULL, lHint);
			}
			else
			{
				if(pNewwallsh)
					delete pNewwallsh;	
			}
			return;
		}
		CDialog *pDlg = NULL;
		pDlg = GetObjectDefineDlg(pNodeData,-1);
		if(pDlg)
		{

			if(pDlg->DoModal() == IDOK)
			{
				/*if( GetARCDocument() && GetARCDocument()->Get3DViewParent() )
					GetARCDocument()->Get3DViewParent()->GetAirside3D()->AddObject(((CAirsideObjectBaseDlg *)pDlg)->GetObject());*/

				GetDocument()->UpdateAllViews(this,VM_NEW_SINGLE_ALTOBJECT,(CObject*)((CAirsideObjectBaseDlg *)pDlg)->GetObject());		
				AddObjectToTree(hItem,((CAirsideObjectBaseDlg *)pDlg)->GetObject());

				HTREEITEM hChildItem = NULL;
				while( (hChildItem = GetTreeCtrl().GetChildItem(m_hObjSurface)) != NULL)
				{
					GetTreeCtrl().DeleteItem(hChildItem);		
				}
				pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hObjSurface);
				pNodeData->bHasExpand = false;
				AddRunwayandTaxiwaytoTree(m_hObjSurface);
			}
			else
			{
				/*if( GetARCDocument() && GetARCDocument()->Get3DViewParent() )
					GetARCDocument()->Get3DViewParent()->GetAirside3D()->AddObject(((CAirsideObjectBaseDlg *)pDlg)->GetObject());

				if( GetARCDocument() && GetARCDocument()->Get3DViewParent() )
					GetARCDocument()->Get3DViewParent()->GetAirside3D()->DeleteObject(-1);*/
				GetARCDocument()->UpdateAllViews(this, VM_DELECT_ALTOBJECT);
			}
			delete pDlg;
		}

		return;
	}

	if (pNodeData == NULL || pNodeData->nodeType != NodeType_Dlg)
	{
//		CTreeView::OnLButtonDblClk(nFlags, point);
		return;
	}

	AirsideGUI::NodeViewDbClickHandler AirsideNodeHandler( 
		GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());

	AirsideNodeHandler.SetUnitsManager(UNITSMANAGER);
	
	if (NULL == m_pAirsideNodeHandler)
	{
		m_pAirsideNodeHandler = new AirsideGUI::NodeViewDbClickHandler( 
			GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	}			
	m_pAirsideNodeHandler->SetUnitsManager(UNITSMANAGER);
	m_pAirsideNodeHandler->SetFlightTypeSelectionCallBack(SelectFlightType);
	m_pAirsideNodeHandler->SetFlightTypeFuncSelectionCallBack(FuncSelectFlightType);
	m_pAirsideNodeHandler->SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	m_pAirsideNodeHandler->SetNewFlightTypeSelectionCallBack(SelectNewFlightType);
	

	AirsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	AirsideNodeHandler.SetFlightTypeFuncSelectionCallBack(FuncSelectFlightType);
	AirsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	AirsideNodeHandler.SetNewFlightTypeSelectionCallBack(SelectNewFlightType);


	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	AirsideNodeHandler.SetACCategoryList(pvACCategory);

	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	AirsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	switch(pNodeData->nSubType)
	{
	case Dlg_VehicleSpecification:
		{
			AirsideNodeHandler.DoModalVehicleSpecification(m_nProjID);
		}
		break;
	case Dlg_ManualData:
		{
			DisplayExternalManual();
		}
		break;
	case Dlg_AirportReferencePoints:
		OnCtxIputLatlong();
		break;
	case Dlg_DatabaseAirline:
		{
			CDlgDBAirline dlg(FALSE,&GetARCDocument()->GetTerminal() );
			dlg.setAirlinesMan( GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan() );
			dlg.DoModal();
		}
		break;
	case Dlg_DatabaseFlight:
		{
			CDlgFlightDB dlg(m_nProjID,&GetARCDocument()->GetTerminal());
			dlg.DoModal();
		}
		break;
	case Dlg_DatabaseAirportSector:
		{
			CDlgDBAirports dlg(FALSE,&GetARCDocument()->GetTerminal());
			dlg.setAirportMan( GetARCDocument()->GetTerminal().m_pAirportDB->getAirportMan() );
			dlg.DoModal();
		}
		break;
	case Dlg_DatabaseAircraft:
		{
			CDlgDBAircraftTypes dlg(FALSE, &GetARCDocument()->GetTerminal() );
			dlg.setAcManager( GetARCDocument()->GetTerminal().m_pAirportDB->getAcMan() );
			dlg.DoModal();
		}
		break;

	case Dlg_AirRoute:
		{
			CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hItem);
			m_hRightClkItem = hItem;
			if (pNodeData->nIDResource == IDR_MENU_AIRROUTES)
			{
				OnDefineAirRoutes();
				GetDocument()->UpdateAllViews(this,VM_NEW_SINGLE_ALTOBJECT,NULL);
			}
			else
			{
				OnEidtAirRoute();				
				GetDocument()->UpdateAllViews(this,VM_MODIFY_AIRROUTE, (CObject*)pNodeData->dwData );
			}
		}
		break;
	case Dlg_WingSpan:
		AirsideNodeHandler.WingspanCategories(m_nProjID);
		break;
	case Dlg_WakeVortexWeight:
		AirsideNodeHandler.VortexWeightCategories(m_nProjID);
		break;
	case Dlg_SurfaceBearingWeight:
		AirsideNodeHandler.SurfaceBearingWeightCategories(m_nProjID);
		break;
	case Dlg_ApproachSpeed:
		AirsideNodeHandler.SpeedCategories(m_nProjID);				
		break;
	case Dlg_FlightPlans:
		{
			CDlgFlightPlan dlg(GetARCDocument()->GetProjectID(),GetARCDocument()->GetTerminal().m_pAirportDB, this);
			dlg.DoModal();
		}
		//AirsideNodeHandler.FlightPlan();
		break;
	case Dlg_ArrivalETAOffset:
		{
			AirsideNodeHandler.ArrivalETAOffset(m_nProjID, GetARCDocument()->GetTerminal().m_pAirportDB);
		}
		break;
	case Dlg_DepartureSlots:
		AirsideNodeHandler.DepartureSlotSpecification(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB);
		break;
	case Dlg_ItinerantTraffic:
		AirsideNodeHandler.ItinerantTraffic(m_nProjID);
		break;
	case Dlg_PerformanceCruise:
		AirsideNodeHandler.FlightPerformanceCruise(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
		break;
	case Dlg_PerformanceTerminal:
		AirsideNodeHandler.FlightPerformanceTerminal(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
		break;
	case Dlg_PerformanceApproach:
		AirsideNodeHandler.ApproachToLand(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
		break;
	case Dlg_PerformanceLanding:
		AirsideNodeHandler.PerformLanding(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
		break;
	case Dlg_PerformanceTaxiInBound:
		AirsideNodeHandler.TaxiInbound(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
		break;
	case Dlg_PerformanceStandService:
		AirsideNodeHandler.StandService(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
		break;
	case Dlg_PerformancePushBack:
		AirsideNodeHandler.PushBack(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
		break;
	case Dlg_PerformanceEngineStartAndParameters:
		AirsideNodeHandler.EngineStartAndParameters(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
		break;
	case Dlg_PerformanceTaxiOutBound:
		AirsideNodeHandler.TaxiOutbound(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
		break;
	case Dlg_PerformanceTakeOff:
		AirsideNodeHandler.PerformanceTakeoff(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
		break;
	case Dlg_PerformanceDepartureClimbout:
		AirsideNodeHandler.DepClimbOut(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
		break;
	case Dlg_AirspaceInTrailSeparation:
		{
			Terminal* pTerminal = &(GetARCDocument()->GetTerminal());
			//AirsideNodeHandler.TaxiwayFlightTypeRestriction(m_nProjID, pTerminal, this);
		    AirsideNodeHandler.InTrailSeparation(m_nProjID, pTerminal,GetARCDocument()->GetInputAirside().m_pAirportDB, this);
		}
		break;
	case Dlg_TrainingFlightSpec:
		{
			Terminal* pTerminal = &(GetARCDocument()->GetTerminal());
			AirsideNodeHandler.TrainingFlgihtSpecification(m_nProjID,pTerminal,&(GetARCDocument()->GetInputAirside()));
		}
		break;
	case Dlg_DirectRoutingCriteria:	
		AirsideNodeHandler.DirectRoutingCriteria(m_nProjID);		
		break;
		//case Dlg_ATCSeparation:
		//	AirsideNodeHandler.ATCSeparation();
		//	break;
		//case Dlg_ExelusiveRunwayUse:
		//	AirsideNodeHandler.ExclusiveRunwayUse();
		//	break;
		//case Dlg_ExitUsageCriteria:
		//	AirsideNodeHandler.ExitUsageCriteria();
		//	break;
		//case Dlg_TakeOffPosAssignCriteria:
		//	AirsideNodeHandler.TakeoffPosAssignCriteria();
		//	break;
//	case Dlg_TaxiwayUsageCriteria:
//		{
//			std::vector<CString> vSegmentName;
//			std::vector<ASLayout::Segment> *pSegments = &(GetARCDocument()->GetTerminal().GetAirsideInput()->GetAirsideLayout()->GetSegments());
//			std::vector<ASLayout::Segment>::iterator iter = pSegments->begin();
//			std::vector<ASLayout::Segment>::iterator iterEnd = pSegments->end();
//			for (;iter != iterEnd;++iter)
//			{
//				CString strName = (*iter).GetName();
//				vSegmentName.push_back(strName);
//			}
//			AirsideNodeHandler.SetSegmentName(vSegmentName);
//			AirsideNodeHandler.TaxiwayUsageCriteria();
//		}
//
//		break;
//	case Dlg_TaxiwayIntersectionBlocking:
//		{
//			std::vector<ASLayout::Intersection>& vIntersections = 
//				GetARCDocument()->GetTerminal().m_AirsideInput->GetAirsideLayout()->GetIntersections();
//			std::vector<CString> vIntersectionsNameList;
//			for (std::vector<ASLayout::Intersection>::iterator iter = vIntersections.begin();
//				iter != vIntersections.end();
//				iter++)
//				vIntersectionsNameList.push_back(iter->GetName());
//			AirsideNodeHandler.SetIntersectionsName(vIntersectionsNameList);
//			AirsideNodeHandler.IntersectionBlocking();
//		}
//
//		break;
	//case Dlg_PushBackBlocking:
	//	AirsideNodeHandler.PushBackBlocking();
		break;
	case Dlg_StandCriteriaAssignment:
		{
			AirsideNodeHandler.StandCriteriaAssign(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
		}
		break;
	//case Dlg_GatesTowingOperationParam:
	//	AirsideNodeHandler.GatesTowingOperationParam();
	//	break;
	case Dlg_DepInTrail:
		//AirsideNodeHandler.DepInTrail();
		break;
	case Dlg_ArrivalInit:
		{
			//airside_engine::Simulation *pSimulation =  new airside_engine::Simulation();
			//if (pSimulation->Init(GetARCDocument()->GetTerminal().GetAirsideInput(),NULL))
			//{
			//	airside_engine::FlightList* _flightlist = pSimulation->getFlightList();
			//	//ns_AirsideInput::CFlightPlans* pFlightplans = (GetARCDocument()->GetAirsieInput()).GetFlightPlans();
			//	std::vector<FlightArrivalInit> vArrivalInitParaList;
			//	FlightArrivalInit _flightarrivalinit;
			//	char pchar[512];
			//	Flight*  _pflight = NULL;
			//	for (int i =0;i<int(_flightlist->size());i++)
			//	{
			//		_pflight = _flightlist->at(i)->GetCFlight();
			//		_pflight->getAirline(pchar); 
			//		_flightarrivalinit.strAirline = pchar;

			//		_pflight->getArrID(pchar);
			//		_flightarrivalinit.strArrID = pchar;

			//		_flightarrivalinit.strArrGate = _pflight->getArrGate().GetIDString();

			//		FlightDescStruct& fltdesc = _pflight->getLogEntry();
			//		_flightarrivalinit.dScheduleTime = _pflight->getArrTime();
			//		_flightarrivalinit.dSimTime = (_flightlist->at(i))->getFEL()->getEventInType(airside_engine::FlightEvent::StopInGate)->getEventTime();
			//		_flightarrivalinit.dOffsetTime = _flightarrivalinit.dScheduleTime - _flightarrivalinit.dSimTime;
			//		vArrivalInitParaList.push_back(_flightarrivalinit);
			//	}
			//	AirsideNodeHandler.SetFlightArrivalInitList(vArrivalInitParaList);
			//}
		}
		AirsideNodeHandler.ArrivalInit();
		break;
	case Dlg_SimSetting:
		{
			CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
			ASSERT( pMDIActive != NULL );

			CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
			ASSERT( pDoc!= NULL );

			CSimEngSettingDlg dlg(pDoc->GetTerminal().m_pSimParam, this );
			dlg.DoModal();

		}
		//AirsideNodeHandler.SimSetting();
		break;
	case Dlg_SimRun:
		theApp.GetMainWnd()->SendMessage(WM_COMMAND, ID_PROJECT_RUNSIM);
		break;
	case Dlg_LandingRunwayAssignment:
		AirsideNodeHandler.LandingRunwayAssignment(m_nProjID, GetARCDocument()->GetTerminal().m_pAirportDB);
		break;
	case Dlg_SIDAssignment:
		AirsideNodeHandler.SidAssignment(m_nProjID, GetARCDocument()->GetTerminal().m_pAirportDB);
		break;
	case Dlg_RunwayTakeOffAssignment:
		{
			std::vector<ALTAirport> vAirport;
			ALTAirport::GetAirportList(m_nProjID,vAirport);
			AirsideNodeHandler.TakeoffRunwayAssignment(m_nProjID, GetARCDocument()->GetTerminal().m_pAirportDB,vAirport.at(0).getID());
			break;
		}
	case Dlg_RunwayTakeOffPositions:
		AirsideNodeHandler.RunwayTakeoffPosition(m_nProjID);
		break;
	case Dlg_LandingRunwayExitStrategies:
		AirsideNodeHandler.LandingRunwayExit(m_nProjID);
		break;
	case Dlg_FollowMeConnection:
		{
			CDlgFollowMeConnections Dlg(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB,this) ;
			Dlg.DoModal() ;
		}
		break ;
	case Dlg_RunwayTakeOffSequencing:
		m_pAirsideNodeHandler->TakeoffSequence(m_nProjID, GetARCDocument()->GetTerminal().m_pAirportDB);
		break;

	case Dlg_RunwayTakeOffQueues:
		m_pAirsideNodeHandler->TakeoffQueues(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB);
		break;
		
	case Dlg_StandBuffers:
		AirsideNodeHandler.StandBuffers(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB);
		break;

	case Dlg_OccupiedAssignedStandAction:
		AirsideNodeHandler.OccupiedAssignedStandAction(m_nProjID);
		break;

	case Dlg_RunwayArrivalDelayTriggers:
		{
			Terminal* pTerminal = &(GetARCDocument()->GetTerminal());
			CDlgArrivalDelayTriggers dlg(GetARCDocument()->GetProjectID(), pTerminal, this);
			dlg.DoModal();			
		}
		break;
	case Dlg_AircraftDisplay:
		{
			CAircraftDispPropDlg dlg(this);
			dlg.DoModal();
		}
		break;
	case Dlg_VehicleDisplay:
		{
			CVehicleDispPropDlg dlg(m_nProjID,this);
			dlg.DoModal();
		}
		break;
	case Dlg_AircraftAlias:
		{
			Terminal* pTerm = &(GetARCDocument()->GetTerminal());
			CDlgAircraftAlias dlg(pTerm,this);
			dlg.DoModal();
		}
		break;
	case Dlg_AircraftTags:
		{
			CDlgAircraftTags dlg(this);
			dlg.DoModal();
		}
		break;
	case Dlg_VehicleTags:
		{
			CDlgVehicleTags dlg(this);
			dlg.DoModal();
		}
		break;
	case Dlg_CalloutDispSpec:
		{
			CCalloutDispSpecDlg dlg(FuncSelectFlightType, m_nProjID, GetARCDocument(), this);
			dlg.DoModal();
		}
		break;
	case Dlg_ApprochSeparation:
		AirsideNodeHandler.ApprochSeparationCriteria(m_nProjID);
		break;

	case Dlg_RunwayClearanceCriteria:
		AirsideNodeHandler.TakeoffClearance(m_nProjID);
		break;

	case Dlg_TrainingFlightsManagement:
		AirsideNodeHandler.TrainingFlgihtSManagement(GetARCDocument()->GetTerminal().m_pAirportDB,GetARCDocument()->GetProjectID(),this);
		break;
				
	case Dlg_TaxiwayWeightConstraints:
		m_pAirsideNodeHandler->TaxiwayConstraint(GetARCDocument()->GetProjectID(),WeightConstraint,GetARCDocument()->GetTerminal().m_pAirportDB,this);
		break;		

	case Dlg_TaxiwayWingspanConstraints:
		m_pAirsideNodeHandler->TaxiwayConstraint(GetARCDocument()->GetProjectID(),WingSpanConstraint,GetARCDocument()->GetTerminal().m_pAirportDB,this);
		break;

	case Dlg_TaxiwayFlightTyperestrictions:
		{
			m_pAirsideNodeHandler->FlightTypeRestrictions(GetARCDocument()->GetProjectID(),GetARCDocument()->GetTerminal().m_pAirportDB, this);

		//	Terminal* pTerminal = &(GetARCDocument()->GetTerminal());
		//	AirsideNodeHandler.TaxiwayFlightTypeRestriction(m_nProjID, pTerminal, this);
		}
		break;

	case Dlg_DirectionalityConstraints:
		{
			std::vector<ALTAirport> vAirport;
			ALTAirport::GetAirportList(m_nProjID,vAirport);
			m_pAirsideNodeHandler->DirectionalityConstraints(m_nProjID, vAirport.at(0).getID());
		}
		break;

	case Dlg_TaxiSpeedConstraints:
		{
			m_pAirsideNodeHandler->TaxiSpeedConstraints(m_nProjID, GetARCDocument()->GetTerminal().m_pAirportDB);
		}
		break;

	case Dlg_IntersectionNodeOption:
		{
			std::vector<ALTAirport> vAirport;
			ALTAirport::GetAirportList(m_nProjID,vAirport);
			m_pAirsideNodeHandler->IntersectionNodeOption(vAirport.at(0).getID());
		}
		break;

	case Dlg_TaxiwayTemporaryParkingCriteria:
		m_pAirsideNodeHandler->TemporaryParkingCriteria(GetARCDocument()->GetProjectID(),this);
		break;

	case Dlg_ConflictResolutionCriteria:	
		AirsideNodeHandler.ConflictResolutionCriteria(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);		
		break;

	case Dlg_TowingRoute:	
		m_pAirsideNodeHandler->TowingRoute(m_nProjID);		
		break;
	case Dlg_Deicingandanti_demand:
		{
		   AirsideNodeHandler.DoDeicingAndAnticings_demand(&GetARCDocument()->GetTerminal(),GetARCDocument()->GetInputAirside().m_pAirportDB,this) ;
		}
		 break ;
	case Dlg_WeatherImpacts:
		{
			AirsideNodeHandler.WeatherImpacts(m_nProjID);
		}
		break;
	case Dlg_PadAssignment:
		{
			AirsideNodeHandler.PadAssignment(m_nProjID);
		}
		break;
	case Dlg_DeiceRoute:
		{
			m_pAirsideNodeHandler->DeicePadRoute(m_nProjID,pNodeData->nOtherData,GetARCDocument()->GetTerminal().m_pAirportDB );
		}
		break;
	case Dlg_PostDeiceRoute:
		{
			m_pAirsideNodeHandler->PostDeiceRoute(m_nProjID,pNodeData->nOtherData,GetARCDocument()->GetTerminal().m_pAirportDB );
		}
		break;
	case Dlg_DeiceAntiStrategies:
		{
			AirsideNodeHandler.DeiceAntiStrategies(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB);
		}
		break;
	case Dlg_DeiceQueue:
		{
			AirsideNodeHandler.DeiceQueue(m_nProjID,pNodeData->nOtherData);
		}
		break;
	case Dlg_StarAssignment:
		{
			AirsideNodeHandler.StarAssignment(m_nProjID, GetARCDocument()->GetTerminal().m_pAirportDB);
		}
		break;
	case Dlg_StandConstraints:
		{
			std::vector<ALTAirport> vAirport;
			ALTAirport::GetAirportList(m_nProjID,vAirport);
			GateAssignmentConstraintList constraints;
			constraints.LoadataStandConstraint(GetARCDocument());
			/*constraints.SetInputTerminal(&GetARCDocument()->GetTerminal());
			constraints.loadDataSet(GetARCDocument()->m_ProjInfo.path);

			constraints.GetAdjConstraintSpec().SetInputTerminal(&GetARCDocument()->GetTerminal());
			constraints.GetAdjConstraintSpec().loadDataSet(GetARCDocument()->m_ProjInfo.path);

			constraints.GetACTypeStandConstraints()->ReadData();*/
			CDlgGateAssignConstraints dlg((InputTerminal*)&GetARCDocument()->GetTerminal(),vAirport.at(0).getID(),GetARCDocument()->GetProjectID(), &constraints);
			dlg.DoModal();
		}
		break;
	case Dlg_TowOffStandAssignment:
		{
			AirsideNodeHandler.TowOffStandAssignment(m_nProjID, GetARCDocument()->GetTerminal().m_pAirportDB);
		}
		break;

	case Dlg_CtrlInterventionSpec:
		{
			AirsideNodeHandler.CtrlInterventionSpec(m_nProjID);
		}
		break;

	case Dlg_FlightSchedule:
		{
			CFlightScheduleDlg dlg( this );
			dlg.DoModal();
		}
		break;

	case Dlg_StandAssignment:
		{
			OnEditStandAssignment();
		}
		break;

	case Dlg_SideStepSpecification:
		{
			AirsideNodeHandler.SideSepSpecification(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);			
		}
		break;

	
	case Dlg_InboundRouteAssignment:
		{
			m_pAirsideNodeHandler->InboundRouteAssignment(m_nProjID, GetARCDocument()->GetInputAirside().m_pAirportDB);
		}
		break;

	case Dlg_OutboundRouteAssignment:
		{
			m_pAirsideNodeHandler->OutboundRouteAssignment(m_nProjID, GetARCDocument()->GetInputAirside().m_pAirportDB);
		}
		break;

	case Dlg_GoAroundCriteriaCustomize:
		{
			AirsideNodeHandler.GoAroundCriteriaCustomize(m_nProjID);
			break;
		}
	case Dlg_PushBackClearanceCriteria:
		{
			AirsideNodeHandler.PushBackClearanceCriteria(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB);
			break;
		}
	case Dlg_ServiceRequirement:
		{
			AirsideNodeHandler.ServicingRequirement(m_nProjID, GetARCDocument()->GetTerminal().m_pAirportDB);
			break;
		}
	case Dlg_VehiclePoolsAndDeployment:
		{
			AirsideNodeHandler.VehiclePoolsAndDeployment(m_nProjID);
			break;
		}

	case Dlg_FlightTypeDimensions:
		{
			AirsideNodeHandler.FlightTypeDimensions(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB);
			break;
		}
	case Dlg_FlightTypeRelativeServiceLocations:
		{
			AirsideNodeHandler.FlightTypeRelativeServiceLocations(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB);
			break;
		}
	case Dlg_VehicleRoute:
		{
			AirsideNodeHandler.VehicleRoute(m_nProjID);
			break;
		}
	case Dlg_Paxbusparking:
		{			
			CDlgPaxBusParking dlg(m_nProjID,&(GetARCDocument()->GetTerminal()),this);
			( dlg.DoModal());
			{
				if( GetARCDocument()->Get3DViewParent() )
				{
					GetARCDocument()->Get3DViewParent()->GetAirside3D()->UpdatePaxBusParkingInfo();
				}
			};
			
			break;
		}
	case Dlg_BagCartsLoadingAreas:
		{
			CDlgBaggageCartParking dlgbagCart(m_nProjID,&GetARCDocument()->GetTerminal(),this);
			( dlgbagCart.DoModal() );
			{
				if( GetARCDocument()->Get3DViewParent() )
				{
					GetARCDocument()->Get3DViewParent()->GetAirside3D()->UpdateBagCartParkingInfo();
				}
			}
			break;
		}
	case Dlg_HoldShortLines:
		{
			CDlgHoldShortLines dlgHoldShortLines(m_nProjID,this);
			dlgHoldShortLines.DoModal();
			break;
		}
	case Dlg_ControllerIntervention:
		{
              AirsideNodeHandler.setControllerIntervention(m_nProjID,this) ;
			  break ;
		}
	case Dlg_SectorFlightMixSpecification:
		{
			AirsideNodeHandler.SectorFlightMixSpec(m_nProjID);
			break;
		}
	case Dlg_VehicularMovementAllowed:
		{
			AirsideNodeHandler.DoModalDlgVehicularMovementAllowed(m_nProjID,this);
			break;
		}
	case Dlg_TaxiInterruptTime:
		{
			AirsideNodeHandler.DoTaxiInterruptTime(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB,this) ;
			break ;
		}
	case Dlg_WaveCrossRunwaySpecification:
		{
			//AirsideNodeHandler.WaveCrossRunwaySpecification(m_nProjID,0);
			CDlgWaveCrossRunwaySpecification dlg(m_nProjID,this);
			dlg.DoModal();
			break;
		}
	case Dlg_Ignition:
		{
			AirsideNodeHandler.IgnitionSpecs(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB);
			break;
		}
	case Dlg_PushBackAndTowOperationSpec:
		{
			AirsideNodeHandler.DoModelPushBackAndTowOperationSpecification(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB,this) ;
		

		}
		break;
	case Dlg_AircraftSurfaceCondition:
		{
          AirsideNodeHandler.DoAircraftSurfaceCondition(GetARCDocument()->GetTerminal().m_pAirportDB,this) ;
		}
		break ;
	case Dlg_WeatherScript:
		{
          AirsideNodeHandler.DoWeatherScript(this) ;
		}
		break ;
	default:
		break;
	}


//	CTreeView::OnLButtonDblClk(nFlags, point);
}

void CAirsideMSView::LoadChildContour(HTREEITEM hParentItem, Contour *pContour)
{
	ASSERT(hParentItem!=NULL);
	ASSERT(pContour!=NULL);

	HTREEITEM hTreeItem = AddItem(pContour->GetObjNameString(), hParentItem);

	CNodeData* pNodeData;
	pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Object);
	pNodeData->dwData = pContour->getID();
	pNodeData->nSubType = pContour->GetType();
	pNodeData->nIDResource = IDR_MENU_AIARSIDEOBJECT;
	pNodeData->nOtherData = pContour->getAptID();
	GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

	for (int i=0; i<pContour->GetChildCount(); i++)
	{
		Contour *pChildContour = pContour->GetChildItem(i);
		ASSERT(pChildContour);

		LoadChildContour(hTreeItem, pChildContour);
	}

}

bool CAirsideMSView::LoadObjects(HTREEITEM hObjRoot)
{

	CString strText= GetTreeCtrl().GetItemText(hObjRoot);
	if (strText.IsEmpty())
		return false;
	
	CNodeData* pNodeData = reinterpret_cast<CNodeData*>(GetTreeCtrl().GetItemData( hObjRoot ));

	if(pNodeData == NULL)
		return false;

	if (pNodeData->nodeType != NodeType_ObjectRoot)
		return false;

	if(pNodeData->bHasExpand == true)
		return false;
	pNodeData->bHasExpand = true;

	ALTOBJECT_TYPE objType = (ALTOBJECT_TYPE)pNodeData->nSubType;
	int nAirportID = pNodeData->nOtherData;

	//clear child node
	HTREEITEM hChildItem = NULL;
	while( (hChildItem = GetTreeCtrl().GetChildItem(hObjRoot)) != NULL)
	{
		GetTreeCtrl().DeleteItem(hChildItem);		
	}

	std::vector<ALTObject> vObject;
	std::vector<ALTObject> vSimilarObject;

	ALTObject::GetObjectList(objType,nAirportID,vObject);
	if (objType == ALT_STRETCH)
	{
		ALTObject::GetObjectList(ALT_CIRCLESTRETCH,nAirportID,vSimilarObject);
		for (int i=0; i<(int)vSimilarObject.size(); i++)
		{
			vObject.push_back(vSimilarObject[i]);
		}
	}
		//return false;
	boost::array<std::pair<std::string,HTREEITEM>,OBJECT_STRING_LEVEL> LastObjectItem;
	

	int nObjectCount = vObject.size();

	if (objType == ALT_CONTOUR)
	{
		vector<int> vRootContourIDs;
		ALTAirport::GetRootContoursIDs(nAirportID, vRootContourIDs);

		for (vector<int>::iterator iter=vRootContourIDs.begin(); iter!=vRootContourIDs.end(); iter++)
		{
			Contour contour;
			contour.ReadObject(*iter);

			HTREEITEM hTreeItem = AddItem(contour.GetObjNameString(), hObjRoot);

			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Object);
			pNodeData->dwData = contour.getID();
			pNodeData->nSubType = contour.GetType();
			pNodeData->nIDResource = IDR_MENU_COUNT_PROPERTY;
			pNodeData->nOtherData = nAirportID;
			GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

			for (int i=0; i<contour.GetChildCount(); i++)
			{
				Contour* pChildContour = contour.GetChildItem(i);
				ASSERT(pChildContour);

				LoadChildContour(hTreeItem, pChildContour);
			}
		}

		return true;
	}
	else
	{
		for (int i =0; i < nObjectCount; ++ i)
		{
			ALTObject& altObject = vObject.at(i);
			ALTObjectID objName;
			altObject.getObjName(objName);

			//Level1
			int nLevel = 0;
			if (objName.m_val[nLevel] != LastObjectItem[nLevel].first)
			{
				for (int j=nLevel +1; j <OBJECT_STRING_LEVEL; ++j)//clear the info
				{
					LastObjectItem[j].first = _T("");
					LastObjectItem[j].second = NULL;
				}


				HTREEITEM hTreeItem = AddItem(objName.m_val[nLevel].c_str(),hObjRoot);
				LastObjectItem[nLevel].first = objName.m_val[nLevel];
				LastObjectItem[nLevel].second = hTreeItem;


				//CNodeData* pNodeData= NULL;
				if(objName.m_val[nLevel+1] == _T("")) //object node
				{
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Object);
					pNodeData->dwData = altObject.getID();
					pNodeData->nSubType = objType;
					pNodeData->nIDResource = IDR_MENU_AIARSIDEOBJECT;
					pNodeData->nOtherData = nAirportID;
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
					continue;
				}
				else
				{
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_ObjectGroup);
					pNodeData->nSubType = objType;
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
				}

			}

			//level2
			nLevel = 1;
			if (objName.m_val[nLevel] != LastObjectItem[nLevel].first)
			{		
				for (int j= nLevel +1; j <OBJECT_STRING_LEVEL; ++j)//clear the info
				{
					LastObjectItem[j].first = _T("");
					LastObjectItem[j].second = NULL;
				}

				HTREEITEM hTreeItem = AddItem(objName.m_val[nLevel].c_str(),LastObjectItem[nLevel-1].second);	
				//			GetTreeCtrl().Expand(LastObjectItem[nLevel-1].second,TVE_EXPAND);
				LastObjectItem[nLevel].first = objName.m_val[nLevel];
				LastObjectItem[nLevel].second = hTreeItem;
				CNodeData* pNodeData= NULL;
				if(objName.m_val[nLevel+1] == _T("")) //object node
				{
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Object);
					pNodeData->dwData = altObject.getID();
					pNodeData->nSubType = objType;
					pNodeData->nIDResource = IDR_MENU_AIARSIDEOBJECT;
					pNodeData->nOtherData = nAirportID;
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);
					continue;	
				}
				else
				{
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_ObjectGroup);
					pNodeData->nSubType = objType;
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);	
				}

			}

			//level3
			nLevel = 2;
			if (objName.m_val[nLevel] != LastObjectItem[nLevel].first)
			{	

				for (int j= nLevel +1; j <OBJECT_STRING_LEVEL; ++j)//clear the info
				{
					LastObjectItem[j].first = _T("");
					LastObjectItem[j].second = NULL;
				}

				HTREEITEM hTreeItem = AddItem(objName.m_val[nLevel].c_str(),LastObjectItem[nLevel-1].second);	
				//			GetTreeCtrl().Expand(LastObjectItem[nLevel-1].second,TVE_EXPAND);
				LastObjectItem[nLevel].first = objName.m_val[nLevel];
				LastObjectItem[nLevel].second = hTreeItem;
				CNodeData* pNodeData= NULL;
				if(objName.m_val[nLevel+1] == _T("")) //object node
				{
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Object);
					pNodeData->dwData = altObject.getID();
					pNodeData->nSubType = objType;
					pNodeData->nIDResource = IDR_MENU_AIARSIDEOBJECT;
					pNodeData->nOtherData = nAirportID;
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);

					continue;	
				}
				else
				{
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_ObjectGroup);
					pNodeData->nSubType = objType;
					GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);	
				}
			}

			nLevel = 3;
			if (objName.m_val[nLevel] != LastObjectItem[nLevel].first)
			{
				HTREEITEM hTreeItem = AddItem(objName.m_val[nLevel].c_str(),LastObjectItem[nLevel-1].second);
				LastObjectItem[nLevel].first = objName.m_val[nLevel];
				LastObjectItem[nLevel].second = hTreeItem;
				CNodeData* pNodeData= NULL;
				//if(objName[nLevel+1] ! == _T("")) //object node
				//{
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Object);
				pNodeData->dwData = altObject.getID();
				pNodeData->nSubType = objType;
				pNodeData->nOtherData = nAirportID;
				pNodeData->nIDResource = IDR_MENU_AIARSIDEOBJECT;
				//}
				//else
				//{
				//	pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_ObjectGroup);
				//}
				GetTreeCtrl().SetItemData(hTreeItem,(DWORD_PTR)pNodeData);	

				//for (int j= nLevel +1; j <OBJECT_STRING_LEVEL; ++j)//clear the info
				//{
				//	LastObjectItem[j].first = _T("");
				//	LastObjectItem[j].second = NULL;
				//}
				//			GetTreeCtrl().Expand(LastObjectItem[nLevel-1].second,TVE_EXPAND);
				continue;
			}

		}
	}
	
	GetTreeCtrl().Expand(hObjRoot,TVE_EXPAND);
	return true;

}


void CAirsideMSView::OnCtxIputLatlong()
{	
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;
	
	CDlgEditARP dlg(m_nProjID,pNodeData->nOtherData,this);
	if(dlg.DoModal()==IDOK)
	{
		if(dlg.m_bModified)
		{

			
			HTREEITEM hLatLong = GetTreeCtrl().GetChildItem(m_hRightClkItem);
			CString strLL;
			strLL.Format("LAT:%s LONG:%s",dlg.m_strLatitude,dlg.m_strLongitude);
			GetTreeCtrl().SetItemText(hLatLong,strLL);

			CString strXY;
			HTREEITEM hXY = GetTreeCtrl().GetNextSiblingItem(hLatLong);
			strXY.Format("X:%.2f Y:%.2f",dlg.m_dx,dlg.m_dy);
			GetTreeCtrl().SetItemText(hXY,strXY);

			CString strElevation;
			strElevation.Format("Elevation(%s): %.0f",CARCLengthUnit::GetLengthUnitString(dlg.GetCurSelLengthUnit()),\
				CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,dlg.GetCurSelLengthUnit(),dlg.m_dElevation));
			HTREEITEM hEle = GetTreeCtrl().GetNextSiblingItem(hXY);
			GetTreeCtrl().SetItemText(hEle,strElevation);
	
			CString strMagVar;
			strMagVar.Format("Magnetic Variation: %.0f %s", dlg.m_fMagVariation,dlg.m_nMagEastWestSelected==0?_T("E"):_T("W"));
			HTREEITEM hMagVar = GetTreeCtrl().GetNextSiblingItem(hEle);
			GetTreeCtrl().SetItemText(hMagVar,strMagVar);

			GetDocument()->UpdateAllViews(this,VM_MODIFY_AIRPORT,(CObject*)pNodeData->nOtherData );

		}
	}

}


void CAirsideMSView::OnFloorProperties() 
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	INT_PTR nRet = IDCANCEL;
	//if( EnvMode_AirSide == m_systemMode)
	{
		int nLevelID = pNodeData->dwData;
		int nCurAirportID = pNodeData->nOtherData;
		CAirsideGround asGround(_T("Airside Ground"));
		asGround.ReadData(nLevelID);
		asGround.ReadCADInfo(nLevelID);
		asGround.ReadFloorLayerFromDB(nLevelID) ;
		CAirsideGroundPropDlg dlg(nCurAirportID,&asGround,(CTermPlanDoc*) GetDocument());
		nRet = dlg.DoModal();
		if(nRet == IDOK ) {
			
			GetDocument()->UpdateAllViews(this,VM_MODIFY_AIRSIDEGROUND,NULL);
			( (CTermPlanDoc*) GetDocument() )->SetCameraWBS();
			
		}

	}
}


void CAirsideMSView::OnFloorPictureInsert()
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	int nLevelID = pNodeData->dwData;
	int nCurAirportID = pNodeData->nOtherData;
	CAirsideGround asGround(_T("Airside Ground"));
	asGround.ReadOverlayInfo(nLevelID);
	//CString strFilePath = _T("");
	//if(asGround.m_picInfo.sFileName.IsEmpty())
	//{
	//	CString strFilters = _T("Floor Picture File (*.bmp;*.jpg;*.png;*.tif;*tiff)|*.bmp;*.jpg;*.png;*.tif;*.tiff|");
	//	CFileDialog fileDlg (TRUE, NULL, NULL, OFN_FILEMUSTEXIST, strFilters, this);

	//	if (fileDlg.DoModal() != IDOK)
	//		return;

	//	strFilePath = fileDlg.GetPathName();
	//}
	//else
	//{
	//	asGround.ReadOverlayInfo(nLevelID);
	//	strFilePath = asGround.m_picInfo.sFileName;
	//}

	C3DView* pView = GetARCDocument()->Get3DView();
	if(pView == NULL)
	{
		GetARCDocument()->GetMainFrame()->SendMessage(WM_COMMAND, ID_VIEW_3DTERMINALVIEW);
	}
	pView = GetARCDocument()->Get3DView();
	ASSERT(pView != NULL);

	CDlgFloorPictureReference dlg(asGround.m_picInfo.sFileName, nLevelID,&asGround, pView, this);
	INT_PTR retCode = dlg.DoModal(); 
	if(IDOK == retCode)
	{
		GetARCDocument()->Get3DView()->UpdateFloorOverlay(nLevelID, asGround.m_picInfo);
	}
	else if(IDCANCEL == retCode)
	{
		asGround.ReadOverlayInfo(nLevelID);
		GetARCDocument()->Get3DView()->UpdateFloorOverlay(nLevelID, asGround.m_picInfo);
	}
}


void CAirsideMSView::OnFloorPictureHide()
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	int nLevelID = pNodeData->dwData;
	int nCurAirportID = pNodeData->nOtherData;
	CAirsideGround asGround(_T("Airside Ground"));
	//asGround.ReadData(nLevelID);
	asGround.ReadOverlayInfo(nLevelID);
	asGround.m_picInfo.bShow = false;
	asGround.SaveOverlayInfo(nLevelID);

	GetARCDocument()->Get3DView()->UpdateFloorOverlay(nLevelID, asGround.m_picInfo);
}

void CAirsideMSView::OnFloorPictureShow()
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	int nLevelID = pNodeData->dwData;
	int nCurAirportID = pNodeData->nOtherData;
	CAirsideGround asGround(_T("Airside Ground"));
	//asGround.ReadData(nLevelID);
	asGround.ReadOverlayInfo(nLevelID);
	asGround.m_picInfo.bShow = true;
	asGround.SaveOverlayInfo(nLevelID);

	GetARCDocument()->Get3DView()->UpdateFloorOverlay(nLevelID, asGround.m_picInfo);
}

void CAirsideMSView::OnFloorPictureSave()
{
	CFileDialog dlgFile( FALSE, ".csv",NULL ,OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"Floor (*.csv)|*.csv||" , NULL, 0, FALSE );
	CString sFileName;
	if (dlgFile.DoModal() == IDOK)
		sFileName = dlgFile.GetPathName();
	else
		return ;

	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	int nLevelID = pNodeData->dwData;
	int nCurAirportID = pNodeData->nOtherData;
	CAirsideGround asGround(_T("Airside Ground"));
	asGround.ReadOverlayInfo(nLevelID);

	ArctermFile  _file ;
	if(!_file.openFile(sFileName,WRITE) )
	{
		MessageBox(_T("Create File error"),_T("Error"),MB_OK) ;
		return  ;
	}

	if (!asGround.m_picInfo.sFileName.IsEmpty())
	{
		asGround.m_picInfo.ExportFile(_file);
		_file.endFile();
	}
}

void CAirsideMSView::OnFloorPictureLoad()
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	int nLevelID = pNodeData->dwData;
	int nCurAirportID = pNodeData->nOtherData;
	CAirsideGround asGround(_T("Airside Ground"));
	asGround.ReadOverlayInfo(nLevelID);
	CString strFilePath = _T("");
	
	CFileDialog dlgFile( TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Floor (*.csv)|*.csv||" , NULL, 0, FALSE  );

	if (dlgFile.DoModal() != IDOK)
		return;

	strFilePath = dlgFile.GetPathName();
	ArctermFile _file;
	if(!_file.openFile(strFilePath,READ) )
	{
		MessageBox(_T("Read File error"),_T("Error"),MB_OK);
		return  ;
	}

	asGround.m_picInfo.ImportFile(_file);
	_file.endFile();

	C3DView* pView = GetARCDocument()->Get3DView();
	if(pView == NULL)
	{
		GetARCDocument()->GetMainFrame()->SendMessage(WM_COMMAND, ID_VIEW_3DTERMINALVIEW);
	}
	pView = GetARCDocument()->Get3DView();
	ASSERT(pView != NULL);

	CDlgFloorPictureReference dlg(asGround.m_picInfo.sFileName, nLevelID,&asGround, pView, this);
	INT_PTR retCode = dlg.DoModal(); 
	if(IDOK == retCode)
	{
		GetARCDocument()->Get3DView()->UpdateFloorOverlay(nLevelID, asGround.m_picInfo);
	}
	else if(IDCANCEL == retCode)
	{
		asGround.ReadOverlayInfo(nLevelID);
		GetARCDocument()->Get3DView()->UpdateFloorOverlay(nLevelID, asGround.m_picInfo);
	}
}

void CAirsideMSView::OnFloorPictureRemove()
{
	if (m_hRightClkItem == NULL)
	{
		return;
	}
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	if (pNodeData == NULL)
	{
		return;
	}
	int nLevelID = pNodeData->dwData;
	int nCurAirportID = pNodeData->nOtherData;
	CAirsideGround asGround(_T("Airside Ground"));
	asGround.ReadOverlayInfo(nLevelID);
	//asGround.m_picInfo.bShow = false;
	asGround.SaveOverlayInfo(nLevelID);
	asGround.m_picInfo.bShow = TRUE;
	asGround.m_picInfo.dRotation = 0;
	asGround.m_picInfo.dScale = 1;
	asGround.m_picInfo.vOffset = ARCVector2(0,0);
	asGround.m_picInfo.vSize = ARCVector2(0,0);
	Point ptList[2];
	ptList[0].setX(0.0);ptList[0].setY(0.0);ptList[0].setZ(0.0);
	ptList[1].setX(0.0);ptList[1].setY(0.0);ptList[1].setZ(0.0);

	asGround.m_picInfo.refLine.init(2, ptList);
	if(GetARCDocument()->Get3DView())
		GetARCDocument()->Get3DView()->UpdateFloorOverlay(nLevelID,asGround.m_picInfo);
	asGround.DelOverlayInfo(nLevelID);
}

void CAirsideMSView::OnUpdateFloorPictureHide(CCmdUI* pCmdUI)
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	int nLevelID = pNodeData->dwData;
	int nCurAirportID = pNodeData->nOtherData;
	CAirsideGround asGround(_T("Airside Ground"));
	//asGround.ReadData(nLevelID);
	asGround.ReadOverlayInfo(nLevelID);

	BOOL bEnable = (!asGround.m_picInfo.sFileName.IsEmpty() && asGround.m_picInfo.bShow);
	
	pCmdUI->Enable(bEnable);		
}

void CAirsideMSView::OnUpdateFloorPictureShow(CCmdUI* pCmdUI)
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	int nLevelID = pNodeData->dwData;
	int nCurAirportID = pNodeData->nOtherData;
	CAirsideGround asGround(_T("Airside Ground"));
	//asGround.ReadData(nLevelID);
	asGround.ReadOverlayInfo(nLevelID);

	BOOL bEnable = (!asGround.m_picInfo.sFileName.IsEmpty() && !asGround.m_picInfo.bShow);

	pCmdUI->Enable(bEnable);	
}

void CAirsideMSView::OnUpdateFloorPictureRemove(CCmdUI* pCmdUI)
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	int nLevelID = pNodeData->dwData;
	int nCurAirportID = pNodeData->nOtherData;
	CAirsideGround asGround(_T("Airside Ground"));
	//asGround.ReadData(nLevelID);
	asGround.ReadOverlayInfo(nLevelID);

	BOOL bEnable = (!asGround.m_picInfo.sFileName.IsEmpty());

	pCmdUI->Enable(bEnable);
}
void CAirsideMSView::OnObjectProperties()
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	if (pNodeData->nodeType != NodeType_Object)
		return;

	int nObjID = pNodeData->dwData;

// 	ALTObject * pObj = NULL;
// 	if(GetARCDocument()->Get3DViewParent() && GetARCDocument()->Get3DViewParent()->GetAirside3D() )
// 	{
// 		ALTObject3D * pObj3D = GetARCDocument()->Get3DViewParent()->GetAirside3D()->GetObject3D(nObjID);
// 		if(pObj3D)
// 		{
// 			pObj = pObj3D->GetObject();
// 		}
// 	}
	

	CDialog *pDlg = /*pObj?GetObjectDefineDlg(pNodeData,pObj) :*/ GetObjectDefineDlg(pNodeData,nObjID);
	if (pDlg)
	{
		if(pDlg->DoModal() == IDOK)
		{	
			if(((CAirsideObjectBaseDlg*)pDlg)->m_bPropModified 
				|| ((CAirsideObjectBaseDlg*)pDlg)->m_bPathModified 
				|| ((CAirsideObjectBaseDlg*)pDlg)->m_bTypeModify)
			{
				HTREEITEM hObjRoot = GetObjRootNode(m_hRightClkItem);
				DeleteObjectFromTree(m_hRightClkItem);
				AddObjectToTree(hObjRoot,((CAirsideObjectBaseDlg*)pDlg)->GetObject());
				GetDocument()->UpdateAllViews(this,VM_MODIFY_ALTOBJECT,(CObject*)((CAirsideObjectBaseDlg*)pDlg)->GetObject());
			}
		}
		else 
		{
			if (-1 != nObjID)
			{
				C3DView* p3DView = ((CTermPlanDoc*)GetDocument())->Get3DView();
				if (p3DView)
				{
					p3DView->GetAirside3D()->UpdateObject(nObjID);
				}
			}
		}

		delete pDlg;
	}

}

void CAirsideMSView::OnReportingAreaDisplayProperty()
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	if (pNodeData->nodeType != NodeType_ObjectRoot)
		return;

	int nAirportID = pNodeData->nOtherData;
	std::vector<int> vIDs;
	ALTAirport::GetReportingAreaIDs(nAirportID,vIDs);
	if (vIDs.empty())
		return;

	if(GetARCDocument()->Get3DViewParent() && GetARCDocument()->Get3DViewParent()->GetAirside3D() )
	{
		CTopograph3D* pTopo3D = GetARCDocument()->Get3DViewParent()->GetAirside3D()->GetTopograph();
	 	ALTObject3D * pObj3D = pTopo3D->GetObject3D(vIDs.at(0));

		CDlgAirsideObjectDisplayProperties dlg(pObj3D);
		if(dlg.DoModal() == IDOK)
		{	
			ALTObject3D* pNext3D = NULL;
			int nCount = vIDs.size();
			for (int i = 1; i < nCount; i++)
			{
				pNext3D = pTopo3D->GetObject3D(vIDs.at(i));
				pNext3D->GetDisplayProp()->CopyData(*(pObj3D->GetDisplayProp()));
				pNext3D->GetDisplayProp()->SaveData(vIDs.at(i));
			}
			GetDocument()->UpdateAllViews(this);
		}

	}

}

void CAirsideMSView::OnObjectDelete()
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	if (pNodeData->nodeType != NodeType_Object)
		return;
	int nObjID = pNodeData->dwData;
	
	ALTObjectList vObjDelete;
	ALTObject* newObj = ALTObject::ReadObjectByID(nObjID);
	vObjDelete.push_back(newObj);
	
	GetARCDocument()->DeleteALTObjects(vObjDelete);
	if(vObjDelete.size() > 0 )
	{
		DeleteObjectFromTree(m_hRightClkItem);
		if (newObj->GetType() == ALT_RUNWAY || newObj->GetType() == ALT_TAXIWAY)
		{
			GetTreeCtrl().Expand(m_hObjSurface,TVE_COLLAPSE);
			pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hObjSurface);
			pNodeData->bHasExpand = false;
		}

		GetDocument()->UpdateAllViews(this,VM_DELECT_ALTOBJECT,NULL);
	}	
}

void CAirsideMSView::DeleteChildCoutour(HTREEITEM hObjItem)
{
	if (NULL == hObjItem)
	{
		return;
	}

	HTREEITEM hChildItem = GetTreeCtrl().GetNextItem(hObjItem, TVGN_CHILD);

	while (NULL != hChildItem)
	{
		DeleteChildCoutour(hChildItem);

		hChildItem = GetTreeCtrl().GetNextItem(hObjItem, TVGN_CHILD);
	}

	CNodeData* pData = (CNodeData*)GetTreeCtrl().GetItemData(hObjItem);
	ASSERT(pData);
	delete pData;

	GetTreeCtrl().DeleteItem(hObjItem);
}

void CAirsideMSView::DeleteObjectFromTree(HTREEITEM hObjItem)
 {
	TRACE("Delete Item BEGIN \n");
	HTREEITEM hChildItem = hObjItem;

	CNodeData* pData = (CNodeData*)GetTreeCtrl().GetItemData(hChildItem);
	ASSERT(pData);

	if (pData->nSubType == ALT_CONTOUR)
	{
		CNodeData *pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hChildItem);
		if(pNodeData->nodeType != NodeType_ObjectRoot)
		{

			//delete pNodeData;

			CString strItemText = GetTreeCtrl().GetItemText(hChildItem);

			TRACE(strItemText + _T("\n"));
			DeleteChildCoutour(hChildItem);
			//GetTreeCtrl().DeleteItem(hChildItem);
		}
	}
	else
	{
		for(int i =0; i < OBJECT_STRING_LEVEL; ++i)
		{
			if(GetTreeCtrl().GetNextSiblingItem(hChildItem) || GetTreeCtrl().GetPrevSiblingItem(hChildItem))
			{
				CNodeData *pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hChildItem);
				if(pNodeData->nodeType == NodeType_ObjectRoot)
					break;

				delete pNodeData;

				CString strItemText = GetTreeCtrl().GetItemText(hChildItem);

				TRACE(strItemText + _T("\n"));
				GetTreeCtrl().DeleteItem(hChildItem);
				break;
			}
			else
			{
				HTREEITEM hItem = GetTreeCtrl().GetParentItem(hChildItem);

				CNodeData *pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hChildItem);
				if(pNodeData->nodeType == NodeType_ObjectRoot)
					break;

				delete pNodeData;	
				CString strItemText = GetTreeCtrl().GetItemText(hChildItem);		
				TRACE(strItemText + _T("\n"));
				GetTreeCtrl().DeleteItem(hChildItem);
				hChildItem = hItem;
			}
		}
	}
	
	TRACE("Delete Item END \n");
}

HTREEITEM CAirsideMSView::GetObjRootNode(HTREEITEM hObjItem)
{
	HTREEITEM hParentItem = NULL;
	HTREEITEM hChildItem = hObjItem;
	for(int i =0; i < OBJECT_STRING_LEVEL; ++i)
	{
		hParentItem = GetTreeCtrl().GetParentItem(hChildItem);
		CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( hParentItem );
		if(pNodeData == NULL)
			break;

		if (pNodeData->nodeType == NodeType_ObjectRoot)
			break;

		//add by adam 2007-10-14
		if (pNodeData->nSubType == ALT_CONTOUR)
		{
			break;
		}
		//End add by adam 2007-10-14

		hChildItem =  hParentItem;
	}

	return hParentItem;
}
HTREEITEM CAirsideMSView::FindTreeItemByObjectID(int nObjID)
{
	HTREEITEM hRetItem = NULL;
	//find in airports
	HTREEITEM hAirportItem = GetTreeCtrl().GetChildItem(m_hItemAirports);
	while (hAirportItem)
	{
		CNodeData *pNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hAirportItem));
		if (pNodeData)
		{
			if(pNodeData->nodeType != NodeType_Airport)
			{
				hAirportItem = GetTreeCtrl().GetNextSiblingItem(hAirportItem);
				continue;
			}
			HTREEITEM hAircraftFac = GetTreeCtrl().GetChildItem(hAirportItem);
			
			while (hAircraftFac)
			{
				CNodeData *pAFData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hAircraftFac));
				if (pAFData)
				{
					 
					
					if (pAFData->nodeType != NodeType_AircraftFacility && pAFData->nodeType != NodeType_VehicleFacility )
					{
						hAircraftFac = GetTreeCtrl().GetNextSiblingItem(hAircraftFac);
						continue;
					}

					HTREEITEM hManeuverSurface = GetTreeCtrl().GetChildItem(hAircraftFac);


					while (hManeuverSurface)
					{
						CNodeData *pMSData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hManeuverSurface));
						
						
						if (pMSData)
						{
							/*if (pMSData->nodeType != NodeType_ManeuverSurface )
							{
								hManeuverSurface = GetTreeCtrl().GetNextSiblingItem(hManeuverSurface);
								continue;
							}*/

							if(pMSData->nodeType == NodeType_ObjectRoot )
							{
								hRetItem = FindTreeItemByObjectID(hAircraftFac,nObjID);

								if (hRetItem != NULL)
									return hRetItem;

							}	
							if(pMSData->nodeType == NodeType_ManeuverSurface )
							{
								HTREEITEM hObjectRootItem = GetTreeCtrl().GetChildItem(hManeuverSurface);

								while (hObjectRootItem)
								{	
									CNodeData *pRootNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hObjectRootItem));
									CString strText = GetTreeCtrl().GetItemText(hObjectRootItem);
									if (pRootNodeData)
									{
										if (pRootNodeData->nodeType != NodeType_ObjectRoot )
										{
											hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
											continue;
										}
										if (pRootNodeData->bHasExpand == false)
										{
											hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
											continue;
										}

										hRetItem = FindTreeItemByObjectID(hObjectRootItem,nObjID);

										if (hRetItem != NULL)
											return hRetItem;
									}
									hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
								}
							}
                            else if(pMSData->nodeType == NodeType_ControlDevice )
							{
								HTREEITEM hObjectRootItem = GetTreeCtrl().GetChildItem(hManeuverSurface);

								while (hObjectRootItem)
								{	
									CNodeData *pRootNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hObjectRootItem));
									CString strText = GetTreeCtrl().GetItemText(hObjectRootItem);
									if (pRootNodeData)
									{
										if (pRootNodeData->nodeType != NodeType_ObjectRoot )
										{
											hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
											continue;
										}
										if (pRootNodeData->bHasExpand == false)
										{
											hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
											continue;
										}

										hRetItem = FindTreeItemByObjectID(hObjectRootItem,nObjID);

										if (hRetItem != NULL)
											return hRetItem;
									}
									hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
								}
							}						
										

							hManeuverSurface = GetTreeCtrl().GetNextSiblingItem(hManeuverSurface);
						}

					}

					hAircraftFac = GetTreeCtrl().GetNextSiblingItem(hAircraftFac);
				}
			}
		}
		hAirportItem = GetTreeCtrl().GetNextSiblingItem(hAirportItem);
	}

	//find in Airspace
	{
		HTREEITEM hObjectRootItem = GetTreeCtrl().GetChildItem(m_hItemAirspace);

		while (hObjectRootItem)
		{	
			CNodeData *pRootNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hObjectRootItem));
			if (pRootNodeData)
			{
				if (pRootNodeData->nodeType != NodeType_ObjectRoot )
				{	
					hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
					continue;
				}
				if (pRootNodeData->bHasExpand == false)
				{
					hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
					continue;
				}

				hRetItem = FindTreeItemByObjectID(hObjectRootItem,nObjID);

				if (hRetItem != NULL)
					return hRetItem;
			}
			hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
		}
	}

	//find in topography
	{
		HTREEITEM hObjectRootItem = GetTreeCtrl().GetChildItem(m_hItemTopography);

		while (hObjectRootItem)
		{	
			CNodeData *pRootNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hObjectRootItem));
			if (pRootNodeData)
			{
				if (pRootNodeData->nodeType != NodeType_ObjectRoot )
				{	
					hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
					continue;
				}
				if (pRootNodeData->bHasExpand == false)
				{
					hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
					continue;
				}

				hRetItem = FindTreeItemByObjectID(hObjectRootItem,nObjID);

				if (hRetItem != NULL)
					return hRetItem;
			}
			hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
		}
	}

	return NULL;
}
HTREEITEM CAirsideMSView::FindTreeItemByObjectID(HTREEITEM hItem,int nObjID)
{

	HTREEITEM hChildItem = GetTreeCtrl().GetChildItem(hItem);
	while (hChildItem)
	{
		CNodeData *pNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hChildItem));
		if (pNodeData)
		{
			if (pNodeData->nodeType == NodeType_Object && pNodeData->dwData == nObjID)
				return hChildItem;
		}

		HTREEITEM hRetItem = FindTreeItemByObjectID(hChildItem,nObjID);
		if (hRetItem != NULL)
			return hRetItem;
		
		hChildItem = GetTreeCtrl().GetNextSiblingItem(hChildItem);
	}

	return NULL;
}
//HTREEITEM CAirsideMSView::FindObjRootByType(ALTOBJECT_TYPE altType)
//{
//
//
//	//find in Airspace
//	{
//		HTREEITEM hObjectRootItem = GetTreeCtrl().GetChildItem(m_hItemAirspace);
//
//		while (hObjectRootItem)
//		{	
//			CNodeData *pRootNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hObjectRootItem));
//			if (pRootNodeData)
//			{
//				if (pRootNodeData->nodeType != NodeType_ObjectRoot )
//				{	
//					hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
//					continue;
//				}
//				if (pRootNodeData->nodeType == altType)
//				{
//					return hObjectRootItem;
//				}
//
//			}
//			hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
//		}
//	}
//
//	//find in topography
//	{
//		HTREEITEM hObjectRootItem = GetTreeCtrl().GetChildItem(m_hItemTopography);
//
//		while (hObjectRootItem)
//		{	
//			CNodeData *pRootNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hObjectRootItem));
//			if (pRootNodeData)
//			{
//				if (pRootNodeData->nodeType != NodeType_ObjectRoot )
//				{	
//					hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
//					continue;
//				}
//				if (pRootNodeData->nodeType == altType)
//				{
//					return hObjectRootItem;
//				}
//			}
//			hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
//		}
//	}
//
//	return NULL;
//}
void CAirsideMSView::OnNewAirport()
{
	try
	{
		CADODatabase::BeginTransaction();
		ALTAirport airport;
		airport.NewAirport(m_nProjID);
		int nAirportID = airport.getID();
		if (nAirportID != -1)
		{
			CAirsideGround asGround;
			asGround.SaveData(nAirportID);
			asGround.SaveCADInfo(nAirportID);
			asGround.SaveGridInfo(nAirportID);
		}	
		
		AddAirportToTree(m_hItemAirports,airport);
		GetDocument()->UpdateAllViews(this,VM_ADD_OR_DELETE_AIRPORT, NULL );

		CADODatabase::CommitTransaction();
	}
	catch (CADOException& e)
	{
		CADODatabase::RollBackTransation();
		CString strError = e.ErrorMessage();
		MessageBox(_T("New airport Error."));
	}


}
void CAirsideMSView::OnDeleteAirport()
{
	CNodeData *pNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(m_hRightClkItem));
	if (pNodeData == NULL)
		return;
	if(pNodeData->nodeType != NodeType_Airport)
		return;

	try
	{	
		CADODatabase::BeginTransaction();
		ALTAirport::DeleteAirport(pNodeData->dwData);
		CAirsideGround asGround;
		asGround.DeleteData(pNodeData->dwData);	
		CADODatabase::CommitTransaction();
		GetTreeCtrl().DeleteItem(m_hRightClkItem);
		GetDocument()->UpdateAllViews(this,VM_ADD_OR_DELETE_AIRPORT, NULL );

		m_hRightClkItem = NULL;
	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		CString strError = e.ErrorMessage();
		MessageBox(_T("Delete Airport Error"));
	}
}

void CAirsideMSView::OnRenameAirport()
{
	CNodeData *pNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(m_hRightClkItem));
	if (pNodeData == NULL)
		return;
	if(pNodeData->nodeType != NodeType_Airport)
		return;

	GetTreeCtrl().EditLabel(m_hRightClkItem);
}

void CAirsideMSView::UpdateAirportLevelOrder()
{
	ASSERT(m_hItemLevels != NULL);

	HTREEITEM hItemChild = GetTreeCtrl().GetChildItem(m_hItemLevels);

	int nOrder = 0;
	while(hItemChild)
	{
		CNodeData *pNodeData =
			reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hItemChild));
		int nLevelID = pNodeData->dwData;
		int nAirportID = pNodeData->nOtherData;
		CAirsideGround airsideGround;
		airsideGround.ReadData(nLevelID);
		airsideGround.SaveData(nAirportID, nOrder);
		nOrder++;
		hItemChild = GetTreeCtrl().GetNextSiblingItem(hItemChild);
	}


}

void CAirsideMSView::OnNewLevelAboveARP()
{
	ASSERT(m_hItemLevels != NULL);

	HTREEITEM hItemChild = GetTreeCtrl().GetChildItem(m_hItemLevels);
	int nCount = 1;
	while(hItemChild && hItemChild != m_hItemBaseLevel)
	{
		nCount++;
		hItemChild = GetTreeCtrl().GetNextSiblingItem(hItemChild);
	}
	CString strNode;
	strNode.Format(_T("ARP +%d"), nCount);

	CAirsideGround airsideGround;
	CNodeData *pNodeDataBaseLevel = 
		reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(m_hItemLevels));
	ASSERT(pNodeDataBaseLevel != NULL);
	int nAirportID = pNodeDataBaseLevel->dwData;
	int nLevelID  = airsideGround.InsertDefaultLevel(nAirportID, strNode, nCount);

	CNodeData *pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Level);
	pNodeData->nIDResource = IDR_CTX_LEVEL_AIRSIDE;
	pNodeData->dwData = nLevelID;
	pNodeData->nOtherData = nAirportID;
	HTREEITEM hBaseLevel = AddItem(strNode,m_hItemLevels, TVI_FIRST);
	GetTreeCtrl().SetItemData(hBaseLevel,(DWORD_PTR)pNodeData);

	UpdateAirportLevelOrder();
	
	GetDocument()->UpdateAllViews(this);
}

void CAirsideMSView::OnNewLevelBelowARP()
{
	HTREEITEM hTreeItem = GetTreeCtrl().GetNextSiblingItem(m_hItemBaseLevel);
	int nCount = 1;
	while(hTreeItem)
	{
		nCount++;
		hTreeItem = GetTreeCtrl().GetNextSiblingItem(hTreeItem);
	}
	CString strNode;
	strNode.Format(_T("ARP -%d"), nCount);

	CAirsideGround airsideGround;
	CNodeData *pNodeDataBaseLevel = 
		reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(m_hItemLevels));
	ASSERT(pNodeDataBaseLevel != NULL);
	int nAirportID = pNodeDataBaseLevel->dwData;
	int nLevelID  = airsideGround.InsertDefaultLevel(nAirportID, strNode, nCount);

	CNodeData *pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Level);
	pNodeData->dwData = nLevelID;
	pNodeData->nOtherData = nAirportID;
	pNodeData->nIDResource = IDR_CTX_LEVEL_AIRSIDE;
	HTREEITEM hLevels = AddItem(strNode, m_hItemLevels, TVI_LAST);
	GetTreeCtrl().SetItemData(hLevels,(DWORD_PTR)pNodeData);

	UpdateAirportLevelOrder();

	GetDocument()->UpdateAllViews(this);
}

void CAirsideMSView::OnDeleteAirsideLevel()
{
	CNodeData *pNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(m_hRightClkItem));
	if (pNodeData == NULL)
		return;

	if(pNodeData->nodeType != NodeType_Level)
		return;
	if(pNodeData->nSubType == 1)//Base Level cannot be deleted
	{
		AfxMessageBox("Base level cannot be deleted");
		return;
	}

	CString sMsg;
	sMsg.Format("Are you sure you want to delete the level \'%s\'?", GetTreeCtrl().GetItemText(m_hRightClkItem));
	if(AfxMessageBox(sMsg, MB_YESNO) == IDNO) {
		return;
	}

	CAirsideGround airsideGround;
	airsideGround.DeleteData(pNodeData->dwData);
	GetTreeCtrl().DeleteItem(m_hRightClkItem);

	GetDocument()->UpdateAllViews(this);
}

void CAirsideMSView::OnRenameAirsideLevel()
{
	CNodeData *pNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(m_hRightClkItem));
	if (pNodeData == NULL)
		return;
	if(pNodeData->nodeType != NodeType_Level)
		return;

	GetTreeCtrl().EditLabel(m_hRightClkItem);

}

/*
defined in document
#define VM_SELECT_ALTOBJECT WM_USER + 2048
#define VM_DELECT_ALTOBJECT WM_USER + 2049
#define VM_CHANGENAME_ALTOBJECT WM_USER + 2050
#define VM_NEW_MULT_ALTOBJECTS WM_USER + 2051
#define VM_NEW_SINGLE_ALTOBJECT WM_USER + 2052
#define VM_MODIFY_ALTOBJECT + 2503
*/
void MSV::CAirsideMSView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	//if(pSender = this)
	//	return;

	switch( (int)lHint)
	{
	case VM_DELECT_ALTOBJECT:
		{
			ALTObjectList* deleteList = (ALTObjectList*)(pHint);
			for (int i = 0; i < static_cast<int>(deleteList->size()); ++ i)
			{
				int nObjID = (*deleteList)[i]->getID();
				HTREEITEM hTreeItem = FindTreeItemByObjectID(nObjID);
				if (hTreeItem)
				{
					DeleteObjectFromTree(hTreeItem);
				}
				////////////////////////////delete surface node
				if((*deleteList)[i]->GetType() == ALT_TAXIWAY || (*deleteList)[i]->GetType() == ALT_RUNWAY)
				{
					GetTreeCtrl().Expand(m_hObjSurface,TVE_COLLAPSE);
					CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hObjSurface);
					pNodeData->bHasExpand = false;
				}
			}

		}
		break;

	case VM_CHANGENAME_ALTOBJECT:
		{
			ALTObject *pObject = (ALTObject *)pHint;
			HTREEITEM hTreeItem = FindTreeItemByObjectID(pObject->getID());
			if (hTreeItem)
			{
				GetTreeCtrl().SelectItem(hTreeItem);
				HTREEITEM hObjRoot = GetObjRootNode(hTreeItem);
				DeleteObjectFromTree(hTreeItem);
				AddObjectToTree(hObjRoot,pObject);
			}

		}
		break;
	case VM_NEW_MULT_ALTOBJECTS:
		{
			ALTObjectList* newList = (ALTObjectList*)(pHint);
			for (int i = 0; i < static_cast<int>(newList->size()); ++ i)
			{
				HTREEITEM hItemRoot = NULL;
				if((*newList)[i]->IsAirportElement())
					hItemRoot = FindAirportObjRootByType((*newList)[i]->GetType());
				else if ((*newList)[i]->IsAirspaceElement())
					hItemRoot = FindAirspaceObjRootByType((*newList)[i]->GetType());
				else
					hItemRoot = FindTopographyObjRootByType((*newList)[i]->GetType());
				if (hItemRoot)
				{
					AddObjectToTree(hItemRoot,(*newList)[i].get());
				}

			}		
		}

	default:
		break;
	}
}
HTREEITEM CAirsideMSView::FindAirportObjRootByType(ALTOBJECT_TYPE altType)
{
	HTREEITEM hRetItem = NULL;

	int nCurAirportID = GetARCDocument()->GetCurrentAirport();
	//find in airports
	HTREEITEM hAirportItem = GetTreeCtrl().GetChildItem(m_hItemAirports);
	while (hAirportItem)
	{
		CNodeData *pNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hAirportItem));
		if (pNodeData)
		{
			if(pNodeData->nodeType != NodeType_Airport)
			{
				hAirportItem = GetTreeCtrl().GetNextSiblingItem(hAirportItem);
				continue;
			}
			if (pNodeData->dwData != nCurAirportID)
			{
				hAirportItem = GetTreeCtrl().GetNextSiblingItem(hAirportItem);
				continue;
			}

			HTREEITEM hAircraftFac = GetTreeCtrl().GetChildItem(hAirportItem);

			while (hAircraftFac)
			{
				CNodeData *pAFData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hAircraftFac));
				if (pAFData)
				{
					if (pAFData->nodeType != NodeType_AircraftFacility )
					{
						hAircraftFac = GetTreeCtrl().GetNextSiblingItem(hAircraftFac);
						continue;
					}

					HTREEITEM hManeuverSurface = GetTreeCtrl().GetChildItem(hAircraftFac);

					while (hManeuverSurface)
					{
						CNodeData *pMSData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hManeuverSurface));
						if (pMSData)
						{

							if (pMSData->nodeType == NodeType_ObjectRoot )
							{
								if(pMSData->nSubType == altType)
									return hManeuverSurface;
							}
							if (pMSData->nodeType != NodeType_ManeuverSurface )
							{
								hManeuverSurface = GetTreeCtrl().GetNextSiblingItem(hManeuverSurface);
								continue;
							}



							HTREEITEM hObjectRootItem = GetTreeCtrl().GetChildItem(hManeuverSurface);

//			HTREEITEM hObjectRootItem = GetTreeCtrl().GetChildItem(hAirportItem);

							while (hObjectRootItem)
							{	
								CNodeData *pRootNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hObjectRootItem));
								CString strText = GetTreeCtrl().GetItemText(hObjectRootItem);
								if (pRootNodeData)
								{
									if (pRootNodeData->nodeType != NodeType_ObjectRoot )
									{
										hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
										continue;
									}
									if (pRootNodeData->nSubType == altType)
									{
										return hObjectRootItem;
									}

								}
								hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
							}
						}
					
						hManeuverSurface = GetTreeCtrl().GetNextSiblingItem(hManeuverSurface);
					}


				}
				
				hAircraftFac = GetTreeCtrl().GetNextSiblingItem(hAircraftFac);
			}
		}
		hAirportItem = GetTreeCtrl().GetNextSiblingItem(hAirportItem);
	}
	return NULL;
}
HTREEITEM CAirsideMSView::FindAirspaceObjRootByType(ALTOBJECT_TYPE altType)
{
	//find in Airspace
	HTREEITEM hObjectRootItem = GetTreeCtrl().GetChildItem(m_hItemAirspace);

	while (hObjectRootItem)
	{	
		CNodeData *pRootNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hObjectRootItem));
		if (pRootNodeData)
		{
			if (pRootNodeData->nodeType != NodeType_ObjectRoot )
			{	
				hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
				continue;
			}
			if (pRootNodeData->nSubType == altType)
			{
				return hObjectRootItem;
			}

		}
		hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
	}

	return NULL;
}
HTREEITEM CAirsideMSView::FindTopographyObjRootByType(ALTOBJECT_TYPE altType)
{

	HTREEITEM hObjectRootItem = GetTreeCtrl().GetChildItem(m_hItemTopography);

	while (hObjectRootItem)
	{	
		CNodeData *pRootNodeData = reinterpret_cast<CNodeData *>(GetTreeCtrl().GetItemData(hObjectRootItem));
		if (pRootNodeData)
		{
			if (pRootNodeData->nodeType != NodeType_ObjectRoot )
			{	
				hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
				continue;
			}
			if (pRootNodeData->nSubType == altType)
			{
				return hObjectRootItem;
			}
		}
		hObjectRootItem = GetTreeCtrl().GetNextSiblingItem(hObjectRootItem);
	}

	return NULL;
}

void MSV::CAirsideMSView::OnNMThemeChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	// This feature requires Windows XP or greater.
	// The symbol _WIN32_WINNT must be >= 0x0501.
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CAirsideMSView::DisplayExternalManual()
{
	if(m_pDlgScheduleRosterContent && m_pDlgScheduleRosterContent->IsWindowVisible()) 
	{
		//hide (close) window
		m_pDlgScheduleRosterContent->DestroyWindow();
		delete m_pDlgScheduleRosterContent;
		m_pDlgScheduleRosterContent = NULL;
	}
	else
	{
		m_pDlgScheduleRosterContent = new CDlgScheduleAndRostContent(&GetARCDocument()->GetTerminal(),GetARCDocument()->GetProjectID(),this);
		m_pDlgScheduleRosterContent->Create(CDlgScheduleAndRostContent::IDD, this);
		m_pDlgScheduleRosterContent->CenterWindow();
		m_pDlgScheduleRosterContent->ShowWindow(SW_SHOW);
	}
	
}
void CAirsideMSView::OnLockALTObject( void )
{
	LockALTObject(true);
}

void CAirsideMSView::OnUnlockALTObject( void )
{
	LockALTObject(false);
}
void CAirsideMSView::LockALTObject( HTREEITEM hTreeItem, bool bLock )
{
	// Set the Lock
	CNodeData* pNodeData = (CNodeData *)GetTreeCtrl().GetItemData(hTreeItem);
	if (pNodeData->nodeType == NodeType_Object)
	{
		bool bFailed = true;
		int nObjID = pNodeData->dwData;
		if (ALTObject::GetLockedByeID(nObjID, bFailed) != bLock && !bFailed)
		{
			ALTObject::SetLockedByID(nObjID, bLock);
			ALTObject* pObj = ALTObject::ReadObjectByID(nObjID);
			if (pObj)
			{
				GetDocument()->UpdateAllViews(this,VM_MODIFY_ALTOBJECT,(CObject*)pObj);
				delete pObj;
			}
		}
	}
	if (m_wndTreeCtrl.ItemHasChildren(hTreeItem))
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = m_wndTreeCtrl.GetChildItem(hTreeItem);

		while (hChildItem != NULL)
		{
			hNextItem = m_wndTreeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
			// call it recursively
			LockALTObject(hChildItem, bLock);
			hChildItem = hNextItem;
		}
	}

}

void CAirsideMSView::LockALTObject( bool bLock )
{
	CNodeData* pNodeData = GetSelectedNodeData();
	ASSERT(pNodeData);
	if (   pNodeData->nodeType == NodeType_ObjectRoot
		|| pNodeData->nodeType == NodeType_ObjectGroup
		|| pNodeData->nodeType == NodeType_Object
		)
	{
		LockALTObject(m_hRightClkItem, bLock);
	}
// 	else if (pNodeData->nodeType == NodeType_ObjectRoot)
// 	{
// 		ALTObject::SetLockedByType((ALTOBJECT_TYPE)pNodeData->nSubType, bLock);
// 	}
}

void CAirsideMSView::OnUpdateAddContour(CCmdUI* pCmdUI)
{
	CNodeData* pNodeData = GetSelectedNodeData();
	bool bFailed = true;
	pCmdUI->Enable(pNodeData
		&& !(pNodeData->nodeType == NodeType_Object && ALTObject::GetLockedByeID(pNodeData->dwData, bFailed)));
}

void CAirsideMSView::OnUpdateObjectDelete(CCmdUI* pCmdUI)
{
	CNodeData* pNodeData = GetSelectedNodeData();
	bool bFailed = true;
	pCmdUI->Enable(pNodeData && pNodeData->nodeType == NodeType_Object && !ALTObject::GetLockedByeID(pNodeData->dwData, bFailed));
}


// CDialog* CAirsideMSView::GetObjectDefineDlg( CNodeData* pNodeData,ALTObject* pObj )
// {
// 	return CAirsideObjectBaseDlg::NewObjectDlg(pObj,m_nProjID, this);
// }

CNodeData* CAirsideMSView::GetSelectedNodeData()
{
	if (m_hRightClkItem != NULL)
	{
		return (CNodeData *)GetTreeCtrl().GetItemData(m_hRightClkItem);
	}
return NULL;
}
void CAirsideMSView::OnUpdateFloorVisible(CCmdUI* pCmdUI) 
{
	CNodeData* pNode = GetSelectedNodeData();
	if(pNode != NULL)
	{
		CAirsideGround asGround;
		asGround.ReadData(pNode->dwData);

		if(asGround.IsVisible()) {
			pCmdUI->SetText(_T("Hide Floor"));
		}
		else {
			pCmdUI->SetText(_T("Show Floor"));
		}
	}
	GetDocument()->UpdateAllViews(this);
}

void CAirsideMSView::OnUpdateGridVisible(CCmdUI* pCmdUI) 
{
	CNodeData* pNode = GetSelectedNodeData();
	if(pNode == NULL )
		return;

	CAirsideGround asGround;
	asGround.ReadGridInfo(pNode->dwData);
	if(asGround.IsGridVisible()) {
		pCmdUI->SetText(_T("Hide Grid"));
	}
	else {
		pCmdUI->SetText(_T("Show Grid"));
	}
	if(asGround.IsVisible())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
	GetDocument()->UpdateAllViews(this);

}



void CAirsideMSView::OnUpdateFloorMonochrome(CCmdUI* pCmdUI)
{
	CNodeData* pNode = GetSelectedNodeData();
	ASSERT(pNode != NULL /*&& pNode->m_nIDResource == IDR_CTX_FLOOR*/);
	
	CAirsideGround asGround;
	asGround.ReadData(pNode->dwData);

	pCmdUI->SetCheck(asGround.IsMonochrome()?1:0);
	pCmdUI->Enable(asGround.IsVisible());
	GetDocument()->UpdateAllViews(this);
}

void CAirsideMSView::OnUpdateOpaqueFloor(CCmdUI* pCmdUI)
{
	CNodeData* pNode = GetSelectedNodeData();
	ASSERT(pNode != NULL /*&& pNode->m_nIDResource == IDR_CTX_FLOOR*/);

	CAirsideGround asGround;
	asGround.ReadData(pNode->dwData);

	pCmdUI->SetCheck(asGround.IsOpaque()?1:0);
	pCmdUI->Enable(TRUE);
	GetDocument()->UpdateAllViews(this);
}

void CAirsideMSView::OnUpdateShowHideMap(CCmdUI* pCmdUI)
{
	CNodeData* pNode = GetSelectedNodeData();
	ASSERT(pNode != NULL  /*&&pNode->m_nIDResource == IDR_CTX_FLOOR*/);

	CAirsideGround asGround;
	asGround.ReadData(pNode->dwData);
	asGround.ReadCADInfo(pNode->dwData);	

	if(asGround.IsShowMap()) {
		pCmdUI->SetText(_T("Hide Map"));
	}
	else {
		pCmdUI->SetText(_T("Show Map"));
	}
	if(asGround.IsMapValid())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);

	GetDocument()->UpdateAllViews(this);
}
void CAirsideMSView::OnUpdateFloorsDelete(CCmdUI* pCmdUI) 
{
//	if( EnvMode_AirSide == GetDocument()->m_systemMode )
		//pCmdUI->Enable(FALSE);
//	else
//		pCmdUI->Enable(TRUE);
	GetDocument()->UpdateAllViews(this);
}
void CAirsideMSView::OnUpdatePlaceMarker(CCmdUI* pCmdUI)
{
	//GetARCDocument()->OnPlaceMarker() ;
	pCmdUI->Enable(TRUE);		
}
void CAirsideMSView::OnUpdatePlaceMarkerLine(CCmdUI* pCmdUI)
{
    //GetARCDocument()->OnPlaceMarker2() ;
	pCmdUI->Enable(TRUE);
}
void CAirsideMSView::OnsetMarker()
{
  C3DView* p3DView = GetARCDocument()->Get3DView();
  if( p3DView == NULL )
  {
	  return ;
  }
  enum FallbackReason enumReason;
  enumReason= PICK_ONEPOINT;

  FallbackData data;
  data.hSourceWnd = GetSafeHwnd();
  if( !(GetARCDocument()->GetMainFrame())->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
  {
	  MessageBox( "Error" );
	  return;
  }
}
void CAirsideMSView::OnsetAlignLine()
{
	C3DView* p3DView = GetARCDocument()->Get3DView();
	if( p3DView == NULL )
	{
		return ;
	}
	enum FallbackReason enumReason;
	enumReason= PICK_TWOPOINTS;

	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();
	if( !(GetARCDocument()->GetMainFrame())->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		return;
	}
}
void CAirsideMSView::OnUpdateAlignMarker(CCmdUI* pCmdUI)
{
   // BOOL res = FALSE ;
	//pCmdUI->Enable(CFloor2::readMarketStateFromDB(EnvMode_AirSide,GetSelectedNodeData()->dwData,res)) ;
	pCmdUI->Enable(use_Marker || use_Marker_line ) ;
}
void CAirsideMSView::OnAlignMarker()
{
	GetARCDocument()->UpdateAllViews(this, VM_ALIGN_AIRSIDE_MARKS);
}

LRESULT CAirsideMSView::SetActiveFloorMarkerLine(WPARAM wParam, LPARAM lParam)
{
	std::vector<ARCVector3>* pData = reinterpret_cast< std::vector<ARCVector3>* >(wParam);
	if(pData->size()>1)
	{
		use_Marker_line = TRUE ;
		use_Marker = FALSE ;
		CAlignLine line ;
		ARCVector2 point ;
		point[0] = pData->at(0)[0];
		point[1] = pData->at(0)[1];
		line.setBeginPoint(point) ;
		point[0] = pData->at(1)[0];
		point[1] = pData->at(1)[1];
		line.setEndPoint(point) ;
		GetARCDocument()->UpdateAllViews(this, VM_SET_AIRSIDEPLACEMARKERLINE, (CObject*)&line);
	//////////////////////////////////////////////////////////////////////////
			//CFloor2::SaveAlignLineFromDB(EnvMode_AirSide,GetSelectedNodeData()->dwData,line) ;
	}if(pData->size() == 1)
	{
		use_Marker = TRUE ;
		use_Marker_line = FALSE ;
//////////////////////////////////////////////////////////////////////////
		ARCVector2 vLoc(pData->at(0)[VX],pData->at(0)[VY]);
		GetARCDocument()->UpdateAllViews(this, VM_SET_AIRSIDEPLACEMARKER, (CObject*)& pData);
	}
	return TRUE ;
}
void CAirsideMSView::OnRemoveAlignLine()
{
	//CFloor2::SaveAlignLineStateToDB(EnvMode_AirSide,GetSelectedNodeData()->dwData,FALSE) ;
	GetARCDocument()->UpdateAllViews(this, VM_REMOVE_AIRSIDEPLACEMARKERLINE);
	use_Marker_line = FALSE ;
}
void CAirsideMSView::OnRemoveMarker()
{
	//CFloor2::SaveMarketStateToDB(EnvMode_AirSide,GetSelectedNodeData()->dwData,FALSE) ;
	GetARCDocument()->UpdateAllViews(this, VM_REMOVE_AIRSIDEPLACEMARKER);
	use_Marker = FALSE ;
}
void CAirsideMSView::OnUpdateRemoveMarker(CCmdUI* pCmdUI)
{
	//BOOL res = FALSE ;
	//CFloor2::readMarketStateFromDB(EnvMode_AirSide,GetSelectedNodeData()->dwData,res) ;
	pCmdUI->Enable(use_Marker) ;
}
void CAirsideMSView::OnUpdateRemoveAlignLine(CCmdUI* pCmdUI)
{
	//BOOL res = FALSE ;
	//CFloor2::ReadAlignLineStateFromDB(EnvMode_AirSide,GetSelectedNodeData()->dwData,res) ;
	pCmdUI->Enable(use_Marker_line) ;
}
void CAirsideMSView::OnUpdateThicknessValue(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}
void CAirsideMSView::OnUpdateVisibleRegins(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}
void CAirsideMSView::OnUpdateInvisibleRegins(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}
void CAirsideMSView::UpdatePopMenu(CCmdTarget* pThis, CMenu* pPopMenu)
{// added by aivin.
	if(pThis==NULL || pPopMenu==NULL)return;

	CCmdUI state;
	state.m_pMenu = pPopMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	state.m_nIndexMax = pPopMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = pPopMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pPopMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // first item of popup can't be routed to
			}
			state.DoUpdate(pThis, FALSE);    // popups are never auto disabled
		}
		else
		{
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			//    set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(pThis, FALSE && state.m_nID < 0xF000);
		}

		// adjust for menu deletions and additions
		UINT nCount = pPopMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}

}
void CAirsideMSView::OnFloorVisible() 
{
	CNodeData* pNode = GetSelectedNodeData();
	ASSERT(pNode != NULL /*&& pNode->m_nIDResource == IDR_CTX_FLOOR*/);

	CAirsideGround asGround;
	asGround.ReadData(pNode->dwData);


	// toggle visibility
	if(asGround.IsVisible()) 
	{
		asGround.IsVisible(FALSE);
	}
	else 
	{
		asGround.IsVisible(TRUE);
	}
	//UpdateViewSharpTextures();
	//UpdateAllViews(NULL);
	asGround.SaveData(pNode->nOtherData);
	if( GetARCDocument()->Get3DViewParent() )
	{
		CAirsideGround * pAirsidelevel = GetARCDocument()->Get3DViewParent()->GetAirside3D()->GetAirportLevel(asGround.getID());
		if(pAirsidelevel)
		{
			pAirsidelevel->ReadData(asGround.getID());	
			GetARCDocument()->UpdateViewSharpTextures();
		}
	}

	GetARCDocument()->UpdateAllViews(NULL);
}
void CAirsideMSView::OnGridVisible() 
{
	CNodeData* pNode = GetSelectedNodeData();
	ASSERT(pNode != NULL /*&& pNode->m_nIDResource == IDR_CTX_FLOOR*/);

	CAirsideGround asGround;
//	asGround.ReadData(pNode->dwData);
	asGround.ReadGridInfo(pNode->dwData);

	// toggle grid visibility
	if(asGround.IsGridVisible())
	{
		asGround.IsGridVisible(FALSE);
	}
	else
	{
		asGround.IsGridVisible(TRUE);
	}
//	UpdateViewSharpTextures();
//	UpdateAllViews(NULL);
	asGround.UpdateGridInfo(pNode->dwData);
	if( GetARCDocument()->Get3DViewParent() )
	{
		CAirsideGround * pAirsidelevel = GetARCDocument()->Get3DViewParent()->GetAirside3D()->GetAirportLevel(asGround.getID());
		if(pAirsidelevel)
		{			
			pAirsidelevel->ReadGridInfo(asGround.getID());
			GetARCDocument()->UpdateViewSharpTextures();
		}
	}

	GetARCDocument()->UpdateAllViews(this);

}

void CAirsideMSView::OnFloorMonochrome()
{
	CNodeData* pNode = GetSelectedNodeData();
	ASSERT(pNode != NULL /*&& pNode->m_nIDResource == IDR_CTX_FLOOR*/);

	CAirsideGround asGround;
	asGround.ReadData(pNode->dwData);

	// if the floor is currently *not* monochrome, get a color from user
	if(!asGround.IsMonochrome()) {
		CColorDialog dlgColor(asGround.GetMonochromeColor(), CC_ANYCOLOR | CC_FULLOPEN);
		if(dlgColor.DoModal() == IDOK) {
			asGround.SetMonochromeColor(dlgColor.GetColor());
		}
	}
	// toggle floor monochrome
	asGround.IsMonochrome(!asGround.IsMonochrome());
	
	asGround.SaveData(pNode->nOtherData);

	if( GetARCDocument()->Get3DViewParent() )
	{
		CAirsideGround * pAirsidelevel = GetARCDocument()->Get3DViewParent()->GetAirside3D()->GetAirportLevel(asGround.getID());
		if(pAirsidelevel)
		{
			pAirsidelevel->SetMonochromeColor(asGround.GetMonochromeColor());
			pAirsidelevel->IsMonochrome(asGround.IsMonochrome());
			pAirsidelevel->InvalidateTexture();
			GetARCDocument()->UpdateViewSharpTextures();
		}
	}	
	GetDocument()->UpdateAllViews(this);
}
void CAirsideMSView::OnShowHideMap()
{
	CNodeData* pNode = GetSelectedNodeData();
	ASSERT(pNode != NULL /*&& pNode->m_nIDResource == IDR_CTX_FLOOR*/);

	CAirsideGround asGround;
	asGround.ReadData(pNode->dwData);

	// toggle show map
	asGround.IsShowMap(!asGround.IsShowMap());	

	asGround.SaveData(pNode->nOtherData);
	
	if( GetARCDocument()->Get3DViewParent() )
	{
		CAirsideGround * pAirsidelevel = GetARCDocument()->Get3DViewParent()->GetAirside3D()->GetAirportLevel(asGround.getID());
		if(pAirsidelevel){
			pAirsidelevel->ReadData(asGround.getID());	
			GetARCDocument()->UpdateViewSharpTextures();
		}
	}
	GetARCDocument()->UpdateAllViews(this);
}

void CAirsideMSView::OnSetVehicleRoutes(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.VehicleRoute(m_nProjID);
}

void CAirsideMSView::OnDefineAirRoutes(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	int nAirRouteID = -1;
	CAirRoute* pAirRoute = NULL;
	AirRouteList m_airRouteList;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	nAirRouteID = airsideNodeHandler.DefineAirRoute(FALSE,m_nProjID,pNodeData->dwData);
	m_airRouteList.ReadData(pNodeData->dwData);
	pAirRoute = m_airRouteList.GetAirRouteByID(nAirRouteID);
	bool bSameAirRouteName = false;
	if( pAirRoute )
	{
		bool bSameAirRouteName = false;
		bSameAirRouteName = CheckAirRouteByName(pAirRoute->getName(),m_hRightClkItem);
		if (bSameAirRouteName)
		{
			pAirRoute->DeleteData();
			::AfxMessageBox("Plz change AirRouteName");
		}
		else
		{
			AddAirRouteToTree(m_hRightClkItem,pAirRoute);
			GetDocument()->UpdateAllViews(this,VM_NEW_SINGLE_ALTOBJECT,NULL);
		}
	}
}

void CAirsideMSView::OnFlightSchedule(void)
{
	CFlightScheduleDlg dlg( this );
	dlg.DoModal();
}

void CAirsideMSView::OnSetItinerantTraffic(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.ItinerantTraffic(m_nProjID);
}

void CAirsideMSView::OnEditAirportSectorsDB(void)
{
	CDlgDBAirports dlg(FALSE,&GetARCDocument()->GetTerminal());
	dlg.setAirportMan( GetARCDocument()->GetTerminal().m_pAirportDB->getAirportMan() );
	dlg.DoModal();
}

void CAirsideMSView::OnEditAirlineGroupDB(void)
{
	CDlgDBAirline dlg( FALSE,&GetARCDocument()->GetTerminal() );
	dlg.setAirlinesMan( GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan() );
	dlg.DoModal();
}

void CAirsideMSView::OnEditAircraftTypeAndCategory(void)
{
	CDlgDBAircraftTypes dlg(FALSE, &GetARCDocument()->GetTerminal());
	dlg.setAcManager( GetARCDocument()->GetTerminal().m_pAirportDB->getAcMan() );
	dlg.DoModal();
}

void CAirsideMSView::OnEditFliTypeDimension(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.FlightTypeDimensions(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB);
}

void CAirsideMSView::OnEditWingspanCategory(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.WingspanCategories(m_nProjID);
}

void CAirsideMSView::OnEditSurfaceBearingCategory(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.SurfaceBearingWeightCategories(m_nProjID);
}

void CAirsideMSView::OnEditWakeVortexCategory(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.VortexWeightCategories(m_nProjID);
}

void CAirsideMSView::OnEditApproachSpeedCategory(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.SpeedCategories(m_nProjID);
}

void CAirsideMSView::OnEditCruise(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.FlightPerformanceCruise(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
}

void CAirsideMSView::OnEditTerminalManeuver(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.FlightPerformanceTerminal(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
}

void CAirsideMSView::OnEditApproachToLand(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.ApproachToLand(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
}

void CAirsideMSView::OnEditLandingPerformance(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.PerformLanding(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
}

void CAirsideMSView::OnEditTaxiInbound(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.TaxiInbound(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
}

void CAirsideMSView::OnEditStandServicePerformance(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.StandService(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
}

void CAirsideMSView::OnEditPushBackPerformance(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.PushBack(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
}

void CAirsideMSView::OnEditTaxiOutboundPerformance(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.TaxiOutbound(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
}

void CAirsideMSView::OnEditTakeoffPerformance(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.PerformanceTakeoff(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
}

void CAirsideMSView::OnEditDepartureClimbOut(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.DepClimbOut(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
}

void CAirsideMSView::OnEditFlightPlan(void)
{
	CDlgFlightPlan dlg(GetARCDocument()->GetProjectID(),GetARCDocument()->GetTerminal().m_pAirportDB, this);
	dlg.DoModal();
}

void CAirsideMSView::OnEditDepartureSlot(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.DepartureSlotSpecification(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB);
}

void CAirsideMSView::OnEditVehicle(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.DoModalVehicleSpecification(m_nProjID);
}

void CAirsideMSView::OnEditFliTypeLocation(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.FlightTypeRelativeServiceLocations(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB);
}

void CAirsideMSView::OnEditServiceRequirement(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.ServicingRequirement(m_nProjID, GetARCDocument()->GetTerminal().m_pAirportDB);
}

void CAirsideMSView::OnEditWeatherImpact(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.WeatherImpacts(m_nProjID);
}

void CAirsideMSView::OnEditSectorFlightMixSpec()
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.SectorFlightMixSpec(m_nProjID);
}

void CAirsideMSView::OnEditInTrail(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	Terminal* pTerminal = &(GetARCDocument()->GetTerminal()); ;
	airsideNodeHandler.InTrailSeparation(m_nProjID, pTerminal,GetARCDocument()->GetInputAirside().m_pAirportDB, this);
}

void CAirsideMSView::OnEditDirectRouting(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.DirectRoutingCriteria(m_nProjID);	
}

void CAirsideMSView::OnEditApproachSeparation(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.ApprochSeparationCriteria(m_nProjID);
}

void CAirsideMSView::OnControllerIntervention(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.CtrlInterventionSpec(m_nProjID);
}

void CAirsideMSView::OnEditSideStep(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.SideSepSpecification(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);			
}

void CAirsideMSView::OnEditGoAround(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.GoAroundCriteriaCustomize(m_nProjID);
}

void CAirsideMSView::OnEditSTARAssignment(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.StarAssignment(m_nProjID, GetARCDocument()->GetTerminal().m_pAirportDB);
}

void CAirsideMSView::OnEditLandingRunwayAssignment(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.LandingRunwayAssignment(m_nProjID, GetARCDocument()->GetTerminal().m_pAirportDB);
}

void CAirsideMSView::OnEditLandingRunwayExitStrategy(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.LandingRunwayExit(m_nProjID);
}

void CAirsideMSView::OnEditTaxiwayWeight(void)
{
	if (NULL == m_pAirsideNodeHandler)
	{
		m_pAirsideNodeHandler = new AirsideGUI::NodeViewDbClickHandler( 
			GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	}			
	m_pAirsideNodeHandler->SetUnitsManager(UNITSMANAGER);
	m_pAirsideNodeHandler->SetFlightTypeSelectionCallBack(SelectFlightType);
	m_pAirsideNodeHandler->SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	m_pAirsideNodeHandler->SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	m_pAirsideNodeHandler->SetSubAirlineList(pManager->GetSubairlineList());

	m_pAirsideNodeHandler->TaxiwayConstraint(GetARCDocument()->GetProjectID(),WeightConstraint,GetARCDocument()->GetTerminal().m_pAirportDB,this);
}

void CAirsideMSView::OnEditTaxiwayWingspanConstraint(void)
{
	if (NULL == m_pAirsideNodeHandler)
	{
		m_pAirsideNodeHandler = new AirsideGUI::NodeViewDbClickHandler( 
			GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	}			
	m_pAirsideNodeHandler->SetUnitsManager(UNITSMANAGER);
	m_pAirsideNodeHandler->SetFlightTypeSelectionCallBack(SelectFlightType);
	m_pAirsideNodeHandler->SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	m_pAirsideNodeHandler->SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	m_pAirsideNodeHandler->SetSubAirlineList(pManager->GetSubairlineList());

	m_pAirsideNodeHandler->TaxiwayConstraint(GetARCDocument()->GetProjectID(),WingSpanConstraint,GetARCDocument()->GetTerminal().m_pAirportDB,this);
}

void CAirsideMSView::OnEditTaxiwayFliTypeRestriction(void)
{
	if (NULL == m_pAirsideNodeHandler)
	{
		m_pAirsideNodeHandler = new AirsideGUI::NodeViewDbClickHandler( 
			GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	}			
	m_pAirsideNodeHandler->SetUnitsManager(UNITSMANAGER);
	m_pAirsideNodeHandler->SetFlightTypeSelectionCallBack(SelectFlightType);
	m_pAirsideNodeHandler->SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	m_pAirsideNodeHandler->SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	m_pAirsideNodeHandler->SetSubAirlineList(pManager->GetSubairlineList());

	m_pAirsideNodeHandler->FlightTypeRestrictions(GetARCDocument()->GetProjectID(),GetARCDocument()->GetTerminal().m_pAirportDB, this);
}

void CAirsideMSView::OnEditDirectionalityConstraint(void)
{
	if (NULL == m_pAirsideNodeHandler)
	{
		m_pAirsideNodeHandler = new AirsideGUI::NodeViewDbClickHandler( 
			GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	}			
	m_pAirsideNodeHandler->SetUnitsManager(UNITSMANAGER);
	m_pAirsideNodeHandler->SetFlightTypeSelectionCallBack(SelectFlightType);
	m_pAirsideNodeHandler->SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	m_pAirsideNodeHandler->SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	m_pAirsideNodeHandler->SetSubAirlineList(pManager->GetSubairlineList());

	std::vector<ALTAirport> vAirport;
	ALTAirport::GetAirportList(m_nProjID,vAirport);
	m_pAirsideNodeHandler->DirectionalityConstraints(m_nProjID, vAirport.at(0).getID());
}

void CAirsideMSView::OnEditTaxiSpeedConstraint(void)
{
	if (NULL == m_pAirsideNodeHandler)
	{
		m_pAirsideNodeHandler = new AirsideGUI::NodeViewDbClickHandler( 
			GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	}			
	m_pAirsideNodeHandler->SetFlightTypeSelectionCallBack(SelectFlightType);
	m_pAirsideNodeHandler->SetFlightTypeFuncSelectionCallBack(FuncSelectFlightType);

	m_pAirsideNodeHandler->TaxiSpeedConstraints(m_nProjID, GetARCDocument()->GetTerminal().m_pAirportDB);
}

void CAirsideMSView::OnEditInboundRouteAssignment(void)
{
	if (NULL == m_pAirsideNodeHandler)
	{
		m_pAirsideNodeHandler = new AirsideGUI::NodeViewDbClickHandler( 
			GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	}			
	m_pAirsideNodeHandler->SetUnitsManager(UNITSMANAGER);
	
	m_pAirsideNodeHandler->InboundRouteAssignment(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);
}

void CAirsideMSView::OnEditOutboundRouteAssignment(void)
{
	if (NULL == m_pAirsideNodeHandler)
	{
		m_pAirsideNodeHandler = new AirsideGUI::NodeViewDbClickHandler( 
			GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	}			
	m_pAirsideNodeHandler->SetUnitsManager(UNITSMANAGER);

	m_pAirsideNodeHandler->OutboundRouteAssignment(m_nProjID, GetARCDocument()->GetInputAirside().m_pAirportDB);
}

void CAirsideMSView::OnEditTemporaryParking(void)
{
	if (NULL == m_pAirsideNodeHandler)
	{
		m_pAirsideNodeHandler = new AirsideGUI::NodeViewDbClickHandler( 
			GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	}			
	m_pAirsideNodeHandler->SetUnitsManager(UNITSMANAGER);
	m_pAirsideNodeHandler->SetFlightTypeSelectionCallBack(SelectFlightType);
	m_pAirsideNodeHandler->SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	m_pAirsideNodeHandler->SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	m_pAirsideNodeHandler->SetSubAirlineList(pManager->GetSubairlineList());

	m_pAirsideNodeHandler->TemporaryParkingCriteria(GetARCDocument()->GetProjectID(),this);
}

void CAirsideMSView::OnEditConflictResolution(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.ConflictResolutionCriteria(m_nProjID,GetARCDocument()->GetInputAirside().m_pAirportDB);		
}

void CAirsideMSView::OnEditParkingStandBuffer(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.StandBuffers(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB);
}

void CAirsideMSView::OnEditStandAssignment(void)
{
	CDlgStandAssignment dlg (m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB, this);
	dlg.DoModal();
}

void CAirsideMSView::OnOccupyStandAction(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.OccupiedAssignedStandAction(m_nProjID);
}

void CAirsideMSView::OnTaxiOutPushBack(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.PushBackClearanceCriteria(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB);
}


void CAirsideMSView::OnEditTakeoffRunwayAssignment(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());
	std::vector<ALTAirport> vAirport;
	ALTAirport::GetAirportList(m_nProjID,vAirport);
	airsideNodeHandler.TakeoffRunwayAssignment(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB,vAirport.at(0).getID());
}

void CAirsideMSView::OnEditSIDAssignment(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.SidAssignment(m_nProjID, GetARCDocument()->GetTerminal().m_pAirportDB);
}

void CAirsideMSView::OnEditTakeoffQueue(void)
{
	if (NULL == m_pAirsideNodeHandler)
	{
		m_pAirsideNodeHandler = new AirsideGUI::NodeViewDbClickHandler( 
			GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	}			
	m_pAirsideNodeHandler->SetUnitsManager(UNITSMANAGER);

	m_pAirsideNodeHandler->TakeoffQueues(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB);
}

void CAirsideMSView::OnEditRunwayTakeoffPosition(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.RunwayTakeoffPosition(m_nProjID);
}

void CAirsideMSView::OnEditTakeoffSequence(void)
{
	if (NULL == m_pAirsideNodeHandler)
	{
		m_pAirsideNodeHandler = new AirsideGUI::NodeViewDbClickHandler( 
			GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	}			
	m_pAirsideNodeHandler->SetUnitsManager(UNITSMANAGER);
	m_pAirsideNodeHandler->SetFlightTypeSelectionCallBack(SelectFlightType);
	m_pAirsideNodeHandler->SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	m_pAirsideNodeHandler->SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	m_pAirsideNodeHandler->SetSubAirlineList(pManager->GetSubairlineList());
	m_pAirsideNodeHandler->TakeoffSequence(m_nProjID,GetARCDocument()->GetTerminal().m_pAirportDB);
}

void MSV::CAirsideMSView::OnEditTrainFlightsManagement( void )
{	
	if (NULL == m_pAirsideNodeHandler)
	{
		m_pAirsideNodeHandler = new AirsideGUI::NodeViewDbClickHandler( 
			GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	}			
	m_pAirsideNodeHandler->SetUnitsManager(UNITSMANAGER);
	m_pAirsideNodeHandler->TrainingFlgihtSManagement(GetARCDocument()->GetTerminal().m_pAirportDB,m_nProjID,this);
}

void CAirsideMSView::OnEditTakeoffClearance(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.TakeoffClearance(m_nProjID);
}

void CAirsideMSView::OnEditArrivalDelayTrigger(void)
{
	Terminal* pTerminal = &(GetARCDocument()->GetTerminal());
	CDlgArrivalDelayTriggers dlg(GetARCDocument()->GetProjectID(), pTerminal, this);
	dlg.DoModal();	
}

void CAirsideMSView::OnEditVehiclePools(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	airsideNodeHandler.VehiclePoolsAndDeployment(m_nProjID);
}

void CAirsideMSView::OnSetSimulationEngine(void)
{
	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );

	CSimEngSettingDlg dlg(pDoc->GetTerminal().m_pSimParam, this );
	dlg.DoModal();
}

void CAirsideMSView::OnRunSimulation(void)
{
	theApp.GetMainWnd()->SendMessage(WM_COMMAND, ID_PROJECT_RUNSIM);
}

void CAirsideMSView::OnDisplayAircraft(void)
{
	CAircraftDispPropDlg dlg(this);
	dlg.DoModal();
}

void CAirsideMSView::OnDisplayVehicle(void)
{
	CVehicleDispPropDlg dlg(m_nProjID,this);
	dlg.DoModal();
}

void CAirsideMSView::OnSetAircraftTags(void)
{
	CDlgAircraftTags dlg(this);
	dlg.DoModal();
}

void CAirsideMSView::OnSetVehicleTags(void)
{
	CDlgVehicleTags dlg(this);
	dlg.DoModal();
}

void CAirsideMSView::OnCalloutDispSpec(void)
{
	CCalloutDispSpecDlg dlg(FuncSelectFlightType, m_nProjID, GetARCDocument(), this);
	dlg.DoModal();
}

void CAirsideMSView::OnStretchOption(void)
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetStretchOption(m_nProjID);

}

void CAirsideMSView::OnHideControl()
{
	//GetARCDocument()->GetCAirsideInputObject()->GetAirportInfo()->m_bHideControl = !(GetARCDocument()->GetCAirsideInputObject()->GetAirportInfo()->m_bHideControl);
	//GetARCDocument()->UpdateAllViews(NULL);
}


void CAirsideMSView::OnUpdateHideControl(CCmdUI* pCmdUI)
{
	//if(GetARCDocument()->GetCAirsideInputObject()->GetAirportInfo()->m_bHideControl==TRUE)
	//	pCmdUI->SetText(_T("Hide ARP"));
	//else
	//	pCmdUI->SetText(_T("Show ARP"));
}

void CAirsideMSView::OnHoldShortLines(void)
{
	CDlgHoldShortLines dlgHoldShortLines(m_nProjID,this);
	dlgHoldShortLines.DoModal();
}

void CAirsideMSView::AddAirRouteToTree(HTREEITEM hAirItem,CAirRoute* pAirRoute)
{
	ASSERT(pAirRoute != NULL);
	hAirItem = AddItem(pAirRoute->getName(),hAirItem);
	CNodeData* pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
	pNodeData->nodeType = NodeType_Dlg;
	pNodeData->nSubType = Dlg_AirRoute;
	pNodeData->bHasExpand = false;
	pNodeData->nIDResource = IDR_MENU_AIRROUTE_MANAGE;
	pNodeData->nOtherData =  m_nProjID;
	pNodeData->dwData = pAirRoute->getID();
	GetTreeCtrl().SetItemData(hAirItem,(DWORD_PTR)pNodeData);
}

void CAirsideMSView::OnEidtAirRoute()
{
	AirsideGUI::NodeViewDbClickHandler airsideNodeHandler(GetARCDocument()->GetTerminal().GetProcessorList(),&GetARCDocument()->GetInputAirside());
	airsideNodeHandler.SetUnitsManager(UNITSMANAGER); 
	airsideNodeHandler.SetFlightTypeSelectionCallBack(SelectFlightType);
	airsideNodeHandler.SetProbDistEntrySelectionCallBack(SelectProbDistEntry);
	CACCATEGORYLIST* pvACCategory = _g_GetActiveACMan(GetARCDocument()->GetTerminal().m_pAirportDB)->GetACCategoryList();
	airsideNodeHandler.SetACCategoryList(pvACCategory);
	CAirlinesManager* pManager = GetARCDocument()->GetTerminal().m_pAirportDB->getAirlineMan();
	airsideNodeHandler.SetSubAirlineList(pManager->GetSubairlineList());

	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	AirRouteList m_airRouteList;
	CAirRoute* pAirRoute = NULL;
	airsideNodeHandler.DefineAirRoute(TRUE,m_nProjID,pNodeData->dwData);
	m_airRouteList.ReadData(pNodeData->nOtherData);
	pAirRoute = m_airRouteList.GetAirRouteByID(pNodeData->dwData);
	bool bSameAirRouteName = false;
	bSameAirRouteName = CheckAirRouteByName(pAirRoute->getName(),GetTreeCtrl().GetParentItem(m_hRightClkItem));
	if (bSameAirRouteName)
	{
		if (pAirRoute->getName().CompareNoCase(GetTreeCtrl().GetItemText(m_hRightClkItem)) == 0)
		{
			;
		}
		else
		{
			::AfxMessageBox("please change the AirRoute Name");
		}
	}
	else
	{
		m_airRouteList.ReadData(pNodeData->nOtherData);
		pAirRoute = m_airRouteList.GetAirRouteByID(pNodeData->dwData);
		GetTreeCtrl().SetItemText(m_hRightClkItem,pAirRoute->getName());
	}
	GetDocument()->UpdateAllViews(this, VM_MODIFY_AIRROUTE, (CObject*)pNodeData->dwData);
}

void CAirsideMSView::OnDeleteAirRoute()
{
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	CAirRoute* pAirRoute = NULL;
	AirRouteList m_airRouteList;
	m_airRouteList.ReadData(pNodeData->nOtherData);
	pAirRoute = m_airRouteList.GetAirRouteByID(pNodeData->dwData);
	int nCount = m_airRouteList.GetAirRouteCount();
	int nIndex = 0;
	for (;nIndex < nCount;nIndex++)
	{
		if(pAirRoute->getID() == m_airRouteList.GetAirRoute(nIndex)->getID())
		{
			break;
		}
	}
	m_airRouteList.DeleteAirRoute(nIndex);
	pAirRoute->DeleteData();
	GetTreeCtrl().DeleteItem(m_hRightClkItem);
	GetDocument()->UpdateAllViews(this, VM_DELECT_ALTOBJECT, NULL);
}

void CAirsideMSView::OnCopyAirRoute()
{
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	CAirRoute* pAirRoute = NULL;
	AirRouteList m_airRouteList;
	m_airRouteList.ReadData(pNodeData->nOtherData);
	pAirRoute = new CAirRoute(*m_airRouteList.GetAirRouteByID(pNodeData->dwData));
	pAirRoute->setName(pAirRoute->getName() + "_COPY");
	CString strTemp = pAirRoute->getName();
	int nCount = strTemp.GetLength();
	int nIndex = 1;
	while (CheckAirRouteByName(strTemp,GetTreeCtrl().GetParentItem(m_hRightClkItem)))
	{
		strTemp = strTemp.Left(nCount);
		strTemp.Format("%s%d",strTemp,nIndex);
		nIndex++;
	}
	pAirRoute->setName(strTemp);
	pAirRoute->setID(-1);
	for (int i = 0; i < (int)pAirRoute->getARWaypoint().size();i++)
	{
		pAirRoute->getARWaypoint().at(i)->setID(-1);
	}
	
	m_airRouteList.AddAirRoute(pAirRoute);
	m_airRouteList.SaveData(m_nProjID);
	
	AddAirRouteToTree(GetTreeCtrl().GetParentItem(m_hRightClkItem),pAirRoute);
	GetDocument()->UpdateAllViews(this,VM_NEW_SINGLE_ALTOBJECT,NULL);
}


bool CAirsideMSView::CheckAirRouteByName(CString strAirRouteName,HTREEITEM hItem)
{
	HTREEITEM hChildItem = GetTreeCtrl().GetChildItem(hItem);
	while (hChildItem != NULL)
	{
		if (strAirRouteName.CompareNoCase(GetTreeCtrl().GetItemText(hChildItem)) == 0)
		{
			return true;
		}
		hChildItem = GetTreeCtrl().GetNextSiblingItem(hChildItem);
	}
	return false;
}

void CAirsideMSView::OnDisplayAirRouteProperties(void)
{
	int nFirstAirRoute = -1;
	BOOL bAirRouteSimple = FALSE;
	HTREEITEM hFirstItem = NULL;
	ALTObject3D* pObj3D = NULL;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	if (pNodeData->nIDResource == IDR_MENU_AIRROUTES)
	{
		hFirstItem = GetTreeCtrl().GetChildItem(m_hRightClkItem);
		if (pNodeData->bHasExpand == false)
		{
			return;
		}
		nFirstAirRoute = ((CNodeData*)GetTreeCtrl().GetItemData(hFirstItem))->dwData;
		bAirRouteSimple = TRUE;
	}
	else
	{
		nFirstAirRoute = pNodeData->dwData;
	}
	if (GetARCDocument()->Get3DViewParent() == NULL)
	{
		return;
	}
	else
		pObj3D = (ALTObject3D*)GetARCDocument()->Get3DViewParent()->GetAirside3D()->GetObject3D(nFirstAirRoute);

	CDlgAirsideObjectDisplayProperties dlg(pObj3D);
	if (IDOK ==dlg.DoModal())
	{
		if (bAirRouteSimple == TRUE)
		{
			AirRouteList airRouteList;
			CAirRoute* pAirRoute = NULL;
			airRouteList.ReadData(pNodeData->nOtherData);
			int nAirRouteCount = airRouteList.GetAirRouteCount();
			int nIndex = 0;
			for (;nIndex < nAirRouteCount;nIndex++)
			{
				pAirRoute = airRouteList.GetAirRoute(nIndex);
				pObj3D = (ALTObject3D*)GetARCDocument()->Get3DViewParent()->GetAirside3D()->GetObject3D(pAirRoute->getID());
				ALTObjectDisplayProp* p3DProp = pObj3D->GetDisplayProp();
				if (dlg.getZDisplay() > 0)
					p3DProp->m_dpName.bOn = true;
				else
					p3DProp->m_dpName.bOn = false;
				p3DProp->m_dpName.cColor = ARCColor4(dlg.getZColor());

				if(dlg.getODisplay() > 0)
					p3DProp->m_dpShape.bOn = true;
				else
					p3DProp->m_dpShape.bOn  = false;

				p3DProp->m_dpShape.cColor = ARCColor4(dlg.getOColor());
				p3DProp->SaveData(pAirRoute->getID());
			}
		}
	}

	GetARCDocument()->Get3DView()->OnInitialUpdate();
}

void CAirsideMSView::OnDisplayAllAirRouteProperties(void)
{
	OnDisplayAirRouteProperties();
}

void CAirsideMSView::OnEditFlightGroupsDB()
{
	CDlgFlightDB dlg(m_nProjID,&GetARCDocument()->GetTerminal());
	dlg.DoModal();
}

int CAirsideMSView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create the style
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS |TVS_EDITLABELS ;

	BOOL bResult = m_wndTreeCtrl.Create(dwStyle, CRect(0,0,0,0),
		this, IDC_AIRSIDE_TREE);

	return (bResult ? 0 : -1);

}

void CAirsideMSView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (::IsWindow(m_wndTreeCtrl.m_hWnd))
		m_wndTreeCtrl.MoveWindow(0, 0, cx, cy, TRUE);

}

void CAirsideMSView::AddRunwayandTaxiwaytoTree(HTREEITEM hItem)
{
	std::vector<ALTAirport> vAirport;
	try
	{
		ALTAirport::GetAirportList(m_nProjID,vAirport);
	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
		MessageBox(_T("Cann't read the airport information."));
		return;
	}

	for (int nAirportCnt = 0; nAirportCnt < (int)vAirport.size(); nAirportCnt++)
	{
		int nAirportID = vAirport.at(nAirportCnt).getID();
		ALTObject altObject;
		std::vector<ALTObject> vObject1;
		ALTObject::GetObjectList(ALT_RUNWAY,nAirportID,vObject1);

		std::vector<ALTObject> vObject2;
		ALTObject::GetObjectList(ALT_TAXIWAY,nAirportID,vObject2);

		CString strNode = _T("");
		CNodeData* pNodeData = NULL;
		HTREEITEM hRunway = NULL;
		if (vObject1.size() > 0)
		{
			strNode.Format(_T("Runways"));
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
			pNodeData->nOtherData = ALT_RUNWAY;
			pNodeData->nIDResource = IDR_CTX_SURFACESSHOW;
			hRunway = AddItem(strNode,hItem);
			GetTreeCtrl().SetItemData(hRunway,(DWORD_PTR)pNodeData);
		}

		for (int i = 0; i < (int)vObject1.size();i++)
		{
			altObject = vObject1.at(i);
			Runway pObject;
			pObject.ReadObject(altObject.getID());
			{
				strNode.Format(_T("%s"),pObject.GetMarking1().c_str());
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->dwData = altObject.getID();
				pNodeData->nIDResource = IDR_CTX_SHOW;
				pNodeData->nOtherData = 0; // 0 first 1 second
				HTREEITEM hMarking1 = AddItem(strNode,hRunway);
				GetTreeCtrl().SetItemData(hMarking1,(DWORD_PTR)pNodeData);
				{
					strNode.Format(_T("Threshold"));
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
					pNodeData->nIDResource = IDR_CTX_SURFACES;
					pNodeData->nOtherData = (int)ThresHold_Type;
					HTREEITEM hThreshold = AddItem(strNode,hMarking1);
					GetTreeCtrl().SetItemData(hThreshold,(DWORD_PTR)pNodeData);
					{
						strNode.Format(_T("Offset from centreline (%s): %.2f"),CARCLengthUnit::GetLengthUnitString(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
							CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),pObject.GetMark1ThresholdOffset()));
						HTREEITEM hMark1ThreshOffset = AddARCBaseTreeItem(strNode,hThreshold,NET_EDIT_WITH_VALUE);
						GetTreeCtrl().SetItemData(hMark1ThreshOffset,(DWORD_PTR)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),\
							pObject.GetMark1ThresholdOffset()*100)));
						for (int j = 0; j < pObject.GetMark1Threshold().getCount();j++)
						{
							strNode.Format(_T("Surface %d"),j+1);
							pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
							pNodeData->nIDResource = IDR_CTX_SURFACESDEL;
							pNodeData->dwData = pObject.GetMark1Threshold().getItem(j)->nID;
							HTREEITEM hSurface1 = AddItem(strNode,hThreshold);
							GetTreeCtrl().SetItemData(hSurface1,(DWORD_PTR)pNodeData);
							{
								strNode.Format(_T("Distance (%s): %.2f"),CARCLengthUnit::GetLengthUnitString(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
									CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),pObject.GetMark1ThresholdDistance(j)));
								HTREEITEM hMark1ThreshDistance = AddARCBaseTreeItem(strNode,hSurface1,NET_EDIT_WITH_VALUE);
								GetTreeCtrl().SetItemData(hMark1ThreshDistance,(DWORD_PTR)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),\
									pObject.GetMark1ThresholdDistance(j)*100)));

								strNode.Format(_T("Angle at offset line (deg): %d"),pObject.GetMark1ThresholdAngle(j));
								HTREEITEM hMark1ThreshAngle = AddARCBaseTreeItem(strNode,hSurface1,NET_EDIT_WITH_VALUE);
								GetTreeCtrl().SetItemData(hMark1ThreshAngle,(DWORD_PTR)(pObject.GetMark1ThresholdAngle(j)*100));
							}
						}
					}

					strNode.Format(_T("Lateral"));
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
					pNodeData->nIDResource = IDR_CTX_SURFACES;
					pNodeData->nOtherData = (int)Lateral_Type;
					HTREEITEM hLateral = AddItem(strNode,hMarking1);
					GetTreeCtrl().SetItemData(hLateral,(DWORD_PTR)pNodeData);
					{
						strNode.Format(_T("Offset from centreline (%s): %.2f"),CARCLengthUnit::GetLengthUnitString(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
							CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),pObject.GetMark1LateralOffset()));
						HTREEITEM hMark1LateralOffset = AddARCBaseTreeItem(strNode,hLateral,NET_EDIT_WITH_VALUE);
						GetTreeCtrl().SetItemData(hMark1LateralOffset,(DWORD_PTR)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),\
							pObject.GetMark1LateralOffset()*100)));
						for (int k = 0; k < pObject.GetMark1Lateral().getCount();k++)
						{
							strNode.Format(_T("Surface %d"),k+1);
							pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
							pNodeData->nIDResource = IDR_CTX_SURFACESDEL;
							pNodeData->dwData = pObject.GetMark1Lateral().getItem(k)->nID;
							HTREEITEM hSurface1 = AddItem(strNode,hLateral);
							GetTreeCtrl().SetItemData(hSurface1,(DWORD_PTR)pNodeData);
							{
								strNode.Format(_T("Distance (%s): %.2f"),CARCLengthUnit::GetLengthUnitString(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
									CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),pObject.GetMark1LateralDistance(k)));
								HTREEITEM hMark1LateralDistance = AddARCBaseTreeItem(strNode,hSurface1,NET_EDIT_WITH_VALUE);
								GetTreeCtrl().SetItemData(hMark1LateralDistance,(DWORD_PTR)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),\
									pObject.GetMark1LateralDistance(k)*100)));

								strNode.Format(_T("Angle at offset line (deg): %d"),pObject.GetMark1LateralAngle(k));
								HTREEITEM hMark1LateralAngle = AddARCBaseTreeItem(strNode,hSurface1,NET_EDIT_WITH_VALUE);
								GetTreeCtrl().SetItemData(hMark1LateralAngle,(DWORD_PTR)(pObject.GetMark1LateralAngle(k)*100));
							}
						}
					}

					strNode.Format(_T("End of Runway"));
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
					pNodeData->nIDResource = IDR_CTX_SURFACES;
					pNodeData->nOtherData = (int)EndOfRunway_Type;
					HTREEITEM hEndofRun = AddItem(strNode,hMarking1);
					GetTreeCtrl().SetItemData(hEndofRun,(DWORD_PTR)pNodeData);
					{
						strNode.Format(_T("Offset from centreline (%s): %.2f"),CARCLengthUnit::GetLengthUnitString(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
							CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),pObject.GetMark1EndOfRunwayOffset()));
						HTREEITEM hMark1EndOffset = AddARCBaseTreeItem(strNode,hEndofRun,NET_EDIT_WITH_VALUE);
						GetTreeCtrl().SetItemData(hMark1EndOffset,(DWORD_PTR)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),\
							pObject.GetMark1EndOfRunwayOffset()*100)));
						for (int n = 0; n < pObject.GetMark1EnOfRunway().getCount(); n++)
						{
							strNode.Format(_T("Surface %d"),n+1);
							pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
							pNodeData->nIDResource = IDR_CTX_SURFACESDEL;
							pNodeData->dwData = pObject.GetMark1EnOfRunway().getItem(n)->nID;
							HTREEITEM hSurface1 = AddItem(strNode,hEndofRun);
							GetTreeCtrl().SetItemData(hSurface1,(DWORD_PTR)pNodeData);
							{
								strNode.Format(_T("Distance (%s): %.2f"),CARCLengthUnit::GetLengthUnitString(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
									CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),pObject.GetMark1EndOfRunwayDistance(n)));
								HTREEITEM hMark1EndDistance = AddARCBaseTreeItem(strNode,hSurface1,NET_EDIT_WITH_VALUE);
								GetTreeCtrl().SetItemData(hMark1EndDistance,(DWORD_PTR)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),\
									pObject.GetMark1EndOfRunwayDistance(n)*100)));

								strNode.Format(_T("Angle at offset line (deg): %d"),pObject.GetMark1EndOfRunwayAngle(n));
								HTREEITEM hMark1EndAngle = AddARCBaseTreeItem(strNode,hSurface1,NET_EDIT_WITH_VALUE);
								GetTreeCtrl().SetItemData(hMark1EndAngle,(DWORD_PTR)(pObject.GetMark1EndOfRunwayAngle(n)*100));
							}
						}
					}
				}
				strNode.Format(_T("%s"),pObject.GetMarking2().c_str());
				pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
				pNodeData->nIDResource = IDR_CTX_SHOW;
				pNodeData->dwData = altObject.getID();
				pNodeData->nOtherData = 1;
				HTREEITEM hMarking2 = AddItem(strNode,hRunway);
				GetTreeCtrl().SetItemData(hMarking2,(DWORD_PTR)pNodeData);
				{
					strNode.Format(_T("Threshold"));
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
					pNodeData->nOtherData = (int)ThresHold_Type;
					pNodeData->nIDResource = IDR_CTX_SURFACES;
					HTREEITEM hThreshold = AddItem(strNode,hMarking2);
					GetTreeCtrl().SetItemData(hThreshold,(DWORD_PTR)pNodeData);
					{
						strNode.Format(_T("Offset from centreline (%s): %.2f"),CARCLengthUnit::GetLengthUnitString(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
							CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),pObject.GetMark2ThresholdOffset()));
						HTREEITEM hMark2ThreshOffset = AddARCBaseTreeItem(strNode,hThreshold,NET_EDIT_WITH_VALUE);
						GetTreeCtrl().SetItemData(hMark2ThreshOffset,(DWORD_PTR)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),\
							pObject.GetMark2ThresholdOffset()*100)));
						for (int m = 0; m < pObject.GetMark2Threshold().getCount();m++)
						{
							strNode.Format(_T("Surface %d"),m+1);
							pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
							pNodeData->nIDResource = IDR_CTX_SURFACESDEL;
							pNodeData->dwData = pObject.GetMark2Threshold().getItem(m)->nID;
							HTREEITEM hSurface2 = AddItem(strNode,hThreshold);
							GetTreeCtrl().SetItemData(hSurface2,(DWORD_PTR)pNodeData);
							{
								strNode.Format(_T("Distance (%s): %.2f"),CARCLengthUnit::GetLengthUnitString(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
									CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),pObject.GetMark2ThresholdDistance(m)));
								HTREEITEM hMark2ThreshDistance = AddARCBaseTreeItem(strNode,hSurface2,NET_EDIT_WITH_VALUE);
								GetTreeCtrl().SetItemData(hMark2ThreshDistance,(DWORD_PTR)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),\
									pObject.GetMark2ThresholdDistance(m)*100)));

								strNode.Format(_T("Angle at offset line (deg): %d"),pObject.GetMark2ThresholdAngle(m));
								HTREEITEM hMark2ThreshAngle = AddARCBaseTreeItem(strNode,hSurface2,NET_EDIT_WITH_VALUE);
								GetTreeCtrl().SetItemData(hMark2ThreshAngle,(DWORD_PTR)(pObject.GetMark2ThresholdAngle(m)*100));
							}
						}
					}

					strNode.Format(_T("Lateral"));
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
					pNodeData->nIDResource = IDR_CTX_SURFACES;
					pNodeData->nOtherData = (int)Lateral_Type;
					HTREEITEM hLateral = AddItem(strNode,hMarking2);
					GetTreeCtrl().SetItemData(hLateral,(DWORD_PTR)pNodeData);
					{
						strNode.Format(_T("Offset from centreline (%s): %.2f"),CARCLengthUnit::GetLengthUnitString(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
							CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),pObject.GetMark2LateralOffset()));
						HTREEITEM hMark2LateralOffset = AddARCBaseTreeItem(strNode,hLateral,NET_EDIT_WITH_VALUE);
						GetTreeCtrl().SetItemData(hMark2LateralOffset,(DWORD_PTR)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),\
							pObject.GetMark2LateralOffset()*100)));

						for (int n = 0; n < pObject.GetMark2Lateral().getCount(); n++)
						{
							strNode.Format(_T("Surface %d"),n+1);
							pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
							pNodeData->nIDResource = IDR_CTX_SURFACESDEL;
							pNodeData->dwData = pObject.GetMark2Lateral().getItem(n)->nID;
							HTREEITEM hSurface2 = AddItem(strNode,hLateral);
							GetTreeCtrl().SetItemData(hSurface2,(DWORD_PTR)pNodeData);
							{
								strNode.Format(_T("Distance (%s): %.2f"),CARCLengthUnit::GetLengthUnitString(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
									CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),pObject.GetMark2LateralDistance(n)));
								HTREEITEM hMark2LateralDistance = AddARCBaseTreeItem(strNode,hSurface2,NET_EDIT_WITH_VALUE);
								GetTreeCtrl().SetItemData(hMark2LateralDistance,(DWORD_PTR)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),\
									pObject.GetMark2LateralDistance(n)*100)));

								strNode.Format(_T("Angle at offset line (deg): %d"),pObject.GetMark2LateralAngle(n));
								HTREEITEM hMark2LateralAngle = AddARCBaseTreeItem(strNode,hSurface2,NET_EDIT_WITH_VALUE);
								GetTreeCtrl().SetItemData(hMark2LateralAngle,(DWORD_PTR)(pObject.GetMark2LateralAngle(n)*100));
							}
						}
					}

					strNode.Format(_T("End of Runway"));
					pNodeData->nIDResource = IDR_CTX_SURFACES;
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
					pNodeData->nOtherData = (int)EndOfRunway_Type;
					HTREEITEM hEndofRun = AddItem(strNode,hMarking2);
					GetTreeCtrl().SetItemData(hEndofRun,(DWORD_PTR)pNodeData);
					{
						strNode.Format(_T("Offset from centreline (%s): %.2f"),CARCLengthUnit::GetLengthUnitString(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
							CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),pObject.GetMark2EndOfRunwayOffset()));
						HTREEITEM hMark2EndOffset = AddARCBaseTreeItem(strNode,hEndofRun,NET_EDIT_WITH_VALUE);
						GetTreeCtrl().SetItemData(hMark2EndOffset,(DWORD_PTR)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),\
							pObject.GetMark2EndOfRunwayOffset()*100)));

						for (int n = 0; n < pObject.GetMark2EndOfRunway().getCount(); n++)
						{
							strNode.Format(_T("Surface %d"),n+1);
							pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
							pNodeData->nIDResource = IDR_CTX_SURFACESDEL;
							pNodeData->dwData = pObject.GetMark2EndOfRunway().getItem(n)->nID;
							HTREEITEM hSurface2 = AddItem(strNode,hEndofRun);
							GetTreeCtrl().SetItemData(hSurface2,(DWORD_PTR)pNodeData);
							{
								strNode.Format(_T("Distance (%s): %.2f"),CARCLengthUnit::GetLengthUnitString(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
									CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),pObject.GetMark2EndOfRunwayDistance(n)));
								HTREEITEM hMark2EndDistance = AddARCBaseTreeItem(strNode,hSurface2,NET_EDIT_WITH_VALUE);
								GetTreeCtrl().SetItemData(hMark2EndDistance,(DWORD_PTR)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),\
									pObject.GetMark2EndOfRunwayDistance(n)*100)));

								strNode.Format(_T("Angle at offset line (deg): %d"),pObject.GetMark2EndOfRunwayAngle(n));
								HTREEITEM hMark2EndAngle = AddARCBaseTreeItem(strNode,hSurface2,NET_EDIT_WITH_VALUE);
								GetTreeCtrl().SetItemData(hMark2EndAngle,(DWORD_PTR)(pObject.GetMark2EndOfRunwayAngle(n)*100));
							}
						}
					}
				}
			}
		}

		HTREEITEM hTaxiway = NULL;
		if(vObject2.size() > 0)
		{
			strNode.Format(_T("Taxiways"));
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
			pNodeData->nOtherData = ALT_TAXIWAY;
			pNodeData->nIDResource = IDR_CTX_SURFACESSHOW;
			hTaxiway = AddItem(strNode,hItem);
			GetTreeCtrl().SetItemData(hTaxiway,(DWORD_PTR)pNodeData);
		}

		for (int i = 0; i < (int)vObject2.size();i++)
		{
			altObject = vObject2.at(i);
			Taxiway pObject;
			pObject.ReadObject(altObject.getID());
			strNode.Format(_T("%s"),pObject.GetMarking().c_str());
			pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
			pNodeData->dwData = altObject.getID();
			pNodeData->nOtherData = No_Type;
			pNodeData->nIDResource = IDR_CTX_SURFACESTAX;
			HTREEITEM hMarking = AddItem(strNode,hTaxiway);
			GetTreeCtrl().SetItemData(hMarking,(DWORD_PTR)pNodeData);
			{
				strNode.Format(_T("Offset from centreline (%s): %.2f"),CARCLengthUnit::GetLengthUnitString(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
					CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),pObject.GetOffset()));
				HTREEITEM hMarkOffset = AddARCBaseTreeItem(strNode,hMarking,NET_EDIT_WITH_VALUE);
				GetTreeCtrl().SetItemData(hMarkOffset,(DWORD_PTR)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),\
					pObject.GetOffset()*100)));

				for (int n = 0; n < pObject.getSurface().getCount(); n++)
				{
					pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
					pNodeData->dwData = pObject.getSurface().getItem(n)->nID;
					pNodeData->nIDResource = IDR_CTX_SURFACESDEL;
					strNode.Format(_T("Surface %d"),n+1);
					HTREEITEM hSurface = AddItem(strNode,hMarking);
					GetTreeCtrl().SetItemData(hSurface,(DWORD_PTR)pNodeData);
					{
						strNode.Format(_T("Distance (%s): %.2f"),CARCLengthUnit::GetLengthUnitString(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
							CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),pObject.GetDistance(n)));
						HTREEITEM hMarkDistance = AddARCBaseTreeItem(strNode,hSurface,NET_EDIT_WITH_VALUE);
						GetTreeCtrl().SetItemData(hMarkDistance,(DWORD_PTR)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),\
							pObject.GetDistance(n)*100)));

						strNode.Format(_T("Angle at offset line (deg): %d"),pObject.GetAngle(n));
						HTREEITEM hMarkAngle = AddARCBaseTreeItem(strNode,hSurface,NET_EDIT_WITH_VALUE);
						GetTreeCtrl().SetItemData(hMarkAngle,(DWORD_PTR)(pObject.GetAngle(n)*100));
					}
				}
			}
		}
	}	
}

void CAirsideMSView::OnAddSurface()
{
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	HTREEITEM hParent = GetTreeCtrl().GetParentItem(m_hRightClkItem);
	SurfaceType emType = (SurfaceType)pNodeData->nOtherData;
	SurfaceData* pData = new SurfaceData();
	int nID = 0;
	int nAngle = 0;
	double dDistance = 0.0;
	int nCount = 0;
	switch(emType)
	{
	case No_Type:
		{
			Taxiway altObject;
			altObject.ReadObject(pNodeData->dwData);
			altObject.getSurface().AddItem(pData);
			pData->nID = altObject.getSurface().GetUniqeID();
			nID = pData->nID;
			nCount = altObject.getSurface().getCount()-1;
			dDistance = pData->dDistance;
			nAngle = pData->nAngle;
			altObject.UpdateSurfaceData();
			GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&altObject );
		}
		break;
	case ThresHold_Type:
		{
			pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hParent);
			Runway altObject;
			altObject.ReadObject(pNodeData->dwData);
			if (pNodeData->nOtherData == 0)
			{
				pData->nID = altObject.GetMark1Threshold().GetUniqeID();
				altObject.GetMark1Threshold().AddItem(pData);
				nCount = altObject.GetMark1Threshold().getCount() -1;
			}
			else
			{
				pData->nID = altObject.GetMark2Threshold().GetUniqeID();
				altObject.GetMark2Threshold().AddItem(pData);
				nCount = altObject.GetMark2Threshold().getCount()-1;
			}
			nID = pData->nID;
			dDistance = pData->dDistance;
			nAngle = pData->nAngle;
			altObject.UpdataSurfaceData();
			GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&altObject );
		}
		break;
	case Lateral_Type:
		{
			pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hParent);
			Runway altObject;
			altObject.ReadObject(pNodeData->dwData);
			if (pNodeData->nOtherData == 0)
			{
				pData->nID = altObject.GetMark1Lateral().GetUniqeID();
				altObject.GetMark1Lateral().AddItem(pData);
				nCount = altObject.GetMark1Lateral().getCount()-1;
			}
			else
			{
				pData->nID = altObject.GetMark2Lateral().GetUniqeID();
				altObject.GetMark2Lateral().AddItem(pData);
				nCount = altObject.GetMark2Lateral().getCount()-1;
			}
			nID = pData->nID;
			dDistance = pData->dDistance;
			nAngle = pData->nAngle;
			altObject.UpdataSurfaceData();
			GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&altObject );
		}
		break;
	case EndOfRunway_Type:
		{
			pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hParent);
			Runway altObject;
			altObject.ReadObject(pNodeData->dwData);
			if (pNodeData->nOtherData == 0)
			{
				pData->nID = altObject.GetMark1EnOfRunway().GetUniqeID();
				altObject.GetMark1EnOfRunway().AddItem(pData);
				nCount = altObject.GetMark1EnOfRunway().getCount()-1;
			}
			else
			{
				pData->nID = altObject.GetMark2EndOfRunway().GetUniqeID();
				altObject.GetMark2EndOfRunway().AddItem(pData);
				nCount = altObject.GetMark2EndOfRunway().getCount()-1;
			}
			nID = pData->nID;
			dDistance = pData->dDistance;
			nAngle = pData->nAngle;
			altObject.UpdataSurfaceData();
			GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&altObject );
		}
		break;
	default:
		break;
	}
	CString strNode = _T("");
	pNodeData = mpNodeDataAlloc->allocateNodeData(NodeType_Normal);
	pNodeData->dwData = (DWORD)nCount;
	pNodeData->nIDResource = IDR_CTX_SURFACESDEL;
	///////////////////check fo unquie name
	bool bFind = TRUE;
	int nUnqiue = 1;
	while (bFind)
	{
		if(!FindUnquieName(nUnqiue))
		{
			bFind = FALSE;
			break;
		}
		nUnqiue++;
	}
	strNode.Format(_T("Surface %d"),nUnqiue);
	HTREEITEM hSurface = AddItem(strNode,m_hRightClkItem);
	GetTreeCtrl().SetItemData(hSurface,(DWORD_PTR)pNodeData);
	{
		strNode.Format(_T("Distance (%s): %.2f"),CARCLengthUnit::GetLengthUnitString(GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit()),\
			CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),dDistance));
		HTREEITEM hMarkDistance = AddARCBaseTreeItem(strNode,hSurface,NET_EDIT_WITH_VALUE);
		GetTreeCtrl().SetItemData(hMarkDistance,(DWORD_PTR)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetARCDocument()->m_umGlobalUnitManager.GetCurSelLengthUnit(),\
			dDistance*100)));

		strNode.Format(_T("Angle at offset line (deg): %d"),nAngle);
		HTREEITEM hMarkAngle = AddARCBaseTreeItem(strNode,hSurface,NET_EDIT_WITH_VALUE);
		GetTreeCtrl().SetItemData(hMarkAngle,(DWORD_PTR)(nAngle*100));
	}
}

void CAirsideMSView::OnRemoveSurface()
{
	BOOL bCanRemove = FALSE;
	HTREEITEM hParent = GetTreeCtrl().GetParentItem(m_hRightClkItem);
	HTREEITEM hGrand = GetTreeCtrl().GetParentItem(hParent);
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hParent);
	SurfaceType emType = (SurfaceType)pNodeData->nOtherData;
	SurfaceData* pData = new SurfaceData();
	switch(emType)
	{
	case No_Type:
		{
			Taxiway altObject;
			altObject.ReadObject(pNodeData->dwData);
			pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
			if (altObject.getSurface().getCount()> 1)
			{
				altObject.getSurface().DeleteItem(pNodeData->dwData);
				altObject.UpdateSurfaceData();
				bCanRemove = TRUE;
			}
			GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&altObject );
		}
		break;
	case ThresHold_Type:
		{
			pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hGrand);
			Runway altObject;
			altObject.ReadObject(pNodeData->dwData);
			if (pNodeData->nOtherData == 0)
			{
				pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
				if (altObject.GetMark1Threshold().getCount()> 1)
				{
					altObject.GetMark1Threshold().DeleteItem(pNodeData->dwData);
					bCanRemove = TRUE;
				}
			}
			else
			{
				pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
				if (altObject.GetMark2Threshold().getCount() > 1)
				{
					altObject.GetMark2Threshold().DeleteItem(pNodeData->dwData);
					bCanRemove = TRUE;
				}
			}
			altObject.UpdataSurfaceData();
			GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&altObject );
		}
		break;
	case Lateral_Type:
		{
			pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hGrand);
			Runway altObject;
			altObject.ReadObject(pNodeData->dwData);
			if (pNodeData->nOtherData == 0)
			{
				pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
				if (altObject.GetMark1Lateral().getCount()>1)
				{
					altObject.GetMark1Lateral().DeleteItem(pNodeData->dwData);
					bCanRemove = TRUE;
				}
			}
			else
			{
				pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
				if (altObject.GetMark2Lateral().getCount()>1)
				{
					altObject.GetMark2Lateral().DeleteItem(pNodeData->dwData);
					bCanRemove = TRUE;
				}
			}
			altObject.UpdataSurfaceData();
			GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&altObject );
		}
		break;
	case EndOfRunway_Type:
		{
			pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hGrand);
			Runway altObject;
			altObject.ReadObject(pNodeData->dwData);
			if (pNodeData->nOtherData == 0)
			{
				pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
				if (altObject.GetMark1EnOfRunway().getCount()>1)
				{
					altObject.GetMark1EnOfRunway().DeleteItem(pNodeData->dwData);
					bCanRemove = TRUE;
				}
			}
			else
			{
				pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
				if (altObject.GetMark2EndOfRunway().getCount()>1)
				{
					altObject.GetMark2EndOfRunway().DeleteItem(pNodeData->dwData);
					bCanRemove = TRUE;
				}
			}
			altObject.UpdataSurfaceData();
			GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&altObject );
		}
		break;
	default:
		break;
	}
	//////////////set surface index
	if (bCanRemove)
	{
		HTREEITEM hChild = GetTreeCtrl().GetChildItem(hParent);
		GetTreeCtrl().DeleteItem(m_hRightClkItem);
		int nID = 0;
		CString strData = _T("");
		while (hChild)
		{
			strData = GetTreeCtrl().GetItemText(hChild);
			int nPos = strData.Find(':');
			if (nPos == -1)
			{
				pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(hChild);
				pNodeData->dwData = nID;
				nID++;
			}
			hChild = GetTreeCtrl().GetNextSiblingItem(hChild);
		}
	}
}

void CAirsideMSView::OnShowSurface()
{
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	ALTObject altObject;
	altObject.ReadObject(pNodeData->dwData);
	CNodeData* pData = (CNodeData*)GetTreeCtrl().GetItemData(GetTreeCtrl().GetParentItem(m_hRightClkItem));
	if ((ALTOBJECT_TYPE)pData->nOtherData == ALT_TAXIWAY)
	{
		Taxiway pObject;
		pObject.ReadObject(altObject.getID());
		pObject.SetShow(TRUE);
		pObject.UpdateObject(altObject.getID());
		GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&pObject );
		return;
	}
	else if((ALTOBJECT_TYPE)pData->nOtherData == ALT_RUNWAY)
	{
		Runway pObject;
		pObject.ReadObject(altObject.getID());
		
		if (pNodeData->nOtherData == 0)
		{
			pObject.SetMark1Show(TRUE);
		}
		else
		{
			pObject.SetMark2Show(TRUE);
		}
		pObject.UpdateObject(altObject.getID());
		GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&pObject );
		return;
	}

	std::vector<ALTAirport> vAirport;
	try
	{
		ALTAirport::GetAirportList(m_nProjID,vAirport);
	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
		MessageBox(_T("Cann't read the airport information."));
		return;
	}

	for (int nAirportCnt = 0; nAirportCnt < (int)vAirport.size(); nAirportCnt++)
	{
		int nAirportID = vAirport.at(nAirportCnt).getID();
		std::vector<ALTObject> vObject1;
		ALTObject::GetObjectList(ALT_RUNWAY,nAirportID,vObject1);

		std::vector<ALTObject> vObject2;
		ALTObject::GetObjectList(ALT_TAXIWAY,nAirportID,vObject2);
		if ((ALTOBJECT_TYPE)pNodeData->nOtherData == ALT_TAXIWAY)
		{
			for (int i = 0; i < (int)vObject2.size();i++)
			{
				Taxiway taxObj;
				altObject = vObject2.at(i);
				taxObj.ReadObject(altObject.getID());
				taxObj.SetShow(TRUE);
				taxObj.UpdateObject(altObject.getID());
				GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&taxObj );
			}
		}
		else if ((ALTOBJECT_TYPE)pNodeData->nOtherData == ALT_RUNWAY)
		{
			for (int i = 0; i < (int)vObject1.size();i++)
			{
				Runway runObj;
				altObject = vObject1.at(i);
				runObj.ReadObject(altObject.getID());
				runObj.SetMark1Show(TRUE);;
				runObj.SetMark2Show(TRUE);
				runObj.UpdateObject(altObject.getID());
				GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&runObj );
			}
		}
	}
}

void CAirsideMSView::OnHideSurface()
{
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	ALTObject altObject;
	altObject.ReadObject(pNodeData->dwData);
	CNodeData* pData = (CNodeData*)GetTreeCtrl().GetItemData(GetTreeCtrl().GetParentItem(m_hRightClkItem));
	if ((ALTOBJECT_TYPE)pData->nOtherData  == ALT_TAXIWAY)
	{
		Taxiway pObject;
		pObject.ReadObject(altObject.getID());
		pObject.SetShow(FALSE);
		pObject.UpdateObject(altObject.getID());
		GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&pObject );
		return;
	}
	else if((ALTOBJECT_TYPE)pData->nOtherData  == ALT_RUNWAY)
	{
		Runway pObject;
		pObject.ReadObject(altObject.getID());

		if (pNodeData->nOtherData == 0)
		{
			pObject.SetMark1Show(FALSE);
		}
		else
		{
			pObject.SetMark2Show(FALSE);
		}
		pObject.UpdateObject(altObject.getID());
		GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&pObject );
		return;
	}
	std::vector<ALTAirport> vAirport;
	try
	{
		ALTAirport::GetAirportList(m_nProjID,vAirport);
	}
	catch (CADOException &e)
	{
		e.ErrorMessage();
		MessageBox(_T("Cann't read the airport information."));
		return;
	}

	for (int nAirportCnt = 0; nAirportCnt < (int)vAirport.size(); nAirportCnt++)
	{
		int nAirportID = vAirport.at(nAirportCnt).getID();
		std::vector<ALTObject> vObject1;
		ALTObject::GetObjectList(ALT_RUNWAY,nAirportID,vObject1);

		std::vector<ALTObject> vObject2;
		ALTObject::GetObjectList(ALT_TAXIWAY,nAirportID,vObject2);

		if ((ALTOBJECT_TYPE)pNodeData->nOtherData == ALT_TAXIWAY)
		{
			for (int i = 0; i < (int)vObject2.size();i++)
			{
				Taxiway taxObj;
				altObject = vObject2.at(i);
				taxObj.ReadObject(altObject.getID());
				taxObj.SetShow(FALSE);
				taxObj.UpdateObject(altObject.getID());
				GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&taxObj );
			}
		}
		else if ((ALTOBJECT_TYPE)pNodeData->nOtherData == ALT_RUNWAY)
		{
			for (int i = 0; i < (int)vObject1.size();i++)
			{
				Runway runObj;
				altObject = vObject1.at(i);
				runObj.ReadObject(altObject.getID());
				runObj.SetMark1Show(FALSE);
				runObj.SetMark2Show(FALSE);
				runObj.UpdateObject(altObject.getID());
				GetARCDocument()->UpdateAllViews(this, VM_MODIFY_ALTOBJECT, (CObject*)&runObj );
			}
		}
	}
}

void CAirsideMSView::OnUpdateHideSurface(CCmdUI* pCmdUI)
{
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	ALTObject altObject;
	altObject.ReadObject(pNodeData->dwData);
	CNodeData* pData = (CNodeData*)GetTreeCtrl().GetItemData(GetTreeCtrl().GetParentItem(m_hRightClkItem));
	if ((ALTOBJECT_TYPE)pData->nOtherData  == ALT_TAXIWAY)
	{
		Taxiway pObject;
		pObject.ReadObject(altObject.getID());
		pCmdUI->Enable(pObject.GetShow());
	}
	else if((ALTOBJECT_TYPE)pData->nOtherData  == ALT_RUNWAY)
	{
		Runway pObject;
		pObject.ReadObject(altObject.getID());

		if (pNodeData->nOtherData == 0)
		{
			pCmdUI->Enable(pObject.GetMark1Show());
		}
		else
		{
			pCmdUI->Enable(pObject.GetMark2Show());
		}
	}
}

void CAirsideMSView::OnUpdateShowSurface(CCmdUI* pCmdUI)
{
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData(m_hRightClkItem);
	ALTObject altObject;
	altObject.ReadObject(pNodeData->dwData);
	CNodeData* pData = (CNodeData*)GetTreeCtrl().GetItemData(GetTreeCtrl().GetParentItem(m_hRightClkItem));
	if ((ALTOBJECT_TYPE)pData->nOtherData == ALT_TAXIWAY)
	{
		Taxiway pObject;
		pObject.ReadObject(altObject.getID());
		pCmdUI->Enable(!pObject.GetShow());
		return;
	}
	else if((ALTOBJECT_TYPE)pData->nOtherData  == ALT_RUNWAY)
	{
		Runway pObject;
		pObject.ReadObject(altObject.getID());

		if (pNodeData->nOtherData == 0)
		{
			pCmdUI->Enable(!pObject.GetMark1Show());
		}
		else
		{
			pCmdUI->Enable(!pObject.GetMark2Show());
		}
		return;
	}

}

bool CAirsideMSView::FindUnquieName(int nUnqiueID)
{
	HTREEITEM hChild = GetTreeCtrl().GetChildItem(m_hRightClkItem);
	while (hChild)
	{
		CString strValue = _T("");
		strValue = GetTreeCtrl().GetItemText(hChild);
		if (strValue.Find(':') == -1)
		{
			strValue  = GetTreeCtrl().GetItemText(hChild);
			int nPos = strValue.Find(' ');
			
			strValue = strValue.Right(strValue.GetLength()-nPos-1);
			if (nUnqiueID == atoi(strValue))
			{
				return true;
			}
		}
		hChild = GetTreeCtrl().GetNextSiblingItem(hChild);
	}
	return false;
}


void CAirsideMSView::OnEditControlPoint()
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	if (pNodeData->nodeType != NodeType_Object)
		return;

	int nObjID = pNodeData->dwData;

	ALTObject3D * pObj3D = GetARCDocument()->Get3DViewParent()->GetAirside3D()->GetObject3D(nObjID);
	ASSERT(pObj3D);
	pObj3D->SetInEdit(true);
	GetDocument()->UpdateAllViews(this);
}

void CAirsideMSView::OnDisableControlPoint()
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	if (pNodeData->nodeType != NodeType_Object)
		return;

	int nObjID = pNodeData->dwData;

	ALTObject3D * pObj3D = GetARCDocument()->Get3DViewParent()->GetAirside3D()->GetObject3D(nObjID);
	ASSERT(pObj3D);
	pObj3D->SetInEdit(false);
	GetDocument()->UpdateAllViews(this);
}
void CAirsideMSView::OnUpdateEditControlPoint(CCmdUI* pCmdUI)
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	if (pNodeData->nodeType != NodeType_Object)
		return;

	int nObjID = pNodeData->dwData;

	CChildFrame* pChildFrame = (CChildFrame*)(GetARCDocument()->Get3DViewParent());
	if (pChildFrame)
	{
		ALTObject3D * pObj3D = pChildFrame->GetAirside3D()->GetObject3D(nObjID);
		ASSERT(pObj3D);
		// if in edit or locked, disable it
		// else, enable it
		pCmdUI->Enable(!(pObj3D->IsInEdit() || pObj3D->GetObject()->GetLocked()));
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CAirsideMSView::OnUpdateDisableControlPoint(CCmdUI* pCmdUI)
{
	if (m_hRightClkItem == NULL)
		return;
	CNodeData* pNodeData = (CNodeData*)GetTreeCtrl().GetItemData( m_hRightClkItem );
	if(pNodeData == NULL)
		return;

	if (pNodeData->nodeType != NodeType_Object)
		return;

	int nObjID = pNodeData->dwData;

	CChildFrame* pChildFrame = (CChildFrame*)(GetARCDocument()->Get3DViewParent());
	if (pChildFrame)
	{
		ALTObject3D * pObj3D = pChildFrame->GetAirside3D()->GetObject3D(nObjID);
		ASSERT(pObj3D);
		// if in edit and not locked, enable it
		// else, disable it
		pCmdUI->Enable(pObj3D->IsInEdit() && !pObj3D->GetObject()->GetLocked());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}