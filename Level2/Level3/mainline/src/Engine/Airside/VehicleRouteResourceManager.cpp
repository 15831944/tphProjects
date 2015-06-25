#include "StdAfx.h"
#include ".\vehiclerouteresourcemanager.h"
#include "../../Common/DistanceLineLine.h"
#include "../../InputAirside/ALTAirport.h"
#include "StandResourceManager.h"
#include "VehiclePoolResourceManager.h"
#include "TaxiwayResourceManager.h"
#include "VehicleRouteInSim.h"
#include "PaxBusParkingResourceManager.h"
#include "DeiceResourceManager.h"
#include "./standleadinlineinSim.h"
#include "./standleadoutlineinSim.h"
#include "AirsideMeetingPointInSim.h"
#include "../../InputAirside/MeetingPoint.h"
#include "AirsidePaxBusParkSpotInSim.h"
#include "../BagCartsParkingSpotResourceManager.h"

const static DistanceUnit DistFromLaneToStand = 100*100; //300m 

//dij_VechileRoute::dij_VechileRoute()
//{
//	m_dist = ARCMath::DISTANCE_INFINITE;
//	m_bIsDoneSearch = false;
//}

bool VehicleRouteResourceManager::Init( int nPrjID, int nAirportID,IntersectionNodeInSimList& NodeList )
{
	m_nAptID = nAirportID;
	m_nPrjId = nPrjID;
	//init RoadIntersections
	for(int i=0;i < NodeList.GetNodeCount();i++)
	{
		IntersectionNodeInSim* pNode = NodeList.GetNodeByIndex(i);
		if( pNode->GetNodeInput().GetStretchIntersectItem().size() )
		{
			VehicleRoadIntersectionInSim * pNewRoadIntersection = new VehicleRoadIntersectionInSim(pNode->GetNodeInput());
			m_vIntersections.push_back( pNewRoadIntersection );
		}
	}	
	
	IntersectedStretchInAirport vIntersectStretchInAirport;
	vIntersectStretchInAirport.ReadData(nAirportID, NodeList.GetInputNodes());
	for(int iNodeIdx = 0; iNodeIdx < (int) m_vIntersections.size();iNodeIdx++)
	{
		VehicleRoadIntersectionInSim* pRoadIntersection = m_vIntersections.at(iNodeIdx);
		pRoadIntersection->InitIntersectStretch(vIntersectStretchInAirport);
	}

	// init user define intersections
	std::vector<int> vIntersectionIDs;
	ALTAirport::GetRoadIntersectionsIDs(nAirportID,vIntersectionIDs);
	for(int i=0;i<(int)vIntersectionIDs.size();i++)
	{
		Intersections* pintersection  = new Intersections;
		pintersection->ReadObject(vIntersectionIDs.at(i));
		VehicleRoadIntersectionInSim * pNewRoadIntersection = new VehicleRoadIntersectionInSim(pintersection);
		m_vIntersections.push_back( pNewRoadIntersection );
	}


	//
	std::vector<int> vStretchIDs;
	ALTAirport::GetStretchsIDs(nAirportID,vStretchIDs);
	for(int i=0;i<(int)vStretchIDs.size();i++)
	{
		Stretch* pStretch = new Stretch;
		pStretch->ReadObject(vStretchIDs.at(i));
		VehicleStretchInSim* pNewStretchInSim  = new VehicleStretchInSim(pStretch);
		pNewStretchInSim->InitSegments(m_vIntersections);
		m_vStretches.push_back( pNewStretchInSim );
	}

	return true;
	
}
const static DistanceUnit sdEntryExitOffset = 400; 

bool VehicleRouteResourceManager::InitRelations(StandResourceManager& standRes,
												DeiceResourceManager& deiceRes, 
												VehiclePoolResourceManager& poolRes,
												TaxiwayResourceManager& taxiwayRes,
												CPaxBusParkingResourceManager& paxBusRes,
												CBagCartsParkingSpotResourceManager& pBagCartsParkingSpotResManager)
{

	//long lFindStartTime = GetTickCount();
	//get entry & exit with stand
	InitRelationsWithStand(standRes);

	//get entry & exit with Vehicle Pool
	InitRelationsWithVehiclePool(poolRes);

	// pax parking place
	InitRelationsWithPaxBusParking(paxBusRes);	

	//init relation with deice pad
	InitRelationsWithDeice(deiceRes);

	//init intersections with taxiway
	InitIntersectionWithTaxiways(taxiwayRes);
	InitTaxiwayLanes(taxiwayRes);

	InitRelationsWithBagCartsParkingSpot(pBagCartsParkingSpotResManager);

	InitDIJShortesRoute();	

	//	InitUserDefinedRoutes
	m_userDefineRoutes.Init(m_nPrjId, this);

	//long lFindEndTime = GetTickCount();
	//long totalMoveTime = 0L;
	//totalMoveTime += lFindEndTime - lFindStartTime;
	//std::ofstream echoFile ("c:\\findveh.log", stdios::app);
	//echoFile<<"VehicleRouteResourceManager::InitRelations"<<lFindEndTime - lFindStartTime<<"          "
	//	<<totalMoveTime
	//	<<"\r\n";
	//echoFile.close();

	return true;
}

