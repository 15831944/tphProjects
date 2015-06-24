#pragma once
#include "AirsideMobileElementEvent.h"

class StandResourceManager;
class StandInSim;
class Clearance;
class BridgeConnector;

class AirsideFlightInSim;
class ENGINE_TRANSFER DepatureFlightTerminalEvent: public CAirsideMobileElementEvent
{
public:
	explicit DepatureFlightTerminalEvent(AirsideFlightInSim* flt, StandResourceManager* pStandRes);
	~DepatureFlightTerminalEvent(void);

	virtual int Process();
	bool GetStandClearence(StandInSim* pStand,ElapsedTime& tStartTime, ElapsedTime& tEndTime,Clearance& theClearance);
	bool WaitAtStand(StandInSim* pStand,ElapsedTime& tStartTime, ElapsedTime& tEndTime,Clearance& theClearance);

	//For shut down event
	virtual int kill (void) { return 0; }

	virtual const char *getTypeName (void) const { return "DepatureFlightTerminal";}

	virtual int getEventType (void) const {return DepatureFlightStandEvent;}

private:
	void DisConnectBridge()const;
	BridgeConnector* GetBridgeConnector()const;
private:
	AirsideFlightInSim* m_pCFlight;
	StandResourceManager* m_pStandResource;;
};
