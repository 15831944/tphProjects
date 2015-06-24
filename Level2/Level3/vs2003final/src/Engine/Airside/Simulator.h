#ifndef __AIRSIDE_SIMULATOR_DEF
#define __AIRSIDE_SIMULATOR_DEF


#include "../EngineDll.h"
#include "../EngineConfig.h"
#include "../SimClock.h"
#include "Flight.h"
//Simulate the Model and change the Flight FEL to PEL 

NAMESPACE_AIRSIDEENGINE_BEGINE



class ENGINE_TRANSFER Simulator{
	
public:
	Simulator();

	void setAirport(AirportModel* pAirport){ m_pAirport = pAirport; }
	void setFlights(FlightList* pflights){ m_pflights = pflights; }
	void setSimClock(SimClock* pSimClk){ m_pSimClock = pSimClk; }
	
	bool doSim();
	
	bool Determinate(FlightEvent* theEvent);

	//return the next time 
	SimClock::TimeType getNextTime(){ return m_minNextTime; }
	void setNextTime(const SimClock::TimeType&  nextEventTime){ m_minNextTime = nextEventTime; } 

private:
	AirportModel * m_pAirport;
	FlightList * m_pflights;
	SimClock * m_pSimClock;
	
	SimClock::TimeType m_minNextTime;
	
};




NAMESPACE_AIRSIDEENGINE_END


#endif