#include "StdAfx.h"
#include ".\closetrunwaystrategyinsim.h"
#include "RunwayResourceManager.h"
#include "RunwayInSim.h"
#include "AirsideFlightInSim.h"
#include "StandInSim.h"

CClosetRunwayStrategyInSim::CClosetRunwayStrategyInSim()
{

}

CClosetRunwayStrategyInSim::~CClosetRunwayStrategyInSim()
{

}

LogicRunwayInSim* CClosetRunwayStrategyInSim::GetRunway(RunwayResourceManager* pRunwayRes)
{

	//if (m_pFlight->GetMode() >= OnCruiseArr || m_pFlight->GetMode() >= OnHeldAtStand)
	//{
		int nRunwayCount = pRunwayRes->GetRunwayCount();
		LogicRunwayInSim* pLogicRunway = NULL;
		double Dist = -1.0;
		for (int i = 0;i < nRunwayCount;i++ )
		{
			CPoint2008 Portpos1 = pRunwayRes->GetRunwayByIndex(i)->GetLogicRunway1()->GetDistancePoint( 0 );
			CPoint2008 Portpos2 = pRunwayRes->GetRunwayByIndex(i)->GetLogicRunway2()->GetDistancePoint( 0 );

			CPoint2008 FlightPos = m_pFlight->GetPosition();
			if (m_pFlight->IsArrivingOperation() && m_pFlight->GetPlanedParkingStand(ARR_PARKING) )
			{
				FlightPos = m_pFlight->GetPlanedParkingStand(ARR_PARKING)->GetDistancePoint(0);
			}
			if(m_pFlight->IsDepartingOperation()&&  m_pFlight->GetPlanedParkingStand(DEP_PARKING))
			{
				FlightPos = m_pFlight->GetPlanedParkingStand(DEP_PARKING)->GetDistancePoint(0);
			}

			double _dist1 = FlightPos.distance(Portpos1);
			double _dist2 = FlightPos.distance(Portpos2);

			if ( Dist < 0.0 )
				Dist = _dist1 < _dist2 ? _dist1:_dist2;

			if (Dist >= _dist1)
				pLogicRunway = pRunwayRes->GetRunwayByIndex(i)->GetLogicRunway1();
			if (Dist >= _dist2)
				pLogicRunway = pRunwayRes->GetRunwayByIndex(i)->GetLogicRunway2();
		}
		return pLogicRunway;
	//}
	//return NULL;

}