#include "StdAfx.h"
#include "AirportResourceManager.h"
#include "../../InputAirside/Runway.h"
#include "../../InputAirside/Taxiway.h"
//#include "../../AirsideInput/AMGraph.h"
#include "../../InputAirside/ALTAirport.h"
#include "../../InputAirside/stand.h"
#include "../../InputAirside/DeicePad.h"
#include "AirsideSimConfig.h"
#include "AirsideMeetingPointInSim.h"
#include "../../InputAirside/MeetingPoint.h"
#include "TaxiwayToPoolRouteFinder.h"
#include "StartPositionInSim.h"

AirportResourceManager::~AirportResourceManager()
{
	int nCount = m_vMeetingPoints.size();
	for (int i = 0; i < nCount; i++)
	{
		AirsideMeetingPointInSim* pMeetingPoint = m_vMeetingPoints.at(i);
		delete pMeetingPoint;
		pMeetingPoint = NULL;
	}
	m_vMeetingPoints.clear();

	if (m_pTaxiwayToNearestStretchRouteFinder)
	{
		delete m_pTaxiwayToNearestStretchRouteFinder;
		m_pTaxiwayToNearestStretchRouteFinder = NULL;
	}
}


bool AirportResourceManager::Init( int projID, int nAirportID, CAirportDatabase* pAirportDB, AircraftClassificationManager* pAircraftClassification,const AirsideSimConfig& simconf )
{
	m_flightGroundRouteManager.SetResourceManager(&m_TaxiwayRes,&m_RunwayRes);
	m_AirportInfo.ReadAirport(nAirportID);
	// init intersection nodes 	
 
	if(!simconf.bOnlySimStand)
	{
		m_RunwayRes.Init(projID,nAirportID,pAircraftClassification);
		m_TaxiwayRes.Init(projID,nAirportID);
		m_DeiceRes.Init(projID,nAirportID);
		m_VehiclePoolRes.Init(projID,nAirportID);
		m_vIntersectionNodes.Init(projID, nAirportID);
		m_VehicleRouteRes.Init(projID, nAirportID,m_vIntersectionNodes);	
		
	}

	m_StandRes.Init(projID,nAirportID);
	m_paxParkingResouceManager.initialize(projID,nAirportID);

	//update intersection node;
	if(!simconf.bOnlySimStand)
	{
		m_TaxiwayRes.InitRelations(m_vIntersectionNodes );
		m_TaxiwayRes.InitConstraints(projID, nAirportID, pAirportDB);
		m_TaxiwayRes.InitTaxiwayAdjacencyConstraints(projID,pAirportDB);

		m_RunwayRes.InitRelations(m_vIntersectionNodes,getGroundRouteResourceManager(),getTakeoffQueuesManager(),m_TaxiwayRes);
		m_StandRes.InitRelations(m_vIntersectionNodes,getTaxiwayResource());
		m_DeiceRes.InitRelations(m_vIntersectionNodes,getTaxiwayResource());

		InitStartPositions(projID,nAirportID);

		InitMeetingPoints(projID, nAirportID);
		m_VehicleRouteRes.InitRelationsWithMeetingPoint(m_vMeetingPoints);


		m_VehicleRouteRes.InitRelations(m_StandRes,m_DeiceRes, m_VehiclePoolRes,m_TaxiwayRes,m_paxParkingResouceManager);
		m_TakeoffQueueManager.Init(projID,this);
		m_InterruptLineList.Init(this);


		m_pTaxiwayToNearestStretchRouteFinder = new TaxiwayToNearestStretchRouteFinder(this);
	}

	


	return true;
}

FlightGroundRouteResourceManager * AirportResourceManager::getGroundRouteResourceManager()
{
	return &m_flightGroundRouteManager;
}

bool AirportResourceManager::InitGroundRouteResourceManager( TemporaryParkingInSim* tempParkingsegs )
{
	m_flightGroundRouteManager.SetResourceManager(&m_TaxiwayRes,&m_RunwayRes);
	m_flightGroundRouteManager.Init( tempParkingsegs);
	return true;
}

