#include "StdAfx.h"
#include "resource.h"
#include "landsidemsimpl.h"
#include "Floor2.h"
#include "..\common\template.h"
#include "TermPlanDoc.h"
//#include <CommonData/Shape.h>
#include "landside/LandsideLevel.h"
#include "Landside/InputLandside.h"

CLandSideMSImpl::CLandSideMSImpl()
{
}

CLandSideMSImpl::~CLandSideMSImpl()
{
}

void CLandSideMSImpl::BuildTVNodeTree()
{
	ASSERT(m_pTermPlanDoc != NULL);
	//Terminal& terminal = m_pTermPlanDoc->GetTerminal();
	CTVNode* pLandSideRootNode=GetTVRootNode();
	//Build Node structure
	CString s;
	//add layout node to root
	s.LoadString(IDS_TVNN_LAYOUT);
	CTVNode* pLayoutNode = new CTVNode(s);
	pLayoutNode->m_eState = CTVNode::expanded;
	pLandSideRootNode->AddChild(pLayoutNode);

	CTVNode *pNode=NULL;

	//add levels 
	s.LoadString(IDS_TVNN_LEVELS);	
	CTVNode* pLevelsNode = new CTVNode(s, IDR_CTX_LEVELS);
	pLayoutNode->AddChild(pLevelsNode);	


	InputLandside* pInputLandside = m_pTermPlanDoc->getARCport()->m_pInputLandside;

	//add each level to Node
	CFloors& levelList = m_pTermPlanDoc->GetFloorByMode(EnvMode_LandSide);
	for (int i=0;i<levelList.GetCount();i++ )
	{
		CFloor2* pLevel = levelList.GetFloor2(i);
		CTVNode* pLevelNode = new CTVNode(pLevel->FloorName(), IDR_CTX_LEVEL);
		pLevelNode->m_iImageIdx = (pLevel==levelList.GetActiveFloor()?ID_NODEIMG_FLOORACTIVE:ID_NODEIMG_FLOORNOTACTIVE);
		pLevelNode->m_dwData = (DWORD) i;
		pLevelsNode->AddChild(pLevelNode);
	}

	//add Rolling Surfaces
	s.LoadString(IDS_TVNN_ROLLSURFACE);
	CTVNode *pRollingSurfaces=new CTVNode(s);
	pRollingSurfaces->m_eState=CTVNode::expanded;
	pLayoutNode->AddChild(pRollingSurfaces);

	//Stretches
	s.LoadString(IDS_TVNN_STRETCHS);
	pNode=new CTVNode(s);
	pRollingSurfaces->AddChild(pNode);
	//AddLayoutObjectGroup(ALT_LSTRETCH,pNode);
	//AddProcessorGroup(StretchProcessor,pNode);
	
	//Intersections
	s.LoadString(IDS_TVNN_INTERSECTIONS);
	pNode=new CTVNode(s);
	pRollingSurfaces->AddChild(pNode);
	//AddProcessorGroup(IntersectionProcessor,pNode);


	//RoundAbout
	s.LoadString(IDS_TVNN_ROUNDABOUT);
	pNode = new CTVNode(s);
	pRollingSurfaces->AddChild(pNode);
	//AddProcessorGroup(RoundAboutProcessor,pNode);

	//Turnoffs
	s.LoadString(IDS_TVNN_TURNOFFS);
	pNode=new CTVNode(s);
	pRollingSurfaces->AddChild(pNode);
	//*A*/ddProcessorGroup(TurnoffProcessor,pNode);

	//Overpasses
	s.LoadString(IDS_TVNN_OVERPASS);
	pNode=new CTVNode(s);
	pRollingSurfaces->AddChild(pNode);
	//AddProcessorGroup(OverPassProcessor,pNode);

	//Underpasses
	s.LoadString(IDS_TVNN_UNDERPASS);
	pNode=new CTVNode(s);
	pRollingSurfaces->AddChild(pNode);
	//AddProcessorGroup(UnderPassProcessor,pNode);

	//LaneAdapter
	s.LoadString(IDS_TVNN_LANEADAPTER);
	pNode=new CTVNode(s);
	pRollingSurfaces->AddChild(pNode);
	//AddProcessorGroup(LaneAdapterProcessor,pNode);

	//Cloverleafs
	s.LoadString(IDS_TVNN_CLOVERLEAF);
	pNode=new CTVNode(s);
	pRollingSurfaces->AddChild(pNode);
	//AddProcessorGroup(CloverLeafProcessor,pNode);


	//add Control Devices
	s.LoadString(IDS_TVNN_CONTROLDEVICE);
	CTVNode *pControlDevices=new CTVNode(s);
	pControlDevices->m_eState=CTVNode::expanded;
	pLayoutNode->AddChild(pControlDevices);
	
	//Traffic lights
	s.LoadString(IDS_TVNN_TRAFFICLIGHT);
	pNode=new CTVNode(s);
	pControlDevices->AddChild(pNode);
	//AddProcessorGroup(TrafficLightProceesor,pNode);

	//Toll Gates
	s.LoadString(IDS_TVNN_TOLLGATE);
	pNode=new CTVNode("Toll Gates");
	pControlDevices->AddChild(pNode);
	//AddProcessorGroup(TollGateProcessor,pNode);


	//Stop signs
	s.LoadString(IDS_TVNN_STOPSIGN);
	pNode=new CTVNode(s);
	pControlDevices->AddChild(pNode);
	//AddProcessorGroup(StopSignProcessor,pNode);

	//Yield sign
	s.LoadString(IDS_TVNN_YIELDSIGN);
	pNode=new CTVNode(s);
	pControlDevices->AddChild(pNode);
	//AddProcessorGroup(YieldDeviceProcessor,pNode);

	
	//add Parking Areas
	s.LoadString(IDS_TVNN_PARKINGAREA);
	CTVNode *pParkingAreas=new CTVNode(s);
	pParkingAreas->m_eState=CTVNode::expanded;
	pLayoutNode->AddChild(pParkingAreas);
	

	//Line park
	s.LoadString(IDS_TVNN_LINEPARKING);
	pNode=new CTVNode(s);
	pParkingAreas->AddChild(pNode);
	//AddProcessorGroup(LineParkingProcessor,pNode);
	
	//Nose In park
	s.LoadString(IDS_TVNN_NOSEINPARKING);
	pNode=new CTVNode(s);
	pParkingAreas->AddChild(pNode);
	//AddProcessorGroup(NoseInParkingProcessor,pNode);

	//parking lot
	s.LoadString(IDS_TVNN_PARKINGLOT);
	pNode=new CTVNode(s);
	pParkingAreas->AddChild(pNode);
	//AddProcessorGroup(ParkingLotProcessor,pNode);

	//add Terminal Interfaces
	CTVNode *pTerminalnterfaces=new CTVNode("Terminal Interfaces");
	pTerminalnterfaces->m_eState=CTVNode::expanded;
	pLayoutNode->AddChild(pTerminalnterfaces);

	//Curbsides
	pNode=new CTVNode("Curbsides");
	pTerminalnterfaces->AddChild(pNode);

	//City Train Station
	pNode=new CTVNode("City Train Station");
	pTerminalnterfaces->AddChild(pNode);

	//Bus Stations
	pNode=new CTVNode("Bus Stations");
	pTerminalnterfaces->AddChild(pNode);

	//Shuttle Stations
	pNode=new CTVNode("Shuttle Stations");
	pTerminalnterfaces->AddChild(pNode);

	
	
	//add Vehicle data to root
	CTVNode *pVehicleData=new CTVNode("Vehicle data");
	pVehicleData->m_eState=CTVNode::expanded;
	pLandSideRootNode->AddChild(pVehicleData);

	//Types
	pNode=new CTVNode("Types");
	pVehicleData->AddChild(pNode);
	
	//Capacities
	//pNode=new CTVNode("Capacities");
	//pVehicleData->AddChild(pNode);

	//Performance
	//pNode=new CTVNode("Performance");
	//pVehicleData->AddChild(pNode);

	//Operations
	pNode=new CTVNode("Operations");
	pVehicleData->AddChild(pNode);



	//add Flight related data to root
	CTVNode *pFlightRelatedData=new CTVNode("Flight related data");
	pFlightRelatedData->m_eState=CTVNode::expanded;
	pLandSideRootNode->AddChild(pFlightRelatedData);
	
	//Flight Schedule
	pNode=new CTVNode("Flight Schedule");
	pFlightRelatedData->AddChild(pNode);

	//Aircraft Capacity
	pNode=new CTVNode("Aircraft Capacity");
	pFlightRelatedData->AddChild(pNode);

	//Load factors
	pNode=new CTVNode("Load Factors");
	pFlightRelatedData->AddChild(pNode);



	//add Passenger related data to root
	CTVNode *pPassengerRelatedData=new CTVNode("Passenger related data");
	pPassengerRelatedData->m_eState=CTVNode::expanded;
	pLandSideRootNode->AddChild(pPassengerRelatedData);
	
	//Names
	pNode=new CTVNode("Names");
	pPassengerRelatedData->AddChild(pNode);

	//Distributions
	pNode=new CTVNode("Distributions");
	pPassengerRelatedData->AddChild(pNode);

	//Bag count
	pNode=new CTVNode("Bag count");
	pPassengerRelatedData->AddChild(pNode);

	//Group size
	pNode=new CTVNode("Group size");
	pPassengerRelatedData->AddChild(pNode);

	//Lead-Lag distribution
	pNode=new CTVNode("Lead-Lag distribution");
	pPassengerRelatedData->AddChild(pNode);



	//add Management data to root
	CTVNode *pManagemetData=new CTVNode("Management data");
	pManagemetData->m_eState=CTVNode::expanded;
	pLandSideRootNode->AddChild(pManagemetData);
	
	//Drop off contraints
	pNode=new CTVNode("Drop off contraints");
	pManagemetData->AddChild(pNode);

	//Parking contraints
	pNode=new CTVNode("Parking contraints");
	pManagemetData->AddChild(pNode);

	//Posted Speeds
	pNode=new CTVNode("Posted Speeds");
	pManagemetData->AddChild(pNode);

	//Height restrictions
	pNode=new CTVNode("Height restrictions");
	pManagemetData->AddChild(pNode);

	//Weight restrictions
	pNode=new CTVNode("Weight restrictions");
	pManagemetData->AddChild(pNode);

	//Access restrictions
	pNode=new CTVNode("Access restrictions");
	pManagemetData->AddChild(pNode);

	//Lane Availability
	pNode=new CTVNode("Lane Availability");
	pManagemetData->AddChild(pNode);

	//Lane changes
	pNode=new CTVNode("Lane changes");
	pManagemetData->AddChild(pNode);

	//Overtaking
	pNode=new CTVNode("Overtaking");
	pManagemetData->AddChild(pNode);



	// add Simulation to root
	CTVNode *pSimulation=new CTVNode("Simulation");
	pSimulation->m_eState=CTVNode::expanded;
	pLandSideRootNode->AddChild(pSimulation);
	
	//Settings
	pNode=new CTVNode("Settings");
	pSimulation->AddChild(pNode);

	//Run
	pNode=new CTVNode("Run");
	pSimulation->AddChild(pNode);



	//add Analysis parameters to root
	CTVNode *pAnalysisParameters=new CTVNode("Analysis parameters");
	pAnalysisParameters->m_eState=CTVNode::expanded;
	pLandSideRootNode->AddChild(pAnalysisParameters);

	//Vehicle display parameters
	pNode=new CTVNode("Vehicle display parameters");
	pAnalysisParameters->AddChild(pNode);

	//Mobile element display parameters
	pNode=new CTVNode("Mobile element display parameters");
	pAnalysisParameters->AddChild(pNode);

	//Report parameters
	pNode=new CTVNode("Report parameters");
	pAnalysisParameters->AddChild(pNode);

	//Callout Display Specification
	pNode=new CTVNode("Callout Display Specification");
	pAnalysisParameters->AddChild(pNode);

}
CTVNode* FindProcParentNode(const ProcessorID& procid, int nlevel, CTVNode* pGroupNode, int& outlevel)
{
	
	outlevel = nlevel;
	if(nlevel>MAX_PROC_IDS) return NULL;
	CString name;
	procid.getNameAtLevel(name.GetBuffer(256),nlevel);
	//first level 
	for(int i=0;i<pGroupNode->GetChildCount();i++)
	{
		CTVNode * pNode =(CTVNode*) pGroupNode->GetChildByIdx(i);
		if( name.CompareNoCase( pNode->Name() ) ==0 )
		{			
			return FindProcParentNode(procid,nlevel+1,pNode, outlevel);
		}		
	}
	return pGroupNode;
	
}
//ProcessorList & CLandSideMSImpl::GetProcessorList()
//{
//	static ProcessorList prolist(NULL);
//	return prolist;//m_pTermPlanDoc->GetLandsideDoc()->GetProcessorList();
//}

