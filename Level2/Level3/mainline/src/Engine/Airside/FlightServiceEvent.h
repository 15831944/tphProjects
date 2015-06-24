#pragma once

#include "AirsideMobileElementEvent.h"
class AirsideVehicleInSim;

class CFlightServiceEvent :
	public CAirsideMobileElementEvent
{
public:
	CFlightServiceEvent(AirsideVehicleInSim *pVehicle);
	~CFlightServiceEvent(void);

public:
	virtual int Process();

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "VehicleService";}

	virtual int getEventType (void) const {return VehicleService;}

protected:
	AirsideVehicleInSim * m_pVehicle;
};
