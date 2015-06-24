#include "StdAfx.h"
#include "InputAirside/VehicleRoute.h"
#include "VehicleRouteUserDefineInSim.h"
#include "VehicleStretchInSim.h"
#include "VehicleRouteResourceManager.h"
#include "StandResourceManager.h"
#include "VehiclePoolResourceManager.h"
#include "../../Common/DistanceLineLine.h"
#include "../../Common/ARCMathCommon.h"
#include "PaxBusParkingInSim.h"

//void VehicleRouteUserDefineInSim::findShortestPath( VehicleRouteNode* pEntry, VehicleRouteNode* pExit )
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
//			if( !m_ShortestRoutes[nodePair].m_bIsDoneSearch )
//			{
//				if(m_ShortestRoutes[nodePair].m_dist  < dShortDist )
//				{
//					dShortDist = m_ShortestRoutes[nodePair].m_dist;
//					pShortestNode = pOtherNode;
//				}
//			}
//		}
//
//		//update 
//		if(pShortestNode )
//		{
//			VehicleRouteNodePair nodePair(pEntry, pShortestNode);
//			m_ShortestRoutes[nodePair].m_bIsDoneSearch = true;
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
//				dij_VechileRoute& routeSrcShort = m_ShortestRoutes[VehicleRouteNodePair(pEntry,pShortestNode)];
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
bool VehicleRouteUserDefineInSim::Init( VehicleRouteResourceManager* pManager , CVehicleRoute* pRoute )
{
	// traverser all route nodes ,
	// if the node have father node , then it entries are from the father node, else all the entry node is valid
	// if the node have child nodes , then its exits are to the child nodes
	m_vRouteSketchSegments.clear();

	typedef std::set<VehicleRoadIntersectionInSim*> IntersectionSet;
	
	IntersectionSet vRoadIntersections;
	for(int i=0;i< pRoute->GetStretchNodeCount(); ++i)
	{
		CStretchNode* pRouteNode = pRoute->GetStretchNodeItem(i);
		CStretchNode* pParentNode = pRoute->GetStretchNodeItemByID(pRouteNode->GetStretchID());
		CVehicleRoute::StretchNodeVector vChildNode = pRoute->GetChildNodes(pRouteNode->GetID());
		
		VehicleStretchInSim* pStretch =  pManager->GetStretchByID(pRouteNode->GetStretchID());
		if(pStretch)
		{			
			bool bHaveFatherNode = pParentNode && pManager->GetStretchByID(pParentNode->GetStretchID());
			bool bHaveChildNodes = (vChildNode.size() != 0) ;		

			
			//add segments
			for(int segidx = 0; segidx < pStretch->GetSegmentCount(); segidx++ )
			{
				VehicleStretchSegmentInSim* pSeg = pStretch->GetSegment(segidx);

				m_vRouteSketchSegments.push_back(pSeg);


				for( int i=0;i< pSeg->GetEdgeCount(); i++)
				{
					VehicleRouteNodePairDist entryExit = pSeg->GetEdge(i);
				
					//dij_VechileRoute& theRoute = m_ShortestRoutes[VehicleRouteNodePair(entryExit.GetNodeFrom(), entryExit.GetNodeTo())];
					//theRoute.m_route.push_back( entryExit );
					//theRoute.m_dist = entryExit.GetDistance();	
					m_vRouteNodeSet.insert(entryExit.GetNodeFrom());
					m_vRouteNodeSet.insert(entryExit.GetNodeTo());

					if(!bHaveFatherNode)
					{
						if( entryExit.GetNodeFrom()->GetType() == VehicleRouteNode::LANE_ENTRY)
						{
							m_vRouteEntries.insert( (VehicleLaneEntry*)entryExit.GetNodeFrom() );
						}
					}
					if(!bHaveChildNodes)
					{
						if(entryExit.GetNodeTo()->GetType() == VehicleRouteNode::LANE_EXIT )
						{
							m_vRouteExits.insert( (VehicleLaneExit*)entryExit.GetNodeTo() );
						}
					}

					//
				}
			}
			//add intersection nodes			
			if(!bHaveFatherNode || !bHaveChildNodes)
			{
				for(int j=0;j< pStretch->GetSegmentCount();++j)
				{
					VehicleStretchSegmentInSim* pSeg = pStretch->GetSegment(j);
					vRoadIntersections.insert(pSeg->GetNode1());
					vRoadIntersections.insert(pSeg->GetNode2());
				}
			}
			else
			{
				IntersectionSet vFromIntersections;
				IntersectionSet vToIntersections;
				//get intersection of father node
				for(int iIntidx=0; iIntidx< pManager->GetIntersectionCount(); ++ iIntidx)
				{
					VehicleRoadIntersectionInSim* pIntersection = pManager->GetIntersection(iIntidx);
					if(pIntersection->IsIntersectionOf(pParentNode->GetStretchID(), pRouteNode->GetStretchID()))
					{
						vFromIntersections.insert(pIntersection);
					}
					for(int iChildIdx = 0;iChildIdx < (int)vChildNode.size(); ++iChildIdx)
					{
						int nChildStretchID = vChildNode[iChildIdx]->GetStretchID();
						if(pIntersection->IsIntersectionOf(pRouteNode->GetStretchID(), nChildStretchID))
						{
							vToIntersections.insert(pIntersection);
							break;
						}
					}
				}

				
				for(IntersectionSet::iterator itrF = vFromIntersections.begin(); itrF!= vToIntersections.end(); itrF++)
				{
					for(IntersectionSet::iterator itrT = vToIntersections.begin(); itrT!= vToIntersections.end(); itrT++)
					{
						std::vector<VehicleStretchSegmentInSim*> vSegments = pStretch->GetSegments(*itrF, *itrT);
						for(int segIdx = 0;segIdx< (int)vSegments.size();++segIdx)
						{
							VehicleStretchSegmentInSim* pSeg = vSegments[segIdx];
							vRoadIntersections.insert(pSeg->GetNode1());
							vRoadIntersections.insert(pSeg->GetNode2());
						}
					}
				}

			}
		}
	}

	std::set<VehicleStretchSegmentInSim* > stretchSegSet;
	std::vector<VehicleRoadIntersectionInSim*> vIntersectionNodes;
	//init intersections
	for(IntersectionSet::iterator itr = vRoadIntersections.begin();itr!= vRoadIntersections.end();++itr)
	{
		VehicleRoadIntersectionInSim* pRoadIntersection = *itr;		
		if(!pRoadIntersection)continue;

		vIntersectionNodes.push_back(pRoadIntersection);

		for( int i=0;i< pRoadIntersection->GetEdgeCount();i++)
		{
			VehicleRouteNodePairDist exitEntry = pRoadIntersection->GetEdge(i);
			//dij_VechileRoute& theRoute = m_ShortestRoutes[ VehicleRouteNodePair(exitEntry.GetNodeFrom(),exitEntry.GetNodeTo()) ];
			//theRoute.m_dist = exitEntry.GetDistance();
			//theRoute.m_route.push_back( exitEntry );

			//m_vRouteNodeSet.insert(exitEntry.GetNodeFrom());
			//m_vRouteNodeSet.insert(exitEntry.GetNodeTo());
			
			//exit entry
			if(exitEntry.GetNodeFrom())
			{
				 AirsideResource *pOrgResource = exitEntry.GetNodeFrom()->GetOrignResource();
				 if(pOrgResource&& pOrgResource->GetType() == AirsideResource::ResType_VehicleStretchLane)
				 {
					stretchSegSet.insert((VehicleStretchSegmentInSim*)((VehicleLaneInSim *)pOrgResource)->getRouteResource());
				 }
				 AirsideResource *pDestResource = exitEntry.GetNodeFrom()->GetDestResource();
				 if(pDestResource&& pDestResource->GetType() == AirsideResource::ResType_VehicleStretchLane)
				 {
					 stretchSegSet.insert((VehicleStretchSegmentInSim*)((VehicleLaneInSim *)pDestResource)->getRouteResource());
				 }
			}
			if(exitEntry.GetNodeTo())
			{
				AirsideResource *pOrgResource = exitEntry.GetNodeTo()->GetOrignResource();
				if(pOrgResource&& pOrgResource->GetType() == AirsideResource::ResType_VehicleStretchLane)
				{
					stretchSegSet.insert((VehicleStretchSegmentInSim*)((VehicleLaneInSim *)pOrgResource)->getRouteResource());
				}
				AirsideResource *pDestResource = exitEntry.GetNodeTo()->GetDestResource();
				if(pDestResource&& pDestResource->GetType() == AirsideResource::ResType_VehicleStretchLane)
				{
					stretchSegSet.insert((VehicleStretchSegmentInSim*)((VehicleLaneInSim *)pDestResource)->getRouteResource());
				}


			}
		}
	}
	
	
	m_pathFinder.Init(stretchSegSet,vIntersectionNodes);
	return true;
}