bool VehicleRouteResourceManager::InitRelationsWithPaxBusParking(CPaxBusParkingResourceManager& paxBusRes)
{
	for(size_t i=0;i< paxBusRes.GetPaxBusParkingCount();i++)
	{
		AirsidePaxBusParkSpotInSim * pBusParking = paxBusRes.GetPaxBusParkingByIndex(i);
		CString strName = pBusParking->GetParkSpot()->getName().GetIDString();
		DistanceUnit MinLeadInDist = ARCMath::DISTANCE_INFINITE;	
		DistanceUnit MinLeadOutDist = ARCMath::DISTANCE_INFINITE;	

		VehicleLaneInSim* pNearestLeadInLane = NULL;
		VehicleLaneInSim* pNearestLeadoutLane = NULL;

		for(int stretchidx = 0;stretchidx < (int)m_vStretches.size(); stretchidx++ )
		{
			VehicleStretchInSim * pStretch = m_vStretches.at(stretchidx);
			CString strStretch = pStretch->GetInput()->getName().GetIDString();
			for(int segidx = 0; segidx < pStretch->GetSegmentCount(); segidx++ )
			{	
				VehicleStretchSegmentInSim* pSeg = pStretch->GetSegment(segidx);

				for(int laneidx = 0; laneidx < pSeg->GetLaneCount(); laneidx++ )
				{
					VehicleLaneInSim * pLane = pSeg->GetLane(laneidx);

					//lead in line
					{
						CPath2008 leadinPath = pBusParking->GetParkSpot()->getLeadInLine();
						IntersectPathPath2D intersectPath;
						if(intersectPath.Intersects(leadinPath,pLane->GetPath(),5))//path and path intersect
						{
							DistanceUnit distInLane = pLane->GetPath().GetIndexDist( (float)intersectPath.m_vIntersectPtIndexInPath2[0] );
							CPoint2008 ptIntersect = pLane->GetPath().GetDistPoint(distInLane);
							pLane->AddExit( new VehicleLaneExit(pLane, pBusParking, max(0,distInLane) ) );
						}
						else
						{
							//does not intersection with
							int nCount = leadinPath.getCount();
							if (nCount > 0)
							{
								CPoint2008 ptPaxBusParking = leadinPath.getPoint(0);
								DistancePointPath3D distPtPath;
								DistanceUnit squareDist = distPtPath.GetSquared(ptPaxBusParking, pLane->GetPath());
								if(squareDist < MinLeadInDist )
								{
									MinLeadInDist = squareDist;
									pNearestLeadInLane = pLane;
								}
							}
						}
					}

					//lead out line
					{
						DistanceUnit dEndDist = pLane->GetLength();
						CPath2008 leadOutPath = pBusParking->GetParkSpot()->getLeadOutLine();
						IntersectPathPath2D intersectPath;
						if (intersectPath.Intersects(leadOutPath,pLane->GetPath(),5))//path and path intersect
						{
							DistanceUnit distInLane = pLane->GetPath().GetIndexDist( (float)intersectPath.m_vIntersectPtIndexInPath2[0] );
							pLane->AddEntry( new VehicleLaneEntry(pLane, pBusParking, min(dEndDist,distInLane) ) );
						}
						else
						{
							//does not intersection with
							int nCount = leadOutPath.getCount();
							if (nCount > 0)
							{	
								CPoint2008 ptPaxBusParking = leadOutPath.getPoint(nCount - 1);
								DistancePointPath3D distPtPath;
								DistanceUnit squareDist = distPtPath.GetSquared(ptPaxBusParking, pLane->GetPath());
								if(squareDist < MinLeadOutDist )
								{
									MinLeadOutDist = squareDist;
									pNearestLeadoutLane = pLane;
								}
							}
						}
					}
				}
			}
		}

		if (pNearestLeadInLane)
		{
			VehicleStretchSegmentInSim* pParentSeg = (VehicleStretchSegmentInSim*)pNearestLeadInLane->getRouteResource();
			InitRelationParkingLeadinLine(pParentSeg,pBusParking);
		}

		if (pNearestLeadoutLane)
		{
			VehicleStretchSegmentInSim* pParentSeg = (VehicleStretchSegmentInSim*)pNearestLeadoutLane->getRouteResource();
			InitRelationParkingLeadoutLine(pParentSeg,pBusParking);
		}
	}
	return true;
}

//doesn't intersection with any lane segment
void VehicleRouteResourceManager::InitRelationParkingLeadinLine( VehicleStretchSegmentInSim* pSegment,AirsidePaxBusParkSpotInSim * pBusParking )
{
	int nLaneCount = pSegment->GetLaneCount();

	for (int nLane = 0; nLane < nLaneCount; ++nLane)
	{
		VehicleLaneInSim * pCurLane = pSegment->GetLane(nLane);
		//lead in
		CPath2008 leadinPath = pBusParking->GetParkSpot()->getLeadInLine();
		{
			if(leadinPath.getCount())
			{
				CPoint2008 ptLeadin = leadinPath.getPoint(0);
				DistancePointPath3D distPtPath;
				DistanceUnit squareDist = distPtPath.GetSquared(ptLeadin, pCurLane->GetPath() );

				DistanceUnit distInLane = pCurLane->GetPath().GetIndexDist( (float)distPtPath.m_fPathParameter );
				pCurLane->AddExit( new VehicleLaneExit(pCurLane, pBusParking, max(0,distInLane) ) );
			}
		}
		
	}
}

void VehicleRouteResourceManager::InitRelationParkingLeadoutLine( VehicleStretchSegmentInSim* pSegment,AirsidePaxBusParkSpotInSim * pBusParking )
{
	int nLaneCount = pSegment->GetLaneCount();

	for (int nLane = 0; nLane < nLaneCount; ++nLane)
	{
		VehicleLaneInSim * pCurLane = pSegment->GetLane(nLane);
		DistanceUnit dEndDist = pCurLane->GetLength();

		//lead out
		CPath2008 leadOutPath = pBusParking->GetParkSpot()->getLeadOutLine();
		{
			int nCount = leadOutPath.getCount();
			if (nCount > 0)
			{	
				CPoint2008 ptLeadout = leadOutPath.getPoint(nCount - 1);
				DistancePointPath3D distPtPath;
				DistanceUnit squareDist = distPtPath.GetSquared(ptLeadout, pCurLane->GetPath());

				DistanceUnit distInLane = pCurLane->GetPath().GetIndexDist( (float)distPtPath.m_fPathParameter );
				pCurLane->AddEntry( new VehicleLaneEntry(pCurLane, pBusParking, min(dEndDist,distInLane) ) );
			}
		}
	}

}

//
void VehicleRouteResourceManager::InitDIJShortesRoute()
{
	m_pathFinder.Init(m_vStretches, m_vIntersections);

	//for(int stretchidx = 0;stretchidx < (int)m_vStretches.size(); stretchidx++ )
	//{
	//	VehicleStretchInSim * pStretch = m_vStretches.at(stretchidx);
	//	for(int segidx = 0; segidx < pStretch->GetSegmentCount(); segidx++ )
	//	{
	//		VehicleStretchSegmentInSim* pSeg = pStretch->GetSegment(segidx);
	//		pSeg->InitGraph();
	//		for( int i=0;i< pSeg->GetEdgeCount(); i++)
	//		{
	//			VehicleRouteNodePairDist entryExit = pSeg->GetEdge(i);
	//			dij_VechileRoute& theRoute = m_ShortestRoutes[VehicleRouteNodePair(entryExit.GetNodeFrom(), entryExit.GetNodeTo())];
	//			theRoute.m_route.push_back( entryExit );
	//			theRoute.m_dist = entryExit.GetDistance();	
	//			m_vRouteNodeSet.insert(entryExit.GetNodeFrom());m_vRouteNodeSet.insert(entryExit.GetNodeTo());
	//		}
	//	}
	//}

	//for( int nodeidx = 0; nodeidx <(int)m_vIntersections.size();nodeidx++)
	//{
	//	VehicleRoadIntersectionInSim* pRoadIntersection = m_vIntersections.at(nodeidx);
	//	pRoadIntersection->InitGraph();
	//	for( int i=0;i< pRoadIntersection->GetEdgeCount();i++)
	//	{
	//		VehicleRouteNodePairDist exitEntry = pRoadIntersection->GetEdge(i);
	//		dij_VechileRoute& theRoute = m_ShortestRoutes[ VehicleRouteNodePair(exitEntry.GetNodeFrom(),exitEntry.GetNodeTo()) ];
	//		theRoute.m_dist = exitEntry.GetDistance();
	//		theRoute.m_route.push_back( exitEntry );
	//		m_vRouteNodeSet.insert(exitEntry.GetNodeFrom());m_vRouteNodeSet.insert(exitEntry.GetNodeTo());
	//	}
	//}
}

