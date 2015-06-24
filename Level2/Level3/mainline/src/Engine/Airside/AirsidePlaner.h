#pragma once
#include "ResourcePlaner.h"
#include "AirspacePlaner.h"
#include "AirportPlaner.h"


class AirsideResourceManager;

class ENGINE_TRANSFER AirsidePlaner : public ResourcePlaner
{
public :
	bool Init(int nPrjID,InputTerminal *pInterm, AirsideResourceManager * pAirsideRes ,CAirportDatabase *pAirportDatabase );

	AirsideResource* ResourceAssignment( FlightInAirsideSimulation* pFlight) ;

	AirspacePlaner  *GetAirspacePlaner();
	AirportPlaner * GetAirportPlaner(int nAirportID);
	double getFlightPlanSpeedInWaypoint(FlightInAirsideSimulation *pFlight, int id);
	

	typedef std::map<int , AirportPlaner> AirportPlanerList;
protected:
	AirspacePlaner m_AirspacePlaner	;
	AirportPlanerList m_vAirportPlaner;

};
