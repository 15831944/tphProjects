#include "StdAfx.h"
#include "airsidemsimpl.h"
#include "resource.h"
#include "..\common\template.h"
#include "TermPlanDoc.h"
#include <CommonData/Shape.h>
#include "..\inputs\airsideinput.h"
#include "../Inputs/AirportInfo.h"
#include "../Engine/DeiceBayProc.h"
#include "../Engine/process.h"
#include "../Inputs/WallShapeList.h"
#include "../Inputs/WallShape.h"
#include "../AirsideGUI/NodeViewDbClickHandler.h"
#include <utility>
#include "Placement.h"

using namespace AirsideGUI;

CAirSideMSImpl::CAirSideMSImpl()
{
	m_tvNodeListPcl.clear();
	m_tvNodeListStr.clear();
}
CAirSideMSImpl::~CAirSideMSImpl()
{
	
}

void CAirSideMSImpl::BuildTVNodeTree()
{
	ASSERT(m_pTermPlanDoc != NULL);
	Terminal& terminal = m_pTermPlanDoc->GetTerminal();
	m_pTermPlanDoc->GetTerminal().m_AirsideInput->GetAirprotInfo(m_pTermPlanDoc->m_ProjInfo.path);
//	CTVNode* pAirSideRootNode = m_pTermPlanDoc->GetAirSideRootTVNode();
	CTVNode* pAirSideRootNode=GetTVRootNode();
	//Build Node structure
	CString s;
	//add layout node to root
	s.LoadString(IDS_TVNN_LAYOUT);
	CTVNode* pLayoutNode = new CTVNode(s);
	pLayoutNode->m_eState = CTVNode::expanded;
	pAirSideRootNode->AddChild(pLayoutNode);
	m_tvNodeListStr.insert(std::make_pair(s, pLayoutNode));

	//airports 
	s.LoadString(IDS_TVNN_AIRPORTS);
	CTVNode *pAirportNode =new CTVNode(s,IDR_CTX_AIRPORTSMENU);
	pAirportNode->m_eState=CTVNode::expanded;
	pLayoutNode->AddChild(pAirportNode);
	m_tvNodeListStr.insert(std::make_pair(s, pAirportNode));

	CTVNode* pNode = NULL;

	//airport1,2,3.....
	
		//=m_pTermPlanDoc->m_AirsideInput.m_csAirportName;
	CTVNode *pSubAirport=NULL;

	for (int i=0;i<(int)m_pTermPlanDoc->GetTerminal().m_AirsideInput->m_csAirportNameList.size();i++)
	{
		CString strText=m_pTermPlanDoc->GetTerminal().m_AirsideInput->m_csAirportNameList.at(i);
		pSubAirport=new CTVNode(strText,IDR_CTX_AIRPORTMENU);
		pSubAirport->m_eState=CTVNode::expanded;
		pAirportNode->AddChild(pSubAirport);
		m_tvNodeListStr.insert(std::make_pair(strText, pSubAirport));
/*
		for (int iFloor=m_pTermPlanDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size()-1; iFloor>=0; iFloor--)
		{
			CTVNode* pFloorNode = new CTVNode(m_pTermPlanDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[iFloor]->FloorName(), IDR_CTX_FLOOR);
			pFloorNode->m_iImageIdx = (iFloor==m_pTermPlanDoc->m_nActiveFloor?ID_NODEIMG_FLOORACTIVE:ID_NODEIMG_FLOORNOTACTIVE);
			pFloorNode->m_dwData = (DWORD) iFloor;
			pSubAirport->AddChild(pFloorNode);
		}
*/
		//add groupmap node to layout
		s.LoadString(IDS_TVNN_GROUPMAP);
		pNode = new CTVNode(s,IDR_CTX_FLOOR);
		pNode->m_eState=CTVNode::expanded;
		pSubAirport->AddChild(pNode);
		m_tvNodeListStr.insert(std::make_pair(s, pNode));

		// Airport Reference points Node
		s.LoadString(IDS_TVNN_AIRPORTREFERENCEPOINTS);		
		CTVNode *pARPNode=new CTVNode(s,IDR_CTX_LATLONG);
		pARPNode->m_eState=CTVNode::expanded;
		pSubAirport->AddChild(pARPNode);
//		AddProcessorGroup(ArpProcessor, pARPNode);
		m_tvNodeListStr.insert(std::make_pair(s, pARPNode));
/*
		//////////////////////////////////////////////////////////////////////////
		//add in 08-19-05
		//Get latitude longitude
		CString strLongitude,strLatitude;
		m_pTermPlanDoc->GetTerminal().m_AirsideInput->GetLL(m_pTermPlanDoc->m_ProjInfo.path,strLongitude,strLatitude);
		CString strLL;
		strLL.Format("LONG:%s LAT:%s",strLongitude,strLatitude);
		pNode=new CTVNode(strLL,IDR_CTX_DEFAULT);
		pARPNode->AddChild(pNode);
		
		// X Y position
		CString strXY;
		double dx=m_pTermPlanDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->m_dx;
		double dy=m_pTermPlanDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->m_dy;
		strXY.Format("X:%.2f Y:%.2f",dx,dy);
		pNode=new CTVNode(strXY,IDR_CTX_DEFAULT);
		pARPNode->AddChild(pNode);
		
		//Elevation
		CString strElevation;
		double dElevation;
		m_pTermPlanDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->GetElevation(dElevation);
		strElevation.Format("Elevation(m): %.0f",dElevation);
		pNode = new CTVNode(strElevation , IDR_CTX_DEFAULT);
		pARPNode->AddChild(pNode);
*/
		AddProcessorGroup(ArpProcessor,pARPNode);


		//////////////////////////////////////////////////////////////////////////
		
		// RunWays Node
		s.LoadString(IDS_TVNN_RUNWAYS);
		pNode = new CTVNode(s);
		pSubAirport->AddChild(pNode);
		AddProcessorGroup(RunwayProcessor, pNode);
		m_tvNodeListStr.insert(std::make_pair(s, pNode));

		// TaxiWays Node
		s.LoadString(IDS_TVNN_TAIWAYS);
		pNode = new CTVNode(s);
		pSubAirport->AddChild(pNode);
		AddProcessorGroup(TaxiwayProcessor, pNode);
		m_tvNodeListStr.insert(std::make_pair(s, pNode));

		//Gates Node
		s.LoadString(IDS_TVNN_GATES);
		pNode=new CTVNode(s,IDR_MENU_GATE);
		pSubAirport->AddChild(pNode);
		AddProcessorGroup(StandProcessor, pNode);
		m_tvNodeListStr.insert(std::make_pair(s, pNode));

		//Deice Stations
		s.LoadString(IDS_TVNN_DEICEPADS);
		pNode = new CTVNode(s);
		pSubAirport->AddChild(pNode);
		AddProcessorGroup(DeiceBayProcessor, pNode);
		m_tvNodeListStr.insert(std::make_pair(s, pNode));

		////Aprons
		//s.LoadString(IDS_TVNN_APRONS);
		//pNode=new CTVNode(s);
		//pSubAirport->AddChild(pNode);
		//AddProcessorGroup(ApronProcessor, pNode);
		//m_tvNodeListStr.insert(std::make_pair(s, pNode));

		//Ground Routes
		s.LoadString(IDS_TVNN_GROUNDROUTES);
		pNode=new CTVNode(s);
		pSubAirport->AddChild(pNode);
		m_tvNodeListStr.insert(std::make_pair(s, pNode));
	}
	//add airspace
	s.LoadString(IDS_TVNN_AIRPALCE);
	CTVNode *pAirplaceNode=new CTVNode(s);
	pAirplaceNode->m_eState=CTVNode::expanded;
	pLayoutNode->AddChild(pAirplaceNode);
	
	//Fixes
	s.LoadString(IDS_TVNN_FIXES);
	pNode = new CTVNode(s);
	pAirplaceNode->AddChild(pNode);
	AddProcessorGroup(FixProcessor, pNode);
	m_tvNodeListStr.insert(std::make_pair(s, pNode));

	//Holds
	s.LoadString(IDS_TVNN_AIRHOLD);
	pNode = new CTVNode(s);
	pAirplaceNode->AddChild(pNode);
	AddProcessorGroup(HoldProcessor,pNode);
	m_tvNodeListStr.insert(std::make_pair(s,pNode));

	//air routes
	s.LoadString(IDS_TVNN_AIRROUTS);
	pNode = new CTVNode(s);
	pAirplaceNode->AddChild(pNode);
	m_mapItemDbClick[pNode] = NULL;//&NodeViewDbClickHandler::AirRoute;

	//Airplace Sectors
	s.LoadString(IDS_TVNN_AIRPLACESECTORS);

	pNode=new CTVNode(s);
	pAirplaceNode->AddChild(pNode);
	AddProcessorGroup(SectorProcessor,pNode);
	m_tvNodeListStr.insert(std::make_pair(s,pNode));

	//add topography
	s.LoadString(IDS_TVNN_TOPOGRAPHY);
	CTVNode *pTopoGraphyNode=new CTVNode(s);
	pTopoGraphyNode->m_eState=CTVNode::expanded;
	pLayoutNode->AddChild(pTopoGraphyNode);
	
	// Surface Areas
	s.LoadString(IDS_TVNN_SURFACEAREAS);
	pNode = new CTVNode(s, IDR_CTX_SURFACEAREAS);
	pTopoGraphyNode->AddChild(pNode);
	m_tvNodeListStr.insert(std::make_pair(s, pNode));
	m_pTermPlanDoc->m_msManager.SetSurfaceAreasNode(pNode, EnvMode_AirSide);
	
	AddStructureGroup(pNode);
	//CStructureList& structList = m_pTermPlanDoc->GetStructurelistByMode(EnvMode_AirSide);
	//for (UINT i = 0; i < structList.getStructureNum(); i++)
	//{
	//	CString strName = structList.getStructureAt(i).GetNameStr();
	//	m_pTermPlanDoc->m_msManager.AddSurfaceAreaNode(strName,i, EnvMode_AirSide);
	//}

	//structures
	s.LoadString(IDS_TVNN_STRUCTURES);
	pNode = new CTVNode(s);
	pTopoGraphyNode->AddChild(pNode);

	//Wall Shapes
	s.LoadString(IDS_TVNN_WALLSHAPES);
	pNode = new CTVNode(s, IDR_CTX_WALLSHAPES);
	pTopoGraphyNode->AddChild(pNode);	
	AddWallShapeGroup(pNode);

	//WallShapeList& wallList = m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_AirSide);
	//for (UINT i = 0; i < wallList.getShapeNum(); i++)
	//{
	//	CString strName = wallList.getShapeAt(i).GetNameStr();
	//	m_pTermPlanDoc->m_msManager.AddWallShapeNode(strName,i, EnvMode_AirSide);
	//}
	m_tvNodeListStr.insert(std::make_pair(s, pNode));

	//add under construction node
	if (m_pTermPlanDoc->GetTerminal().m_AirsideInput->GetPlacementPtr()->m_vUndefined.size() > 0)
	{
/*		s.LoadString(IDS_TVNN_UNDERCONSTRUCTION);
		CTVNode* pUCNode = new CTVNode(s);
		pLayoutNode->AddChild(pUCNode);
*/
		//add undefined to under construction
		for (int i=0; i<static_cast<int>(m_pTermPlanDoc->GetTerminal().m_AirsideInput->GetPlacementPtr()->m_vUndefined.size()); i++) 
		{
			s.Format("%s-%d", m_pTermPlanDoc->GetTerminal().m_AirsideInput->GetPlacementPtr()->m_vUndefined[i]->GetShape()->Name(), i);
			//// TRACE("<s=:%s> <i=%d>\n", s, i);
			CTVNode *pUCNode = new CTVNode(s, IDR_CTX_UCPROCESSOR);
			pUCNode->m_dwData = (DWORD) m_pTermPlanDoc->GetTerminal().m_AirsideInput->GetPlacementPtr()->m_vUndefined[i];
			pNode->AddChild(pUCNode);
		}
	}

	//Contours
	s.LoadString(IDS_TVNN_CONTOURS);
	pNode=new CTVNode(s);
	pTopoGraphyNode->AddChild(pNode);
	m_tvNodeListStr.insert(std::make_pair(s, pNode));
	AddProcessorGroup(ContourProcessor, pNode);


/////////////////

    //add aircraft classifications node to root
	s.LoadString(IDS_TVNN_AIRCRAFTCLASSIFICATIONS);
	CTVNode* pAPPNode=new CTVNode(s);
	pAPPNode->m_eState=CTVNode::expanded;
	pAirSideRootNode->AddChild(pAPPNode);

	//Wingspan based
	s.LoadString(IDS_TVNN_WINGSPANBASED);
	pNode=new CTVNode(s);
	pAPPNode->AddChild(pNode);

	//Surface bearing weight based
	s.LoadString(IDS_TVNN_SURFACEBEARINGWEIGHTBASED);
	pNode=new CTVNode(s);
	pAPPNode->AddChild(pNode);

	//Wake vortex weight based
	s.LoadString(IDS_TVNN_WAKEVORTEXWEIGHTBASED);
	pNode=new CTVNode(s);
	pAPPNode->AddChild(pNode);

	//Approach speed based
	s.LoadString(IDS_TVNN_APPROACHSPEEDBASED);
	pNode=new CTVNode(s);
	pAPPNode->AddChild(pNode);

	//add flights node to root
	s.LoadString(IDS_TVNN_FLIGHTS);
	CTVNode* pFLINode = new CTVNode(s);
	pFLINode->m_eState = CTVNode::expanded;
	pAirSideRootNode->AddChild(pFLINode);

	//schedule
	s.LoadString(IDS_TVNN_SCHEDULE);
	pNode = new CTVNode(s);
	pFLINode->AddChild(pNode);

	//itinerant flights generator
	s.LoadString(IDS_TVNN_ITINEANTFLIGHTSGENERATOR);
	pNode = new CTVNode(s);
	pFLINode->AddChild(pNode);

	////Stand buffers
	//s.LoadString(IDS_TVNN_STANDBUFFERS);
	//pNode = new CTVNode(s);
	//pFLINode->AddChild(pNode);

	//s.LoadString(IDS_TVNN_GATEASSIGNMENTS);
	//CTVNode *pSubAirportGateAssign = new CTVNode(s);
	//pSubAirportGateAssign->m_eState = CTVNode::expanded;
	//pFLINode->AddChild(pSubAirportGateAssign);

	//pSubAirport = NULL;

	//for (int i=0;i<(int)m_pTermPlanDoc->GetTerminal().m_AirsideInput->m_csAirportNameList.size();i++)
	//{
	//	CString strText=m_pTermPlanDoc->GetTerminal().m_AirsideInput->m_csAirportNameList.at(i);
	//	pSubAirport=new CTVNode(strText);
	//	pSubAirportGateAssign->AddChild(pSubAirport);
	//	//m_tvNodeListStr.insert(std::make_pair(strText, pSubAirport));
	//}
	
	//Flight Plans
	s.LoadString(IDS_TVNN_FLIGHTPLANS);
	pNode=new CTVNode(s);
	pFLINode->AddChild(pNode);
	m_mapItemDbClick[pNode] = &NodeViewDbClickHandler::FlightPlan;
	
	//Performance
	s.LoadString(IDS_TVNN_PERFORMANCE);
	CTVNode *pPerformanceNode=new CTVNode(s);
	pPerformanceNode->m_eState=CTVNode::expanded;
	pFLINode->AddChild(pPerformanceNode);
	
	//Cruise
	s.LoadString(IDS_TVNN_CRUISE);
	pNode=new CTVNode(s);
	pPerformanceNode->AddChild(pNode);
//	m_mapItemDbClick[pNode] = &NodeViewDbClickHandler::FlightPerformanceCruise;
	//Terminal
	s.LoadString(IDS_TVNN_TERMINAL);
	pNode=new CTVNode(s);
	pPerformanceNode->AddChild(pNode);
//	m_mapItemDbClick[pNode] = &NodeViewDbClickHandler::FlightPerformanceTerminal;
	//Approach to land
	s.LoadString(IDS_TVNN_APPROACHTOLAND);
	pNode=new CTVNode(s);
	pPerformanceNode->AddChild(pNode);
//	m_mapItemDbClick[pNode] = &NodeViewDbClickHandler::ApproachToLand;

	//Landing
	s.LoadString(IDS_TVNN_LANDING);
	pNode=new CTVNode(s);
	pPerformanceNode->AddChild(pNode);
//	m_mapItemDbClick[pNode] = &NodeViewDbClickHandler::PerformLanding;

	//Runway exit performance
	s.LoadString(IDS_TVNN_RUNWAYEXITPERFORMANCE);
	pNode=new CTVNode(s);
	pPerformanceNode->AddChild(pNode);
	m_mapItemDbClick[pNode] = NULL;//&NodeViewDbClickHandler::RunwayExitPerformance;
	//Taxi inbound
	s.LoadString(IDS_TVNN_TAXUINBOUND);
	pNode=new CTVNode(s);
	pPerformanceNode->AddChild(pNode);
//	m_mapItemDbClick[pNode] = &NodeViewDbClickHandler::TaxiInbound;

	//Stand Service
	s.LoadString(IDS_TVNN_STANDSERVICE);
	pNode=new CTVNode(s);
	pPerformanceNode->AddChild(pNode);

	//Push Back
	s.LoadString(IDS_TVNN_PUSHBACK);
	pNode=new CTVNode(s);
	pPerformanceNode->AddChild(pNode);
//	m_mapItemDbClick[pNode] = &NodeViewDbClickHandler::PushBack;

	//Taxi outbound
	s.LoadString(IDS_TVNN_TAXIOUTBOUND);
	pNode=new CTVNode(s);
	pPerformanceNode->AddChild(pNode);
//	m_mapItemDbClick[pNode] = &NodeViewDbClickHandler::TaxiOutbound;

	//Deicing times
	s.LoadString(IDS_TVNN_DEICINGTIMES);
	pNode=new CTVNode(s);
	pPerformanceNode->AddChild(pNode);

	//Takeoff
	s.LoadString(IDS_TVNN_TAKEOFF);
	pNode = new CTVNode(s);
	pPerformanceNode->AddChild(pNode);
//	m_mapItemDbClick[pNode] = &NodeViewDbClickHandler::PerformanceTakeoff;

	//Departure climbout
	s.LoadString(IDS_TVNN_DEPARTURECLIMBOUT);
	pNode=new CTVNode(s);
	pPerformanceNode->AddChild(pNode);
//	m_mapItemDbClick[pNode] = &NodeViewDbClickHandler::DepClimbOut;

	//add Noise Data
	s.LoadString(IDS_TVNN_NOISEDATA);
	pNode=new CTVNode(s);
	pFLINode->AddChild(pNode);


	////add Controller related data to root
	//s.LoadString(IDS_TVNN_CONTROLLERRELATEDDATA);					////
	//CTVNode *pControllerRelatedNode=new CTVNode(s);
	//pControllerRelatedNode->m_eState=CTVNode::expanded;
	//pAirSideRootNode->AddChild(pControllerRelatedNode);
	
	////Workload factors
	//s.LoadString(IDS_TVNN_WORKLOADFACTORS);
	//pNode=new CTVNode(s);
	//pControllerRelatedNode->AddChild(pNode);

	////Response time
	//s.LoadString(IDS_TVNN_RESPONSETIME);
	//pNode=new CTVNode(s);
	//pControllerRelatedNode->AddChild(pNode);

	//add Resource Management to root
	s.LoadString(IDS_TVNN_RESOURCEMANAGEMENT);
	CTVNode *pResourceManagementRoot=new CTVNode(s);
	pResourceManagementRoot->m_eState=CTVNode::expanded;
	pAirSideRootNode->AddChild(pResourceManagementRoot);

	//add Airspace
	s.LoadString(IDS_TVNN_AIRSPACE);
	CTVNode *pEnrouteAirspaceRoot=new CTVNode(s);
	pEnrouteAirspaceRoot->m_eState=CTVNode::expanded;
	pResourceManagementRoot->AddChild(pEnrouteAirspaceRoot);

	////Direct Clearance criteria												////
	//s.LoadString(IDS_TVNN_DIRECTCLEARANCECRITERIA);
	//pNode=new CTVNode(s);
	//pEnrouteAirspaceRoot->AddChild(pNode);

	//Sector flight mix limitation
	s.LoadString(IDS_TVNN_SECTORFLIGHTMIXLIMITIATIONS);
	pNode=new CTVNode(s);
	pEnrouteAirspaceRoot->AddChild(pNode);

	//In trail separation
	s.LoadString(IDS_TVNN_INTRAILSEPARATIONS);
	pNode=new CTVNode(s);
	pEnrouteAirspaceRoot->AddChild(pNode);

	//Controller intervention(airspace)
	s.LoadString(IDS_TVNN_CONTROLLERINTERVENTION_AIRSPACE);
	pNode=new CTVNode(s);
	pEnrouteAirspaceRoot->AddChild(pNode);

	//Vectoring criteria
	s.LoadString(IDS_TVNN_VECTORINGCRITERIA);
	pNode=new CTVNode(s);
	pEnrouteAirspaceRoot->AddChild(pNode);

	//Direct routing criteria
	s.LoadString(IDS_TVNN_DIRECTROUTINGCRITERIA);
	pNode=new CTVNode(s);
	pEnrouteAirspaceRoot->AddChild(pNode);


	//add Approach
	s.LoadString(IDS_TVNN_APPROACH);
	CTVNode *pApproachRoot=new CTVNode(s);
	pApproachRoot->m_eState=CTVNode::expanded;
	pResourceManagementRoot->AddChild(pApproachRoot);

	//Approach separation
	s.LoadString(IDS_TVNN_APPROACHSEPARATIONS);
	pNode=new CTVNode(s);
	pApproachRoot->AddChild(pNode);

	//Controller intervention(Approach)
	s.LoadString(IDS_TVNN_CONTROLLERINTERVENTION_APPROACH);
	pNode=new CTVNode(s);
	pApproachRoot->AddChild(pNode);

	//Side step criteria
	s.LoadString(IDS_TVNN_SIDESTEPCRITERIA);
	pNode=new CTVNode(s);
	pApproachRoot->AddChild(pNode);

	//Go around criteria
	s.LoadString(IDS_TVNN_GOAROUNDCRITERIA);
	pNode=new CTVNode(s);
	pApproachRoot->AddChild(pNode);

	////STAR-Runway assignment and criteria
	//s.LoadString(IDS_TVNN_STARRUNWAYASSIGNMENT);
	//pNode=new CTVNode(s);
	//pApproachAreasRoot->AddChild(pNode);


	//add Runways
	s.LoadString(IDS_TVNN_RUNWAYS);
	CTVNode *pRunwaysRoot=new CTVNode(s);
	pRunwaysRoot->m_eState=CTVNode::expanded;
	pResourceManagementRoot->AddChild(pRunwaysRoot);

	////ATC Separations criteria															////
	//s.LoadString(IDS_TVNN_ATCSEPARATIONSCRITERIA);
	//pNode = new CTVNode(s);
	//pRunwaysRoot->AddChild(pNode);
	//m_mapItemDbClick[pNode] = &NodeViewDbClickHandler::ATCSeparation;

	////Exclusive runway use criteria
	//s.LoadString(IDS_TVNN_EXCLUSIVERUNWAY);
	//pNode = new CTVNode(s);
	//pRunwaysRoot->AddChild(pNode);
	//m_mapItemDbClick[pNode] = &NodeViewDbClickHandler::ExclusiveRunwayUse;

	////Exit usage criteria
	//s.LoadString(IDS_TVNN_EXITUSAGECRITERIA);
	//pNode=new CTVNode(s);
	//pRunwaysRoot->AddChild(pNode);
	//m_mapItemDbClick[pNode] = &NodeViewDbClickHandler::ExitUsageCriteria;

	////Take-off position assignment and ctiteria
	//s.LoadString(IDS_TVNN_TAKEOFFPOSITION);
	//pNode=new CTVNode(s);
	//pRunwaysRoot->AddChild(pNode);
	//m_mapItemDbClick[pNode] = &NodeViewDbClickHandler::TakeoffPosAssignCriteria;

	//Landing runway assignment
	s.LoadString(IDS_TVNN_LANDINGRUNWAYASSIGNMENT);
	pNode=new CTVNode(s);
	pRunwaysRoot->AddChild(pNode);

	//Landing runway exit strategies
	s.LoadString(IDS_TVNN_LANDINGRUNWAYEXITSTRATEGIES);
	pNode=new CTVNode(s);
	pRunwaysRoot->AddChild(pNode);

	//Runway takeoff positions
	s.LoadString(IDS_TVNN_RUNWAYTAKEOFFPOSITIONS);
	pNode=new CTVNode(s);
	pRunwaysRoot->AddChild(pNode);

	//Takeoff runway assignment
	s.LoadString(IDS_TVNN_TAKEOFFRUNWAYASSIGNMENT);
	pNode=new CTVNode(s);
	pRunwaysRoot->AddChild(pNode);

	//Takeoff queues
	s.LoadString(IDS_TVNN_TAKEOFFQUEUES);
	pNode=new CTVNode(s);
	pRunwaysRoot->AddChild(pNode);

	//Takeoff sequencing
	s.LoadString(IDS_TVNN_TAKEOFFSEQUENCING);
	pNode=new CTVNode(s);
	pRunwaysRoot->AddChild(pNode);

	//Clearance criteria
	s.LoadString(IDS_TVNN_CLEARANCECRITERIA);
	pNode=new CTVNode(s);
	pRunwaysRoot->AddChild(pNode);

	//Arrival delay triggers
	s.LoadString(IDS_TVNN_ARRIAVLDELAYTRIGGERS);
	pNode=new CTVNode(s);
	pRunwaysRoot->AddChild(pNode);

	//add Taxiways
	s.LoadString(IDS_TVNN_TAIWAYS);
	CTVNode *pTaxiwaysRoot=new CTVNode(s);
	pTaxiwaysRoot->m_eState=CTVNode::expanded;
	pResourceManagementRoot->AddChild(pTaxiwaysRoot);
	
	//Taxiway usage criteria
	s.LoadString(IDS_TVNN_TAXIWAYUSAGECRITERIA);
	pNode=new CTVNode(s);
	pTaxiwaysRoot->AddChild(pNode);
	m_mapItemDbClick[pNode] = NULL;//&NodeViewDbClickHandler::TaxiwayUsageCriteria;
	//Intersection blocking criteria
	s.LoadString(IDS_TVNN_INTERSECTIONBLOCKING);
	pNode=new CTVNode(s);
	pTaxiwaysRoot->AddChild(pNode);
	m_mapItemDbClick[pNode] = NULL;// &NodeViewDbClickHandler::IntersectionBlocking;

	//Inbound taxi route assignment and criteria
	s.LoadString(IDS_TVNN_INBOUNDTAXI);
	pNode=new CTVNode(s);
	pTaxiwaysRoot->AddChild(pNode);

	//Outbound taxi route assignment and criteria
	s.LoadString(IDS_TVNN_OUTBOUNDTAXI);
	pNode=new CTVNode(s);
	pTaxiwaysRoot->AddChild(pNode);

	//// add Aprons
	//s.LoadString(IDS_TVNN_APRONS);
	//CTVNode *pApronsRoot=new CTVNode(s);
	//pApronsRoot->m_eState=CTVNode::expanded;
	//pResourceManagementRoot->AddChild(pApronsRoot);

	////Apron Operation Designations
	//s.LoadString(IDS_TVNN_APRONOPERATION);
	//pNode=new CTVNode(s);
	//pApronsRoot->AddChild(pNode);

	// add Gates
	s.LoadString(IDS_TVNN_GATES);
	CTVNode *pGatesRoot=new CTVNode(s);
	pGatesRoot->m_eState=CTVNode::expanded;
	pResourceManagementRoot->AddChild(pGatesRoot);

	//Stand buffers
	s.LoadString(IDS_TVNN_STANDBUFFERS);
	pNode = new CTVNode(s);
	pGatesRoot->AddChild(pNode);

	s.LoadString(IDS_TVNN_GATE_ASSIGNMENT);
	CTVNode *pGateAssignment =new CTVNode(s);
	pGateAssignment->m_eState=CTVNode::expanded;
	pGatesRoot->AddChild(pGateAssignment);
	
		//stand gate assignment
		s.LoadString(IDS_TVNN_STAND_GATE_ASSIGNMENT);
		CTVNode *pSubAirportStandGateAssign = new CTVNode(s);
		pSubAirportStandGateAssign->m_eState = CTVNode::expanded;
		pGateAssignment->AddChild(pSubAirportStandGateAssign);

		//arrival gate assignment
		s.LoadString(IDS_TVNN_ARRIVAL_GATE_ASSIGNMENT);
		CTVNode *pSubAirportArrivalGateAssign = new CTVNode(s);
		pSubAirportArrivalGateAssign->m_eState = CTVNode::expanded;
		pGateAssignment->AddChild(pSubAirportArrivalGateAssign);

		//departure gate assignment
		s.LoadString(IDS_TVNN_DEPARTURE_GATE_ASSIGNMENT);
		CTVNode *pSubAirportDepartureGateAssign = new CTVNode(s);
		pSubAirportDepartureGateAssign->m_eState = CTVNode::expanded;
		pGateAssignment->AddChild(pSubAirportDepartureGateAssign);
	

	pSubAirport = NULL;

	for (int i=0;i<(int)m_pTermPlanDoc->GetTerminal().m_AirsideInput->m_csAirportNameList.size();i++)
	{
		CString strText=m_pTermPlanDoc->GetTerminal().m_AirsideInput->m_csAirportNameList.at(i);
		pSubAirport=new CTVNode(strText);
		pSubAirportStandGateAssign->AddChild(pSubAirport);
		m_tvNodeListStr.insert(std::make_pair(strText, pSubAirport));
	}

	//Gate assignment criteria
	s.LoadString(IDS_TVNN_GATEASSIGNMENTCRITERIA);
	pNode=new CTVNode(s);
	pGatesRoot->AddChild(pNode);

	//Pre-gate hold positioning criteria
	s.LoadString(IDS_TVNN_PREGATEHOLD);
	pNode=new CTVNode(s);
	pGatesRoot->AddChild(pNode);

	//Push back blocking criteria
	s.LoadString(IDS_TVNN_PUSHBACKBLOCKING);
	pNode=new CTVNode(s);
	pGatesRoot->AddChild(pNode);
	m_mapItemDbClick[pNode] = NULL;//&NodeViewDbClickHandler::PushBackBlocking;

	//Towing operation parameters
	s.LoadString(IDS_TVNN_TOWINGOPERATION);
	pNode=new CTVNode(s);
	pGatesRoot->AddChild(pNode);
	m_mapItemDbClick[pNode] = NULL;//&NodeViewDbClickHandler::GatesTowingOperationParam;
	//add deice stations
	s.LoadString(IDS_TVNN_DEICEPADS);
	CTVNode *pDeiceStaionsRoot=new CTVNode(s);
	pDeiceStaionsRoot->m_eState=CTVNode::expanded;
	pResourceManagementRoot->AddChild(pDeiceStaionsRoot);
	
	//Mode of transit to pads
	s.LoadString(IDS_TVNN_MODEOFTRANSIT);
	pNode=new CTVNode(s);
	pDeiceStaionsRoot->AddChild(pNode);

	//Deicing pads assignment
	s.LoadString(IDS_TVNN_DEICINGPADSASSIGNMENT);
	pNode=new CTVNode(s);
	pDeiceStaionsRoot->AddChild(pNode);

	//pads' aircraft eligibility
	s.LoadString(IDS_TVNN_PADSAIRCRAFT);
	pNode=new CTVNode(s);
	pDeiceStaionsRoot->AddChild(pNode);

	//Add Departure Airspace 
	s.LoadString(IDS_TVNN_DEPARTUREAIRSPACE);

	CTVNode *pDepartureAirsideRoot= new CTVNode(s);
	pDepartureAirsideRoot->m_eState=CTVNode::expanded;
	pResourceManagementRoot->AddChild(pDepartureAirsideRoot);

	//Runway-SID assignment and criteria
	s.LoadString(IDS_TVN_RUNWAYSIDASSIGNMENT);
	pNode=new CTVNode(s);
	pDepartureAirsideRoot->AddChild(pNode);

	//Departure in-trail sepratations and criteria
	s.LoadString(IDS_TVNN_DEPARTUREINTRAIL);
	pNode=new CTVNode(s);
	pDepartureAirsideRoot->AddChild(pNode);
	m_mapItemDbClick[pNode] =NULL;// &NodeViewDbClickHandler::DepInTrail;

	//add Noise model specifications
	s.LoadString(IDS_TVNN_NOISEMODEL);
	pNode=new CTVNode(s);
	pResourceManagementRoot->AddChild(pNode);

	//add SIMULATION node to root
	s.LoadString(IDS_TVNN_SIMULATION);
	CTVNode* pSIMNode = new CTVNode(s);
	pSIMNode->m_eState = CTVNode::expanded;
	pAirSideRootNode->AddChild(pSIMNode);

	//Schedule Initialization
	s.LoadString(IDS_TVNN_SCHEDULEINIT);
	pNode=new CTVNode(s);
	pSIMNode->AddChild(pNode);

	//// arrival initialization
	//s.LoadString(IDS_TVNN_ARRIVALINIT);
	//pNode=new CTVNode(s);
	//pSIMNode->AddChild(pNode);
	//m_mapItemDbClick[pNode] = &NodeViewDbClickHandler::ArrivalInit;

	// setting
	s.LoadString(IDS_TVNN_SETTINGS);
	pNode = new CTVNode(s);
	pSIMNode->AddChild(pNode);
	m_mapItemDbClick[pNode] = NULL;//&NodeViewDbClickHandler::SimSetting;

	// run
	s.LoadString(IDS_TVNN_RUN);
	pNode = new CTVNode(s);
	pSIMNode->AddChild(pNode);


	//add AnalysisParameters node to root
	s.LoadString(IDS_TVNN_ANALYSISPARAMETERS);
	CTVNode* pANANode = new CTVNode(s);
	pANANode->m_eState = CTVNode::expanded;
	pAirSideRootNode->AddChild(pANANode);

	s.LoadString(IDS_TVNN_AIRSPACEDISPLAY);
	pNode = new CTVNode(s);
	pANANode->AddChild(pNode);

	//Aircraft Tags
	s.LoadString(IDS_TVNN_AIRCRAFTTAGS);
	pNode=new CTVNode(s);
	pANANode->AddChild(pNode);

	//Vehicle Tags
	s.LoadString(IDS_TVNN_VEHICLETAGS);
	pNode=new CTVNode(s);
	pANANode->AddChild(pNode);

	//Report Parameters
	s.LoadString(IDS_TVNN_REPORTPARAMETERS);
	pNode=new CTVNode(s);
	pANANode->AddChild(pNode);

}
void CAirSideMSImpl::AddProcessorGroup(int iType, CTVNode* pGroupNode)
{	
	if (pGroupNode == NULL)
	{
		pGroupNode = FindProcTypeNode(iType);	
	}	

	pGroupNode->DeleteAllChildren();

	//special handle the Contour Process 
	if (iType == ContourProcessor)
	{
		//code here ..
		//Get the proc name from the structure

		// get the proc from the processor list

		//
/*
		CString procName="1-2";
		
		CProcessor2 *proc2 = m_pTermPlanDoc->GetProcessor2FromName(procName,EnvMode_AirSide);
		
		if (proc2)
		{
			CTVNode* proc2Node = new CTVNode(procName,IDR_MENU_CONTOUR);
			proc2Node->m_dwData=(DWORD)proc2;
			pGroupNode->AddChild(proc2Node);
		}
*/
		CContourNode *rootNode= m_pTermPlanDoc->GetTerminal().m_AirsideInput->pContourTree->GetRootNode();
		
		CContourNode* pContourNode = rootNode->m_pFirstChild;
		CTVNode *pNode=NULL;
		CProcessor2 *proc2=NULL;
		while (pContourNode != NULL)
		{
			CTVNode* pParentNode = new CTVNode(pContourNode->m_KeyVal,IDR_MENU_CONTOUR);
			proc2 = m_pTermPlanDoc->GetProcessor2FromName(pContourNode->m_KeyVal,EnvMode_AirSide);
			pParentNode->m_dwData=(DWORD)proc2;

			pGroupNode->AddChild(pParentNode);

			AddNode(pContourNode,pParentNode);

			pContourNode = pContourNode->m_pNext;
		}

		m_pTermPlanDoc->UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pGroupNode);
		return ;
	}
	// add deice bay processor to node view 

	if (iType == DeiceBayProcessor)
	{
        AddProcessorDeiceGroup(pGroupNode);	
		return;
	}
	if (iType == ArpProcessor)
	{

		AddProcessorARPGroup(pGroupNode);
		return;

	}

	
	ASSERT(m_pTermPlanDoc != NULL);
	if (pGroupNode == NULL)
		pGroupNode = FindProcTypeNode(iType);
	ASSERT(pGroupNode != NULL);

	Terminal& terminal = m_pTermPlanDoc->GetTerminal();
	pGroupNode->DeleteAllChildren();
	ProcessorID id, *pid = NULL;
	id.SetStrDict(terminal.inStrDict);
	CString sID;
	StringList strListL1;
	terminal.GetAirsideProcessorList()->getAllGroupNames(strListL1, iType);

	//level 1
	for(int i = 0; i < strListL1.getCount(); i++)
	{

		CString strTemp1 = strListL1.getString(i);
		strTemp1.TrimLeft(); strTemp1.TrimRight();
		if (strTemp1.IsEmpty())
		{
			continue;
		}
		CTVNode*  pL1Node= new CTVNode(strTemp1,IDR_CTX_PROCESSORGROUP);
		pGroupNode->AddChild(pL1Node);
		sID = strTemp1;
		id.setID((LPCSTR) sID);
		StringList strListL2;
		terminal.GetAirsideProcessorList()->getMemberNames(id, strListL2, -1);
		terminal.GetAirsideProcessorList()->getMemberNames(id, strListL2, iType);
		if(strListL2.IsEmpty()) 
		{
			pL1Node->m_nIDResource = IDR_CTX_PROCESSOR;
			//get CProcessor2* associated with this id
			CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
			ASSERT(pP2 != NULL);
			pL1Node->m_dwData = (DWORD) pP2;
		}
		// Level 2
		for( int j = 0; j < strListL2.getCount(); j++ )
		{
			CString strTemp2 = strListL2.getString(j);
			strTemp2.TrimLeft();  strTemp2.TrimRight();
			if (strTemp2.IsEmpty())
			{
				continue;
			}
			//CTVNode* pL2Node = new CTVNode(strTemp1+"-"+strTemp2);
			CTVNode* pL2Node = new CTVNode(strTemp2, IDR_CTX_PROCESSORGROUP);
			pL1Node->AddChild(pL2Node);
			sID = strTemp1+"-"+strTemp2;
			id.setID((LPCSTR) sID);
			StringList strListL3;
			terminal.GetAirsideProcessorList()->getMemberNames( id, strListL3, -1 );

			terminal.GetAirsideProcessorList()->getMemberNames( id, strListL3, RunwayProcessor );
			if(strListL3.IsEmpty())
			{
				pL2Node->m_nIDResource = IDR_CTX_PROCESSOR;
				//get CProcessor2* associated with this id
				CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
				ASSERT(pP2 != NULL);
				pL2Node->m_dwData = (DWORD) pP2;
			}
			// Level 3
			for( int k = 0; k < strListL3.getCount(); k++ ) 
			{
				CString strTemp3 = strListL3.getString(k);
				strTemp3.TrimLeft(); strTemp3.TrimRight();
				if (strTemp3.IsEmpty())
				{
					continue;
				}
				//CTVNode* pL3Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3);
				CTVNode* pL3Node = new CTVNode(strTemp3, IDR_CTX_PROCESSORGROUP);
				pL2Node->AddChild(pL3Node);
				sID = strTemp1+"-"+strTemp2+"-"+strTemp3;
				id.setID((LPCSTR) sID);
				StringList strListL4;
				terminal.GetAirsideProcessorList()->getMemberNames( id, strListL4, -1 );
				terminal.GetAirsideProcessorList()->getMemberNames( id, strListL4, RunwayProcessor );

				if(strListL4.IsEmpty())
				{
					pL3Node->m_nIDResource = IDR_CTX_PROCESSOR;
					//get CProcessor2* associated with this id
					CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
					ASSERT(pP2 != NULL);
					pL3Node->m_dwData = (DWORD) pP2;
				}
				// Level 4
				for( int l = 0; l < strListL4.getCount(); l++ ) 
				{
					CString strTemp4 = strListL4.getString(l);
					strTemp4.TrimLeft(); strTemp4.TrimRight();
					if (strTemp4.IsEmpty())
					{				
						continue;
					}
					//CTVNode* pL4Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4, IDR_CTX_PROCESSOR);
					CTVNode* pL4Node = new CTVNode(strTemp4, IDR_CTX_PROCESSOR);
					pL3Node->AddChild(pL4Node);
					//get CProcessor2* associated with this id
					sID = strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4;
					id.setID((LPCSTR) sID);
					CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
					ASSERT(pP2 != NULL);
					pL4Node->m_dwData = (DWORD) pP2;
				}
			}
		}
	}

	m_pTermPlanDoc->UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pGroupNode);
}