//#include "../../Common/PreciseTimer.h"
//TextFileLog vehicleDebugLog("C:\\vehiclePathlog.txt");



//get the shortest path form res F to 
bool VehicleRouteResourceManager::GetShortestPath( AirsideResource * pResFrom, AirsideResource* pResTo,int nPRI, VehicleRouteInSim& resltRoute )
{
		
	//find path
	std::vector<VehicleRouteNodePairDist> RouteNodeList;
	if( m_pathFinder.GetRoute(pResFrom, pResTo,RouteNodeList ) )
	{
		resltRoute = RouteNodeList;
		return true;
	}

	return false;
	//CString strFunc;
	//CString strDebug;
	//strDebug.Format("From %s to %s", pResFrom->PrintResource().GetString(), pResTo->PrintResource().GetString() );
	//strFunc.Format("find route %s nodes %d, %d", strDebug.GetString(), vFromNodes.size(), vToNodes.size());

	//vehicleDebugLog.FuncBegin(strFunc);
	//find the nearest node pair
	//std::vector<VehicleRouteNode*> vFromNodes;
	//std::vector<VehicleRouteNode*> vToNodes;
	//
	//for(VehicleRouteNodeSet::iterator nodeItr = m_vRouteNodeSet.begin(); nodeItr!= m_vRouteNodeSet.end();nodeItr++)
	//{	
	//	VehicleRouteNode* pNode = *nodeItr;

	//	if(pNode == NULL || pNode->GetOrignResource() == NULL || pNode->GetDestResource() == NULL)
	//		continue;

	//	if( pNode->GetOrignResource() == pResFrom )
	//	{
	//		if(pResFrom->GetType() == AirsideResource::ResType_VehicleLane )
	//		{
	//			if( pNode->GetDestResource() == pResTo 
	//				|| pNode->GetDestResource()->GetType() == AirsideResource::ResType_VehicleRoadIntersection )
	//			{
	//				if(vFromNodes.size()==0)
	//					vFromNodes.push_back(pNode);
	//			}
	//		}
	//		else
	//			vFromNodes.push_back(pNode);
	//	}
	//	if( pNode->GetDestResource() == pResTo )
	//	{
	//		vToNodes.push_back(pNode);
	//	}
	//}
	//
	//DistanceUnit minDist = ARCMath::DISTANCE_INFINITE;
	//VehicleRouteNodePair nearestNodePair(NULL,NULL);
	//for(int entryIdx = 0;entryIdx<(int)vFromNodes.size();entryIdx++)
	//{
	//	VehicleRouteNode* pEntry = vFromNodes.at(entryIdx);
	//	for(int exitIdx =0;exitIdx<(int)vToNodes.size();exitIdx++)
	//	{
	//		VehicleRouteNode* pExit = vToNodes.at(exitIdx);
	//		dij_VechileRoute& nodepairRoute = m_ShortestRoutes[VehicleRouteNodePair(pEntry,pExit)];
	//		if(!nodepairRoute.m_bIsDoneSearch)
	//		{
	//			findShortestPath(pEntry,pExit);
	//		}
	//		
	//		if(nodepairRoute.m_bIsDoneSearch && nodepairRoute.m_dist < minDist)
	//		{
	//			nearestNodePair = VehicleRouteNodePair(pEntry,pExit);
	//			minDist = nodepairRoute.m_dist;		
	//			break;
	//		}
	//	}
	//	if(minDist != ARCMath::DISTANCE_INFINITE)
	//		break;
	//}
	////find the first 
	///*VehicleRouteNodePair nearestNodePair(NULL,NULL);
	//if( vFromNodes.size() && vToNodes.size())
	//{
	//	nearestNodePair = VehicleRouteNodePair(*vFromNodes.begin(), *vToNodes.begin());
	//	dij_VechileRoute& theDijRoute = m_ShortestRoutes[nearestNodePair];
	//	if(!theDijRoute.m_bIsDoneSearch)
	//	{
	//		findShortestPath(nearestNodePair.first, nearestNodePair.second);
	//	}
	//}*/

	////vehicleDebugLog.FuncEnd();
	//
	//dij_VechileRoute& theDijRoute = m_ShortestRoutes[nearestNodePair];
	//if(theDijRoute.m_bIsDoneSearch && theDijRoute.m_dist < ARCMath::DISTANCE_INFINITE)
	//{
	//	resltRoute = VehicleRouteInSim(theDijRoute.m_route);
	//	return true;
	//}
	//else 
	//	return false;
}