void CLandSideMSImpl::AddLayoutObjectGroup(  ALTOBJECT_TYPE iType,CTVNode* pGroupNode )
{
	//if (pGroupNode == NULL)
	//{
	//	pGroupNode = FindObjectTypeNode(iType);	
	//}	
	//ASSERT(pGroupNode != NULL);
	//pGroupNode->DeleteAllChildren();

	//ASSERT(m_pTermPlanDoc != NULL);
	//InputLandside* pInputLandside = m_pTermPlanDoc->getARCport()->m_pInputLandside;
	//LandsideLayoutObjectList& objlist = pInputLandside->getObjectList();
	//
	////Terminal& terminal = m_pTermPlanDoc->GetTerminal();
	//pGroupNode->DeleteAllChildren();
	///*ProcessorID id, *pid = NULL;
	//id.SetStrDict(terminal.inStrDict);*/
	////CString sID;
	//StringVector strListL1;
	//objlist.getAllGroupNames(strListL1, iType);

	////level 1
	//for(int i = 0; i < (int)strListL1.size(); i++)
	//{

	//	CString strTemp1 = strListL1.at(i);
	//	strTemp1.TrimLeft(); strTemp1.TrimRight();
	//	if (strTemp1.IsEmpty())
	//	{
	//		continue;
	//	}
	//	CTVNode*  pL1Node= new CTVNode(strTemp1,IDR_CTX_LANDPROCESSORGROUP);
	//	pGroupNode->AddChild(pL1Node);
	//	CString sID = strTemp1;
	//	ALTObjectID id;
	//	id.FromString(sID.GetString());
	//	StringVector strListL2;
	//	objlist.getMemberNames(id, strListL2,ALT_LSTRETCH);		
	//	if(strListL2.empty()) 
	//	{
	//		pL1Node->m_nIDResource = IDR_CTX_LANDPROCESSOR;
	//		//get CProcessor2* associated with this id
	//		LandsideLayoutObject* pP2 = objlist.getObjectByName(id);
	//		ASSERT(pP2 != NULL);
	//		pL1Node->m_dwData = (DWORD) pP2;
	//	}
	//	// Level 2
	//	for( int j = 0; j < (int)strListL2.size(); j++ )
	//	{
	//		CString strTemp2 = strListL2.at(j);
	//		strTemp2.TrimLeft();  strTemp2.TrimRight();
	//		if (strTemp2.IsEmpty())
	//		{
	//			continue;
	//		}
	//		//CTVNode* pL2Node = new CTVNode(strTemp1+"-"+strTemp2);
	//		CTVNode* pL2Node = new CTVNode(strTemp2, IDR_CTX_LANDPROCESSORGROUP);
	//		pL1Node->AddChild(pL2Node);
	//		sID = strTemp1+"-"+strTemp2;	
	//		id.FromString(sID.GetString());

	//		StringVector strListL3;
	//		objlist.getMemberNames(id, strListL3, ALT_LSTRETCH);
	//		
	//		if(strListL3.empty())
	//		{
	//			pL2Node->m_nIDResource = IDR_CTX_LANDPROCESSOR;
	//			//get CProcessor2* associated with this id				
	//			LandsideLayoutObject* pP2 = objlist.getObjectByName(id);
	//			ASSERT(pP2 != NULL);
	//			pL2Node->m_dwData = (DWORD) pP2;
	//		}
	//		// Level 3
	//		for( int k = 0; k < (int)strListL3.size(); k++ ) 
	//		{
	//			CString strTemp3 = strListL3.at(k);
	//			strTemp3.TrimLeft(); strTemp3.TrimRight();
	//			if (strTemp3.IsEmpty())
	//			{
	//				continue;
	//			}
	//			//CTVNode* pL3Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3);
	//			CTVNode* pL3Node = new CTVNode(strTemp3, IDR_CTX_LANDPROCESSORGROUP);
	//			pL2Node->AddChild(pL3Node);
	//			sID = strTemp1+"-"+strTemp2+"-"+strTemp3;
	//			id.FromString((LPCSTR) sID);

	//			StringVector strListL4;
	//			objlist.getMemberNames(id, strListL4,ALT_LSTRETCH);

	//			if(strListL4.empty())
	//			{
	//				pL3Node->m_nIDResource = IDR_CTX_LANDPROCESSOR;
	//				//get CProcessor2* associated with this id
	//				LandsideLayoutObject* pP2 = objlist.getObjectByName(id);
	//				ASSERT(pP2 != NULL);
	//				pL3Node->m_dwData = (DWORD) pP2;
	//			}
	//			// Level 4
	//			for( int l = 0; l < (int)strListL4.size(); l++ ) 
	//			{
	//				CString strTemp4 = strListL4.at(l);
	//				strTemp4.TrimLeft(); strTemp4.TrimRight();
	//				if (strTemp4.IsEmpty())
	//				{				
	//					continue;
	//				}
	//				//CTVNode* pL4Node = new CTVNode(strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4, IDR_CTX_PROCESSOR);
	//				CTVNode* pL4Node = new CTVNode(strTemp4, IDR_CTX_LANDPROCESSOR);
	//				pL3Node->AddChild(pL4Node);
	//				//get CProcessor2* associated with this id
	//				sID = strTemp1+"-"+strTemp2+"-"+strTemp3+"-"+strTemp4;
	//				id.FromString((LPCSTR) sID);

	//				LandsideLayoutObject* pP2 = objlist.getObjectByName(id);
	//				ASSERT(pP2 != NULL);
	//				pL4Node->m_dwData = (DWORD) pP2;
	//			}
	//		}
	//	}
	//}

	//m_pTermPlanDoc->UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pGroupNode);
}

