#pragma once
#include "../EngineDll.h"
#include "common\elaptime.h"
#include "common\util.h"
#include "AirsideController.h"
#include "AirspaceController.h"
#include "RunwayController.h"
#include "TaxiwayController.h"
#include "GateController.h"


class FlightInAirsideSimulation;


class ENGINE_TRANSFER AirsideControllerManager
{
public:
	AirsideControllerManager(void);
	~AirsideControllerManager(void);

	ResourceOccupy FlightNextResourceAssignment( FlightInAirsideSimulation* pFlight,ResourceOccupy resource);

private:
	AirspaceController* m_airspaceController;
	TaxiwayController* m_taxiwayController;
	RunwayController* m_runwayController;
	GateController* m_gateController;

};