//void VehicleRouteResourceManager::findShortestPath( VehicleRouteNode* pEntry, VehicleRouteNode* pExit )
//{
//	if(!(pEntry && pExit))
//		return;	
//
//	const int nNodeCount = m_vRouteNodeSet.size();
//
//	for(int i=0;i<nNodeCount;i++)
//	{
//				
//		//find shortest node form src to other node
//		VehicleRouteNode * pShortestNode  = NULL;
//		DistanceUnit dShortDist = ARCMath::DISTANCE_INFINITE;
//		for(VehicleRouteNodeSet::iterator nodeItr= m_vRouteNodeSet.begin(); nodeItr!= m_vRouteNodeSet.end();nodeItr++)
//		{
//			VehicleRouteNode * pOtherNode = *nodeItr;			
//			if(pOtherNode == pEntry ) continue;
//			
//			VehicleRouteNodePair nodePair(pEntry, pOtherNode);
//			dij_VechileRoute& routNodePair = m_ShortestRoutes[nodePair];
//
//			if( !routNodePair.m_bIsDoneSearch )
//			{
//				if(routNodePair.m_dist  < dShortDist )
//				{
//					dShortDist = routNodePair.m_dist;
//					pShortestNode = pOtherNode;
//				}
//			}
//		}
//
//		//update 
//		if(pShortestNode )
//		{
//			//VehicleRouteNodePair nodePair(pEntry, pShortestNode);
//			dij_VechileRoute& routeSrcShort = m_ShortestRoutes[VehicleRouteNodePair(pEntry,pShortestNode)];
//			
//			routeSrcShort.m_bIsDoneSearch = true;
//			if(pShortestNode == pExit) break;
//			
//			for(VehicleRouteNodeSet::iterator nodeItr= m_vRouteNodeSet.begin(); nodeItr!= m_vRouteNodeSet.end();nodeItr++)
//			{
//				VehicleRouteNode * pNodeI = *nodeItr;
//
//				if(pNodeI == pEntry) continue;
//				if(pNodeI == pShortestNode) continue;	
//
//
//				dij_VechileRoute& routeSrcI = m_ShortestRoutes[VehicleRouteNodePair(pEntry, pNodeI)];
//				dij_VechileRoute& routeShortI = m_ShortestRoutes[VehicleRouteNodePair(pShortestNode,pNodeI)];
//
//				if(!routeSrcI.m_bIsDoneSearch)
//				{
//					if(dShortDist + routeShortI.m_dist < routeSrcI.m_dist )
//					{
//						routeSrcI.m_dist = dShortDist + routeShortI.m_dist;				
//						//merge the route
//						routeSrcI.m_route.clear();
//						routeSrcI.m_route.insert(routeSrcI.m_route.end(),routeSrcShort.m_route.begin(),routeSrcShort.m_route.end() );
//						routeSrcI.m_route.insert(routeSrcI.m_route.end(),routeShortI.m_route.begin(), routeShortI.m_route.end() );
//					}
//				}
//
//			}
//				
//		}	
//
//	}
//}
//
//
VehicleStretchInSim* VehicleRouteResourceManager::GetStretchByID(int nID)
{
	for (int i =0; i <(int)m_vStretches.size(); i++ )
	{
		if (m_vStretches.at(i)->GetInput()->getID() == nID )
			return m_vStretches.at(i);
	}
	return NULL;
}

bool VehicleRouteResourceManager::GetVehicleRoute(AirsideResource * pResAt, AirsideResource* pResTo,VehicleRouteInSim& resltRoute )
{

	if(pResAt == pResTo)
		return true;	

	long lFindStartTime = GetTickCount();

	//std::ofstream echoFile ("c:\\findveh.log", stdios::app);
	//echoFile<<"VehicleRouteResourceManager::GetVehicleRoute- start      "<<lFindStartTime<<"    pResAt      "<<pResAt->PrintResource()<< "   pResTo     "<<pResTo->PrintResource()<<"\r\n";
	//echoFile.close();
	//
	if(pResAt && pResAt->GetType() == AirsideResource::ResType_StandLeadInLine)
	{
		StandLeadInLineInSim* pLeadInLine = (StandLeadInLineInSim*)pResAt;
		pResAt = pLeadInLine->GetStandInSim();
	}
	if(pResAt && pResAt->GetType() == AirsideResource::ResType_StandLeadOutLine)
	{
		StandLeadOutLineInSim* pLeadOutLine = (StandLeadOutLineInSim*)pResAt;
		pResAt = pLeadOutLine->GetStandInSim();
	}

	//
	bool bResult =GetShortestPathWithUserDefineRoutes(pResAt, pResTo, resltRoute);
	if(bResult == false)
		bResult = GetShortestPath(pResAt, pResTo, 0, resltRoute);

	//long lFindEndTime = GetTickCount();	
	// static long totalFindTime = 0L;
	// totalFindTime += lFindEndTime - lFindStartTime;

	//std::ofstream echoFile1 ("c:\\findveh.log", stdios::app);
	// echoFile1<<"VehicleRouteResourceManager::GetVehicleRoute-end      "<<lFindEndTime - lFindStartTime<<"          "
	//	 <<totalFindTime
	//	 <<"\r\n";
	// echoFile1.close();


	return bResult;
}


VehicleRouteResourceManager::~VehicleRouteResourceManager()
{
	for(int i=0; i<(int)m_vStretches.size();i++)
		delete m_vStretches.at(i);

	for(int i=0;i<(int)m_vIntersections.size();i++)
	{
		delete m_vIntersections.at(i);
	}
}


