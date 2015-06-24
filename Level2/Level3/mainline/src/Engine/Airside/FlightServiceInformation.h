#pragma once
#include "InputAirside/FlightTypeServiceLoactions.h"
#include "FlightServiceRoute.h"
class AirsideFlightInSim;
class CFlightServiceInformation
{
public:
	CFlightServiceInformation(AirsideFlightInSim* pFlightInSim);
	~CFlightServiceInformation(void);

	void SetFltServiceLocations(const FlightTypeServiceLoactions& fltServiceLocations);

	void SetRingRoute(const RingRoad& ringRoad);
	int GetFltServicePointCount(int nVehicleTypeID);
	
	CFlightServiceRoute *GetFlightServiceRoute(){ return m_fltServiceRoute.get();}

protected:
	CFlightServiceRoute::RefPtr m_fltServiceRoute;
};
