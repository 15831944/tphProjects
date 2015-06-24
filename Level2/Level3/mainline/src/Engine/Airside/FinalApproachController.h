#pragma once

class AirsideFlightInSim;
class Clearance;
class ClearanceItem;
class AirportResourceManager;
class AirRouteNetworkInSim;
class CGoAroundCriteriaDataProcess;
//class CFinalApproachSegInSim;
class LogicRunwayInSim;
class CCtrlInterventionSpecDataList;
#include "SideStepIntervention.h"

class CFinalApproachController
{
public:
	CFinalApproachController(void);
	~CFinalApproachController(void);

	void Init(int nPrjID, AirportResourceManager * pAirportRes, AirRouteNetworkInSim* pAirspaceRes);

	bool FindClearanceInConcern(AirsideFlightInSim * pFlight,ClearanceItem& lastItem, Clearance& newClearance);

	
protected:
	AirportResourceManager * m_pAirportRes;
	AirRouteNetworkInSim * m_pAirspaceRes;
	CGoAroundCriteriaDataProcess *m_pGoAroundCriteriaDataProcess;

	CCtrlInterventionSpecDataList* m_pCtrlInterventionSpec;
	SideStepIntervention m_SideStepIntervention;

	//CFinalApproachSegInSim* GetFinalApproachSeg(AirsideFlightInSim * pFlight, ClearanceItem& lastItem);	
	bool IsNeedToSideStep(AirsideFlightInSim * pFlight, ClearanceItem& lastItem)const;

};
 