bool VehicleRouteResourceManager::InitRelationsWithVehiclePool( VehiclePoolResourceManager& poolRes )
{
	for(int i =0; i< (int) poolRes.GetVehiclePoolCount();i++)
	{
		
		VehiclePoolInSim * pPool = poolRes.GetVehiclePoolByIdx(i);
		CPoint2008 poolPos = pPool->GetCenterPoint();

		DistanceUnit MinDist = ARCMath::DISTANCE_INFINITE;

		VehicleStretchSegmentInSim * pNearestSeg = NULL;
		CPoint2008 nearestPtInSeg;		
		//bool bOverlap = false;

		bool bHasIntersectWithStretch = false;
		for(int stretchidx = 0;stretchidx < (int)m_vStretches.size(); stretchidx++ )
		{
			
			VehicleStretchInSim * pStretch = m_vStretches.at(stretchidx);
			for(int segidx = 0; segidx < pStretch->GetSegmentCount(); segidx++ )
			{
				//new intersect with drive pipe
				VehicleStretchSegmentInSim* pSeg = pStretch->GetSegment(segidx);
				int nPipeCount = pPool->GetPipeCount();
				for(int pipeIdx =0;pipeIdx<nPipeCount;pipeIdx++)
				{
					VehiclePoolPipeInSim* pipe  = pPool->GetPipe(pipeIdx);
					for(int pipeLaneidx = 0; pipeLaneidx<pipe->getLaneCount();++pipeLaneidx)
					{
						VehiclePoolLaneInSim* plane = pipe->getLane(pipeLaneidx);
						CPoint2008 beginPt;
						if( plane->m_path.GetBeginPoint(beginPt))
						{
							DistancePointPath3D distancePP;
							if( sqrt(distancePP.GetSquared(beginPt, pSeg->getPath())) < pSeg->getWidth() )
							{
								//add pool entry
								for(int stretchlaneidx = 0;stretchlaneidx<pSeg->GetLaneCount();stretchlaneidx++)
								{
									VehicleLaneInSim* pStretchLane = pSeg->GetLane(stretchlaneidx);

									DistanceUnit distInSLane  = pStretchLane->GetPath().GetPointDist(distancePP.m_clostPoint);
									VehiclePoolEntry* poolentry = new VehiclePoolEntry(pStretchLane,pPool, distInSLane);
									poolentry->m_pPoolLane = plane;
									poolentry->m_distInPoolLane = plane->getPath().GetPointDist(distancePP.m_clostPoint) ;
									pStretchLane->AddExit(poolentry);
									pPool->AddEntry(poolentry);									
								}

							}
						}
						CPoint2008 endPt;
						if(plane->m_path.GetEndPoint(endPt))
						{
							DistancePointPath3D distancePP;
							if( sqrt(distancePP.GetSquared(endPt, pSeg->getPath())) < pSeg->getWidth() )
							{
								//add pool exit
								for(int stretchlaneidx = 0;stretchlaneidx<pSeg->GetLaneCount();stretchlaneidx++)
								{
									VehicleLaneInSim* pStretchLane = pSeg->GetLane(stretchlaneidx);

									DistanceUnit distInSLane  = pStretchLane->GetPath().GetPointDist(distancePP.m_clostPoint);
									VehiclePoolExit* poolentry = new VehiclePoolExit(pStretchLane,pPool, distInSLane);
									poolentry->m_pPoolLane = plane;
									poolentry->m_distInPoolLane =  plane->getPath().GetPointDist(distancePP.m_clostPoint) ;
									pStretchLane->AddEntry(poolentry);
									pPool->AddExit(poolentry);									
								}
							}
						}
					}
					
				}
				
				
				//old code
				/*for(int laneidx = 0; laneidx < pSeg->GetLaneCount(); laneidx++ )
				{
					VehicleLaneInSim * pLane = pSeg->GetLane(laneidx);

					IntersectPathPolygon2D intersectPathPoly;
					if( intersectPathPoly.Intersects(pLane->GetPath(), pPool->GetPoolInput()->GetPath()) )
					{
						DistanceUnit dEndDist = pLane->GetLength();
						for(int i=0;i<(int)intersectPathPoly.m_vpathParameters.size();i++)
						{
							DistanceUnit distInLane = pLane->GetPath().GetIndexDist( (float)intersectPathPoly.m_vpathParameters.at(i) );
							VehiclePoolExit* poolExit = new VehiclePoolExit(pLane, pPool, min(dEndDist,distInLane +sdEntryExitOffset) );
							VehiclePoolEntry* poolEntry =  new VehiclePoolEntry(pLane, pPool, max(0,distInLane -sdEntryExitOffset) );
							
							pLane->AddEntry( poolExit );
							pPool->AddExit(poolExit);
							
							pLane->AddExit( poolEntry );	
							pPool->AddEntry(poolEntry);							
						}
					}	
				}*/

				//get nearest seg
				{
					DistancePointPath3D distPtPath;
					DistanceUnit squareDist = distPtPath.GetSquared(poolPos, pSeg->getPath() );
					if(squareDist < MinDist )
					{
						MinDist = squareDist;
						nearestPtInSeg = distPtPath.m_clostPoint;
						pNearestSeg = pSeg;
					}
				}
			}
		}
		

		//if can not find any entry or exit, use the closet point
		if( !pPool->GetEntryCount() )
		{
			ASSERT(pNearestSeg);
			if(pNearestSeg)
			{
				int nLaneCount = pNearestSeg->GetLaneCount();

				for (int nLane = 0; nLane < nLaneCount; ++nLane)
				{
					VehicleLaneInSim * pCurLane = pNearestSeg->GetLane(nLane);
					DistanceUnit dEndDist = pCurLane->GetLength();
					DistanceUnit distInLane = pCurLane->GetPath().GetPointDist(nearestPtInSeg);
					VehiclePoolEntry* poolEntry =  new VehiclePoolEntry(pCurLane, pPool, max(0,distInLane -sdEntryExitOffset) );

					pCurLane->AddExit( poolEntry );	
					pPool->AddEntry(poolEntry);
				}
			}
			
		}
		if(!pPool->GetExitCount())
		{
			ASSERT(pNearestSeg);
			if(pNearestSeg)
			{
				int nLaneCount = pNearestSeg->GetLaneCount();

				for (int nLane = 0; nLane < nLaneCount; ++nLane)
				{
					VehicleLaneInSim * pCurLane = pNearestSeg->GetLane(nLane);
					DistanceUnit dEndDist = pCurLane->GetLength();
					DistanceUnit distInLane = pCurLane->GetPath().GetPointDist(nearestPtInSeg);
					VehiclePoolExit* poolExit = new VehiclePoolExit(pCurLane, pPool, min(dEndDist,distInLane +sdEntryExitOffset) );

					pCurLane->AddEntry( poolExit );
					pPool->AddExit(poolExit);
				}
			}

		}
		pPool->Build();	
	}
	return true;
}

bool VehicleRouteResourceManager::InitRelationsWithStand( StandResourceManager& standRes )
{
	for(int i=0;i< (int) standRes.GetStandCount();i++)
	{
		StandInSim * pStand = standRes.GetStandByIdx(i);
		CPoint2008 standPt = pStand->GetDistancePoint(0);
		DistanceUnit MinDist = ARCMath::DISTANCE_INFINITE;
		DistanceUnit distInLane = 0;
		VehicleLaneInSim * pNearestlane = NULL;

		for(int stretchidx = 0;stretchidx < (int)m_vStretches.size(); stretchidx++ )
		{
			VehicleStretchInSim * pStretch = m_vStretches.at(stretchidx);
			for(int segidx = 0; segidx < pStretch->GetSegmentCount(); segidx++ )
			{
				VehicleStretchSegmentInSim* pSeg = pStretch->GetSegment(segidx);
				for(int laneidx = 0; laneidx < pSeg->GetLaneCount(); laneidx++ )
				{
					VehicleLaneInSim * pLane = pSeg->GetLane(laneidx);
					DistancePointPath3D distPtPath;
					DistanceUnit squareDist = distPtPath.GetSquared(standPt, pLane->GetPath());
					if(squareDist < MinDist )
					{
						MinDist = squareDist;
						distInLane = pLane->GetPath().GetIndexDist( distPtPath.m_fPathParameter );
						pNearestlane = pLane;
					}
				}
			}
		}

		if(pNearestlane)
		{
			VehicleStretchSegmentInSim* pParentSeg = (VehicleStretchSegmentInSim*)pNearestlane->getRouteResource();
			int nLaneCount = pParentSeg->GetLaneCount();

			for (int nLane = 0; nLane < nLaneCount; ++nLane)
			{
				VehicleLaneInSim * pCurLane = pParentSeg->GetLane(nLane);

				DistanceUnit dEndDist = pCurLane->GetLength();

				DistancePointPath3D distPtPath;
				DistanceUnit squareDist = distPtPath.GetSquared(standPt, pCurLane->GetPath());

				DistanceUnit distInLane = pCurLane->GetPath().GetIndexDist( distPtPath.m_fPathParameter );

				VehicleLaneEntry* pEntry = new VehicleLaneEntry(pCurLane, pStand, min(dEndDist,distInLane +sdEntryExitOffset) );
				VehicleLaneExit* pExit = new VehicleLaneExit(pCurLane, pStand, max(0,distInLane -sdEntryExitOffset) );

				pCurLane->AddEntry( pEntry );
				pCurLane->AddExit( pExit );

				pStand->AddOutLane(pEntry);
			}		
		}

	}
	return true;
}