void CLandSideMSImpl::RebuildLayoutObjectTree(ALTOBJECT_TYPE ProcType)
{
	CTVNode * pGroupNode = FindObjectTypeNode(ProcType);
	ASSERT(pGroupNode);
	if (pGroupNode)
	{
		pGroupNode->DeleteAllChildren();
	}
	AddLayoutObjectGroup(ProcType,pGroupNode);	
}

CTVNode* CLandSideMSImpl::FindObjectTypeNode(ALTOBJECT_TYPE ProcType)
{
	ASSERT(m_pTermPlanDoc != NULL);
	CTVNode* pRetNode = NULL;
	//	CTVNode* pAirSideRootNode = m_pTermPlanDoc->GetAirSideRootTVNode();
	CTVNode* pLandsideRootNode=GetTVRootNode();
	if (pLandsideRootNode->GetChildCount() == 0)
		return NULL;
	

	UINT nProcTypeNameResID = 0;
	switch(ProcType)
	{
	case ALT_LSTRETCH : 
		nProcTypeNameResID = IDS_TVNN_STRETCHS;
		break;
	case ALT_LINTERSECTION :
		nProcTypeNameResID = IDS_TVNN_INTERSECTIONS;
		break;
	/*case RoundAboutProcessor :
		nProcTypeNameResID = IDS_TVNN_ROUNDABOUT;
		break;
	case TurnoffProcessor :
		nProcTypeNameResID = IDS_TVNN_TURNOFFS;
		break;
	case LaneAdapterProcessor :
		nProcTypeNameResID = IDS_TVNN_LANEADAPTER;
		break;
	case OverPassProcessor :
		nProcTypeNameResID = IDS_TVNN_OVERPASS;
		break;
	case UnderPassProcessor :
		nProcTypeNameResID = IDS_TVNN_UNDERPASS;
		break;
	case TrafficLightProceesor :
		nProcTypeNameResID = IDS_TVNN_TRAFFICLIGHT;
		break;
	case StopSignProcessor :
		nProcTypeNameResID = IDS_TVNN_STOPSIGN;
		break;
	case YieldDeviceProcessor :
		nProcTypeNameResID = IDS_TVNN_YIELDSIGN;
		break;
	case TollGateProcessor :
		nProcTypeNameResID = IDS_TVNN_TOLLGATE;
		break;
	case ParkingLotProcessor :
		nProcTypeNameResID = IDS_TVNN_PARKINGLOT;
		break;
	case LineParkingProcessor :
		nProcTypeNameResID = IDS_TVNN_LINEPARKING;
		break;
	case NoseInParkingProcessor :
		nProcTypeNameResID = IDS_TVNN_NOSEINPARKING;
		break;
	case CloverLeafProcessor :
		nProcTypeNameResID = IDS_TVNN_CLOVERLEAF;
		break;*/
	default: 
		ASSERT(false);
	}
	CString strProcTypeName;
	if (nProcTypeNameResID != 0)
	{
		if (strProcTypeName.LoadString(nProcTypeNameResID))
			pRetNode =  m_pTermPlanDoc->m_msManager.FindNodeByName(strProcTypeName);
	}

	return pRetNode;

}

