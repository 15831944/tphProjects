#pragma once

#include "AirsideMobileElementEvent.h"
#include "../MobileElementGenerator.h"
class Flight;
class ENGINE_TRANSFER FlightArrivalStandEvent : public CAirsideMobileElementEvent
{
public:
	FlightArrivalStandEvent(AirsideFlightInSim* AirsideFlt);
	virtual ~FlightArrivalStandEvent(void);

	virtual int process(CARCportEngine* _pEngine);
	virtual int Process(){	return 0; }
	void SetArrivalFlight(Flight* pFlight);
	void SetActualDepartureTime(const ElapsedTime& dtTime);
	bool IsAirsideMobileEllementEvent(){ return false;}

	virtual const char *getTypeName (void) const
	{
		return "FlightArrivalStand";
	}

    virtual int getEventType (void) const
	{
		return FlightArrivalStand;
	}

private:
	void ConnectBridge(CARCportEngine* _pEngine)const;
	BridgeConnector* GetBridgeConnector()const;
protected:
	Flight* m_pFlight;
	AirsideFlightInSim* m_pAirsideFlt ;
	ElapsedTime m_dtActualDepTime;
};
