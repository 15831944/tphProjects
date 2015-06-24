#pragma once

#include "Engine/EngineDll.h"


class FlightInAirsideSimulation;
class ResourceOccupy;
class Clearance;
class ElapsedTime;
class AirsideResource;

class FlightState;

//interface of the traffic controller
class ENGINE_TRANSFER TrafficController
{
public:
	//assign next clearance to the flight
	virtual Clearance* AssignNextClearance(FlightInAirsideSimulation * pFlight) = 0;

public:
	virtual bool AmendClearance(FlightInAirsideSimulation * pFlight, Clearance* pClearance){ return true; }

	//assign clearance for flight to enter specified resource;
	virtual Clearance * GetEnterResourceClearance(FlightInAirsideSimulation* pFlight,const FlightState& fltstate)
	{
		return NULL;
	}

	// flt enter next resource
	static void ModifyOcuppyTable(FlightInAirsideSimulation * pFligth, AirsideResource * pPreRes, AirsideResource * pNextRes , const ElapsedTime& enterNextResT );

	
};
