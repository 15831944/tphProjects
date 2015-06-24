#include "StdAfx.h"


#include "../../Common/ARCUnit.h"
#include "../../InputAirside/ALTAirport.h"
#include "./TaxiwayResourceManager.h"
#include "TaxiwayConstraintsManager.h"
#include "../../InputAirside/IntersectItem.h"
#include "../../InputAirside/HoldShortLines.h"

TaxiwayResourceManager::TaxiwayResourceManager()
:m_pAdjcancyConstraintWingSpan(NULL)
{

}

TaxiwayResourceManager::~TaxiwayResourceManager()
{
	clear();
}

bool TaxiwayResourceManager::Init( int nPrjID, int nAirportID )
{
	clear();
	ALTObjectList vTaxiways;
	
	ALTAirport::GetTaxiwayList(nAirportID,vTaxiways);

	for(int i=0 ;i< (int)vTaxiways.size(); ++i)
	{				
		Taxiway * pTaxiway = (Taxiway*)vTaxiways.at(i).get();		
		m_vTaxiways.push_back( TaxiwayInSim(pTaxiway) );
		HoldPositionList allHoldpos;
		HoldPosition::ReadHoldPositionList(pTaxiway->getID(),allHoldpos );
		for(int j=0;j<(int)allHoldpos.size();++j)
		{
			HoldPosition& theHold = allHoldpos[j];
			HoldPositionInSim newHoldShort(theHold.GetUniqueID(),pTaxiway->getID(), theHold.m_intersectNode.GetID(), theHold.m_distToIntersect + theHold.m_intersectPos - theHold.m_intersectNode.GetDistance(pTaxiway->getID()) );
			m_vHoldShortLineProps.push_back(newHoldShort);
		}

	}
	//init hold short lines
	//TaxiInterruptLineList userHoldLines;
	//userHoldLines.ReadData(nAirportID);
	//for(int i=0;i<(int)userHoldLines.GetElementCount();++i)
	//{
	//	CTaxiInterruptLine* pHold = userHoldLines.GetItem(i);
	//	HoldPositionInSim newHoldShort(pHold->GetID(),pHold->m_nSelObjectID, pHold->m_nInterNodeID, pHold->m_dDis2InterNode);
	//	m_vHoldShortLineProps.push_back(newHoldShort);
	//}
	return true;
}

bool TaxiwayResourceManager::InitRelations( IntersectionNodeInSimList& NodeList )
{
	// init
	for(int i=0;i<GetTaxiwayCount();i++)
	{
		GetTaxiwayByIdx(i)->InitSegments(NodeList, m_vHoldShortLineProps);
	}

	return true;
}

bool TaxiwayResourceManager::InitConstraints(int nProjID, int nAirportID, CAirportDatabase* pAirportDB)
{
	TaxiwayConstraintsManager consManager;
	consManager.ReadData(nProjID, nAirportID, pAirportDB);

	for(int i=0;i<GetTaxiwayCount();i++)
	{
		GetTaxiwayByIdx(i)->InitConstraints(consManager);
	}

	
	return true;
}


void TaxiwayResourceManager::InitTaxiwayAdjacencyConstraints(int nProjID, CAirportDatabase* pAirportDB)
{
	m_pAdjcancyConstraintWingSpan = new CWingspanAdjacencyConstraintsList(pAirportDB);
	m_pAdjcancyConstraintWingSpan->LoadData(nProjID);

	for(int i=0;i<GetTaxiwayCount();i++)
	{
		GetTaxiwayByIdx(i)->InitTaxiwayAdjacencyConstraints(m_pAdjcancyConstraintWingSpan);
	}
}

struct SegmentPriorty
{
	bool operator()(TaxiwayDirectSegInSim* pSeg1, TaxiwayDirectSegInSim* pSeg2)const
	{
		return pSeg1->GetEndDist() < pSeg2->GetEndDist();
	}	
};

