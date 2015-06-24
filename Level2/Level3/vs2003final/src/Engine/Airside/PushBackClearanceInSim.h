#pragma once
#include "common/ALTObjectID.h"
#include "common/elaptime.h"

class CPushBackClearanceProcessor;
class StandInSim;
class AirsideFlightInSim;
class LogicRunwayInSim;
class CAirportDatabase;

class PushBackClearanceInSim
{
public:
	PushBackClearanceInSim(void);
	~PushBackClearanceInSim(void);

	void Init(int nPrjId, CAirportDatabase* pDatabase);

	int GetMaxTakeoffQueueFlightCount(LogicRunwayInSim* pLogicRunway);
	int GetMaxDeicingQueueFlightCount();
	ElapsedTime GetMaxSlotTime(AirsideFlightInSim* pFlight);
	int GetMaxInboundFlightCount(StandInSim* pStand);
	int GetMaxOutboundToAssignedRunwayFlightCount();
	int GetMaxOutboundFlightCount();
	ALTObjectID GetInboundStandFamily(StandInSim* pStand);

	double GetClearanceDistanceOnTaxiway();
	ElapsedTime GetTimeClearanceOnTaxiway();
private:
	CPushBackClearanceProcessor* m_pPushBackClearance;

};
