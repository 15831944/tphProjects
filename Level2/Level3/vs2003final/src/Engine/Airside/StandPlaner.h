#pragma once

#include "ResourcePlaner.h"

class StandResourceManager;
class StandInSim;	

class ENGINE_TRANSFER StandPlaner : public ResourcePlaner
{
public:
	StandPlaner(void);
	~StandPlaner(void);

	bool Init(int nPrjID, int nAirportID, StandResourceManager* pAirportRes );

	 virtual  AirsideResource* ResourceAssignment( FlightInAirsideSimulation* pFlight);

	 StandInSim * GetPlanedStand(FlightInAirsideSimulation * pFlight)const;

	 StandResourceManager * m_pStandRes;
};
