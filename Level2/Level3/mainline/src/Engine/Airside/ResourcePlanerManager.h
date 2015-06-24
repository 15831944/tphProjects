#pragma once
#include "../EngineDll.h"
#include "AirspacePlaner.h"
#include "RunwayPlaner.h"
#include "TaxiwayPlaner.h"
#include "GatePlaner.h"

class AirportResource;
class FlightInAirsideSimulation;


class ENGINE_TRANSFER ResourcePlanerManager
{
public:
	ResourcePlanerManager();
	~ResourcePlanerManager(void);


	void Init(int nPrjID, AirsideResource);

	AirsideResource * NextResourceAssign(FlightInAirsideSimulation * pFlight);

protected:
	AirspacePlaner m_airspacePlaner;
	RunwayPlaner m_runwayPlaner;
	TaxiwayPlaner m_taxiwayPlaner; 
	StandPlaner m_gatePlaner;



};