//bool VehicleRouteUserDefineInSim::IsLinked( VehicleRouteNode* pEntry, VehicleRouteNode* pExit )
//{
//	findShortestPath(pEntry, pExit);
//	
//	VehicleRouteNodePair nodePair;
//
//	dij_VechileRoute& theDijRoute = m_ShortestRoutes[nodePair];
//
//	if(theDijRoute.m_bIsDoneSearch && theDijRoute.m_dist < ARCMath::DISTANCE_INFINITE)
//	{		
//		return true;
//	}
//	else 
//		return false;
//}
//
bool VehicleRouteUserDefineInSim::GetShortestPath( VehicleRouteNode* pEntry, VehicleRouteNode* pExit, std::vector<VehicleRouteNodePairDist>& the_route)
{

	
	//std::vector<VehicleRouteNode*> vNodes;
	//SearchChildNode(pEntry,pExit,vNodes);
	if(m_pathFinder.GetRoute(pEntry,pExit,the_route))
	{
		
		return true;
	}
	else
	{
		return false;
	}
	


	//VehicleRouteNodePair nodePair(pEntry,pExit);
	//
	//dij_VechileRoute& theDijRoute = m_ShortestRoutes[nodePair];

	//if(theDijRoute.m_bIsDoneSearch && theDijRoute.m_dist < ARCMath::DISTANCE_INFINITE)
	//{
	//	the_route = theDijRoute.m_route;
	//	return true;
	//}
	//else
	//{
	//	findShortestPath(pEntry,pExit);
	//	return false;
	//}

	//VehicleRouteNode* pExit =pExit;
	//DistanceUnit minDist = ARCMath::DISTANCE_INFINITE;
	//VehicleRouteNodePair nearestNodePair(NULL,NULL);
	//dij_VechileRoute& nodepairRoute = m_ShortestRoutes[VehicleRouteNodePair(pEntry,pExit)];
	//if(!nodepairRoute.m_bIsDoneSearch)
	//{
	//	findShortestPath(pEntry,pExit);
	//}

	//if(nodepairRoute.m_bIsDoneSearch && nodepairRoute.m_dist < minDist)
	//{	
	//	//nearestNodePair = VehicleRouteNodePair(pEntry,pExit);
	//	the_route = nodepairRoute.m_route;
	//	return true;

	//	//minDist = nodepairRoute.m_dist;		
	//	//break;
	//}

	return false;

}


