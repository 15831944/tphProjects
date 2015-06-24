#include "StdAfx.h"
#include ".\runwayrelatedintersectionnodeinsim.h"
#include "RunwayInSim.h"
#include "RunwaysController.h"
#include "AirsideFlightInSim.h"

RunwayRelatedIntersectionNodeInSim::RunwayRelatedIntersectionNodeInSim(CRunwaySystem* pRunwaySystem,  LogicRunwayInSim* pRunway1, LogicRunwayInSim* pRunway2, AIRCRAFTOPERATIONTYPE eACOpType)
{
	m_pRunwaySystem = pRunwaySystem;
	m_pRunway1 = pRunway1;
	m_pRunway2 = pRunway2;

	if (eACOpType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED)
		m_ACOpType = LANDING_LANDING;

	if ( eACOpType == AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF)
		m_ACOpType = TAKEOFF_TAKEOFF;

	if ( eACOpType == AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF || eACOpType == AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED)
		m_ACOpType = LANDING_TAKEOFF;

}

RunwayRelatedIntersectionNodeInSim::~RunwayRelatedIntersectionNodeInSim(void)
{
}

ElapsedTime RunwayRelatedIntersectionNodeInSim::GetFlightDelayTime(AirsideFlightInSim* pOpFlight, ElapsedTime tTimeAtPoint )
{


	AirsideMobileElementMode eOpMode = OnLanding;
	if (pOpFlight->IsDepartingOperation())
		eOpMode = OnTakeoff;

	if (m_vOccupyTable.empty())
	{
		AddNewInstance(pOpFlight,eOpMode,tTimeAtPoint);
		return 0L;
	}

	ElapsedTime tSepToAfterFlt = 0L;
	ElapsedTime tSepToFrontFlt = 0L;
	ElapsedTime tDelay = 0L;
	ElapsedTime tAvailableTime = tTimeAtPoint;
		
	int nSize = (int)m_vOccupyTable.size();
	for (int i = 0; i < nSize-1; i++)
	{
		RunwayRelatedIntersectionNodeOccupyInstance frontIns = m_vOccupyTable.at(i);
		RunwayRelatedIntersectionNodeOccupyInstance afterIns = m_vOccupyTable.at(i+1);

		if (tAvailableTime > afterIns.m_tOccTime)
		{
			if (i < nSize -2)		
				continue;

			if (afterIns.m_pOccFlight == pOpFlight || afterIns.m_pOccFlight->GetMode() == OnTerminate)	// same flight instance or terminated flight, neglect
				continue;

			// later than last instance
			tSepToFrontFlt = m_pRunwaySystem->GetSeperationTime(pOpFlight, afterIns.m_pOccFlight, GetAcOperationType(eOpMode, afterIns.m_eMode));

			if (afterIns.m_tOccTime + tSepToFrontFlt > tAvailableTime)
				tAvailableTime = afterIns.m_tOccTime + tSepToFrontFlt;

			break;
		}

		if (i == 0 && tAvailableTime < frontIns.m_tOccTime)		//earlier than first instance
		{
			if (frontIns.m_pOccFlight == pOpFlight|| frontIns.m_pOccFlight->GetMode() == OnTerminate)		//same flight
				break;

			tSepToAfterFlt = m_pRunwaySystem->GetSeperationTime(frontIns.m_pOccFlight, pOpFlight, GetAcOperationType(frontIns.m_eMode,eOpMode));

			if (tAvailableTime + tSepToAfterFlt <= frontIns.m_tOccTime)		//can insert before first instance
				break;
		}

		if (frontIns.m_pOccFlight != pOpFlight && frontIns.m_pOccFlight->GetMode() != OnTerminate)
		{
			tSepToFrontFlt = m_pRunwaySystem->GetSeperationTime(pOpFlight, frontIns.m_pOccFlight, GetAcOperationType(eOpMode, frontIns.m_eMode));

			if (frontIns.m_tOccTime + tSepToFrontFlt > tAvailableTime)
				tAvailableTime = frontIns.m_tOccTime + tSepToFrontFlt;
		}

		if (afterIns.m_pOccFlight != pOpFlight && afterIns.m_pOccFlight->GetMode() != OnTerminate)
		{
			tSepToAfterFlt = m_pRunwaySystem->GetSeperationTime(afterIns.m_pOccFlight, pOpFlight, GetAcOperationType(afterIns.m_eMode,eOpMode));

			if (tAvailableTime + tSepToAfterFlt > afterIns.m_tOccTime)
			{
				if (i <nSize -2)	//the after instance is not the last one
					continue;

				// later than last instance
				tSepToFrontFlt = m_pRunwaySystem->GetSeperationTime(pOpFlight, afterIns.m_pOccFlight, GetAcOperationType(eOpMode, afterIns.m_eMode));

				if (afterIns.m_tOccTime + tSepToFrontFlt > tAvailableTime)
					tAvailableTime = afterIns.m_tOccTime + tSepToFrontFlt;
			}
		}

		break;		//can insert between two instances,stop
	}

	//if (eOpMode == OnLanding || (eOpMode == OnTakeoff && tAvailableTime == tTimeAtPoint))
	{
		AddNewInstance(pOpFlight,eOpMode,tAvailableTime);
	}


	return tAvailableTime - tTimeAtPoint;
}

