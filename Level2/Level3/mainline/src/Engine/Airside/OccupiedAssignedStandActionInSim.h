#pragma once
#include "common/elaptime.h"
#include "InputAirside/OccupiedAssignedStandAction.h"

class AirsideFlightInSim;
class CAirportDatabase;
class AirportResourceManager;
class AirsideResource;

// struct StandStrategy
// {
// 	StrategyType m_Strategy;
// 	int m_nTempResoure;
// 	ElapsedTime m_tDelayTime ;
// };

class COccupiedAssignedStandActionInSim
{
public:
	COccupiedAssignedStandActionInSim(void);
	~COccupiedAssignedStandActionInSim(void);

	COccupiedAssignedStandStrategy& GetStrategyByFlightType(AirsideFlightInSim* pFlight,  AirportResourceManager* pResManager);
	void Init(int nPrjId,CAirportDatabase* pDatabase);
protected:
	int calculateMatchValue(int valueArray[],int arraySize);

private:
 	COccupiedAssignedStandCriteria* m_pOccupiedAssignedStandCriteria;
};
