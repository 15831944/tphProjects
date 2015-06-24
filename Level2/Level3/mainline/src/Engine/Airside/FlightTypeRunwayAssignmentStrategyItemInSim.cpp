#include "StdAfx.h"
#include ".\flighttyperunwayassignmentstrategyiteminsim.h"

#include "./InputAirside/FlightTypeRunwayAssignmentStrategyItem.h"
#include "./InputAirside/TimeRangeRunwayAssignmentStrategyItem.h"
#include "./Common/elaptime.h"
#include "FlightInAirsideSimulation.h"
#include "./InputAirside/StrategyType.h"
#include "ManagedRunwayStrategyInSim.h"
#include "RunwayResourceManager.h"
#include "ClosetRunwayStrategyInSim.h"
#include "RunwayInSim.h"
#include "RunwayResourceManager.h"
#include "FlightPlanedRunwayStrategyInSim.h"

CFlightTypeRunwayAssignmentStrategyItemInSim::CFlightTypeRunwayAssignmentStrategyItemInSim(void)
{

}

CFlightTypeRunwayAssignmentStrategyItemInSim::~CFlightTypeRunwayAssignmentStrategyItemInSim(void)
{
}

RunwayPortInSim* CFlightTypeRunwayAssignmentStrategyItemInSim::GetRunway(FlightInAirsideSimulation* pFlight,RunwayResourceManager * pRunwayRes)
{
	ElapsedTime tTime = pFlight->GetCurrentTime();
	int nItemCount = m_pFlightTypeRunwayAssignmentStrategyItem->GetTimeRangeStrategyItemCount();
	RunwayPortInSim* pRunwayPort = NULL;
	for (int i =0; i< nItemCount; i++)
	{
		TimeRangeRunwayAssignmentStrategyItem* pItem = m_pFlightTypeRunwayAssignmentStrategyItem->GetTimeRangeStrategyItem( i);
		if (tTime >= pItem->getStartTime()&& tTime <= pItem->getEndTime())
		{
			RunwayAssignmentStrategyType StrateType = pItem->getStrategyType();
			switch(StrateType)
			{
				case FlightPlannedRunway:
					{
						CFlightPlanedRunwayStrategyInSim StrategyInSim;
						StrategyInSim.SetFlight(pFlight);
						pRunwayPort = StrategyInSim.GetRunway(pRunwayRes);
						break;
					}
				case ClosestRunway:
					{		
						CClosetRunwayStrategyInSim StrategyInSim;
						StrategyInSim.SetFlightPosition(pFlight->GetCurrentPosition()) ;
						pRunwayPort = StrategyInSim.GetRunway(pRunwayRes);
						break;
					}
				case ManagedAssignment:
					{		
						CManagedRunwayStrategyInSim StrategyInSim;
						StrategyInSim.SetRunwayAssignmentPercentItems(pItem->GetManagedAssignmentRWAssignPercent());
						pRunwayPort = StrategyInSim.GetRunway(pRunwayRes);
						break;
					}

			default:
				break;
			}
			break;
		}

	}
	return pRunwayPort;
}
