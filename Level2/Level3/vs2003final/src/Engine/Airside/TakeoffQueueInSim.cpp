#include "StdAfx.h"
#include "../EngineDll.h"
#include ".\takeoffqueueinsim.h"
#include "../../InputAirside/TakeoffQueues.h"
#include "../../InputAirside/TakeoffQueuePosi.h"
#include "./AirportResourceManager.h"
#include ".../../InputAirside/TakeoffQueuesItem.h"
#include "AirsideFlightInSim.h"
#include <algorithm>

TakeoffQueueInSim::TakeoffQueueInSim(void)
{
}

TakeoffQueueInSim::~TakeoffQueueInSim(void)
{
}

void TakeoffQueueInSim::Init( TakeoffQueuePosi* pQueueInput, AirportResourceManager * pAirportRes )
{
	//get Runway
	RunwayExitInSim* pExit = pAirportRes->getRunwayResource()->GetRunwayExitByID(pQueueInput->GetTakeoffPosiInterID());

	m_pRunway = pExit->GetLogicRunway(); //pAirportRes->getRunwayResource()->GetLogicRunway(pQueueInput->GetRunwayID(),(RUNWAY_MARK)pQueueInput->GetRunwayMKIndex());
	//RunwayExitDescription runwayDes = pQueueInput->GetRunwayExitDescription();
	IntersectionNodeInSim* pNode = pAirportRes->GetIntersectionNodeList().GetNodeByID(pExit->GetIntersectionID());// pAirportRes->GetIntersectionNodeList().GetIntersectionNode(runwayDes.GetRunwayID(),runwayDes.GetTaxiwayID(),runwayDes.GetIndex());
	if (!pNode)
		return;

	int nNodeID = pNode->GetID();
	m_pNodeOnRunway = pAirportRes->GetIntersectionNodeList().GetNodeByID(nNodeID);

	//ASSERT(m_pNodeOnRunway);
	if(!m_pNodeOnRunway)
		return ;

	std::vector<TaxiwayIntersectItem*> vTaxiItems = m_pNodeOnRunway->GetNodeInput().GetTaxiwayIntersectItemList();
	if( vTaxiItems.size() )
	{
		int nRootTaxiwayId = vTaxiItems[0]->GetObjectID();// m_pNodeOnRunway->GetNodeInput().GetTaxiwayIntersectItemList().begin()GetObjectID();

		for(int i=0 ;i< pQueueInput->GetTakeoffQueuesItemCount();i++)
			{
				TakeoffQueuesItem* pItem  = pQueueInput->GetItem(i);

				TakeoffQueuesItem* parentItem  = pQueueInput->GetItemByTreeId(pItem->GetParentID());
				IntersectionNodeInSim * fromNode = NULL; 
				IntersectionNodeInSim * toNode = NULL;
				TaxiwayInSim * pTaxiway  = NULL;
				if(parentItem)
				{
					fromNode = pAirportRes->GetIntersectionNodeList().GetIntersectionNode(pItem->GetTaxiwayID(),parentItem->GetTaxiwayID(),pItem->GetIdx());
					int taxiwayID = parentItem->GetTaxiwayID();
					pTaxiway = pAirportRes->getTaxiwayResource()->GetTaxiwayByID(taxiwayID);			
					TakeoffQueuesItem * pParentParentItem = pQueueInput->GetItemByTreeId(parentItem->GetParentID());
					if(pParentParentItem)
					{
						toNode = pAirportRes->GetIntersectionNodeList().GetIntersectionNode( parentItem->GetTaxiwayID(),pParentParentItem->GetTaxiwayID(),parentItem->GetIdx() );
					}
					else
					{
						toNode = pAirportRes->GetIntersectionNodeList().GetIntersectionNode(parentItem->GetTaxiwayID(),nRootTaxiwayId,parentItem->GetIdx());
					}
				}
				else
				{			
					fromNode = pAirportRes->GetIntersectionNodeList().GetIntersectionNode(pItem->GetTaxiwayID(),nRootTaxiwayId,pItem->GetIdx());
					pTaxiway = pAirportRes->getTaxiwayResource()->GetTaxiwayByID(nRootTaxiwayId);
					toNode = m_pNodeOnRunway;			
				}

				ASSERT(fromNode && toNode && pTaxiway);
				if(fromNode && toNode && pTaxiway)
				{
					FlightGroundRouteDirectSegList segList;
					pTaxiway->GetTaxiwayDirectSegmentList(fromNode->GetNodeInput().GetID(),toNode->GetNodeInput().GetID(), segList);
					for(int j=0;j<(int)segList.size();j++)
					{
						AddDirectSeg(segList.at(j));
					}
					if( pQueueInput->IsLeafItem(pItem) )
					{
						m_vEntryNodes.push_back(fromNode);
					}
				}

			}
	}
}

void TakeoffQueueInSim::AddDirectSeg( FlightGroundRouteDirectSegInSim * pDirSeg )
{
	if( std::find(m_vTaxiSegs.begin(),m_vTaxiSegs.end(),pDirSeg) == m_vTaxiSegs.end() )
	{
		m_vTaxiSegs.push_back(pDirSeg);
	}
}