VehicleRoadIntersectionInSim * VehicleRouteResourceManager::GetRelatedIntersectionWithLane( VehicleLaneInSim* pExitLane,VehicleLaneInSim *pEntryLane )
{
	VehicleRoadIntersectionInSim *pReturnIntersection = NULL;
	std::vector<VehicleRoadIntersectionInSim*> ::iterator iter = m_vIntersections.begin();
	for (; iter != m_vIntersections.end(); ++iter)
	{
		if ((*iter)->IsRelateWithLanes(pExitLane,pEntryLane))
		{
			pReturnIntersection = *iter;
			break;
		}
	}
		

	return pReturnIntersection;
}

bool VehicleRouteResourceManager::InitIntersectionWithTaxiways( TaxiwayResourceManager& taxiwayRes )
{
	//get intersection with taxiways
	for(int stretchidx = 0;stretchidx < (int)m_vStretches.size(); stretchidx++ )
	{
		VehicleStretchInSim * pStretch = m_vStretches.at(stretchidx);
		for(int segidx = 0; segidx < pStretch->GetSegmentCount(); segidx++ )
		{
			VehicleStretchSegmentInSim* pSeg = pStretch->GetSegment(segidx);
			for(int laneidx = 0; laneidx < pSeg->GetLaneCount(); laneidx++ )
			{
				VehicleLaneInSim * pLane = pSeg->GetLane(laneidx);
				for(int k=0;k < taxiwayRes.GetTaxiwayCount();k++)
				{
					TaxiwayInSim * pTaxiway = taxiwayRes.GetTaxiwayByIdx(k);
					DistanceUnit taxiWidth = pTaxiway->GetTaxiwayInput()->GetWidth();
					for(int n = 0 ; n < pTaxiway->GetSegmentCount(); n++)
					{
						TaxiwaySegInSim * pSeg = pTaxiway->GetSegment(n);
						CPath2008 segPath = pSeg->GetPositiveDirSeg()->GetPath();

						//get intersection info
						HoldPositionInfoList holdinfos;
						GetHoldPositionsInfo(pLane->GetPath(),1,segPath,taxiWidth+20*SCALE_FACTOR,holdinfos,false);
						for(int m = 0; m< (int)holdinfos.size();m++)
						{
							HoldPositionInfo holdinfo = holdinfos.at(m);
							CPoint2008 pt =  pLane->GetPath().GetDistPoint(holdinfo.first);
							if(pt.getZ()!= 0)
								continue;

							LaneFltGroundRouteIntersectionInSim* intectItem = pLane->GetFltGroundRouteIntersection(pSeg,holdinfo.intersectidx);
							pSeg->addIntersectionLane(pLane);
							intectItem->SetIntersectDistAtLane(holdinfo.first);
							if(holdinfo.IsFirst())
							{
								intectItem->SetHold1( holdinfo.second );
							}
							else
							{
								intectItem->SetHold2( holdinfo.second );
							}
							
							if (intectItem->MakeValid())		//valid intersection, view it as a lane entry pos
							{
								VehicleLaneEntry * pNewEntry = new VehicleLaneEntry(pLane,intectItem, intectItem->GetIntersectDistAtLane()) ;
								pLane->AddEntry(pNewEntry);
							}
						}
					}
				}

				pLane->SortFltGroundRouteIntersections();			
			}
		}
	}
	return true;
}

bool VehicleRouteResourceManager::GetShortestPathWithUserDefineRoutes( AirsideResource * pResFrom, AirsideResource* pResTo, VehicleRouteInSim& resltRoute )
{
	//VehicleRouteNodeSet vFromNodes;
	//VehicleRouteNodeSet vToNodes;

	//for(VehicleRouteNodeSet::iterator nodeItr = m_vRouteNodeSet.begin();  nodeItr!= m_vRouteNodeSet.end();nodeItr++)
	//{
	//	VehicleRouteNode* pNode = *nodeItr;
	//	if( pNode->GetOrignResource() == pResFrom )
	//	{
	//		vFromNodes.insert(pNode);
	//	}
	//	if( pNode->GetDestResource() == pResTo )
	//	{
	//		vToNodes.insert(pNode);
	//	}
	//}
	std::map<std::pair<AirsideResource *, AirsideResource*>, VehicleRouteInSim >::iterator iterMap = m_mapResourceRoute.find(std::make_pair(pResFrom,pResTo));
	if( iterMap!= m_mapResourceRoute.end())
	{
		resltRoute = (*iterMap).second;
		return true;
	}

	for(int i=0;i<m_userDefineRoutes.GetCount();++i)
	{
		VehicleRouteUserDefineInSim* pUserRoute = m_userDefineRoutes.GetItem(i);
		//std::set<VehicleLaneEntry*> vRouteEntries = pUserRoute->GetEntries();
		//std::set<VehicleLaneExit*> vRouteExits = pUserRoute->GetExits();

		std::vector<VehicleRouteNode* > vSameNodesF;
		//vSameNodesF.resize(vFromNodes.size());
		//set_intersection(vFromNodes.begin(), vFromNodes.end(), vRouteEntries.begin(), vRouteEntries.end(), vSameNodesF.begin());
		//
		std::vector<VehicleRouteNode*> vSameNodesT;
		//vSameNodesT.resize(vToNodes.size());
		//set_intersection(vRouteExits.begin(), vRouteExits.end(), vToNodes.begin(), vToNodes.end() , vSameNodesT.begin() );

		pUserRoute->GetEntryExitNode(pResFrom,pResTo,vSameNodesF,vSameNodesT);
		
		if(vSameNodesF.size() ==0 || vSameNodesT.size() == 0)
			continue;

		int nBestFrom = -1;
		int nBestTo = -1;
		VehicleRouteInSim shortestRoute;
		double dShortestDistance = (std::numeric_limits<double>::max)();
		for (int nFrom = 0; nFrom < (int)vSameNodesF.size(); ++ nFrom)
		{
			for (int nTo = 0; nTo < (int)vSameNodesT.size(); ++ nTo)
			{
				std::vector<VehicleRouteNodePairDist> resltSegs;

				if(pUserRoute->GetShortestPath( vSameNodesF.at(nFrom), vSameNodesT.at(nTo), resltSegs ))
				{
					VehicleRouteInSim nodeRoute = VehicleRouteInSim(resltSegs);
					double dDistance = nodeRoute.GetLength();
					if (dShortestDistance > dDistance)
					{
						nBestFrom = nFrom;
						nBestTo = nTo;
						shortestRoute = nodeRoute;
						dShortestDistance = dDistance;
					}
					
				}
			}
		}

		if (nBestFrom != -1 && nBestTo != -1)
		{
			resltRoute = shortestRoute;
			m_mapResourceRoute[std::make_pair(pResFrom,pResTo)] = shortestRoute;
			return true;
		}
	}

	
	return false;
}

