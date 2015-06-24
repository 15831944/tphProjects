#pragma once


#ifndef _TAKEOFFSEQUENCINGINSIM_H
#define _TAKEOFFSEQUENCINGINSIM_H

#include "../../Common/elaptime.h"
#include "../../InputAirside/TakeoffSequencing.h"

class AirsideFlightInSim;
class RunwayExitInSim;
class QueueToTakeoffPosList;
class QueueToTakeoffPos;
class CAirportDatabase ;
class AirRouteNetworkInSim;

//make decision which flight takeoff first
class TakeoffSequencingInSim
{
public:
	TakeoffSequencingInSim();
	~TakeoffSequencingInSim(void);
	
	void Init(int nPrjID, const AirRouteNetworkInSim* pAirRouteNetwork, CAirportDatabase* _airportDB);

	bool IsWaitingInQueue(AirsideFlightInSim* pFlight, ElapsedTime tCurTime);

private:
	TakeOffSequenceInfo* GetRelativeTakeoffSequenceAndIndex( AirsideFlightInSim* pFlight,int& nSeqIdx);
	AirsideFlightInSim* GetEarliestEnterFlightInQueue(QueueToTakeoffPosList& QueueList, const std::vector<AirsideFlightInSim*>& vFlights);
	const CPriorityData& GetNextPriorityData(TakeOffSequenceInfo* pSequenceInfo, int nNextIdx);
	void GetSatisfiedTakeoffFlight(const CPriorityData& _priorityData,
									const std::vector<RunwayExitInSim*>& vTakeoffPosList,
									QueueToTakeoffPosList& QueueList,
									const ElapsedTime& tCurTime,
									std::vector<AirsideFlightInSim*>& vSatisfiedFlights);

	void GetPriorityFlights(const CPriorityData& _priorityData,
							const std::vector<RunwayExitInSim*>& vTakeoffPosList,
							QueueToTakeoffPosList& QueueList,
							const ElapsedTime& tCurTime,
							std::vector<AirsideFlightInSim*>& vPriorityFlights );
	
private:
	AirRouteNetworkInSim* m_pAirRouteNetwork;
	TakeoffSequencingList* m_pTakeoffSequencing;
	QueueToTakeoffPos* m_pPreviousTakeoffQueue;
};

#endif