int TakeoffQueueInSim::GetInQueueFlights( std::vector<AirsideFlightInSim*> vFlights ) const
{
	for(int i=0;i<(int) m_vTaxiSegs.size();i++)
	{
		FlightGroundRouteDirectSegInSim * pSeg = m_vTaxiSegs.at(i);
		std::vector<CAirsideMobileElement*> vMobileElements = pSeg->GetInResouceMobileElement();
		for(size_t i=0 ;i< vMobileElements.size();i++)
		{
			CAirsideMobileElement * pMobileElement = vMobileElements.at(i);
			if(pMobileElement && pMobileElement->GetType() == CAirsideMobileElement::AIRSIDE_FLIGHT)
			{
				vFlights.push_back((AirsideFlightInSim*)pMobileElement);
			}
		}
		
	}
	return vFlights.size();
}

bool TakeoffQueueInSim::FindClearanceInConcern( AirsideFlightInSim * pFlight,ClearanceItem& lastItem, Clearance& newClearance )
{
	//find clearance until conflict with a flight
	
	AirsideResource * pRes = lastItem.GetResource();
	if(pRes && pRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
	{
		TaxiwayDirectSegInSim * pDirSeg = (TaxiwayDirectSegInSim*)pRes;
		TaxiRouteInSim taxiRoute(OnQueueToRunway,pRes,pFlight->GetAndAssignTakeoffRunway());

		if(pFlight->GetMode() != OnQueueToRunway)
		{
			taxiRoute = GetTaxiRoute(pDirSeg->GetExitNode());
		}
		else
		{
			taxiRoute = GetTaxiRoute(pDirSeg->GetEntryNode());
		}	
		
		return taxiRoute.FindClearanceInConcern(pFlight,lastItem,ARCMath::DISTANCE_INFINITE,newClearance);
	}
	return false;
}

TaxiRouteInSim TakeoffQueueInSim::GetTaxiRoute( IntersectionNodeInSim * entryNode )
{	
	TaxiRouteInSim taxiRoute(OnQueueToRunway,entryNode,entryNode);
	TakeoffQueueRouteFinder routeFinder(this);
	routeFinder.InitPathInfo();
	bool bFound = routeFinder.FindRoute(entryNode,m_pNodeOnRunway,taxiRoute);	
	//ASSERT(bFound);
	return taxiRoute;	
}

ElapsedTime TakeoffQueueInSim::GetInQueueTime(AirsideFlightInSim * pFlight,const ElapsedTime& curTime) const
{
	ElapsedTime inQueueTime;
	for(int i =0 ;i< (int)m_vTaxiSegs.size();i++)
	{
		FlightGroundRouteDirectSegInSim * pDirSeg = m_vTaxiSegs.at(i);
		OccupancyInstance ocyInstance = pDirSeg->GetOccupyInstance(pFlight);
		if(ocyInstance.IsValid() && ocyInstance.IsEnterTimeValid()&& ocyInstance.GetOccupyType() == OnQueueToRunway)
		{
			if(ocyInstance.IsExitTimeValid())
			{
				inQueueTime += ocyInstance.GetExitTime() - ocyInstance.GetEnterTime();
			}
			else
			{
				inQueueTime += curTime - ocyInstance.GetEnterTime();
			}
		}
	}
	return inQueueTime;
}

bool TakeoffQueueInSim::IsHaveSegment( FlightGroundRouteDirectSegInSim * pSeg ) const
{
	return std::find(m_vTaxiSegs.begin(),m_vTaxiSegs.end(),pSeg) != m_vTaxiSegs.end();
}


void TakeoffQueuesManager::Init( int nPrjId, AirportResourceManager * pAirportRes )
{

	return ;
	TakeoffQueues takeoffQueuesInput(nPrjId);
	
	takeoffQueuesInput.ReadData();

	for(int i =0; i< takeoffQueuesInput.GetTakeoffQueuePosiCount();i++)
	{
		TakeoffQueuePosi * pTakeoffQueue =  takeoffQueuesInput.GetItem(i);
		if(pTakeoffQueue)
		{
			TakeoffQueueInSim  *pNewQueue = new TakeoffQueueInSim;
			pNewQueue->Init(pTakeoffQueue,pAirportRes);
			m_vTakeoffQueues.push_back(pNewQueue);
		}
	}
}


TakeoffQueuesManager::~TakeoffQueuesManager()
{
	for(size_t i=0;i<m_vTakeoffQueues.size();i++)
	{
		delete m_vTakeoffQueues[i];
	}
}

TakeoffQueueInSim * TakeoffQueuesManager::GetRunwayExitQueue( RunwayExitInSim * pRunwayExit )
{
	//ASSERT(pRunwayExit);
	if(!pRunwayExit)
		return NULL;

	for(int i = 0 ;i< GetQueueCount() ;i++)
	{
		TakeoffQueueInSim * pQueue = GetQueue(i);
		FlightGroundRouteDirectSegInSim * pDirSeg  = pRunwayExit->GetRouteSeg();
		FlightGroundRouteDirectSegInSim * pOtherSeg = pDirSeg->GetOppositeSegment();

		if( pQueue->GetRunway() == pRunwayExit->GetLogicRunway() && pQueue->IsHaveSegment(pOtherSeg) )
		{
			return pQueue;
		}
	}
	return NULL;
}

TakeoffQueueRouteFinder::dijRouteInQueue::dijRouteInQueue()
{
	m_bIsDoneSearch = false;
}

double TakeoffQueueRouteFinder::dijRouteInQueue::GetWeight() const
{
	if(m_vRoutes.size() > 0)
	{
		int nFlightCnt = 1;
		DistanceUnit rotueLen = 0;
		for(int i =0;i< (int)m_vRoutes.size();i++)
		{
			FlightGroundRouteDirectSegInSim * pSeg = m_vRoutes.at(i);
			std::vector<CAirsideMobileElement*> vFlights = pSeg->GetInResouceMobileElement();
			nFlightCnt += vFlights.size();
			rotueLen += pSeg->GetEndDist();			
		}
		return rotueLen / nFlightCnt;

	}
	else
		return ARCMath::DISTANCE_INFINITE;

}


void TakeoffQueueRouteFinder::InitPathInfo()
{
	ASSERT(m_pQueue);
	FlightGroundRouteDirectSegList segList = m_pQueue->GetTaxiSegments();
	for(int i=0;i< (int)segList.size();i++)
	{
		FlightGroundRouteDirectSegInSim * pDirSeg = segList.at(i);
		m_vNodeList.insert(pDirSeg->GetEntryNode());
		m_vNodeList.insert(pDirSeg->GetExitNode());
		NodePair nodePair(pDirSeg->GetEntryNode(), pDirSeg->GetExitNode());
		std::vector<FlightGroundRouteDirectSegInSim*> vSegs;
		vSegs.push_back(pDirSeg);
		dijRouteInQueue dijRoute;
		dijRoute.m_vRoutes = vSegs;

		if(  dijRoute.GetWeight()< m_dijRouteMap[nodePair].GetWeight())
		{	
			m_dijRouteMap[nodePair].m_vRoutes = dijRoute.m_vRoutes;
		}
  
	}
}

bool TakeoffQueueRouteFinder::FindRoute( IntersectionNodeInSim * pNodeFrom, IntersectionNodeInSim* pNodeTo, TaxiRouteInSim& route )
{
	for(int i=1; i< (int)m_vNodeList.size();i++)
	{
		IntersectionNodeInSim * pShorestNode = NULL;
		DistanceUnit dLessWeight = ARCMath::DISTANCE_INFINITE;
		for(IntersectionNodesSet::iterator itr = m_vNodeList.begin();itr!= m_vNodeList.end();itr++)
		{
			IntersectionNodeInSim *pOtherNode = *itr;
			if(pOtherNode == pNodeFrom) continue;
			NodePair nodepair(pNodeFrom,pOtherNode);

			if( !m_dijRouteMap[nodepair].m_bIsDoneSearch ) // not done search
			{
				DistanceUnit dWeight  = m_dijRouteMap[nodepair].GetWeight();
				if( dWeight < dLessWeight )
				{
					pShorestNode = pOtherNode;
					dLessWeight = dWeight;
				}
			}
		}	
		if(pShorestNode)
		{
			NodePair nodepair(pNodeFrom,pShorestNode);
			m_dijRouteMap[nodepair].m_bIsDoneSearch = true;
			for(IntersectionNodesSet::iterator itr = m_vNodeList.begin();itr!= m_vNodeList.end();itr++)
			{
				IntersectionNodeInSim * pNodeI = *itr;

				dijRouteInQueue& routeSrcI = m_dijRouteMap[NodePair(pNodeFrom,pNodeI)];
				dijRouteInQueue& routeShortI = m_dijRouteMap[NodePair(pShorestNode,pNodeI)];
				dijRouteInQueue& routeSrcShort = m_dijRouteMap[NodePair(pNodeFrom,pShorestNode)];
				if(!routeSrcI.m_bIsDoneSearch)
				{
					if(dLessWeight + routeShortI.GetWeight() < routeSrcI.GetWeight())
					{
						FlightGroundRouteDirectSegList segList;
						segList.insert(segList.end(),routeSrcShort.m_vRoutes.begin(),routeSrcShort.m_vRoutes.end() );
						segList.insert(segList.end(),routeShortI.m_vRoutes.begin(), routeShortI.m_vRoutes.end() ); 
						routeSrcI.m_vRoutes = segList;
					}
				}
			}
		}
		if(pShorestNode == pNodeTo)
			break;

	}
	route.AddTaxiwaySegList(m_dijRouteMap[NodePair(pNodeFrom,pNodeTo)].m_vRoutes);
	return route.GetItemCount() > 0; 
	
}
