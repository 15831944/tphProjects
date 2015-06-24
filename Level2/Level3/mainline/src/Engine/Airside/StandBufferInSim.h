#pragma once
#include "Common/elaptime.h"

class CParkingStandBuffer;
class CAirportDatabase;
class AirsideFlightInSim;
class StandInSim;

class StandBufferInSim
{
public:
	StandBufferInSim(void);
	~StandBufferInSim(void);

	void Init(int nPrjId, CAirportDatabase* pDatabase);
	ElapsedTime GetStandBufferTime(AirsideFlightInSim* pFlight,StandInSim* pStand);

private:
	CParkingStandBuffer* m_pParkingStandBuffer;

};
