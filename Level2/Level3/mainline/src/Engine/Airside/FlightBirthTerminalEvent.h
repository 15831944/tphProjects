#pragma once


#include "AirsideMobileElementEvent.h"

class StandResourceManager;
class StandInSim;
class Clearance;

class AirsideFlightInSim;
class ENGINE_TRANSFER FlightBirthTerminalEvent : public CAirsideMobileElementEvent
{
public:
	explicit FlightBirthTerminalEvent(AirsideFlightInSim* flt, StandResourceManager* pStandRes);

	virtual int Process();
	bool GetStandClearence(StandInSim* pStand,const ElapsedTime& tStartTime, const ElapsedTime& tEndTime,Clearance& theClearance);

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "FlightBirthTerminal";}

	virtual int getEventType (void) const {return FlightBirth;}


private:
	AirsideFlightInSim* m_pCFlight;
	StandResourceManager* m_pStandResource;;
};