CTVNode* CAirSideMSImpl::FindProcTypeNode(int iType)
{
	ASSERT(m_pTermPlanDoc != NULL);
	CTVNode* pRetNode = NULL;
//	CTVNode* pAirSideRootNode = m_pTermPlanDoc->GetAirSideRootTVNode();
	CTVNode* pAirSideRootNode=GetTVRootNode();
	if (pAirSideRootNode->GetChildCount() == 0)
		return NULL;

	ProcessorClassList ProcType = (ProcessorClassList) iType;

	UINT nProcTypeNameResID = 0;
	switch(ProcType)
	{
	case ArpProcessor:
		nProcTypeNameResID = IDS_TVNN_AIRPORTREFERENCEPOINTS;
		break;

	case RunwayProcessor:
		nProcTypeNameResID = IDS_TVNN_RUNWAYS;
		break;

	case TaxiwayProcessor:
		nProcTypeNameResID = IDS_TVNN_TAIWAYS;
		break;

	case StandProcessor:
		nProcTypeNameResID = IDS_TVNN_GATES;//IDS_TVNN_AIRCRAFTSTANDS
		break;

	case NodeProcessor:
		nProcTypeNameResID = IDS_TVNN_AIRFIELDNETWORDNODES;
		break;

	case DeiceBayProcessor:
		nProcTypeNameResID = IDS_TVNN_DEICEPADS;
		break;

	case FixProcessor:
		nProcTypeNameResID = IDS_TVNN_FIXES;
		break;

	case HoldProcessor :
		nProcTypeNameResID = IDS_TVNN_AIRHOLD;
		break;
	case SectorProcessor :
		nProcTypeNameResID = IDS_TVNN_AIRPLACESECTORS;
		break;

	case ContourProcessor:
		nProcTypeNameResID=IDS_TVNN_CONTOURS;
		break;
	case ApronProcessor:
		nProcTypeNameResID=IDS_TVNN_APRONS;
		break;

	}

	CString strProcTypeName;
	if (nProcTypeNameResID != 0)
	{
		if (strProcTypeName.LoadString(nProcTypeNameResID))
			pRetNode =  m_pTermPlanDoc->m_msManager.FindNodeByName(strProcTypeName);
	}

	return pRetNode;
}

