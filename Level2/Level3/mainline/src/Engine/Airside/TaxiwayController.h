#pragma once

#include "TrafficController.h"
#include "./TaxiwayResourceManager.h"
class AirportTrafficController;
class CInboundRouteInSimList;
class COutboundRouteInSimList;

class ENGINE_TRANSFER TaxiwayTrafficController : public TrafficController
{

public:
	explicit TaxiwayTrafficController(AirportTrafficController * pAirportController);
	~TaxiwayTrafficController();

public:

	bool Init(int nPrjId, TaxiwayResourceManager * taxiwayRes);

	virtual Clearance* AssignNextClearance(FlightInAirsideSimulation * pFlight);
	//get clearance in the segment
	Clearance * GetConflictClearance(FlightInAirsideSimulation*pFlight);

	virtual bool AmendClearance(FlightInAirsideSimulation * pFlight, Clearance* pClearance);	

protected:
	AirportTrafficController* m_pAirportController;
	TaxiwayResourceManager * m_pTaxiwayRes ;

	ElapsedTime GetTaxiwaySeparationTime(FlightInAirsideSimulation* pLeadFlight, FlightInAirsideSimulation* pFollowFlight)const;
	DistanceUnit GetTaxiwaySeperationDistance(FlightInAirsideSimulation * pLeadFlight, FlightInAirsideSimulation * pFollowFlight)const;

	//EnterSolution GetEnterSegmentSolution(FlightInAirsideSimulation* pFlight, TaxiwayDirectSegInSim * pSegment, const ElapsedTime& enterT);
	//EnterSolution GetEnterNodeSolution(FlightInAirsideSimulation* pFlight, IntersectionNodeInSim* pNode , const ElapsedTime& enterT);
	// estimate next taxiway segment 
	TaxiwayDirectSegInSim * EstimateNextTaxiwaySegment(IntersectionNodeInSim * pFrom , IntersectionNodeInSim * pTo, FlightInAirsideSimulation * pFlight, bool bOtherChoose = false);

	// estimate which airside resource the flight should get	
	AirsideResource * GetNextResource(FlightInAirsideSimulation * pFlight,bool bOtherChoose = false );

	//std::map<FlightInAirsideSimulation* ,int> m_RoutePriorityMap;

private:
	CInboundRouteInSimList  *m_pInboundRouteInSimList;
	COutboundRouteInSimList *m_pOutboundRouteInSimList;
};