void AirportResourceManager::InitMeetingPoints( int projID, int nAirportID )
{
	ALTObjectList vMeetingPoints;
	ALTAirport::GetMeetingPointList(nAirportID,vMeetingPoints);

	int nCount = vMeetingPoints.size();
	for (int i =0; i <nCount; i++)
	{
		CMeetingPoint* pPoint = (CMeetingPoint*)vMeetingPoints.at(i).get();

		TaxiwayInSim* pTaxiway = m_TaxiwayRes.GetTaxiwayByID(pPoint->GetTaxiwayID());
		if (pTaxiway)
		{		
			AirsideMeetingPointInSim* pMeetingPoint = new AirsideMeetingPointInSim(pPoint);
			TaxiwaySegInSim* pTaxiSeg = pTaxiway->GetPointInSegment(pMeetingPoint->GetPosition());
			if (pTaxiSeg)
			{
				pTaxiSeg->AddMeetingPoints(pMeetingPoint);
				pMeetingPoint->SetAttachedTaxiwaySegment(pTaxiSeg);
			}
			m_vMeetingPoints.push_back(pMeetingPoint);
		}

	}
}

void AirportResourceManager::InitStartPositions( int nPrjID, int nAirportID )
{
	ALTObjectList vStartPositions;
	ALTAirport::GetStartPositionList(nAirportID,vStartPositions);

	size_t nCount = vStartPositions.size();
	for (size_t i =0; i <nCount; i++)
	{
		CStartPosition* pPoint = (CStartPosition*)vStartPositions.at(i).get();

		TaxiwayInSim* pTaxiway = m_TaxiwayRes.GetTaxiwayByID(pPoint->GetTaxiwayID());
		if (pTaxiway)
		{		
			StartPositionInSim* pStartPosition = new StartPositionInSim(pPoint);
			TaxiwaySegInSim* pTaxiSeg = pTaxiway->GetPointInSegment(pStartPosition->GetPosition());
			if (pTaxiSeg)
			{
				pTaxiSeg->AddStartPosition(pStartPosition);
				pStartPosition->SetAttachedTaxiwaySegment(pTaxiSeg);
			}
			m_vStartPositions.push_back(pStartPosition);
		}

	}
}

TaxiwayToNearestStretchRouteFinder* AirportResourceManager::GetTaxiwayToNearestStretchRouteFinder()
{
	return m_pTaxiwayToNearestStretchRouteFinder;
}

AirportResourceManager::AirportResourceManager()
{
	m_pTaxiwayToNearestStretchRouteFinder = NULL;
}

void AirportResourceManager::GetVehicleLastRouteFromTaxiRoute( const FlightGroundRouteDirectSegList& vSegments,std::vector<VehicleRouteNodePairDist>& vNodePairs)
{
	vNodePairs.clear();

	int nCount = vSegments.size();
	if (nCount ==0)
		return;
	
	FlightGroundRouteSegInSim* pSeg = vSegments.at(0)->GetRouteSegInSim();
	IntersectionNodeInSim* pConnectNode = NULL;
	for (int i =1; i < nCount; i++)
	{
		FlightGroundRouteSegInSim* pSegNext = vSegments.at(i)->GetRouteSegInSim();

		if (pSeg->GetNode1() == pSegNext->GetNode1() || pSeg->GetNode1() == pSegNext->GetNode2())	//node 1 is connect node
			pConnectNode = pSeg->GetNode1();
		else
			pConnectNode = pSeg->GetNode2();

		pSeg = pSegNext;		
	}

	if (pSeg)	//the last segment
	{
		VehicleLaneInSim* pLane = NULL;
		std::vector<VehicleLaneInSim*> vLinkedLane;

		for (int iLane = 0; iLane < (int)pSeg->GetLaneCount(); iLane++)
		{
			VehicleLaneInSim* pLane = pSeg->GetLane(iLane);
			for (int iExit = 0; iExit < pLane->GetExitCount(); iExit++)
			{
				VehicleLaneExit* pExit = pLane->GetExit(iExit);
				VehicleRouteNodePairDist nodePair(pLane->GetBeginEntry(),pExit,0);
				vNodePairs.push_back(nodePair);		
			}
			
			VehicleLaneExit* pExit = pLane->GetEndExit();
			VehicleRouteNodePairDist nodePair(pLane->GetBeginEntry(),pExit,0);
			vNodePairs.push_back(nodePair);	
		}
	}
}

