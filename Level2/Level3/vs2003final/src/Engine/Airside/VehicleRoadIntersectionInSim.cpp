#include "StdAfx.h"
#include ".\vehicleroadintersectioninsim.h"
#include "./IntersectionNodeInSim.h"



CString VehicleRoadIntersectionInSim::PrintResource() const
{
	return _T("");
}

CPoint2008 VehicleRoadIntersectionInSim::GetDistancePoint( double dist )const
{	
	return m_vCenter;
}

VehicleRoadIntersectionInSim::VehicleRoadIntersectionInSim( const IntersectionNode& nodeInput )
{
	m_nodeInput = nodeInput;
	m_vCenter = m_nodeInput.GetPosition();
}

VehicleRoadIntersectionInSim::VehicleRoadIntersectionInSim( Intersections* pIntersect )
{
	m_pUserDefineInteresct = pIntersect;
	if(m_pUserDefineInteresct.get())
	{
		m_vCenter= m_pUserDefineInteresct->GetPosition();
	}

}
void VehicleRoadIntersectionInSim::InitGraph()
{
	m_vExitEntryPairs.clear();
	for(VehicleRouteExitSet::iterator exitItr = m_vExits.begin(); exitItr!= m_vExits.end(); exitItr++ )
	{
		VehicleLaneExit * pExit = *exitItr;
		for(VehicleRouteEntrySet::iterator entryItr = m_vEntrys.begin();entryItr!= m_vEntrys.end();entryItr++)
		{
			VehicleLaneEntry * pEntry = *entryItr;
			DistanceUnit dist = pEntry->GetPosition().distance( pExit->GetPosition() );			
			m_vExitEntryPairs.push_back( VehicleRouteNodePairDist(pExit,pEntry, dist) );
		}
	}
}

void VehicleRoadIntersectionInSim::InitIntersectStretch( const IntersectedStretchInAirport& intersectStretchs )
{
	for(int i=0;i< intersectStretchs.GetCount(); i++)
	{
		IntersectedStretch theStretch( intersectStretchs.ItemAt(i) );
		if( theStretch.GetIntersectionNode().GetID() == m_nodeInput.GetID() )
		{
			m_vIntersectStretchs.push_back(theStretch);
		}
	}
}

void VehicleRoadIntersectionInSim::GetMinMaxHoldDistAtStretch( int nStretchID , DistanceUnit& minDist, DistanceUnit& maxDist )
{
	DistanceUnit dMin = 0; DistanceUnit dMax= 0;
	for(int i=0;i< (int)m_vIntersectStretchs.size();i++)
	{	
		IntersectedStretch & theStretch = m_vIntersectStretchs[i];
		IntersectedStretchPoint thePoint;
		if(theStretch.GetObject1ID() == nStretchID)
		{
			thePoint = theStretch.GetFristIntersectedStretchPoint();
		}
		if( theStretch.GetObject2ID() == nStretchID )
		{
			thePoint = theStretch.GetSecondIntersectedStretchPoint();
		}
		DistanceUnit distToNode = thePoint.GetDistToIntersect();
		if( distToNode < 0 && distToNode < dMin )
		{
			dMin = distToNode;
		}
		if( distToNode > 0 && distToNode > dMax )
		{
			dMax = distToNode;
		}
	}
	minDist = dMin;
	maxDist = dMax;

}

bool VehicleRoadIntersectionInSim::IsHeadNodeOfStretch(VehicleStretchInSim* pStretch)
{
	if( m_pUserDefineInteresct.get() )
	{
		InsecObjectPartVector& objPartsVector = m_pUserDefineInteresct->GetIntersectionsPart();
		DistanceUnit strethDist = pStretch->GetInput()->GetPath().GetTotalLength();
		for(int i=0;i<(int) objPartsVector.size();i++)
		{
			InsecObjectPart& objPart = objPartsVector[i];
			if( objPart.GetObjectID() == pStretch->GetInput()->getID() )
			{
				if ( objPart.pos < (  pStretch->GetInput()->GetPath().getCount()-1 )/2.0 )
				{
					return true;
				}				
			}
		}
	}

	return false;
}

bool VehicleRoadIntersectionInSim::IsTrailNodeOfStretch( VehicleStretchInSim * pStretch )
{
	if( m_pUserDefineInteresct.get() )
	{
		const InsecObjectPartVector& objPartsVector = m_pUserDefineInteresct->GetIntersectionsPart();
		DistanceUnit strethDist = pStretch->GetInput()->GetPath().GetTotalLength();
		for(int i=0;i<(int) objPartsVector.size();i++)
		{
			InsecObjectPart objPart = objPartsVector.at(i);
			if( objPart.GetObjectID() == pStretch->GetInput()->getID() )
			{
				if ( objPart.pos > ( pStretch->GetInput()->GetPath().getCount()-1 )/2.0 )
				{
					return true;
				}				
			}
		}
	}

	return false;
}

bool VehicleRoadIntersectionInSim::IsRelateWithLanes( VehicleLaneInSim* pExitLane,VehicleLaneInSim *pEntryLane )
{
	bool bFind = false;
	//find from exit lane
	VehicleRouteExitSet::iterator iterExit = m_vExits.begin();
	for (;iterExit != m_vExits.end(); ++iterExit)
	{
		if((*iterExit)->GetLane() == pExitLane)
		{
			bFind = true;
			break;
		}
	}

	if (bFind == false)
		return false;

	bFind = false;
	VehicleRouteEntrySet::iterator iterEnrty = m_vEntrys.begin();
	for (;iterEnrty != m_vEntrys.end(); ++iterEnrty)
	{
		if((*iterEnrty)->GetLane() == pEntryLane)
		{
			bFind = true;
			break;
		}
	}


	return bFind;
}

bool VehicleRoadIntersectionInSim::IsIntersectionOf( int nStrtechId1, int nStretchId2 )
{
	if(m_pUserDefineInteresct.get())
	{

	}
	else{
		return m_nodeInput.HasObject(nStretchId2) && m_nodeInput.HasObject(nStrtechId1);
	}
	return false;
}

const VehicleRouteExitSet& VehicleRoadIntersectionInSim::getExitSet()
{
	return m_vExits;
}

const VehicleRouteEntrySet& VehicleRoadIntersectionInSim::getEntrySet()
{
	return m_vEntrys;
}

#include "..\..\Results\AirsideFlightEventLog.h"
void VehicleRoadIntersectionInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = GetNodeInput().GetID();
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource().GetString();
}