#pragma once

#include "ResourcePlaner.h"
#include <vector>
#include "FlightTypeRunwayAssignmentStrategiesInSim.h"
class FlightInAirsideSimulation;
class RunwayResourceManager;
class RunwayInSim;
class RunwayExitInSim;




class ENGINE_TRANSFER RunwayPlaner : public ResourcePlaner
{
public:
	RunwayPlaner(void);
	~RunwayPlaner(void);

	bool Init(int nPrjID, int nAirportID, RunwayResourceManager * pRunwayRes,CAirportDatabase *pAirportDatabase );

	virtual  AirsideResource* ResourceAssignment( FlightInAirsideSimulation* pFlight);


	RunwayExitInSim * GetFlightExit(RunwayInSim * pRunway, FlightInAirsideSimulation* pFlight);
	RunwayExitInSim * GetInExit(FlightInAirsideSimulation *pFlight);


private:

	RunwayResourceManager * m_pRunwayRes;
	CFlightTypeRunwayAssignmentStrategiesInSim m_RunwayStategies;
	CAirportDatabase* m_pAirportDatabase;

};
