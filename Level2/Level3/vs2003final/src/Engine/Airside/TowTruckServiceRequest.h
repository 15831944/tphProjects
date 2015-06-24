#pragma once
#include "VehicleServiceRequest.h"
#include "../../InputAirside/VehicleDefine.h"
#include "FlightRouteSegInSim.h"
#include "VehicleRouteInSim.h"

class TaxiRouteInSim;
class AirsideResource;
class TowTruckServiceRequest : public VehicleServiceRequest
{
public:
	TowTruckServiceRequest(TOWOPERATIONTYPE type, AirsideFlightInSim* pFlight);
	~TowTruckServiceRequest(void);

	virtual enumVehicleBaseType GetType()const{ return VehicleType_TowTruck; }	

	AirsideResource* m_pReleasePoint;
	TOWOPERATIONTYPE m_ServiceType;
	TaxiRouteInSim* m_pOutboundRoute;
	VehicleRouteInSim m_ReturnRoute;
	FlightGroundRouteDirectSegList m_vSegments;//get return route the same as complement return pool
	ElapsedTime m_tApplyTime;

};
