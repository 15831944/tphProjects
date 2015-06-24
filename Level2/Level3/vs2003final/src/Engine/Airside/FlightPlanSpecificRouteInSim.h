#pragma once
#include "AirspaceResource.h"
#include "..\..\Common\elaptime.h"
class AirsideFlightInSim;
class AirRouteSegInSim;
class HeadingAirRouteSegInSim;

class FlightPlanSpecificRouteInSim :
	public FlightRouteInSim
{
public:
	FlightPlanSpecificRouteInSim(void);
	~FlightPlanSpecificRouteInSim(void);

public:
	virtual ns_FlightPlan::FlightPlanBase::ProfileType getType();


public:
	virtual ElapsedTime GetFlightTimeInSeg(AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg);
	virtual ElapsedTime GetFlightMaxTimeInSeg(AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg);
	virtual ElapsedTime GetFlightMinTimeInSeg(AirsideFlightInSim* pFlight, AirRouteSegInSim* pSeg);


	virtual ElapsedTime GetHeadingMinTimeInSeg(AirsideFlightInSim *pFlight, HeadingAirRouteSegInSim*pSeg, HeadingAirRouteSegInSim*pNextSeg);
	virtual ElapsedTime GetHeadingMaxTimeInSeg(AirsideFlightInSim *pFlight, HeadingAirRouteSegInSim*pSeg, HeadingAirRouteSegInSim*pNextSeg);

	//STAR
	virtual ElapsedTime CalculateTimeFromLastSegToLogicRunway(AirsideFlightInSim *pFlight);
	virtual ElapsedTime CalculateMinTimeFromLastSegToLogicRunway(AirsideFlightInSim *pFlight);

	//SID
	virtual ElapsedTime CalculateTakeoffTimeFromRwyToFirstSeg( AirsideFlightInSim *pFlight );

};
