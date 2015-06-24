#pragma once
#include "event.h"

class LandsideVehicleInSim;
class ElapsedTime;
//
//class LandsideVehicleServiceEndEvent :
//	public Event
//{
//public:
//	LandsideVehicleServiceEndEvent(LandsideVehicleInSim *pVehicle, const ElapsedTime& eEventTime);
//	~LandsideVehicleServiceEndEvent(void);
//
//public:
//	LandsideVehicleInSim *m_pVehicle;
//	
//	int process(CARCportEngine* pEngine );
//
//
//	For shut down event
//	virtual int kill (void);
//
//	It returns event's name
//	virtual const char *getTypeName (void) const;
//
//	It returns event type
//	virtual int getEventType (void) const;
//
//	not need to processor the event
//	void CloseEvent(bool bClose);
//	
//	bool IsEventClosed();
//
//protected:
//	bool m_bClosed;
//
//};