TaxiwaySegInSim * TaxiwayResourceManager::GetTaxiwaySegment( IntersectionNodeInSim *pNode1, IntersectionNodeInSim * pNode2)
{
	for(int i =0 ;i<(int)m_vTaxiways.size();i++)
	{
		TaxiwayDirectSegInSim * pSegment =GetTaxiwayByIdx(i)->GetTaxiwayDirectSegment(pNode1,pNode2);
		if(pSegment)
		{
			return pSegment->GetTaxiwaySeg();
		}
	}

	return NULL;
}

void TaxiwayResourceManager::GetTaxiwaySegment(int nTaxiwayID, int nIntersectNodeIDFrom, int nIntersectNodeIDTo, FlightGroundRouteDirectSegList& taxiwayDirectSegLst)
{
	if (nTaxiwayID < 0 || nIntersectNodeIDFrom < 0 || nIntersectNodeIDTo < 0)
		return;

	TaxiwayInSim* pTaxiway = GetTaxiwayByID(nTaxiwayID);
	if(pTaxiway)
	{
		return pTaxiway->GetTaxiwayDirectSegmentList(nIntersectNodeIDFrom, nIntersectNodeIDTo, taxiwayDirectSegLst);
	}	
}

TaxiwayDirectSegInSim* TaxiwayResourceManager::GetTaxiwayDirectSegment( int nIntersectNodeIDFrom, int nIntersectNodeIDTo, int nTaxiwayID)
{
	int nCount = GetTaxiwayCount();
	std::vector<TaxiwayDirectSegInSim*> vSegments;
	vSegments.clear();

	for(int i =0 ;i<nCount; i++)
	{
		TaxiwayInSim* pTaxiway = GetTaxiwayByIdx(i);
		int nSegCount = pTaxiway->GetSegmentCount();
		for(int j =0 ; j <nSegCount;j++ )
		{
			TaxiwaySegInSim * pSeg = pTaxiway->GetSegment(j);
			if( pSeg->GetNode1()->GetID() == nIntersectNodeIDFrom && pSeg->GetNode2()->GetID() == nIntersectNodeIDTo )
				vSegments.push_back(pSeg->GetPositiveDirSeg());

			if (pSeg->GetNode2()->GetID() == nIntersectNodeIDFrom && pSeg->GetNode1()->GetID() == nIntersectNodeIDTo)
				vSegments.push_back(pSeg->GetNegativeDirSeg()) ;				
		}
	}
	if (vSegments.empty())
		return NULL;

	for(int i = 0; i < (int)vSegments.size(); i++)
	{
		TaxiwayDirectSegInSim* pDirSeg = vSegments.at(i);

		if (pDirSeg->GetTaxiwayID() == nTaxiwayID)
			return pDirSeg;
	}

	return vSegments.at(0);
}

FlightGroundRouteDirectSegList TaxiwayResourceManager::GetLinkedDirectSegmentsTaxiway( IntersectionNodeInSim * pNode )
{
	FlightGroundRouteDirectSegList reslt;
	for(int i =0 ;i< GetTaxiwayCount(); i++)
	{
		TaxiwayInSim* pTaxiway = GetTaxiwayByIdx(i);
		for(int j =0 ; j <pTaxiway->GetSegmentCount();j++ )
		{
			TaxiwaySegInSim * pSeg = pTaxiway->GetSegment(j);
			if( pSeg->GetPositiveDirSeg() )
			{
				if( pNode == pSeg->GetNode1() )
				{
					reslt.push_back(pSeg->GetPositiveDirSeg());
				}
			}
			if( pSeg->GetNegativeDirSeg() )
			{
				if(pNode == pSeg->GetNode2() )
				{
					reslt.push_back(pSeg->GetNegativeDirSeg());
				}
			}
		}
	}

	return reslt;
}
//
FlightGroundRouteDirectSegList TaxiwayResourceManager::GetAllLinkedDirectSegmentsTaxiway( IntersectionNodeInSim * pNode )
{
	FlightGroundRouteDirectSegList reslt;
	for(int i =0 ;i< GetTaxiwayCount(); i++)
	{
		TaxiwayInSim* pTaxiway = GetTaxiwayByIdx(i);
		for(int j =0 ; j <pTaxiway->GetSegmentCount();j++ )
		{
			TaxiwaySegInSim * pSeg = pTaxiway->GetSegment(j);
		
			if( pNode == pSeg->GetNode1() || pNode == pSeg->GetNode2() )
			{
				reslt.push_back(pSeg->GetPositiveDirSeg());
				reslt.push_back(pSeg->GetNegativeDirSeg());
			}
		}
	}

	return reslt;
}