bool VehicleRouteUserDefineInSim::IsStretchExist( VehicleStretchSegmentInSim* pStretchSegEntry, VehicleStretchSegmentInSim* pStretchSegExit )
{
	if(std::find(m_vRouteSketchSegments.begin(),m_vRouteSketchSegments.end(),pStretchSegEntry) == m_vRouteSketchSegments.end())
		return false;

	if(std::find(m_vRouteSketchSegments.begin(),m_vRouteSketchSegments.end(),pStretchSegExit) == m_vRouteSketchSegments.end())
		return false;

	return true;
}

bool VehicleRouteUserDefineInSim::SearchSibingNode( VehicleRouteNode* pEntry,VehicleRouteNode* pExit, std::vector<VehicleRouteNode*> vRouteNodes )
{
	//AirsideResource *pResource =  pEntry->GetDestResource();
	//if(pResource->GetType() == AirsideResource::ResType_VehicleStretchLane)
	//{

	//}

	return true;
}

bool VehicleRouteUserDefineInSim::SearchChildNode( VehicleRouteNode* pStartNode,VehicleRouteNode* pEndNode, std::vector<VehicleRouteNode*> vRouteNodes )
{
	//std::vector<VehicleRouteNodePairDist> the_route;

	vRouteNodes.push_back(pStartNode);

	{
		int nTempCount = (int)vRouteNodes.size();
		for (int ntemp = 0; ntemp < nTempCount; ++ ntemp)
		{
			VehicleRouteNode* pTempNode = vRouteNodes.at(ntemp);
			CString strOrg;
			if(pTempNode&& pTempNode->GetOrignResource())
				strOrg = pTempNode->GetOrignResource()->PrintResource();
			CString strDest;
			if(pTempNode && pTempNode->GetDestResource())
				strDest= pTempNode->GetDestResource()->PrintResource();

			TRACE("Node list: %s - %s\r\n",strOrg,strDest);
		}
	}

	{
		CString strOrg;
		if(pStartNode&& pStartNode->GetOrignResource())
			strOrg = pStartNode->GetOrignResource()->PrintResource();
		CString strDest;
		if(pStartNode && pStartNode->GetDestResource())
			strDest= pEndNode->GetDestResource()->PrintResource();

		TRACE("Node Info: %s - %s\r\n",strOrg,strDest);
	}


	AirsideResource *pDestResource = pStartNode->GetDestResource();
	//AirsideResource *pOrgResource = pStartNode->GetOrignResource();
	if(pDestResource->GetType() == AirsideResource::ResType_VehicleStretchLane)
	{
		VehicleLaneInSim *pDestVehicleLane = (VehicleLaneInSim *)pDestResource;
		
		//VehicleRouteNode* pLaneEntryNode = NULL;
		//int nEntryCount = pDestVehicleLane->GetEntryCount();
		//if(int nEntry =0; nEntry < nEntryCount; ++ nEntry)
		//{
		//	if(pDestVehicleLane->GetEntry(nEntry)->GetOrignResource() == pOrgResource)
		//	{
		//		//the lane entry node
		//		pLaneEntryNode = pDestVehicleLane->GetEntry(nEntry);
		//		break;
		//	}
		//}
		//vRouteNodes.push_back(pLaneEntryNode);

		//check if exit exists
			
		//pVehicleLane
		int nCount = pDestVehicleLane->GetExitCount();
		////for ( int nExit = 0; nExit < nCount; ++ nExit)
		////{
		//	if(pDestVehicleLane->GetEndExit()->GetDestResource() == pEndNode->GetOrignResource())
		//	{
		//		//the_route.push_back()
		//		vRouteNodes.push_back(pEndNode);
		//		return true;
		//	}
		//}

			
		//for ( int nIndex = 0; nIndex < nCount; ++ nIndex)
		//{
			VehicleRouteNode* pTempNode = pDestVehicleLane->GetEndExit();
			if(SearchChildNode(pTempNode,pEndNode,vRouteNodes))
			{
				return true;
			}

		//}

		//vRouteNodes.erase(vRouteNodes.rbegin());

	}
	else if(pDestResource->GetType() == AirsideResource::ResType_VehicleRoadIntersection)
	{
		VehicleRoadIntersectionInSim* pIntersetion = (VehicleRoadIntersectionInSim*)pDestResource;
		

		//check if exit exists
			
		//pVehicleLane
		const VehicleRouteEntrySet& exitSet = pIntersetion->getEntrySet();
		
		VehicleRouteEntrySet::const_iterator iter =  exitSet.begin();

		for ( ; iter != exitSet.end() ; ++iter)
		{
			if((*iter)->GetDestResource() == pEndNode->GetOrignResource())
			{
				vRouteNodes.push_back(*iter);
				return true;
			}
		}

		iter =  exitSet.begin();
		for ( ; iter != exitSet.end() ; ++iter)
		{
			VehicleRouteNode* pTempNode = *iter;
			if(SearchChildNode(pTempNode,pEndNode,vRouteNodes))
			{
				return true;
			}

		}
	}
	else
	{
	
	}
	if(vRouteNodes.size() > 0)
		vRouteNodes.erase(vRouteNodes.begin() + vRouteNodes.size());

	return false;
}

