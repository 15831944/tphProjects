#pragma once
#include "AirsideMobileElementEvent.h"


class AirsideVehicleInSim;

class ENGINE_TRANSFER VehicleMoveEvent : public CAirsideMobileElementEvent
{
public:
	int Process();

	VehicleMoveEvent(AirsideVehicleInSim * pVehicle);//{ m_pVehicle = pVehicle; }

	AirsideVehicleInSim * getVehicle() const { return m_pVehicle ; } 

	virtual const char *getTypeName (void) const { return "VehicleMove";}

	virtual int getEventType (void) const {return VehicleMove;}

protected:
	AirsideVehicleInSim * m_pVehicle;


};


class ENGINE_TRANSFER VehicleServiceEvent : public CAirsideMobileElementEvent
{
public:
	int Process();
	VehicleServiceEvent(AirsideVehicleInSim* pVehicle);

	inline AirsideVehicleInSim* getVehicle()const{ return (AirsideVehicleInSim*)m_pMobileElement; }

	virtual const char *getTypeName (void) const { return "VehicleService";}

	virtual int getEventType (void) const {return VehicleService;}

};
