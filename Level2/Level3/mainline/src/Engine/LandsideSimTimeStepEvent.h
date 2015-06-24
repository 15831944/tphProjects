#pragma once
#include "event.h"
class LandsideVehicleInSim;
class LandsideSimTimeStepManager;
class LandsideSimTimeStepEvent : public Event
{
public:
	LandsideSimTimeStepEvent(const ElapsedTime& timeStep, LandsideSimTimeStepManager *pTimeStepManager);

	virtual int process(CARCportEngine *_pEngine);
	virtual int kill (void);


	const char *getTypeName (void) const ;
	int getEventType (void) const ;
	void addEvent();
protected:
	ElapsedTime removeMoveEvent();

protected:
	ElapsedTime m_eTimeStep;

	LandsideSimTimeStepManager *m_pTimeStepManager;

};