CTVNode* CAirSideMSImpl::GetProcNode(ProcessorClassList ProcType)
{
	TVNODELISTPCL::const_iterator iter = m_tvNodeListPcl.find(ProcType);
	if(iter == m_tvNodeListPcl.end())
		return NULL;

	return (CTVNode*)iter->second;
}

CTVNode* CAirSideMSImpl::GetProcNode(CString& str)
{
	std::string s = str.GetBuffer(0);
	TVNODELISTSTR::const_iterator iter = m_tvNodeListStr.find(s);
	str.ReleaseBuffer();
	if(iter == m_tvNodeListStr.end())
		return NULL;

	return (CTVNode*)iter->second;
}

BOOL CAirSideMSImpl::ChangeNodeName(const CString& strSrc, const CString& strDest)
{
	/*
	std::string s = strSrc.GetBuffer(0);
	TVNODELISTSTR::const_iterator iter = m_tvNodeListStr.find(s);
	strSrc.ReleaseBuffer();
	if(iter == m_tvNodeListStr.end())
		return FALSE;

	m_tvNodeListStr.erase(iter);
	s = strDest.GetBuffer(0);
	m_tvNodeListStr.insert(make_pair(s, (CTVNode *)iter.second));
	strDest.ReleaseBuffer();
	*/
	return TRUE;
}