bool VehicleRouteUserDefineInSim::GetEntryExitNode( AirsideResource * pResFrom, AirsideResource* pResTo,std::vector<VehicleRouteNode* >& vSameNodesF,std::vector<VehicleRouteNode*>& vSameNodesT )
{
	return m_pathFinder.GetEntryExitNode(pResFrom,pResTo,vSameNodesF,vSameNodesT);
}

void VehicleStrethRouteUserDefineInSimList::Init( int nPrjID,VehicleRouteResourceManager* pManager )
{
	m_pVehicleRouteResourceManager = pManager;

	CVehicleRouteList routeList;
	routeList.ReadData(nPrjID);
	for(int i=0;i<routeList.GetVehicleRouteCount();++i)
	{
		CVehicleRoute* pRoute = routeList.GetVehicleRouteItem(i);
		if(pRoute)
		{
			VehicleRouteUserDefineInSim * pRotueInSim = new VehicleRouteUserDefineInSim();
			pRotueInSim->Init(pManager, pRoute);
			m_vStretchRouteList.push_back(pRotueInSim);
		}
	}

}

bool VehicleStrethRouteUserDefineInSimList::GetVehicleDefineRoute( AirsideResource * pResFrom, AirsideResource* pResTo, VehicleRouteInSim& resltRoute )
{	
	VehicleRouteNodeSet vFromNodes;
	VehicleRouteNodeSet vToNodes;
	//get entry node

	VehicleStretchSegmentInSim *pStrechSegEntry= GetClosestLane(pResFrom,vFromNodes,true);

	//get exit node
	VehicleStretchSegmentInSim *pStrechSegExit= GetClosestLane(pResTo,vToNodes,false);

	int nRouteCount  = (int)m_vStretchRouteList.size();
	//for (int nRoute = 0; nRoute < nRouteCount; ++nRoute)
	//{
	//	if(m_vStretchRouteList.at(nRoute)->IsStretchExist(pStrechSegEntry,pStrechSegExit))
	//	{
	//		m_vStretchRouteList.at(nRoute)->GetShortestPath(vEntryNodes,)
	//		break;
	//	}
	//}

	for(int i=0;i<nRouteCount;++i)
	{
		VehicleRouteUserDefineInSim* pUserRoute = m_vStretchRouteList.at(i);
		std::set<VehicleLaneEntry*> vRouteEntries = pUserRoute->GetEntries();
		std::set<VehicleLaneExit*> vRouteExits = pUserRoute->GetExits();

		std::vector<VehicleRouteNode* > vSameNodesF;
		set_intersection(vFromNodes.begin(), vFromNodes.end(), vRouteEntries.begin(), vRouteEntries.end(), vSameNodesF.end());

		std::vector<VehicleRouteNode*> vSameNodesT;
		set_intersection(vRouteExits.begin(), vRouteExits.end(), vToNodes.begin(), vToNodes.end() , vSameNodesT.end() );


		if(vSameNodesF.size() && vSameNodesT.size() ) //get the 
		{
			std::vector<VehicleRouteNodePairDist> resltSegs;

			if(pUserRoute->GetShortestPath( *vSameNodesF.begin(), *vSameNodesT.begin(), resltSegs ))
			{
				resltRoute = VehicleRouteInSim(resltSegs);
				return true;
			}
		}

	}
	return false;
}

