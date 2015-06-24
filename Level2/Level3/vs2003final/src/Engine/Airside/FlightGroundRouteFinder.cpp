#include "StdAfx.h"
#include ".\flightgroundRoutefinder.h"
#include "../../Common/RunTimeDebug.h"
#include "TaxiwayResourceManager.h"
#include "AirsideFlightInSim.h"
#include "RunwaySegInSim.h"
#include <Engine/BoostPathFinder.h>


int CFlightGroundRouteFinder::GetNodeIndex( IntersectionNodeInSim* pNode ) const
{
	std::vector<IntersectionNodeInSim*>::const_iterator itr = std::find(m_vNodeList.begin(), m_vNodeList.end(), pNode);
	if(itr!=m_vNodeList.end())
		return itr - m_vNodeList.begin();
	return -1;
}

IntersectionNodeInSim* CFlightGroundRouteFinder::GetNode( int idx ) const
{
	return m_vNodeList[idx];
}

int CFlightGroundRouteFinder::AddNode( IntersectionNodeInSim* pNode )
{
	std::vector<IntersectionNodeInSim*>::const_iterator itr = std::find(m_vNodeList.begin(), m_vNodeList.end(), pNode);
	if(itr!=m_vNodeList.end())
		return itr - m_vNodeList.begin();
	else{
		m_vNodeList.push_back(pNode);
		return (int)m_vNodeList.size()-1;
	}
}

void CFlightGroundRouteFinder::Init( std::vector<TaxiwayInSim>& vTaxiways,std::vector<FlightGroundRouteSegInSim*> excludeSegs )
{
	//init shortest route matrix	
	for(int i=0 ;i< (int)vTaxiways.size(); i++)
	{
		TaxiwayInSim & taxiwayInSim = vTaxiways[i];
		for(int j =0 ;j< taxiwayInSim.GetSegmentCount(); ++j )
		{
			TaxiwaySegInSim * pSeg = taxiwayInSim.GetSegment(j);
			if( excludeSegs.end() != std::find(excludeSegs.begin(),excludeSegs.end(), pSeg) )
				continue;

			AddNode(pSeg->GetNode1());
			AddNode(pSeg->GetNode2());
			if(pSeg->GetPositiveDirSeg() && !(pSeg->GetPositiveDirSeg()->IsForbidDirection()) )
			{									
				m_vEdges.push_back(pSeg->GetPositiveDirSeg());					
			}
			if(pSeg->GetNegativeDirSeg() && !(pSeg->GetNegativeDirSeg()->IsForbidDirection()))
			{					
				m_vEdges.push_back(pSeg->GetNegativeDirSeg());					
			}
		}
	}	
	InitBoostGraph();
}