void CAirSideMSImpl::AddNode( CContourNode *pNode,CTVNode *pTVNode)
{
	CContourNode *pChild = pNode->m_pFirstChild;
	CProcessor2 *proc2=NULL;
	while (pChild != NULL)
	{
		CTVNode *pChildTVNode =new CTVNode(pChild->m_KeyVal,IDR_MENU_CONTOUR);
		proc2 = m_pTermPlanDoc->GetProcessor2FromName(pChild->m_KeyVal,EnvMode_AirSide);
		pChildTVNode->m_dwData=(DWORD)proc2;
		pTVNode->AddChild(pChildTVNode);

		AddNode(pChild,pChildTVNode);
		
		pChild = pChild->m_pNext ;
	}
}

//add deice processor to node view
void CAirSideMSImpl::AddProcessorDeiceGroup(CTVNode* pGroupNode /* = NULL */)
{
	int iType = DeiceBayProcessor;
	ASSERT(m_pTermPlanDoc != NULL);
	if (pGroupNode == NULL)
		pGroupNode = FindProcTypeNode(iType);
	ASSERT(pGroupNode != NULL);

	//get stand proc list
	ProcessorArray standProcArray;
	CPROCESSOR2LIST standProc2List;
	m_pTermPlanDoc->GetTerminal().GetAirsideProcessorList()->getProcessorsOfType(StandProcessor,standProcArray);
	for (int i = 0 ;i < standProcArray.getCount() ; i++)
	{
		CProcessor2 *proc2 = m_pTermPlanDoc->GetProcessor2FromID(*(standProcArray.getItem(i)->getID()),EnvMode_AirSide);;
		if (proc2)
		{
			standProc2List.push_back(proc2);
		}
	}

	//get the deice proc list
	ProcessorArray deiceProcArray;
	m_pTermPlanDoc->GetTerminal().GetAirsideProcessorList()->getProcessorsOfType(DeiceBayProcessor ,deiceProcArray);
	
	for (int i = 0 ;i < deiceProcArray.getCount() ; i++)
	{

//		CProcessor2 *proc2 = m_pTermPlanDoc->getProcessor2ByProcessor(deiceProcArray.getItem(i));
		CProcessor2 *proc2 = m_pTermPlanDoc->GetProcessor2FromID(*(deiceProcArray.getItem(i)->getID()),EnvMode_AirSide);

		CTVNode *pDeiceNode = new CTVNode(deiceProcArray.getItem(i)->getID()->GetIDString());
		pDeiceNode->m_nIDResource = IDR_CTX_PROCESSOR;
		//get CProcessor2* associated with this id
		ASSERT(proc2 != NULL);
		pDeiceNode->m_dwData = (DWORD) proc2;
		pGroupNode->AddChild(pDeiceNode);

		//add stand that in the deice
		std::vector<CProcessor2 *> proc2InBound;
		proc2InBound = ((DeiceBayProc *)deiceProcArray.getItem(i))->getInBoundStands(&standProc2List);
		
		for (int ii = 0 ; ii <static_cast<int>(proc2InBound.size()) ; ii++)
		{
			CTVNode *pNode = new CTVNode(proc2InBound[ii]->GetProcessor()->getID()->GetIDString());
			pDeiceNode->AddChild(pNode);
		}

		//add deice process to deice group

//		if (proc2)
//		{
//			standProc2List.push_back(proc2);
//		}
	}
	


/*
	Terminal& terminal = m_pTermPlanDoc->GetTerminal();
	pGroupNode->DeleteAllChildren();
	ProcessorID id, *pid = NULL;
	id.SetStrDict(terminal.inStrDict);
	CString sID;
	StringList strListL1;
	terminal.GetProcessorList(EnvMode_AirSide)->getAllGroupNames(strListL1, iType);

	//level 1
	for(int i = 0; i < strListL1.getCount(); i++)
	{

		CString strTemp1 = strListL1.getString(i);
		strTemp1.TrimLeft(); strTemp1.TrimRight();
		if (strTemp1.IsEmpty())
		{
			continue;
		}
//		CTVNode*  pL1Node= new CTVNode(strTemp1);
//		pGroupNode->AddChild(pL1Node);
		sID = strTemp1;
		id.setID((LPCSTR) sID);
		StringList strListL2;
		terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames(id, strListL2, -1);
		terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames(id, strListL2, iType);
		if(strListL2.IsEmpty()) 
		{
			CTVNode *pL1Node = new CTVNode(id.GetIDString());
			pL1Node->m_nIDResource = IDR_CTX_PROCESSOR;
			//get CProcessor2* associated with this id
			CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
			ASSERT(pP2 != NULL);
			pL1Node->m_dwData = (DWORD) pP2;
			pGroupNode->AddChild(pL1Node);
		}
		// Level 2
		for( int j = 0; j < strListL2.getCount(); j++ )
		{
			CString strTemp2 = strListL2.getString(j);
			strTemp2.TrimLeft();  strTemp2.TrimRight();
			if (strTemp2.IsEmpty())
			{
				continue;
			}
			//CTVNode* pL2Node = new CTVNode(strTemp1+"-"+strTemp2);
//			CTVNode* pL2Node = new CTVNode(strTemp2, IDR_CTX_PROCESSORGROUP);
//			pL1Node->AddChild(pL2Node);
			sID = strTemp1+"-"+strTemp2;
			id.setID((LPCSTR) sID);
			StringList strListL3;
			terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL3, -1 );

			terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL3, RunwayProcessor );
			if(strListL3.IsEmpty())
			{
				CTVNode *pL2Node = new CTVNode(id.GetIDString());
				pL2Node->m_nIDResource = IDR_CTX_PROCESSOR;
				//get CProcessor2* associated with this id
				CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
				ASSERT(pP2 != NULL);
				pL2Node->m_dwData = (DWORD) pP2;
				pGroupNode->AddChild(pL2Node);
			}
			// Level 3
			for( int k = 0; k < strListL3.getCount(); k++ ) 
			{
				CString strTemp3 = strListL3.getString(k);
				strTemp3.TrimLeft(); strTemp3.TrimRight();
				if (strTemp3.IsEmpty())
				{
					continue;
				}
				//CTVNode* pL3Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3);
//				CTVNode* pL3Node = new CTVNode(strTemp3, IDR_CTX_PROCESSORGROUP);
//				pL2Node->AddChild(pL3Node);
				sID = strTemp1+"-"+strTemp2+"-"+strTemp3;
				id.setID((LPCSTR) sID);
				StringList strListL4;
				terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL4, -1 );
				terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL4, RunwayProcessor );

				if(strListL4.IsEmpty())
				{
					CTVNode *pL3Node = new CTVNode(id.GetIDString());
					pL3Node->m_nIDResource = IDR_CTX_PROCESSOR;
					//get CProcessor2* associated with this id
					CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
					ASSERT(pP2 != NULL);
					pL3Node->m_dwData = (DWORD) pP2;
					pGroupNode->AddChild(pL3Node);
				}
				// Level 4
				for( int l = 0; l < strListL4.getCount(); l++ ) 
				{
					CString strTemp4 = strListL4.getString(l);
					strTemp4.TrimLeft(); strTemp4.TrimRight();
					if (strTemp4.IsEmpty())
					{				
						continue;
					}
					//CTVNode* pL4Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4, IDR_CTX_PROCESSOR);
//					CTVNode* pL4Node = new CTVNode(strTemp4, IDR_CTX_PROCESSOR);
//					pL3Node->AddChild(pL4Node);
					//get CProcessor2* associated with this id
					sID = strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4;
					id.setID((LPCSTR) sID);
					CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
					ASSERT(pP2 != NULL);
					CTVNode *pL4Node = new CTVNode(id.GetIDString(),IDR_CTX_PROCESSOR);
					pL4Node->m_dwData = (DWORD) pP2;
					pGroupNode->AddChild(pL4Node);
				}
			}
		}
	}
*/
	m_pTermPlanDoc->UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pGroupNode);
}

