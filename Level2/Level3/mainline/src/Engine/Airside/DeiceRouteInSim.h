#pragma once

#include ".\TaxiRouteInSim.h"
class DeiceRouteInSim : public TaxiRouteInSim
{
public:
	DeiceRouteInSim(AirsideResource* pOrign, AirsideResource* pDest);	
	void setAfterRoute(FlightGroundRouteDirectSegList& routes,bool isCyclicRoute);
protected:
	virtual void PlanRoute(AirsideFlightInSim* pFlt,const ElapsedTime& t);
	virtual void FlightExitRoute(AirsideFlightInSim* pFlight, const ElapsedTime& releaseTime);
	TaxiRouteInSim mAfterDeiceRoute; //the route after deice
};