VehicleRouteUserDefineInSim * VehicleStrethRouteUserDefineInSimList::GetItem( int idx )
{
	return m_vStretchRouteList[idx];
}

VehicleStretchSegmentInSim* VehicleStrethRouteUserDefineInSimList::GetClosestLaneToPaxBusParking( CPaxBusParkingInSim * pPaxBusRes )
{

	if (m_mapResource_StretchSeg.find(((AirsideResource *)pPaxBusRes)) != m_mapResource_StretchSeg.end())
	{
		return m_mapResource_StretchSeg[pPaxBusRes];
	}

	VehicleStretchSegmentInSim* pStretchSeg = NULL;

	CPoint2008 ptPaxBusParking = pPaxBusRes->GetDistancePoint(0);
	DistanceUnit MinDist = ARCMath::DISTANCE_INFINITE;
	//DistanceUnit distInLane = 0;
	VehicleLaneInSim * pNearestlane = NULL;	

	for(int stretchidx = 0;stretchidx < m_pVehicleRouteResourceManager->GetStretchCount(); stretchidx++ )
	{
		VehicleStretchInSim * pStretch = m_pVehicleRouteResourceManager->GetStretch(stretchidx);
		for(int segidx = 0; segidx < pStretch->GetSegmentCount(); segidx++ )
		{	
			VehicleStretchSegmentInSim* pSeg = pStretch->GetSegment(segidx);

			for(int laneidx = 0; laneidx < pSeg->GetLaneCount(); laneidx++ )
			{
				VehicleLaneInSim * pLane = pSeg->GetLane(laneidx);
				DistancePointPath3D distPtPath;
				DistanceUnit squareDist = distPtPath.GetSquared(ptPaxBusParking, pLane->GetPath());
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
		pStretchSeg = (VehicleStretchSegmentInSim*)pNearestlane->getRouteResource();
		m_mapResource_StretchSeg[pPaxBusRes] = pStretchSeg;
	}

	return pStretchSeg;
}

VehicleStretchSegmentInSim* VehicleStrethRouteUserDefineInSimList::GetClosestLaneToVehiclePool( VehiclePoolInSim * pPoolRes )
{

	if (m_mapResource_StretchSeg.find(pPoolRes) != m_mapResource_StretchSeg.end())
	{
		return m_mapResource_StretchSeg[pPoolRes];
	}


	VehicleStretchSegmentInSim* pStretchSeg = NULL;


	CPoint2008 poolPos = pPoolRes->GetRandPoint();

	DistanceUnit MinDist = ARCMath::DISTANCE_INFINITE;
	DistanceUnit distInLane = 0;
	VehicleLaneInSim * pNearestlane = NULL;

	for(int stretchidx = 0;stretchidx < m_pVehicleRouteResourceManager->GetStretchCount(); stretchidx++ )
	{
		VehicleStretchInSim * pStretch = m_pVehicleRouteResourceManager->GetStretch(stretchidx);
		for(int segidx = 0; segidx < pStretch->GetSegmentCount(); segidx++ )
		{
			VehicleStretchSegmentInSim* pSeg = pStretch->GetSegment(segidx);
			for(int laneidx = 0; laneidx < pSeg->GetLaneCount(); laneidx++ )
			{
				VehicleLaneInSim * pLane = pSeg->GetLane(laneidx);
				DistancePointPath3D distPtPath;
				DistanceUnit squareDist = distPtPath.GetSquared(poolPos, pLane->GetPath());
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
		pStretchSeg = (VehicleStretchSegmentInSim*)pNearestlane->getRouteResource();
		m_mapResource_StretchSeg[pPoolRes] = pStretchSeg;
	}

	return pStretchSeg;
}

VehicleStretchSegmentInSim* VehicleStrethRouteUserDefineInSimList::GetClosestLaneToStand( StandInSim * standres )
{
	if (m_mapResource_StretchSeg.find(standres) != m_mapResource_StretchSeg.end())
	{
		return m_mapResource_StretchSeg[standres];
	}

	VehicleStretchSegmentInSim* pStretchSeg = NULL;

	CPoint2008 standPt = standres->GetDistancePoint(0);
	DistanceUnit MinDist = ARCMath::DISTANCE_INFINITE;
	DistanceUnit distInLane = 0;
	VehicleLaneInSim * pNearestlane = NULL;

	for(int stretchidx = 0;stretchidx < m_pVehicleRouteResourceManager->GetStretchCount(); stretchidx++ )
	{
		VehicleStretchInSim * pStretch = m_pVehicleRouteResourceManager->GetStretch(stretchidx);
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
		pStretchSeg = (VehicleStretchSegmentInSim*)pNearestlane->getRouteResource();
		m_mapResource_StretchSeg[standres] = pStretchSeg;


	}
	return pStretchSeg;
}



VehicleStretchSegmentInSim *VehicleStrethRouteUserDefineInSimList::GetClosestLane( AirsideResource * pRes, VehicleRouteNodeSet vNodes, bool bEntry )
{
	VehicleStretchSegmentInSim *pStrechSeg = NULL;
	CPoint2008 PtResource;
	if(pRes->GetType() == AirsideResource::ResType_Stand)
	{
		pStrechSeg = GetClosestLaneToStand((StandInSim *)pRes);
		PtResource = ((StandInSim *)pRes)->GetDistancePoint(0);
	}
	else if(pRes->GetType() == AirsideResource::ResType_VehiclePool)
	{
		pStrechSeg = GetClosestLaneToVehiclePool((VehiclePoolInSim *)pRes);
		PtResource = ((VehiclePoolInSim *)pRes)->GetRandPoint();
	}
	else if(pRes->GetType() == AirsideResource::ResType_PaxBusParking)
	{
		pStrechSeg = GetClosestLaneToPaxBusParking((CPaxBusParkingInSim *)pRes);
		PtResource = ((CPaxBusParkingInSim *)pRes)->GetDistancePoint(0);
	}
	else if (pRes->GetType() == AirsideResource::ResType_VehicleStretchLane)
	{
	}
	else if (pRes->GetType() == AirsideResource::ResType_FlightServiceRingRoute)
	{
	}
	else if (pRes->GetType() == AirsideResource::ResType_VehicleRoadIntersection)
	{

	}

	if(pStrechSeg)
	{
		int nLaneCount = pStrechSeg->GetLaneCount();

		for (int nLane = 0; nLane < nLaneCount; ++nLane)
		{
			VehicleLaneInSim * pCurLane = pStrechSeg->GetLane(nLane);

			DistanceUnit dEndDist = pCurLane->GetLength();

			DistancePointPath3D distPtPath;
			DistanceUnit squareDist = distPtPath.GetSquared(PtResource, pCurLane->GetPath());

			DistanceUnit distInLane = pCurLane->GetPath().GetIndexDist( distPtPath.m_fPathParameter );

			if(bEntry)// entry lane, find lane exits
			{
				int nCount = pCurLane->GetExitCount();
				for (int nIndex = 0; nIndex < nCount; ++nIndex)
				{
					vNodes.insert(pCurLane->GetExit(nIndex));
				}

			}
			else
			{
				int nCount = pCurLane->GetEntryCount();
				for (int nIndex = 0; nIndex < nCount; ++nIndex)
				{
					vNodes.insert(pCurLane->GetEntry(nIndex));
				}
			}
			//pCurLane->AddEntry( new VehicleLaneEntry(pCurLane, pRes, min(dEndDist,distInLane +sdEntryExitOffset) ) );
		}
	}

	return pStrechSeg;
}

VehicleStrethRouteUserDefineInSimList::VehicleStrethRouteUserDefineInSimList()
{
	m_pVehicleRouteResourceManager = NULL;
}

VehicleStrethRouteUserDefineInSimList::~VehicleStrethRouteUserDefineInSimList()
{

}

