void CAirSideMSImpl::AddProcessorARPGroup(CTVNode* pGroupNode /* = NULL */)
{
	int iType = DeiceBayProcessor;
	ASSERT(m_pTermPlanDoc != NULL);
	if (pGroupNode == NULL)
		pGroupNode = FindProcTypeNode(iType);
	ASSERT(pGroupNode != NULL);

	Terminal& terminal = m_pTermPlanDoc->GetTerminal();
	pGroupNode->DeleteAllChildren();

	CTVNode *pNode = NULL;

	CString strLongitude,strLatitude;
	m_pTermPlanDoc->GetTerminal().m_AirsideInput->GetLL(m_pTermPlanDoc->m_ProjInfo.path,strLongitude,strLatitude);
	CString strLL;
	strLL.Format("LAT:%s LONG:%s",strLatitude,strLongitude);
	pNode=new CTVNode(strLL,IDR_CTX_DEFAULT);
	pGroupNode->AddChild(pNode);

	// X Y position
	CString strXY;
	double dx=m_pTermPlanDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->m_dx;
	double dy=m_pTermPlanDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->m_dy;
	strXY.Format("X:%.2f Y:%.2f",dx,dy);
	pNode=new CTVNode(strXY,IDR_CTX_DEFAULT);
	pGroupNode->AddChild(pNode);

	//Elevation
	CString strElevation;
	double dElevation;
	m_pTermPlanDoc->GetTerminal().m_AirsideInput->m_pAirportInfo->GetElevation(dElevation);
	strElevation.Format("Elevation(m): %.0f",dElevation);
	pNode = new CTVNode(strElevation , IDR_CTX_DEFAULT);
	pGroupNode->AddChild(pNode);

}

