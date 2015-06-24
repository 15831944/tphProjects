#include "StdAfx.h"
#include <limits>
#include ".\findflightshortestpath.h"
#include "TaxiwayResourceManager.h"
#include "AirsideFlightInSim.h"
#include "../../Common/RunTimeDebug.h"
#include "RunwaySegInSim.h"

const static DistanceUnit INVALID_DIST = ARCMath::DISTANCE_INFINITE;

static void DijFindShortestPath(IntersectionNodeInSim * pSrc,IntersectionNodeInSim * pDest,CFindFlightShortestPath::IntersectionNodesSet& m_vNodeList, CFindFlightShortestPath::ShortestRouteMapType& m_ShortestRouteMatrix)
{
	for(int i=0;i< (int)m_vNodeList.size(); i++)
	{
		//find shortest node from src to other node 
		IntersectionNodeInSim * pShorestNode = NULL;
		DistanceUnit dShortDist = ARCMath::DISTANCE_INFINITE;
		for(CFindFlightShortestPath::IntersectionNodesSet::iterator itr = m_vNodeList.begin();itr!= m_vNodeList.end();itr++)
		{
			IntersectionNodeInSim *pOtherNode = *itr;
			if(pOtherNode == pSrc) continue;			
			CFindFlightShortestPath::RouteInGraph& routeNodepair = m_ShortestRouteMatrix[CFindFlightShortestPath::NodePair(pSrc,pOtherNode)];

			if( !routeNodepair.m_bIsDoneSearch ) // not done search
			{
				if(routeNodepair.m_dist < dShortDist )
				{
					dShortDist = routeNodepair.m_dist;
					pShorestNode = pOtherNode;
				}
			}
		}		
		//update other shortest 
		if(pShorestNode)
		{
			//CFindFlightShortestPath::NodePair nodepair(pSrc,pShorestNode);
			CFindFlightShortestPath::RouteInGraph& routeSrcShort = m_ShortestRouteMatrix[CFindFlightShortestPath::NodePair(pSrc,pShorestNode)];
			routeSrcShort.m_bIsDoneSearch = true;
			if(pShorestNode == pDest) break;

			for(CFindFlightShortestPath::IntersectionNodesSet::iterator itr = m_vNodeList.begin();itr!= m_vNodeList.end();itr++)
			{
				IntersectionNodeInSim * pNodeI = *itr;
				
				if(pSrc == pNodeI)continue;
				if(pShorestNode == pNodeI) continue;	

				CFindFlightShortestPath::RouteInGraph& routeSrcI = m_ShortestRouteMatrix[CFindFlightShortestPath::NodePair(pSrc,pNodeI)];
				CFindFlightShortestPath::RouteInGraph& routeShortI = m_ShortestRouteMatrix[CFindFlightShortestPath::NodePair(pShorestNode,pNodeI)];
							

				if(!routeSrcI.m_bIsDoneSearch)
				{
					if(dShortDist + routeShortI.m_dist < routeSrcI.m_dist)
					{
						routeSrcI.m_dist = dShortDist + routeShortI.m_dist;
						routeSrcI.m_vSegs.clear();
						routeSrcI.m_vSegs.insert(routeSrcI.m_vSegs.end(),routeSrcShort.m_vSegs.begin(),routeSrcShort.m_vSegs.end() );
						routeSrcI.m_vSegs.insert(routeSrcI.m_vSegs.end(),routeShortI.m_vSegs.begin(), routeShortI.m_vSegs.end() );
					}
				}
			}
		}
		
	}
}


static bool IsTaxiwaySegValid(AirsideFlightInSim* pFlight, FlightGroundRouteDirectSegInSim* pDirSeg)
{
	if(pFlight && pDirSeg && pDirSeg->GetRouteSegInSim() )
	{
		return pDirSeg->GetRouteSegInSim()->CanServeFlight(pFlight);
	}
	return false;
}



bool CFindFlightShortestPath::RouteInGraph::operator<( const RouteInGraph& otherRoute ) const
{
	return m_dist < otherRoute.m_dist;
}





bool CFindFlightShortestPath::RouteInGraph::CanServeFlight( AirsideFlightInSim*pFlight ) const
{
	if(m_vSegs.size() == 0)
		return false;

	for(int i=0;i<(int)m_vSegs.size();++i )
	{
		if( !IsTaxiwaySegValid(pFlight, m_vSegs[i]) )
		{
			return false;
		}
	}
	return true;
}

CFindFlightShortestPath::RouteInGraph::RouteInGraph( FlightGroundRouteDirectSegInSim* pDirSeg )
{
	m_dist = pDirSeg->GetEndDist();
	m_vSegs.push_back(pDirSeg);
	m_bIsDoneSearch = false;
}

