#pragma once


#include "TrafficController.h"

class AirportResourceManager;
class RunwayTrafficController;
class TaxiwayTrafficController;
class StandTrafficController;
class AirsideTrafficController;
class AirsideSimConfig;
#include "FlightInAirsideSimulation.h"

//Group of Airport Element Controller
class ENGINE_TRANSFER AirportTrafficController : public TrafficController
{
public:
	explicit AirportTrafficController(AirsideTrafficController * pAirsideController);

public :
	bool Init(int nPrjID, AirportResourceManager* pResources,const AirsideSimConfig& simconf, CAirportDatabase* pAirportDatabase);

	virtual Clearance* AssignNextClearance(FlightInAirsideSimulation * pFlight);
	virtual bool AmendClearance(FlightInAirsideSimulation * pFlight, Clearance* pClearance);

	TaxiwayTrafficController * GetTaxiwayTrafficController(){ return m_pTaxiController; }
	RunwayTrafficController * GetRunwayTrafficController(){ return m_pRunwayController; }
	StandTrafficController * GetStandTrafficController(){ return m_pStandController; } 

	AirsideTrafficController * GetAirsideTrafficController(){ return m_pAirsideController; }

	TrafficController * GetAppropriateController(AirsideResource * pResource);

	ResourceOccupy ConflictSolution(AirsideResource* pNextResource,FlightInAirsideSimulation * pFlight);
	

	virtual Clearance * GetEnterResourceClearance(FlightInAirsideSimulation* pFlight,const FlightState& fltstate );
protected:
	TaxiwayTrafficController*  m_pTaxiController;
	RunwayTrafficController* m_pRunwayController;
	StandTrafficController * m_pStandController;
	
	AirportResourceManager * m_pAirportRes;

	AirsideTrafficController* m_pAirsideController;

};