CTVNode* CAirSideMSImpl::RebuildProcessorTree(ProcessorClassList ProcType)
{
	CTVNode * pGroupNode = FindProcTypeNode(ProcType);
	ASSERT(pGroupNode);
	if (pGroupNode)
	{
		pGroupNode->DeleteAllChildren();
	}
	AddProcessorGroup(ProcType,pGroupNode);
	
	return pGroupNode;
}


/************************************************************************
FUNCTION: add WallShape to node view
IN		: pGroupNode,the parent node of the node added(NULL DEFAULT)
OUT		:                                                                
/************************************************************************/
void CAirSideMSImpl::AddWallShapeGroup(CTVNode* pGroupNode /* = NULL */)
{
	

	ASSERT(m_pTermPlanDoc != NULL);
	if (pGroupNode == NULL)
	{
		CString strProcTypeName;
		if (strProcTypeName.LoadString(IDS_TVNN_WALLSHAPE))
			pGroupNode =  m_pTermPlanDoc->m_msManager.FindNodeByName(strProcTypeName);
	}
	ASSERT(pGroupNode != NULL);

	Terminal& terminal = m_pTermPlanDoc->GetTerminal();
	pGroupNode->DeleteAllChildren();
	ProcessorID id, *pid = NULL;
	id.SetStrDict(terminal.inStrDict);
	CString sID;
	StringList strListL1;
	m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_AirSide).getAllGroupNames(strListL1);

	//level 1
	for(int i = 0; i < strListL1.getCount(); i++)
	{

		CString strTemp1 = strListL1.getString(i);
		strTemp1.TrimLeft(); strTemp1.TrimRight();
		if (strTemp1.IsEmpty())
		{
			continue;
		}
		CTVNode*  pL1Node= new CTVNode(strTemp1);
		pGroupNode->AddChild(pL1Node);
		sID = strTemp1;
		id.setID((LPCSTR) sID);
		StringList strListL2;
		m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_AirSide).getMemberNames(id, strListL2);
