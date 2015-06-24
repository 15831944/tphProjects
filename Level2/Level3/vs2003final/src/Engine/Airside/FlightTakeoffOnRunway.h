#pragma once

#include "FlightMovementsOnResource.h"


class AirWayPointInSim;
//only handle takeoff movements
class ENGINE_TRANSFER FlightTakeoff : public FlightMovements
{
public:
	explicit FlightTakeoff(FlightInAirsideSimulation * pFlight):FlightMovements(pFlight){}

	virtual void End(Clearance * pClearance);

	//virtual void EndAtWayPoint(AirWayPointInSim  *pWayPt, double spd, double dAlt);

	virtual void EndAtClimbUp();

protected:

};

class ENGINE_TRANSFER FlightPreTakeoff: public FlightMovements
{
public:
	explicit FlightPreTakeoff(FlightInAirsideSimulation * pFlight):FlightMovements(pFlight){}

	virtual void End(Clearance * pClearance);
protected:

};