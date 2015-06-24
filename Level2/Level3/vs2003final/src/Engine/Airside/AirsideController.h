#pragma once


#include "TrafficController.h"
#include "AirportTrafficController.h"
#include "AirspaceTrafficController.h"
//#include "AirsidePlaner.h"

class AirsideResourceManager;
class AirspacePlaner;
class AirsideSimConfig;
class InputTerminal;

class  ENGINE_TRANSFER AirsideTrafficController  : public TrafficController
{
public:
	AirsideTrafficController(void);
	virtual ~AirsideTrafficController(void);

	bool Init(int nPrjID, AirsideResourceManager* pResources,const AirsideSimConfig& simconf, CAirportDatabase *pAirportDatabase);

	virtual Clearance* AssignNextClearance(FlightInAirsideSimulation * pFlight);

	AirportTrafficController * GetAirportController();
	AirportTrafficController * GetAirportController(int nAirportID );
	AirspaceTrafficController * GetAirspaceController();
	
	AirsideResourceManager* GetAirsideResource(){ return m_pAirsideRes; }

	virtual Clearance * GetEnterResourceClearance(FlightInAirsideSimulation* pFlight,const FlightState& fltstate );

protected:
	
	std::map<int, AirportTrafficController*> m_vAirportController;
	AirspaceTrafficController* m_pAirspaceController;
	TrafficController* GetAppropriateController(FlightInAirsideSimulation * pFlight);
	AirsideResourceManager * m_pAirsideRes;
	
	
};