AIRCRAFTOPERATIONTYPE RunwayRelatedIntersectionNodeInSim::GetAcOperationType( AirsideMobileElementMode eTrailMode,  AirsideMobileElementMode eLeadMode )
{
	if (eTrailMode == OnLanding)
	{
		if (eLeadMode == OnLanding)
			return AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED;
		else
			return AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF;
	}

	if (eLeadMode == OnTakeoff)
		return AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF;

	return AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED;
}

//bool TimeCompare(RunwayRelatedIntersectionNodeInSim::RunwayRelatedIntersectionNodeOccupyInstance fIns, RunwayRelatedIntersectionNodeInSim::RunwayRelatedIntersectionNodeOccupyInstance sIns)
//{
//	if (fIns.m_tOccTime <= sIns.m_tOccTime)
//		return true;
//
//	return false;
//}

void RunwayRelatedIntersectionNodeInSim::AddNewInstance( AirsideFlightInSim* pOccFlight, AirsideMobileElementMode eMode, ElapsedTime tOccTime )
{
	size_t nSize = m_vOccupyTable.size();
	for (size_t i = 0; i < nSize; i++)
	{
		if (m_vOccupyTable.at(i).m_pOccFlight == pOccFlight && m_vOccupyTable.at(i).m_eMode == eMode)
		{
			m_vOccupyTable.erase(m_vOccupyTable.begin() + i);
			break;
		}
	}

	RunwayRelatedIntersectionNodeOccupyInstance NewIns;
	NewIns.m_pOccFlight = pOccFlight;
	NewIns.m_eMode = eMode;
	NewIns.m_tOccTime = tOccTime;

	m_vOccupyTable.push_back(NewIns);

	std::sort(m_vOccupyTable.begin(), m_vOccupyTable.end()/*, TimeCompare*/);

}

bool RunwayRelatedIntersectionNodeInSim::IsSameNode( const LogicRunwayInSim* pRwy1,const LogicRunwayInSim* pRwy2, const CPoint2008& pos, AIRCRAFTOPERATIONTYPE eOpType )
{
	if (pRwy1 != m_pRunway1 && pRwy1 != m_pRunway2)
		return false;

	if (pRwy2 != m_pRunway1 && pRwy2 != m_pRunway2)
		return false;

	if (pos.distance(m_Position) > 100.0)
		return false;

	if (m_ACOpType == LANDING_LANDING && eOpType != AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED)
		return false;

	if (m_ACOpType == TAKEOFF_TAKEOFF && eOpType != AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_TAKEOFF)
		return false;

	if (m_ACOpType == LANDING_TAKEOFF && (eOpType != AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF && eOpType != AIRCRAFTOPERATION_TYPE_TAKEOFF_BEHIND_LANDED))
		return false;

	return true;

}