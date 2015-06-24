#pragma once
#include "airsidemobileelementevent.h"
class AirsideVehicleInSim;

class CPaxBusServiceGateEvent :
	public CAirsideMobileElementEvent
{
public:
	CPaxBusServiceGateEvent(AirsideVehicleInSim *pVehicle);
	~CPaxBusServiceGateEvent(void);

public:
	virtual int Process();

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "VehicleServiceGate";}

	virtual int getEventType (void) const {return VehicleServiceGate;}

protected:
	AirsideVehicleInSim * m_pVehicle;
};
