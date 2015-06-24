#pragma once
#include "event.h"

class LandsideResidentVehicleInSim;
class ElapsedTime;

class LandsideResidentVehicleGetOffEvent :
	public Event
{
public:
	LandsideResidentVehicleGetOffEvent(LandsideResidentVehicleInSim *pVehicle, const ElapsedTime& eEventTime);
	~LandsideResidentVehicleGetOffEvent(void);

	int process(CARCportEngine* pEngine );


	//For shut down event
	virtual int kill (void);

	//It returns event's name
	virtual const char *getTypeName (void) const;

	//It returns event type
	virtual int getEventType (void) const;

private:
	LandsideResidentVehicleInSim* m_pResidentVehicleInSim;
};
