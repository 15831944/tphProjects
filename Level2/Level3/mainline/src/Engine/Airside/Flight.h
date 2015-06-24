#ifndef __AIRSIDE_FLIGTH_DEF
#define __AIRSIDE_FLIGTH_DEF
#include "../EngineDll.h"
#include "../EngineConfig.h"

#include "Entity.h"
#include "Event.h"
#include <vector>

#include "AirportModel.h"
#include "../../Inputs/flight.h"

class FlightSchedule;

NAMESPACE_AIRSIDEENGINE_BEGINE

class ENGINE_TRANSFER SimFlight : public Referenced
{

public:
	SimFlight();
	//SimFlight(ACType* fltType):m_ACType(fltType){}
	virtual ~SimFlight();

	//void setACType(ACType* fltType){ m_ACType = fltType; }

	
	FlightEventList* getFEL(){ return &m_FEL; }
	FlightEventList* getPEL(){ return &m_PEL; }

	//void setArrivalTime(const SimClock::TimeType& _arrtm){ m_arrTime = _arrtm; }
	//void setDepartureTime(const SimClock::TimeType& _dpttm){ m_depTime = _dpttm; }
	//void setDepartureRoute(CString _dpRoute){m_strDepartureRoute = _dpRoute	;}
	//void setArrivalRoute(CString _arRoute){m_strArriveRoute = _arRoute;}

	SimClock::TimeType getArrivalTime()const;
	SimClock::TimeType getDepartureTime()const ;
	CString getDepartureRoute()const ;
	CString getArrivalRoute()const ;

	SimClock::TimeType getBirthTime();
	SimClock::TimeType getLastTime();
	//ACType* getFlightType();
	
	Flight *GetCFlight(){ return m_pflight;} 
	void SetCFlight(Flight * _pflight){ m_pflight = _pflight; }

private:
	//
	FlightEventList m_FEL;                //future events list
	FlightEventList m_PEL;                //passed events list	
	//properties of the flight
	//SimClock::TimeType m_arrTime;
	//SimClock::TimeType m_depTime;
	//CString m_strDepartureRoute;
	//CString m_strArriveRoute;
	//CString m_strGateID;

	//ref_ptr<ACType> m_ACType;

	Flight * m_pflight;
};
//
class FlightList : public std::vector< ref_ptr<SimFlight> >
{
public:
	bool getTimeRange(SimClock::TimeType&start_time, SimClock::TimeType& end_time );
	FlightEvent* GetFirstEvent();
	FlightEvent* GetNextFlightEvent(const FlightEvent& currentEvent);
};


//generate flights from the flight schedule
class ENGINE_TRANSFER FlightGenerator{
public:
	void Initialize( FlightSchedule* pflightSch );
	//get next flight 
	void GenerateFlights(FlightList& FltList,const AirportModel& simModel);

private:
    FlightSchedule * m_pfltSch;
	
};

NAMESPACE_AIRSIDEENGINE_END
#endif