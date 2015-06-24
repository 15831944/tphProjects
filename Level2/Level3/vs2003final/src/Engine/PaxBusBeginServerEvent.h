#pragma once
#include "Airside/AirsideMobileElementEvent.h"
class CPaxGenerator ;
class CAirsidePaxBusInSim ;
class CPaxBusBeginServerEvent : public CAirsideMobileElementEvent
{
public:
	CPaxBusBeginServerEvent(CAirsidePaxBusInSim* _bus,ElapsedTime _time,CARCportEngine *_pEngine);
	virtual ~CPaxBusBeginServerEvent(void);
	int Process() ;
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "PaxBusBeginServer";}

	virtual int getEventType (void) const {return VehicleServiceGate;}

private:
	CAirsidePaxBusInSim* m_bus ;
	//Terminal* p_terminal ;
	CARCportEngine *m_pEngine;
protected:

	void NoticePaxTakeOffBusToArrGate() ;
	//notice paxnum take on bus from the holding area 
	void NoticePaxTakeOnBus(int paxNum) ;
};
