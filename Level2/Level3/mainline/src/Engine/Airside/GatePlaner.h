#pragma once
#include "../EngineDll.h"
#include "common\elaptime.h"
#include "common\util.h"
#include "ResourcePlaner.h"

class AirportResourceManager;

class ENGINE_TRANSFER GatePlaner : public ResourcePlaner
{
public:
	GatePlaner(void);
	~GatePlaner(void);

	 virtual  AirsideResource* ResourceAssignment( FlightInAirsideSimulation* pFlight);

};