bool VehicleRouteResourceManager::GetVehicleDefineRoute( AirsideResource * pResFrom, AirsideResource* pResTo, VehicleRouteInSim& resltRoute )
{
	//get the resource
	
	return m_userDefineRoutes.GetVehicleDefineRoute(pResFrom,pResTo,resltRoute);
}


bool VehicleRouteResourceManager::InitRelationsWithDeice( DeiceResourceManager& deiceRes )
{
	for(int i=0;i< (int) deiceRes.GetCount();i++)
	{
		DeicePadInSim * pDeice = deiceRes.GetPadByIndex(i);
		CPoint2008 standPt = pDeice->GetInput()->GetServicePoint();
		DistanceUnit MinDist = ARCMath::DISTANCE_INFINITE;
		DistanceUnit distInLane = 0;
		VehicleLaneInSim * pNearestlane = NULL;

		for(int stretchidx = 0;stretchidx < (int)m_vStretches.size(); stretchidx++ )
		{
			VehicleStretchInSim * pStretch = m_vStretches.at(stretchidx);
			for(int segidx = 0; segidx < pStretch->GetSegmentCount(); segidx++ )
			{
				VehicleStretchSegmentInSim* pSeg = pStretch->GetSegment(segidx);
				for(int laneidx = 0; laneidx < pSeg->GetLaneCount(); laneidx++ )
				{
					VehicleLaneInSim * pLane = pSeg->GetLane(laneidx);
					DistancePointPath3D distPtPath;
					DistanceUnit squareDist = distPtPath.GetSquared(standPt, pLane->GetPath() );
					if(squareDist < MinDist )
					{
						MinDist = squareDist;
						distInLane = pLane->GetPath().GetIndexDist( distPtPath.m_fPathParameter );
						pNearestlane = pLane;
					}
				}
			}
		}

		if(pNearestlane)
		{
			VehicleStretchSegmentInSim* pParentSeg = (VehicleStretchSegmentInSim*)pNearestlane->getRouteResource();
			int nLaneCount = pParentSeg->GetLaneCount();

			for (int nLane = 0; nLane < nLaneCount; ++nLane)
			{
				VehicleLaneInSim * pCurLane = pParentSeg->GetLane(nLane);

				DistanceUnit dEndDist = pCurLane->GetLength();

				DistancePointPath3D distPtPath;
				DistanceUnit squareDist = distPtPath.GetSquared(standPt, pCurLane->GetPath() );

				DistanceUnit distInLane = pCurLane->GetPath().GetIndexDist( distPtPath.m_fPathParameter );

				VehicleLaneEntry* pEntry = new VehicleLaneEntry(pCurLane, pDeice, min(dEndDist,distInLane +sdEntryExitOffset) );
				VehicleLaneExit* pExit = new VehicleLaneExit(pCurLane, pDeice, max(0,distInLane -sdEntryExitOffset) );

				pCurLane->AddEntry(pEntry);
				pCurLane->AddExit(pExit);

				pDeice->AddOutLane(pEntry);
			}		
		}

	}
	return true;
}

void VehicleRouteResourceManager::InitTaxiwayLanes( TaxiwayResourceManager& taxiRes )
{
	int nTaxiCount = taxiRes.GetTaxiwayCount();
	for(int i=0;i < nTaxiCount;i++)
	{
		TaxiwayInSim * pTaxiway = taxiRes.GetTaxiwayByIdx(i);
		for(int n = 0 ; n < pTaxiway->GetSegmentCount(); n++)
		{
			TaxiwaySegInSim * pSeg = pTaxiway->GetSegment(n);
			pSeg->InitLanes();
		}
	}
}