CFindFlightShortestPath::RouteInGraph::RouteInGraph()
{
	m_bIsDoneSearch = false;
	m_dist = INVALID_DIST;
}

DistanceUnit CFindFlightShortestPath::GetRoute(AirsideFlightInSim *pFlight,IntersectionNodeInSim * pSrc,IntersectionNodeInSim * pDest,FlightGroundRouteDirectSegList& segList)
{
	if(pSrc== NULL || pDest==NULL )
		return -1;

	if(pSrc == pDest)
		return 0;

	NodePair nodepair(pSrc,pDest);			
	RouteInGraph& route = m_ShortestRouteMap[nodepair];	
	if(!route.m_bIsDoneSearch)
	{
		DijFindShortestPath(pSrc,pDest, m_taxiwayGraph.m_vNodeList, m_ShortestRouteMap);
	}

	if( !m_ShortestRouteMap[nodepair].CanServeFlight(pFlight) )//the route can't server the flight
	{
		//copy valid current matrix
		ShortestRouteMapType m_RouteMapForFlight;
		for(ShortestRouteMapType::iterator itr = m_ShortestRouteMap.begin(); itr!= m_ShortestRouteMap.end();++itr)
		{
			if( itr->second.CanServeFlight(pFlight) )//copy the valid route
			{
                m_RouteMapForFlight[itr->first] = itr->second;
				if(itr->first.first == pSrc){
					m_RouteMapForFlight[itr->first].m_bIsDoneSearch = false;
				}
			}
			else //reset to the default
			{
				FlightGroundRouteDirectSegInSim* pDirseg = m_taxiwayGraph.GetValidEdge(itr->first,pFlight);
				if(pDirseg)
				{
					m_RouteMapForFlight[itr->first] = RouteInGraph(pDirseg);
				}
				else
					m_RouteMapForFlight[itr->first] = RouteInGraph();
			}
		}
		
		if(!m_RouteMapForFlight[nodepair].m_bIsDoneSearch)
		{
			DijFindShortestPath(pSrc, pDest, m_taxiwayGraph.m_vNodeList, m_RouteMapForFlight);
		}

		if(!m_RouteMapForFlight[nodepair].CanServeFlight(pFlight)) //the route still can't server flight error
		{
			if ( pSrc && pDest &&  m_RouteMapForFlight[nodepair].m_dist == INVALID_DIST )
			{
				return -1;
			}
		}
		else
		{ //get the route from the route map for flight return
			RouteInGraph& theRoute = m_RouteMapForFlight[nodepair];
			segList = theRoute.m_vSegs;
			return theRoute.m_dist;
		}
	}
	else //the route is ok  return;
	{
		RouteInGraph& theRoute = m_ShortestRouteMap[nodepair];
		segList = theRoute.m_vSegs;
		return theRoute.m_dist;		
	}
	return -1;
}

void CFindFlightShortestPath::Init(std::vector< TaxiwayInSim >& vTaxiways, std::vector<FlightGroundRouteSegInSim*> excludeSegs)
{
	m_taxiwayGraph.clear();
	m_ShortestRouteMap.clear();


	//init shortest route matrix
	for(int i=0 ;i< (int)vTaxiways.size(); i++)
	{
		TaxiwayInSim & taxiwayInSim = vTaxiways[i];
		for(int j =0 ;j< taxiwayInSim.GetSegmentCount(); ++j )
		{
			TaxiwaySegInSim * pSeg = taxiwayInSim.GetSegment(j);
			if( excludeSegs.end() != std::find(excludeSegs.begin(),excludeSegs.end(), pSeg) )
				continue;
		

			if(pSeg->GetPositiveDirSeg())
			{
				if (pSeg->GetPositiveDirSeg()->IsForbidDirection())
					continue;

				RouteInGraph route(pSeg->GetPositiveDirSeg());				
				NodePair nodepair(pSeg->GetNode1(),pSeg->GetNode2());
				m_taxiwayGraph.AddEdge(pSeg->GetPositiveDirSeg());
				if( route.m_dist < m_ShortestRouteMap[nodepair].m_dist )
				{
					m_ShortestRouteMap[nodepair] = route;					
				}
			}
			if(pSeg->GetNegativeDirSeg())
			{
				if (pSeg->GetNegativeDirSeg()->IsForbidDirection())
					continue;

				RouteInGraph route(pSeg->GetNegativeDirSeg());
				NodePair nodepair(pSeg->GetNode2(),pSeg->GetNode1());
				m_taxiwayGraph.AddEdge(pSeg->GetNegativeDirSeg());
				if( route.m_dist < m_ShortestRouteMap[nodepair].m_dist )
				{					
					m_ShortestRouteMap[nodepair] = route;					
				}
			}
		}
	}

}

