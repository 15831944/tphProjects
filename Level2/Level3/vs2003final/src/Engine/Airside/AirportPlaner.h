#pragma once

#include "ResourcePlaner.h"
#include "./RunwayPlaner.h"
//#include "./TaxiwayPlaner.h"
#include "./StandPlaner.h"

class AirportResourceManager;

class ENGINE_TRANSFER AirportPlaner : public ResourcePlaner
{
friend class TaxiwayPlaner;
friend class StandPlaner;
friend class RunwayPlaner;

public:

	AirportPlaner();
	
	bool Init(int nPrjID, int nAirportID, AirportResourceManager* pAirportRes ,CAirportDatabase *pAirportDatabase  );
	
	AirsideResource* ResourceAssignment( FlightInAirsideSimulation* pFlight);

protected:
	AirportResourceManager* m_pAirportRes;
public:
	RunwayPlaner m_RunwayPlaner;
	//TaxiwayPlaner m_TaxiwayPlaner;
	StandPlaner m_StandPlaner;

};