//		m_pTermPlanDoc->GetCurWallShapeList(EnvMode_AirSide).getMemberNames(id, strListL2);
		if(strListL2.IsEmpty()) 
		{
			pL1Node->m_nIDResource = IDR_CTX_WALLSHAPE;
			//get CProcessor2* associated with this id
			WallShape * pWallShap = m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_AirSide).getWallShape(id);
			ASSERT(pWallShap != NULL);
			pL1Node->m_dwData = (DWORD) pWallShap;
		}
		// Level 2
		for( int j = 0; j < strListL2.getCount(); j++ )
		{
			CString strTemp2 = strListL2.getString(j);
			strTemp2.TrimLeft();  strTemp2.TrimRight();
			if (strTemp2.IsEmpty())
			{
				continue;
			}
			//CTVNode* pL2Node = new CTVNode(strTemp1+"-"+strTemp2);
			CTVNode* pL2Node = new CTVNode(strTemp2);
			pL1Node->AddChild(pL2Node);
			sID = strTemp1+"-"+strTemp2;
			id.setID((LPCSTR) sID);
			StringList strListL3;
//			terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL3, -1 );
			m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_AirSide).getMemberNames(id, strListL3);
//			terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL3, RunwayProcessor );
			if(strListL3.IsEmpty())
			{
				pL2Node->m_nIDResource = IDR_CTX_WALLSHAPE;
				//get CProcessor2* associated with this id
//				CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
				WallShape * pWallShap = m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_AirSide).getWallShape(id);
				ASSERT(pWallShap != NULL);
				pL2Node->m_dwData = (DWORD) pWallShap;
			}
			// Level 3
			for( int k = 0; k < strListL3.getCount(); k++ ) 
			{
				CString strTemp3 = strListL3.getString(k);
				strTemp3.TrimLeft(); strTemp3.TrimRight();
				if (strTemp3.IsEmpty())
				{
					continue;
				}
				//CTVNode* pL3Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3);
				CTVNode* pL3Node = new CTVNode(strTemp3);
				pL2Node->AddChild(pL3Node);
				sID = strTemp1+"-"+strTemp2+"-"+strTemp3;
				id.setID((LPCSTR) sID);
				StringList strListL4;
//				terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL4, -1 );
				m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_AirSide).getMemberNames(id, strListL4);
