#pragma once


#include "../../InputAirside/ConflictResolution.h"

class AirsideFlightInSim;
class CAirportDatabase;
class CTaxiwayConflictResolutionInEngine
{
public:
	CTaxiwayConflictResolutionInEngine(int nProjID,CAirportDatabase *pAirportDatabase);
	~CTaxiwayConflictResolutionInEngine(void);


public:
	//whether  set the conflict settings 
	bool IsDefault();

	bool IsConflictRightFlight(AirsideFlightInSim *pFlight,AirsideFlightInSim *pRightFlight);

	bool IsConflictNextTaxiway(AirsideFlightInSim *pFlight,AirsideFlightInSim *pRightFlight);

	bool IsConflictSpecifiedTaxiway(int nTaxiwayID,AirsideFlightInSim *pFlight,AirsideFlightInSim *pRightFlight);

	CConflictResolution * GetConflictResolution();

protected:
	int m_nProjID;
	CConflictResolution m_conflictSolution;
};
