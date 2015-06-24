#pragma once
class AirsideFlightInSim;
class ClearanceItem;
class Clearance;
class AirRouteNetworkInSim;

class CMissApproachFindClearanceInConcern
{
public:
	CMissApproachFindClearanceInConcern(void);
	~CMissApproachFindClearanceInConcern(void);

	void Init(int nPrjId, AirRouteNetworkInSim * pAirspaceRes );

public:
	bool FindClearanceInConcern(AirsideFlightInSim * pFlight,  ClearanceItem& lastItem,  Clearance& newClearance);

protected:
	AirRouteNetworkInSim * m_pAirspaceRes ;


};