void CFindFlightShortestPath::Init( std::vector<TaxiwayInSim>& vTaxiways,std::vector<RunwayInSim::RefPtr>& vRunways , std::vector<FlightGroundRouteSegInSim*> excludeSegs )
{
	m_taxiwayGraph.clear();
	m_ShortestRouteMap.clear();


	//init shortest route matrix
	{
		for(int i=0 ;i< (int)vTaxiways.size(); i++)
		{
			TaxiwayInSim & taxiwayInSim = vTaxiways[i];
			for(int j =0 ;j< taxiwayInSim.GetSegmentCount(); ++j )
			{
				TaxiwaySegInSim * pSeg = taxiwayInSim.GetSegment(j);
				if( excludeSegs.end() != std::find(excludeSegs.begin(),excludeSegs.end(), pSeg) )
					continue;


				if(pSeg->GetPositiveDirSeg())
				{
					RouteInGraph route(pSeg->GetPositiveDirSeg());				
					NodePair nodepair(pSeg->GetNode1(),pSeg->GetNode2());				
					m_taxiwayGraph.AddEdge(pSeg->GetPositiveDirSeg());
					if( route.m_dist < m_ShortestRouteMap[nodepair].m_dist )
					{
						m_ShortestRouteMap[nodepair] = route;					
					}
				}
				if(pSeg->GetNegativeDirSeg())
				{
					RouteInGraph route(pSeg->GetNegativeDirSeg());
					NodePair nodepair(pSeg->GetNode2(),pSeg->GetNode1());
					m_taxiwayGraph.AddEdge(pSeg->GetNegativeDirSeg());
					if( route.m_dist < m_ShortestRouteMap[nodepair].m_dist )
					{					
						m_ShortestRouteMap[nodepair] = route;					
					}
				}
			}
		}
	}

//init runway
	for(int nRunway=0 ;nRunway< (int)vRunways.size(); nRunway++)
	{
		RunwayInSim::RefPtr&  refRunwayInSim = vRunways[nRunway];
		RunwayInSim * pRunwayInSim = refRunwayInSim.get();
		for(int j =0 ;j< pRunwayInSim->GetSegmentCount(); ++j )
		{
			RunwaySegInSim * pSeg = pRunwayInSim->GetSegment(j);
			if( excludeSegs.end() != std::find(excludeSegs.begin(),excludeSegs.end(), pSeg) )
				continue;


			if(pSeg->GetPositiveDirSeg())
			{
				RouteInGraph route(pSeg->GetPositiveDirSeg());				
				NodePair nodepair(pSeg->GetNode1(),pSeg->GetNode2());				
				m_taxiwayGraph.AddEdge(pSeg->GetPositiveDirSeg());
				if( route.m_dist < m_ShortestRouteMap[nodepair].m_dist )
				{
					m_ShortestRouteMap[nodepair] = route;					
				}
			}
			if(pSeg->GetNegativeDirSeg())
			{
				RouteInGraph route(pSeg->GetNegativeDirSeg());
				NodePair nodepair(pSeg->GetNode2(),pSeg->GetNode1());
				m_taxiwayGraph.AddEdge(pSeg->GetNegativeDirSeg());
				if( route.m_dist < m_ShortestRouteMap[nodepair].m_dist )
				{					
					m_ShortestRouteMap[nodepair] = route;					
				}
			}
		}
	}

}

void CFindFlightShortestPath::TaxiGraphInSim::AddEdge( FlightGroundRouteDirectSegInSim* pSeg )
{
	if(!pSeg)return;
	m_vNodeList.insert(pSeg->GetEntryNode());
	m_vNodeList.insert(pSeg->GetExitNode());
	m_vEdegeList.push_back(pSeg);
}

FlightGroundRouteDirectSegInSim* CFindFlightShortestPath::TaxiGraphInSim::GetValidEdge( const NodePair& nodePair,AirsideFlightInSim* pFlight )
{
	for(int i=0;i<(int)m_vEdegeList.size();++i)
	{
		FlightGroundRouteDirectSegInSim* ptheSeg = m_vEdegeList[i];
		if( NodePair(ptheSeg->GetEntryNode(), ptheSeg->GetExitNode())  == nodePair  && ptheSeg->GetRouteSegInSim()->CanServeFlight(pFlight) )
			return ptheSeg;
	}
	return NULL;
}

void CFindFlightShortestPath::TaxiGraphInSim::clear()
{
	m_vEdegeList.clear(); 
	m_vNodeList.clear();
}
