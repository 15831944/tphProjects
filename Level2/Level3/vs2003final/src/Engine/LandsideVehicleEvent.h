#pragma once
#include "event.h"
class LandsideVehicleInSim;

class LandsideVehicleEvent : public Event
{
public:
	LandsideVehicleEvent(LandsideVehicleInSim*  pVehicle);
	virtual int process(CARCportEngine *_pEngine);
	virtual int kill (void);
	

	const char *getTypeName (void) const ;
	int getEventType (void) const ;
	void addEvent();
protected:
	LandsideVehicleInSim* mpVehicle;


	ElapsedTime removeMoveEvent(LandsideVehicleInSim *anElement);
	
};
