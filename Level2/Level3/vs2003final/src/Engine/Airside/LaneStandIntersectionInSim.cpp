#include "StdAfx.h"
#include ".\lanestandintersectioninsim.h"
#include "VehicleStretchInSim.h"
#include "AirsideFlightInSim.h"
#include "StandInSim.h"

LaneStandIntersectionInSim::LaneStandIntersectionInSim( void )
{
	m_pLane = NULL;
	m_pStand = NULL;
	m_idx = -1;
	m_lineId = -1; 
	m_leadlineType = Stand::IN_LINE;
}

LaneStandIntersectionInSim::LaneStandIntersectionInSim( VehicleLaneInSim* pLane, StandInSim* pStand, int lineid, Stand::LeadLineType linetype, int idx )
{
	m_pLane = pLane;
	m_pStand = pStand;
	m_lineId = lineid;
	m_leadlineType = linetype;
	m_idx = idx;
}
LaneStandIntersectionInSim::~LaneStandIntersectionInSim(void)
{
}

AirsideFlightInSim * LaneStandIntersectionInSim::GetLatestFlightInSight( const DistanceUnit& radius,const ElapsedTime&tTime )
{
	Point intersectPos = GetPosition();
	
	//check flight locked this stand
	AirsideFlightInSim * pFlight = m_pStand->GetLockedFlight();
	if(pFlight)
	{
		Point fltPos = pFlight->GetPosition(tTime);
		DistanceUnit dist = fltPos.distance(intersectPos);
		if(dist <= radius )
		{
			if( pFlight->GetMode() >  OnHeldAtStand  || pFlight->GetPreState().m_fltMode < OnHeldAtStand ) //flight is moving
				return pFlight;
		}
	}	
	return NULL;
}

Point LaneStandIntersectionInSim::GetPosition() const
{
	ASSERT(m_pLane);
	return m_pLane->GetDistancePoint(GetIntersectDistAtLane());
}