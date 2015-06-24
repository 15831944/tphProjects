#ifndef __AIRSIDE_PROCESSGENERATOR_DEF
#define __AIRSIDE_PROCESSGENERATOR_DEF


#include "../EngineDll.h"
#include "../EngineConfig.h"
#include "Event.h"


NAMESPACE_AIRSIDEENGINE_BEGINE

class AirportModel;
class SimFlight;
class ENGINE_TRANSFER ProcessGenerator{

public:
	
	void GenProcess(SimFlight* the_flight,AirportModel * pAirport );


};













NAMESPACE_AIRSIDEENGINE_END











#endif