TaxiwayInSim* TaxiwayResourceManager::GetTaxiwayByID(int nID)
{
	for(int i=0;i<GetTaxiwayCount();i++)
	{
		TaxiwayInSim * pTaxiway = GetTaxiwayByIdx(i);
		if(pTaxiway->GetTaxiwayInput()->getID() == nID)
			return pTaxiway;
	}
	for (int i =0; i < (int)m_vTaxiways.size(); i++)
	{
		if (m_vTaxiways[i].GetTaxiwayInput()->getID() == nID)
		{
			return &m_vTaxiways[i];
		}
	}
	return NULL;
}

int TaxiwayResourceManager::GetLinkedTaxiwayID( IntersectionNodeInSim * pNode1, IntersectionNodeInSim* pNode2)
{
	if (!pNode1 || !pNode2)
		return -1;

	std::vector<TaxiwayIntersectItem*> vTaxiwayList = pNode1->GetNodeInput().GetTaxiwayIntersectItemList();
	std::vector<TaxiwayIntersectItem*> vTaxiwayList2 = pNode2->GetNodeInput().GetTaxiwayIntersectItemList();

	int nCount = vTaxiwayList.size();
	int nSize = vTaxiwayList2.size();

	for (int i = 0; i < nCount; i++)
	{
		int nID = vTaxiwayList[i]->GetObjectID();
		for (int j = 0; j < nSize; j++)
		{
			if (vTaxiwayList2[j]->GetObjectID() == nID)
			{
				return nID;
			}
		}
	}

	return -1;
}



TaxiwayInSim * TaxiwayResourceManager::GetTaxiwayByIdx( int idx )
{
	if(idx < GetTaxiwayCount() && idx>=0 )
	{
		return &m_vTaxiways[idx];
	}
	return NULL;
}

void TaxiwayResourceManager::clear()
{
	m_vTaxiways.clear();
	//m_vParkingSegs.clear();
	m_vHoldShortLineProps.clear();

	if (m_pAdjcancyConstraintWingSpan)
	{
		delete m_pAdjcancyConstraintWingSpan;
		m_pAdjcancyConstraintWingSpan = NULL;
	}
}

std::vector< TaxiwayInSim >& TaxiwayResourceManager::getTaxiwayResource()
{
	return m_vTaxiways;
}

std::vector<HoldPositionInSim *> TaxiwayResourceManager::GetHoldPositionInSim( int nTaxiwayID,int nIntersectionID )
{
	std::vector<HoldPositionInSim *> vReturnHoldPosition;
	std::vector<HoldPositionInSim>::iterator iter = m_vHoldShortLineProps.begin();
	for (; iter != m_vHoldShortLineProps.end(); ++iter)
	{
		if((*iter).GetTaxiwayID() == nTaxiwayID && (*iter).GetNodeID() == nIntersectionID)
		{
			vReturnHoldPosition.push_back(&(*iter));
		}
	}

	return vReturnHoldPosition;
}

bool TaxiwayResourceManager::IsNodeOnTaxiway( int nTaxiwayID, int nIntersectNodeID )
{
	TaxiwayInSim* pTaxiway = GetTaxiwayByID(nTaxiwayID);
	if (pTaxiway)
	{
		IntersectionNodeInSimList vNodeList = pTaxiway->GetIntersectionNodeList();
		int nCount = vNodeList.GetNodeCount();
		for (int i=0;i<nCount;i++)
		{
			if ( vNodeList.GetNodeByIndex(i)->GetID() == nIntersectNodeID )
			{
				return true;
			}
		}
	}
	return false;
}