void AirportResourceManager::GetVehiclePartOfReturnRouteFromTaxiRoute(const FlightGroundRouteDirectSegList& vSegments, std::vector<VehicleRouteNodePairDist>& vNodePairs)
{
	vNodePairs.clear();

	int nCount = vSegments.size();
	if (nCount ==0)
		return;

	FlightGroundRouteSegInSim* pSeg = vSegments.at(0)->GetRouteSegInSim();
	IntersectionNodeInSim* pConnectNode = NULL;
	bool bFindBegin = false;

	if (vSegments.size() == 1)
		pConnectNode = vSegments.at(0)->GetEntryNode();

	for (int i =1; i < nCount; i++)
	{
		FlightGroundRouteSegInSim* pSegNext = vSegments.at(i)->GetRouteSegInSim();

		if (pSeg->GetNode1() == pSegNext->GetNode1() || pSeg->GetNode1() == pSegNext->GetNode2())	//node 1 is connect node
			pConnectNode = pSeg->GetNode1();
		else
			pConnectNode = pSeg->GetNode2();

		if (pConnectNode)
		{
			VehicleLaneInSim* pLane = pSeg->GetExitLane(pConnectNode);
			ASSERT(pLane);		//the lane should not be null
			if (pLane)
			{			
				VehicleRouteNodePairDist nodePair(pLane->GetBeginEntry(),pLane->GetEndExit(),0);
				vNodePairs.push_back(nodePair);
			}
		}
		pSeg = pSegNext;		
	}
}

void AirportResourceManager::GetVehicleReturnRouteFromTaxiRoute( const FlightGroundRouteDirectSegList& vSegments, std::vector<VehicleRouteNodePairDist>& vNodePairs)
{
	vNodePairs.clear();

	int nCount = vSegments.size();
	if (nCount ==0)
		return;

	FlightGroundRouteSegInSim* pSeg = vSegments.at(0)->GetRouteSegInSim();
	IntersectionNodeInSim* pConnectNode = NULL;
	bool bFindBegin = false;

	if (vSegments.size() == 1)
		pConnectNode = vSegments.at(0)->GetEntryNode();

	for (int i =1; i < nCount; i++)
	{
		FlightGroundRouteSegInSim* pSegNext = vSegments.at(i)->GetRouteSegInSim();

		if (pSeg->GetNode1() == pSegNext->GetNode1() || pSeg->GetNode1() == pSegNext->GetNode2())	//node 1 is connect node
			pConnectNode = pSeg->GetNode1();
		else
			pConnectNode = pSeg->GetNode2();

		if (pConnectNode)
		{
			VehicleLaneInSim* pLane = pSeg->GetExitLane(pConnectNode);
			ASSERT(pLane);		//the lane should not be null
			if (pLane)
			{			
				VehicleRouteNodePairDist nodePair(pLane->GetBeginEntry(),pLane->GetEndExit(),0);
				vNodePairs.push_back(nodePair);
			}
		}
		pSeg = pSegNext;		
	}

	if (pSeg)	//the last segment
	{
		VehicleLaneInSim* pLane = NULL;
		if (pConnectNode == pSeg->GetNode1())
			pLane = pSeg->GetExitLane(pSeg->GetNode2());
		else
			pLane = pSeg->GetExitLane(pSeg->GetNode1());

		VehicleLaneExit* pExit = pLane->GetEndExit();
		if (pLane->GetExitCount() > 0)
		{
			pExit = pLane->GetFirstExit();
		}

		VehicleRouteNodePairDist nodePair(pLane->GetBeginEntry(),pExit,0);
		vNodePairs.push_back(nodePair);			
	}
}

void AirportResourceManager::GetStartPosition( const ALTObjectID& objID, std::vector<StartPositionInSim*>& vStartPos )
{
	size_t nSize = m_vStartPositions.size();
	for (size_t i =0; i < nSize; i++)
	{
		StartPositionInSim* pStartPos = m_vStartPositions.at(i);
		if (pStartPos->GetStartPosition()->GetObjectName().idFits(objID))
			vStartPos.push_back(pStartPos);
	}
}