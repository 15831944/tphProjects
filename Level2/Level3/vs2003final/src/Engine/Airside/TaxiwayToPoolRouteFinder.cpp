#include "StdAfx.h"
#include ".\taxiwaytopoolroutefinder.h"
#include "AirportResourceManager.h"
#include "IntersectionNodeInSim.h"
#include "TaxiwayResourceManager.h"
#include <limits>

TaxiwayToNearestStretchRouteFinder::TaxiwayToNearestStretchRouteFinder(AirportResourceManager* pAirportRes)
:m_pAirportRes(pAirportRes)
{
	m_mapFoundRoute.clear();
}

TaxiwayToNearestStretchRouteFinder::~TaxiwayToNearestStretchRouteFinder(void)
{
}

void TaxiwayToNearestStretchRouteFinder::GetShortestPathFromTaxiIntersectionToNearestStretch( const IntersectionNodeInSim* pInNode, FlightGroundRouteDirectSegList& path )
{
	IntersectionNodeInSim* pNode = const_cast<IntersectionNodeInSim*>(pInNode);
	if (m_mapFoundRoute.find(pNode) != m_mapFoundRoute.end())
	{
		path.assign(m_mapFoundRoute[pNode].begin(),m_mapFoundRoute[pNode].end());
		return;
	}

	TaxiwayDirectSegInSim* pLaneEntrySeg = NULL;
	IntersectionNodeInSim* pDestNode = GetTaxiIntersectionBesideNearestStretch(pNode, &pLaneEntrySeg);

	ASSERT(pDestNode);

	if (pDestNode !=NULL)
	{
		double dPathWeight = (std::numeric_limits<double>::max)();
		m_pAirportRes->getGroundRouteResourceManager()->GetRoute(pNode, pDestNode, NULL, 0, path, dPathWeight);

		if (std::find(path.begin(),path.end(),pLaneEntrySeg) == path.end())		//add the lane entry segment
			path.push_back(pLaneEntrySeg);
		
	}
	
	m_mapFoundRoute.insert(std::map<IntersectionNodeInSim*, FlightGroundRouteDirectSegList>::value_type(pNode, path));
	
}

IntersectionNodeInSim* TaxiwayToNearestStretchRouteFinder::GetTaxiIntersectionBesideNearestStretch( const IntersectionNodeInSim* pInNode, TaxiwayDirectSegInSim** pLaneEntrySeg )
{
	IntersectionNodeInSim* pNode = const_cast<IntersectionNodeInSim*>(pInNode);

	std::vector<IntersectionNodeInSim*> vUnSearchParNodes, vSearchedNodes;
	std::vector<IntersectionNodeInSim*> vSubLevelNodes;
	vSearchedNodes.clear();
	vUnSearchParNodes.push_back(pNode);

	IntersectionNodeInSim* pSearchNode = NULL;
	IntersectionNodeInSim* pSubLevelNode = NULL;

	while (true)
	{	

		int nNodeCount = vUnSearchParNodes.size();

		for (int i =0; i < nNodeCount; i++)
		{
			pSearchNode = vUnSearchParNodes.at(i);

			FlightGroundRouteDirectSegList segList = m_pAirportRes->getTaxiwayResource()->GetLinkedDirectSegmentsTaxiway(pSearchNode);
			int nSize = segList.size();

			for (int idx = 0; idx < nSize; idx++)
			{
				TaxiwayDirectSegInSim* pDirSeg = dynamic_cast<TaxiwayDirectSegInSim*>(segList.at(idx));
				if (pDirSeg == NULL)
					continue;

				if (pDirSeg->IsPositiveDir())
					pSubLevelNode = pDirSeg->GetTaxiwaySeg()->GetNode2();
				else
					pSubLevelNode = pDirSeg->GetTaxiwaySeg()->GetNode1();

				if (pDirSeg->GetTaxiwaySeg()->getIntersectLaneCount() >0)
				{
					*pLaneEntrySeg = pDirSeg;
					return pSearchNode;
				}

				if (std::find(vSearchedNodes.begin(),vSearchedNodes.end(),pSubLevelNode) != vSearchedNodes.end())
					continue;

				if (std::find(vUnSearchParNodes.begin(),vUnSearchParNodes.end(),pSubLevelNode) != vUnSearchParNodes.end())
					continue;

				if (std::find(vSubLevelNodes.begin(),vSubLevelNodes.end(),pSubLevelNode) != vSubLevelNodes.end())
					continue;

				vSubLevelNodes.push_back(pSubLevelNode);

			}
		}

		if (vSubLevelNodes.empty())
			return NULL;

		vSearchedNodes.insert(vSearchedNodes.end(),vUnSearchParNodes.begin(),vUnSearchParNodes.end());
		vUnSearchParNodes.clear();

		vUnSearchParNodes.assign(vSubLevelNodes.begin(),vSubLevelNodes.end());
		vSubLevelNodes.clear();
	}

	*pLaneEntrySeg = NULL;
	return NULL;
}