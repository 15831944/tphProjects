#pragma once

#include "FlightInAirsideSimulation.h"
#include "FlightMovementsOnResource.h"

class ENGINE_TRANSFER FlightMovementsToClearance :public FlightMovements
{

public:
	FlightMovementsToClearance(FlightInAirsideSimulation * pFlight):FlightMovements(pFlight){}

	void End( Clearance * pClearance );

protected:


};