//				terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL4, RunwayProcessor );

				if(strListL4.IsEmpty())
				{
					pL3Node->m_nIDResource = IDR_CTX_WALLSHAPE;
					//get CProcessor2* associated with this id
//					CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
					WallShape * pWallShap = m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_AirSide).getWallShape(id);
					ASSERT(pWallShap != NULL);
					pL3Node->m_dwData = (DWORD) pWallShap;
				}
				// Level 4
				for( int l = 0; l < strListL4.getCount(); l++ ) 
				{
					CString strTemp4 = strListL4.getString(l);
					strTemp4.TrimLeft(); strTemp4.TrimRight();
					if (strTemp4.IsEmpty())
					{				
						continue;
					}
					//CTVNode* pL4Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4, IDR_CTX_PROCESSOR);
					CTVNode* pL4Node = new CTVNode(strTemp4, IDR_CTX_WALLSHAPE);
					pL3Node->AddChild(pL4Node);
					//get CProcessor2* associated with this id
					sID = strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4;
					id.setID((LPCSTR) sID);
					WallShape * pWallShap = m_pTermPlanDoc->GetWallShapeListByMode(EnvMode_AirSide).getWallShape(id);
//					CProcessor2* pWallShap = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
					ASSERT(pWallShap != NULL);
					pL4Node->m_dwData = (DWORD) pWallShap;
				}
			}
		}
	}
	m_pTermPlanDoc->UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pGroupNode);
}
/************************************************************************
FUNCTION: add structure(surface) to node view
IN		: pGroupNode,the parent node of the node added
OUT		:                                                                
/************************************************************************/
void CAirSideMSImpl::AddStructureGroup(CTVNode* pGroupNode /* = NULL */)
{


	ASSERT(m_pTermPlanDoc != NULL);
	if (pGroupNode == NULL)
	{
		CString strProcTypeName;
		if (strProcTypeName.LoadString(IDS_TVNN_SURFACEAREAS))
			pGroupNode =  m_pTermPlanDoc->m_msManager.FindNodeByName(strProcTypeName);
	}
	ASSERT(pGroupNode != NULL);

	Terminal& terminal = m_pTermPlanDoc->GetTerminal();
	pGroupNode->DeleteAllChildren();
	ProcessorID id, *pid = NULL;
	id.SetStrDict(terminal.inStrDict);
	CString sID;
	StringList strListL1;
	m_pTermPlanDoc->GetStructurelistByMode(EnvMode_AirSide).getAllGroupNames(strListL1);

	//level 1
	for(int i = 0; i < strListL1.getCount(); i++)
	{

		CString strTemp1 = strListL1.getString(i);
		strTemp1.TrimLeft(); strTemp1.TrimRight();
		if (strTemp1.IsEmpty())
		{
			continue;
		}
		CTVNode*  pL1Node= new CTVNode(strTemp1);
		pGroupNode->AddChild(pL1Node);
		sID = strTemp1;
		id.setID((LPCSTR) sID);
		StringList strListL2;
		m_pTermPlanDoc->GetStructurelistByMode(EnvMode_AirSide).getMemberNames(id, strListL2);
		//		m_pTermPlanDoc->GetCurWallShapeList(EnvMode_AirSide).getMemberNames(id, strListL2);
		if(strListL2.IsEmpty()) 
		{
			pL1Node->m_nIDResource = IDR_CTX_STRUCTURE;
			//get CProcessor2* associated with this id
			CStructure * pStructure = m_pTermPlanDoc->GetStructurelistByMode(EnvMode_AirSide).getStructure(id);
			ASSERT(pStructure != NULL);
			pL1Node->m_dwData = (DWORD) pStructure;
		}
		// Level 2
		for( int j = 0; j < strListL2.getCount(); j++ )
		{
			CString strTemp2 = strListL2.getString(j);
			strTemp2.TrimLeft();  strTemp2.TrimRight();
			if (strTemp2.IsEmpty())
			{
				continue;
			}
			//CTVNode* pL2Node = new CTVNode(strTemp1+"-"+strTemp2);
			CTVNode* pL2Node = new CTVNode(strTemp2);
			pL1Node->AddChild(pL2Node);
			sID = strTemp1+"-"+strTemp2;
			id.setID((LPCSTR) sID);
			StringList strListL3;
			//			terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL3, -1 );
			m_pTermPlanDoc->GetStructurelistByMode(EnvMode_AirSide).getMemberNames(id, strListL3);
			//			terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL3, RunwayProcessor );
			if(strListL3.IsEmpty())
			{
				pL2Node->m_nIDResource = IDR_CTX_STRUCTURE;
				//get CProcessor2* associated with this id
				//				CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
				CStructure * pStructure = m_pTermPlanDoc->GetStructurelistByMode(EnvMode_AirSide).getStructure(id);
				ASSERT(pStructure != NULL);
				pL2Node->m_dwData = (DWORD) pStructure;
			}
			// Level 3
			for( int k = 0; k < strListL3.getCount(); k++ ) 
			{
				CString strTemp3 = strListL3.getString(k);
				strTemp3.TrimLeft(); strTemp3.TrimRight();
				if (strTemp3.IsEmpty())
				{
					continue;
				}
				//CTVNode* pL3Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3);
				CTVNode* pL3Node = new CTVNode(strTemp3);
				pL2Node->AddChild(pL3Node);
				sID = strTemp1+"-"+strTemp2+"-"+strTemp3;
				id.setID((LPCSTR) sID);
				StringList strListL4;
				//				terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL4, -1 );
				m_pTermPlanDoc->GetStructurelistByMode(EnvMode_AirSide).getMemberNames(id, strListL4);
				//				terminal.GetProcessorList(EnvMode_AirSide)->getMemberNames( id, strListL4, RunwayProcessor );

				if(strListL4.IsEmpty())
				{
					pL3Node->m_nIDResource = IDR_CTX_STRUCTURE;
					//get CProcessor2* associated with this id
					//					CProcessor2* pP2 = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
					CStructure * pStructure = m_pTermPlanDoc->GetStructurelistByMode(EnvMode_AirSide).getStructure(id);
					ASSERT(pStructure != NULL);
					pL3Node->m_dwData = (DWORD) pStructure;
				}
				// Level 4
				for( int l = 0; l < strListL4.getCount(); l++ ) 
				{
					CString strTemp4 = strListL4.getString(l);
					strTemp4.TrimLeft(); strTemp4.TrimRight();
					if (strTemp4.IsEmpty())
					{				
						continue;
					}
					//CTVNode* pL4Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4, IDR_CTX_PROCESSOR);
					CTVNode* pL4Node = new CTVNode(strTemp4, IDR_CTX_STRUCTURE);
					pL3Node->AddChild(pL4Node);
					//get CProcessor2* associated with this id
					sID = strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4;
					id.setID((LPCSTR) sID);
					CStructure * pStructure = m_pTermPlanDoc->GetStructurelistByMode(EnvMode_AirSide).getStructure(id);
					//					CProcessor2* pWallShap = m_pTermPlanDoc->GetProcessor2FromID(id, EnvMode_AirSide);
					ASSERT(pStructure != NULL);
					pL4Node->m_dwData = (DWORD) pStructure;
				}
			}
		}
	}
	m_pTermPlanDoc->UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pGroupNode);
}

PMSVDbClickFunction CAirSideMSImpl::GetItemDbClickFunction(CTVNode* pNode)
{
	std::map<CTVNode*, PMSVDbClickFunction>::iterator iter =  m_mapItemDbClick.find(pNode);
	if (iter == m_mapItemDbClick.end())
		return NULL;

	return iter->second;
}
