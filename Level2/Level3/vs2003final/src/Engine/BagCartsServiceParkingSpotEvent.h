#pragma once
#include "Airside\AirsideMobileElementEvent.h"

class AirsideVehicleInSim;

class CBagCartsServiceParkingSpotEvent :
	public CAirsideMobileElementEvent
{
public:
	CBagCartsServiceParkingSpotEvent(AirsideVehicleInSim *pVehicle);
	~CBagCartsServiceParkingSpotEvent(void);

public:
	virtual int Process();

	//For shut down event
	virtual int kill (void);

	virtual const char *getTypeName (void) const;

	virtual int getEventType (void) const;

protected:

};
