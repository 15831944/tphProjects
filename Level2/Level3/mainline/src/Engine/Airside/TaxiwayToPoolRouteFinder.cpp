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

	std::vector<IntersectionNodeInSim*> vSearchedNodes;
	std::map<IntersectionNodeInSim*,double> vUnSearchParNodes;
	std::map<IntersectionNodeInSim*,double> vSubLevelNodes;
	vSearchedNodes.clear();
	vUnSearchParNodes.insert(std::make_pair(pNode,0.0));

	IntersectionNodeInSim* pSearchNode = NULL;
	IntersectionNodeInSim* pSubLevelNode = NULL;
	IntersectionNodeInSim* pBestNode = NULL;

	double dMinDistancePath = (std::numeric_limits<double>::max)();
	while (true)
	{	
		std::map<IntersectionNodeInSim*,double>::iterator iter = vUnSearchParNodes.begin();

		for(; iter != vUnSearchParNodes.end(); ++iter)
		{
			pSearchNode = iter->first;

			vSearchedNodes.push_back(pSearchNode);
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

				CPoint2008 pSearchPos = pSearchNode->GetNodeInput().GetPosition();
				CPoint2008 pSubLevelPos = pSubLevelNode->GetNodeInput().GetPosition();
			
				double dDistance = pSearchPos.distance3D(pSubLevelPos);
				double dPathDistance = iter->second + dDistance;
				if (pDirSeg->GetTaxiwaySeg()->getIntersectLaneCount() >0)
				{
					if (dPathDistance < dMinDistancePath)
					{
						dMinDistancePath = dPathDistance;
						*pLaneEntrySeg = pDirSeg;
						pBestNode = pSearchNode;
					}
				}

				if (std::find(vSearchedNodes.begin(),vSearchedNodes.end(),pSubLevelNode) != vSearchedNodes.end())
					continue;

	
				if (vUnSearchParNodes.find(pSubLevelNode) != vUnSearchParNodes.end())
					continue;

				if (vSubLevelNodes.find(pSubLevelNode) != vSubLevelNodes.end())
					continue;

				vSubLevelNodes.insert(std::make_pair(pSubLevelNode,dPathDistance));

			}
		}

		if (vSubLevelNodes.empty())
			break;

		vUnSearchParNodes.clear();

		vUnSearchParNodes.insert(vSubLevelNodes.begin(),vSubLevelNodes.end());
		vSubLevelNodes.clear();
	}

	return pBestNode;
	//*pLaneEntrySeg = NULL;
	//return NULL;
}