void CFlightGroundRouteFinder::Init( std::vector<TaxiwayInSim>& vTaxiways,std::vector<RunwayInSim::RefPtr>& vRunways ,std::vector<FlightGroundRouteSegInSim*> excludeSegs )
{
	//init shortest route matrix	
	for(int i=0 ;i< (int)vTaxiways.size(); i++)
	{
		TaxiwayInSim & taxiwayInSim = vTaxiways[i];
		for(int j =0 ;j< taxiwayInSim.GetSegmentCount(); ++j )
		{
			TaxiwaySegInSim * pSeg = taxiwayInSim.GetSegment(j);
			if( excludeSegs.end() != std::find(excludeSegs.begin(),excludeSegs.end(), pSeg) )
				continue;

			AddNode(pSeg->GetNode1());
			AddNode(pSeg->GetNode2());
			if(pSeg->GetPositiveDirSeg() && !(pSeg->GetPositiveDirSeg()->IsForbidDirection()) )
			{									
				m_vEdges.push_back(pSeg->GetPositiveDirSeg());

				//CString strmsg;
				//strmsg.Format(_T("%s,%d"),pSeg->GetPositiveDirSeg()->PrintResource(),(int)pSeg->GetPositiveDirSeg()->GetDir());
				//std::ofstream echoFile1 ("c:\\routeedge1111.log", stdios::app);
				//echoFile1<<strmsg<<"\r\n";
				//echoFile1.close();
			}
			if(pSeg->GetNegativeDirSeg() && !(pSeg->GetNegativeDirSeg()->IsForbidDirection()) )
			{					
				m_vEdges.push_back(pSeg->GetNegativeDirSeg());	
				//CString strmsg;
				//strmsg.Format(_T("%s,%d"),pSeg->GetPositiveDirSeg()->PrintResource(),(int)pSeg->GetPositiveDirSeg()->GetDir());
				//std::ofstream echoFile1 ("c:\\routeedge1111.log", stdios::app);
				//echoFile1<<strmsg<<"\r\n";
				//echoFile1.close();
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
			
			AddNode(pSeg->GetNode1());
			AddNode(pSeg->GetNode2());
			if(pSeg->GetPositiveDirSeg())
			{							
				m_vEdges.push_back(pSeg->GetPositiveDirSeg());				
			}
			if(pSeg->GetNegativeDirSeg())
			{				
				m_vEdges.push_back(pSeg->GetNegativeDirSeg());				
			}
		}
	}


	InitBoostGraph();

}

CFlightGroundRouteFinder::CFlightGroundRouteFinder()
	: mpBoostPathFinder(NULL)
	, m_pAppliedStrategy(&m_sBasicStrategy)
{	
}

CFlightGroundRouteFinder::~CFlightGroundRouteFinder()
{
	delete mpBoostPathFinder;
	mpBoostPathFinder = NULL;
}

void CFlightGroundRouteFinder::InitBoostGraph(AirsideFlightInSim *pFlight/* = NULL*/)
{
	//ASSERT(mpBoostPathFinder);
	if(mpBoostPathFinder){
		delete mpBoostPathFinder;
		mpBoostPathFinder = NULL;
	}
	
	//a star 
	if(false){
		CBoostAStarPathFinder* pAstarFinder = new CBoostAStarPathFinder();
		CBoostAStarPathFinder::VertexLocationMap& vertexLocMap = pAstarFinder->GetVertexLocation();
		vertexLocMap.clear();
		vertexLocMap.reserve(m_vNodeList.size());
		for(int i=0;i<(int)m_vNodeList.size();i++)
		{
			IntersectionNodeInSim* pNode = m_vNodeList[i];
			vertexLocMap.push_back( pNode->GetDistancePoint(0));
		}		
		mpBoostPathFinder = pAstarFinder;
	}
	else
	{
		CBoostDijPathFinder* pDijFinder = new CBoostDijPathFinder();
		mpBoostPathFinder = pDijFinder;
	}


	CBoostDirectedGraph& bg = mpBoostPathFinder->mGraph;
	bg.Init(m_vNodeList.size());

	for(EdgeSet::iterator itr = m_vEdges.begin();itr!=m_vEdges.end();itr++)
	{
		FlightGroundRouteDirectSegInSim* pSeg = *itr;
		
//		CString strMsg;
// 		strMsg.Format(_T("%s,%d"),pSeg->PrintResource(),(int)pSeg->GetDir());
// 		std::ofstream echoFile1 ("c:\\routeedge.log", stdios::app);
// 		echoFile1<<strMsg<<"\r\n";
// 		echoFile1.close();


		// if pFlight presents, and pSeg can not serve the flight
		// do not add the edge
		if (pFlight && !m_pAppliedStrategy->SegCanServeFlight(pSeg, pFlight))
			continue;

		bg.AddEdge( GetNodeIndex(pSeg->GetEntryNode()), GetNodeIndex(pSeg->GetExitNode()),
			pFlight ? m_pAppliedStrategy->EvalueWeight(pSeg, pFlight) : pSeg->GetLength() );
	}
	
}

// bIgnoreDoubtfulSeg: if true, ignore any segment checking. default to false.
bool CFlightGroundRouteFinder::GetRoute( AirsideFlightInSim *pFlight,IntersectionNodeInSim * pSrc,IntersectionNodeInSim * pDest,
										FlightGroundRouteDirectSegList& pathNodes, double& dPathWeight, FlightGroundRouteFinderStrategy* pStrategy /*= NULL*/,
										bool bIgnoreDoubtfulSeg /*= false*/ )
{
	SetStrategy(pStrategy); // ####

	ASSERT(mpBoostPathFinder);
	int srcIndex = GetNodeIndex(pSrc);
	int destIndex = GetNodeIndex(pDest);
	
	ASSERT(srcIndex>=0 && destIndex >=0 );
	if(srcIndex>=0 && destIndex >=0 )
	{
		std::vector<myvertex_descriptor> vRetPath;
		if( mpBoostPathFinder->FindPath(srcIndex, destIndex, vRetPath, dPathWeight) )
		{
			pathNodes.clear();
			for(int i=0;i<(int)vRetPath.size()-1;i++)
			{
				int nNodeidx1 = vRetPath[i];
				int nNodeidx2 = vRetPath[i+1];
				IntersectionNodeInSim* pNode1  =GetNode(nNodeidx1);
				IntersectionNodeInSim* pNode2 = GetNode(nNodeidx2);
				FlightGroundRouteDirectSegInSim* pSeg = GetEdge(pNode1, pNode2);
				if(pSeg)
				{
					if (bIgnoreDoubtfulSeg || m_pAppliedStrategy->IsRouteUndoubted(pSeg, pFlight))
					{
						pathNodes.push_back(pSeg);
					} 
					else
					{
						// if found some route segment can not serve the flight
						// just to make the can serve sub graph
						// and get an valid route
						CFlightGroundRouteFinder subGraphFinder;
						subGraphFinder.SetStrategy(pStrategy); // ####
						MakeCanServeFlightSubGraphFinder(&subGraphFinder, pFlight);
						return subGraphFinder.GetRoute(pFlight, pSrc, pDest, pathNodes, dPathWeight, pStrategy, true);
					}
				}
			}
			RestoreBasicStrategy(); // ####
			return true;
		}
	}	
	RestoreBasicStrategy(); // ####
	return false;

}

FlightGroundRouteDirectSegInSim* CFlightGroundRouteFinder::GetEdge( IntersectionNodeInSim* pNode1, IntersectionNodeInSim* pNode2 ) const
{
	for(EdgeSet::const_iterator itr = m_vEdges.begin();itr!=m_vEdges.end();itr++)
	{
		FlightGroundRouteDirectSegInSim* pSeg = *itr;
		if(pSeg->GetEntryNode() == pNode1 && pSeg->GetExitNode() == pNode2)
			return pSeg;
	}
	return NULL;
}

DistanceUnit CFlightGroundRouteFinder::GetRouteDist( IntersectionNodeInSim * pSrc,IntersectionNodeInSim * pDest ) const
{
	int NodeIdx1 = GetNodeIndex(pSrc);
	int NodeIdx2 = GetNodeIndex(pDest);
	return mpBoostPathFinder->GetRouteDist(NodeIdx1, NodeIdx2);
}

void CFlightGroundRouteFinder::MakeCanServeFlightSubGraphFinder( CFlightGroundRouteFinder* pSubGraphFinder, AirsideFlightInSim *pFlight ) const
{
	ASSERT(pSubGraphFinder);
	pSubGraphFinder->m_vNodeList = m_vNodeList;
	pSubGraphFinder->m_vEdges = m_vEdges;
	pSubGraphFinder->mpBoostPathFinder = NULL;

	pSubGraphFinder->InitBoostGraph(pFlight);
}
const FlightGroundRouteFinderStrategy CFlightGroundRouteFinder::m_sBasicStrategy;

void CFlightGroundRouteFinder::SetStrategy( const FlightGroundRouteFinderStrategy* pNewStrategy )
{
	if (pNewStrategy)
	{
		m_pAppliedStrategy = pNewStrategy;
	} 
	else
	{
		m_pAppliedStrategy = &m_sBasicStrategy;
	}
}
void CFlightGroundRouteFinder::RestoreBasicStrategy()
{
	m_pAppliedStrategy = &m_sBasicStrategy;
}