bool VehicleRouteResourceManager::InitRelationsWithMeetingPoint( const std::vector<AirsideMeetingPointInSim*>& vMeetingPoints )
{
	int nCount = (int)vMeetingPoints.size();
	for(int i=0;i< nCount;i++)
	{
		AirsideMeetingPointInSim* pMeetingPoint = vMeetingPoints.at(i);
		CPoint2008 posPt = pMeetingPoint->GetCenterPoint();
		DistanceUnit MinDist = ARCMath::DISTANCE_INFINITE;
		DistanceUnit distInLane = 0;
		VehicleLaneInSim * pNearestlane = NULL;
		
		CPath2008 arePath = *pMeetingPoint->GetMeetingPointInput()->GetPath();
		int nStretchCount = (int)m_vStretches.size();
		for(int stretchidx = 0;stretchidx < nStretchCount; stretchidx++ )
		{
			VehicleStretchInSim * pStretch = m_vStretches.at(stretchidx);
			for(int segidx = 0; segidx < pStretch->GetSegmentCount(); segidx++ )
			{
				VehicleStretchSegmentInSim* pSeg = pStretch->GetSegment(segidx);
				for(int laneidx = 0; laneidx < pSeg->GetLaneCount(); laneidx++ )
				{
					VehicleLaneInSim * pLane = pSeg->GetLane(laneidx);

					IntersectPathPolygon2D intersectPathPoly;
					if( intersectPathPoly.Intersects(pLane->GetPath(), arePath) )
					{
						DistanceUnit dEndDist = pLane->GetLength();
						for(int i=0;i<(int)intersectPathPoly.m_vpathParameters.size();i++)
						{
							distInLane = pLane->GetPath().GetIndexDist( intersectPathPoly.m_vpathParameters.at(i) );
							pLane->AddEntry( new VehicleLaneEntry(pLane, pMeetingPoint, min(dEndDist,distInLane +sdEntryExitOffset) ) );
							pLane->AddExit( new VehicleLaneExit(pLane, pMeetingPoint, max(0,distInLane -sdEntryExitOffset) ) );
						}
					}					
					else
					{
						DistancePointPath3D distPtPath;
						DistanceUnit squareDist = distPtPath.GetSquared(posPt, pLane->GetPath());
						if(squareDist < MinDist )
						{
							MinDist = squareDist;
							distInLane = pLane->GetPath().GetIndexDist( distPtPath.m_fPathParameter );
							pNearestlane = pLane;
						}
					}

				}
			}
		}
		if(pNearestlane)
		{
			VehicleStretchSegmentInSim* pParentSeg = (VehicleStretchSegmentInSim*)pNearestlane->getRouteResource();
			int nLaneCount = pParentSeg->GetLaneCount();

			for (int nLane = 0; nLane < nLaneCount; ++nLane)
			{
				VehicleLaneInSim * pCurLane = pParentSeg->GetLane(nLane);
				DistanceUnit dEndDist = pCurLane->GetLength();

				//if (pCurLane != pNearestlane )
				//{
				DistancePointPath3D distPtPath;
				DistanceUnit squareDist = distPtPath.GetSquared(posPt, pCurLane->GetPath());

				distInLane = pCurLane->GetPath().GetIndexDist( distPtPath.m_fPathParameter );
				//}

				pCurLane->AddEntry( new VehicleLaneEntry(pCurLane, pMeetingPoint, min(dEndDist,distInLane +sdEntryExitOffset) ) );
				pCurLane->AddExit( new VehicleLaneExit(pCurLane, pMeetingPoint, max(0,distInLane -sdEntryExitOffset) ) );
			}
		}
	}
	return true;
}

bool VehicleRouteResourceManager::InitRelationsWithBagCartsParkingSpot( CBagCartsParkingSpotResourceManager& pBagCartsParkingSpotResManager )
{
	for(size_t i=0;i< pBagCartsParkingSpotResManager.GetSpotCount();i++)
	{
		CBagCartsParkingSpotInSim * pCartSpot = pBagCartsParkingSpotResManager.GetSpotByIndex(i);
		CPoint2008 ptCartsParking = pCartSpot->GetDistancePoint(0);
		DistanceUnit MinDist = ARCMath::DISTANCE_INFINITE;
		//DistanceUnit distInLane = 0;
		VehicleLaneInSim * pNearestlane = NULL;	

		for(int stretchidx = 0;stretchidx < (int)m_vStretches.size(); stretchidx++ )
		{
			VehicleStretchInSim * pStretch = m_vStretches.at(stretchidx);
			for(int segidx = 0; segidx < pStretch->GetSegmentCount(); segidx++ )
			{	
				VehicleStretchSegmentInSim* pSeg = pStretch->GetSegment(segidx);

				for(int laneidx = 0; laneidx < pSeg->GetLaneCount(); laneidx++ )
				{
					VehicleLaneInSim * pLane = pSeg->GetLane(laneidx);
					DistancePointPath3D distPtPath;
					DistanceUnit squareDist = distPtPath.GetSquared(ptCartsParking, pLane->GetPath());
					if(squareDist < MinDist )
					{
						MinDist = squareDist;
						pNearestlane = pLane;
					}
				}
			}
		}

		if(pNearestlane)
		{

			//get sibling lane
			VehicleStretchSegmentInSim* pParentSeg = (VehicleStretchSegmentInSim*)pNearestlane->getRouteResource();
			int nLaneCount = pParentSeg->GetLaneCount();

			for (int nLane = 0; nLane < nLaneCount; ++nLane)
			{
				VehicleLaneInSim * pCurLane = pParentSeg->GetLane(nLane);

				DistanceUnit dEndDist = pCurLane->GetLength();

				//lead in and lead out
				{
					//lead in
					CPath2008 leadinPath = pCartSpot->GetBagCartsSpotInput()->getLeadInLine();
					{
						IntersectPathPath2D intersectPath;
						if(intersectPath.Intersects(leadinPath,pCurLane->GetPath(),5))//path and path intersect
						{
							DistanceUnit distInLane = pCurLane->GetPath().GetIndexDist( (float)intersectPath.m_vIntersectPtIndexInPath2[0] );
							CPoint2008 ptIntersect = pCurLane->GetPath().GetDistPoint(distInLane);
							pCurLane->AddExit( new VehicleLaneExit(pCurLane, pCartSpot, max(0,distInLane) ) );
						}
						else//use lead in line front point to find shortest distance in the path
						{
							if(leadinPath.getCount())
							{
								CPoint2008 ptLeadin = leadinPath.getPoint(0);
								DistancePointPath3D distPtPath;
								DistanceUnit squareDist = distPtPath.GetSquared(ptLeadin, pCurLane->GetPath());

								DistanceUnit distInLane = pCurLane->GetPath().GetIndexDist( (float)distPtPath.m_fPathParameter );
								pCurLane->AddExit( new VehicleLaneExit(pCurLane, pCartSpot, max(0,distInLane) ) );
							}
						}
					}
					//lead out
					CPath2008 leadOutPath = pCartSpot->GetBagCartsSpotInput()->getLeadOutLine();
					{
						IntersectPathPath2D intersectPath;
						if (intersectPath.Intersects(leadOutPath,pCurLane->GetPath(),5))//path and path intersect
						{
							DistanceUnit distInLane = pCurLane->GetPath().GetIndexDist( (float)intersectPath.m_vIntersectPtIndexInPath2[0] );
							pCurLane->AddEntry( new VehicleLaneEntry(pCurLane, pCartSpot, min(dEndDist,distInLane) ) );
						}
						else //use lead out line last point to find shortest distance in the path
						{
							int nCount = leadOutPath.getCount();
							if (nCount > 0)
							{	
								CPoint2008 ptLeadout = leadOutPath.getPoint(nCount - 1);
								DistancePointPath3D distPtPath;
								DistanceUnit squareDist = distPtPath.GetSquared(ptLeadout, pCurLane->GetPath());

								DistanceUnit distInLane = pCurLane->GetPath().GetIndexDist( (float)distPtPath.m_fPathParameter );
								pCurLane->AddEntry( new VehicleLaneEntry(pCurLane, pCartSpot, min(dEndDist,distInLane) ) );
							}

						}
					}

				}
			}

		}	

	}
	return true;
}

