#pragma once
#include "airsidemobileelementevent.h"
#include "../MobileElementGenerator.h"
class Flight;
class ENGINE_TRANSFER FlightExitStandEvent :
	public CAirsideMobileElementEvent
{
public:
	FlightExitStandEvent(AirsideFlightInSim* AirsideFlt);
	~FlightExitStandEvent(void);

	virtual int process(CARCportEngine* _pEngine);
	virtual int Process(){	return 0; }
	void SetExitFlight(Flight* pFlight);
	void SetActualDepartureTime(const ElapsedTime& dtTime);
	bool IsAirsideMobileEllementEvent(){ return false;}

	virtual const char *getTypeName (void) const
	{
		return "FlightExitStand";
	}

	virtual int getEventType (void) const
	{
		return FlightExitStand;
	}

protected:
	Flight* m_pFlight;
	AirsideFlightInSim* m_pAirsideFlt ;
	ElapsedTime m_dtActualDepTime;






};
