#pragma once


#include <InputAirside/DeiceAndAnticeManager.h>
#include <common/Point2008.h>
//flight 
class TaxiRouteInSim;
class AirsideResource;
class DeicepadGroup;
class DeicepadGroupEntry;

class FlightDeiceStrategyDecision //flight decide how to deice
{
public:
	FlightDeiceStrategyDecision();
	AirsideResource* m_deicePlace;
	DeicepadGroup* m_pDeicepadGroup;
	DeicepadGroupEntry* m_pDeicepadGroupEntry;
	DistanceUnit m_DistInResource;
	PriorityType mtype;    //
	PostionMethod mPosMethod;
	EngineState mEngState;  //when deicing turn engine on /off
	TaxiRouteInSim* m_deiceRoute; 
	ElapsedTime m_InspecAndHoldTime;
	bool bAfterDeice;

};
