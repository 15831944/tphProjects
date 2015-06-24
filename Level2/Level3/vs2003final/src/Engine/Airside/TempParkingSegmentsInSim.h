#pragma once
#include "TaxiwayResource.h"
#include "Clearance.h"
#include "common/ARCMathCommon.h"
#include "TempParkingInSim.h"


class AirsideFlightInSim;
class AirportResourceManager;
class TaxiRouteInSim;
class StandInSim;

/************************************************************************/
/* temp parking segments                                                                     */
/************************************************************************/
class ENGINE_TRANSFER TempParkingSegmentsInSim : public TempParkingInSim
{
public:
	typedef ref_ptr<TempParkingSegmentsInSim> RefPtr;
	TempParkingSegmentsInSim(void);
	~TempParkingSegmentsInSim(void);

	void ReleaseLock(AirsideFlightInSim* pFlight);
	bool TryLock(AirsideFlightInSim* pFlight);
	bool GetLock(AirsideFlightInSim* pFlight);
	
	void InitTempParking(TaxiwayInSim* pTaxiway, int nStartNode, int nEndNode,int nEntryNode,AirportResourceManager* pResManager,bool bAngle);
	TaxiwayInSim* GetParkingTaxiway(){ return m_pTaxiway;}
	IntersectionNodeInSim* GetParkingTaxiwayNode(AirsideFlightInSim* pFlight);


	bool FindClearanceInConcern(AirsideFlightInSim * pFlight,ClearanceItem& lastItem, const DistanceUnit& radius, Clearance& newClearance);
	IntersectionNodeInSim* GetInNode()const{ return m_pEnterNode; }	
	IntersectionNodeInSim* GetOutNode(AirsideFlightInSim*pFlight){ return GetParkingTaxiwayNode(pFlight); } 
	Type GetParkingType()const{ return TempSegments; }
 
	bool IsAngleParking()const{ return m_bIsAngled; }
	virtual CString PrintTempParking()const;

	const static DistanceUnit defaultAngle;
	const static DistanceUnit defaultParkingSep;
	
	std::vector<FlightGroundRouteSegInSim*> m_vUseSegs;
	
private:
	
	TaxiRouteInSim* m_pParkingRouteFrom;
	TaxiRouteInSim* m_pParkingRouteTo;
	DistanceUnit m_dRouteFromLeft; // available space left in the taxiway ( nodeEntry, nodeFrom )
	DistanceUnit m_dRouteToLeft; // available space left in the taxiway (nodeEntry, nodeTo);

	std::map<AirsideFlightInSim*,IntersectionNodeInSim::RefPtr> m_vParkingFlights;
	IntersectionNodeInSim* m_pEnterNode;
	IntersectionNodeInSim* m_pExitNodeFrom;
	IntersectionNodeInSim* m_pExitNodeTo;
	TaxiwayInSim* m_pTaxiway;
	int m_nTempParkingNum;

	bool m_bIsAngled;

};

/************************************************************************/
/* temp parking behind a IntersectionNode                                */
/************************************************************************/
class ENGINE_TRANSFER TempParkingNodeInSim : public TempParkingInSim
{
public:
	typedef ref_ptr<TempParkingNodeInSim> RefPtr;
	TempParkingNodeInSim(IntersectionNodeInSim* pNode){ m_pNode = pNode; }

	void ReleaseLock(AirsideFlightInSim* pFlight);
	bool TryLock(AirsideFlightInSim* pFlight);
	bool GetLock(AirsideFlightInSim* pFlight);


	Type GetParkingType()const{ return TempNode; }
	bool FindClearanceInConcern(AirsideFlightInSim * pFlight,ClearanceItem& lastItem, const DistanceUnit& radius, Clearance& newClearance);
	IntersectionNodeInSim* GetInNode()const{ return m_pNode; }	
	IntersectionNodeInSim* GetOutNode(AirsideFlightInSim*pFlight);
	virtual CString PrintTempParking()const;

protected:
	IntersectionNodeInSim* m_pNode;
};

/************************************************************************/
/* temp parking at a Stand                                               */
/************************************************************************/
class ENGINE_TRANSFER TempParkingStandInSim : public TempParkingInSim
{
public:
	typedef ref_ptr<TempParkingStandInSim> RefPtr;
	TempParkingStandInSim(StandInSim* pStand ){ m_pStand = pStand; }
	IntersectionNodeInSim* GetInNode()const;
	IntersectionNodeInSim* GetOutNode(AirsideFlightInSim*pFlight);


	void ReleaseLock(AirsideFlightInSim* pFlight);
	bool TryLock(AirsideFlightInSim* pFlight);
	bool GetLock(AirsideFlightInSim* pFlight);


	Type GetParkingType()const{ return TempStand; }
	bool FindClearanceInConcern(AirsideFlightInSim * pFlight,ClearanceItem& lastItem, const DistanceUnit& radius, Clearance& newClearance);

	virtual CString PrintTempParking()const;
	StandInSim* GetStand()const{ return m_pStand; }
	
protected:
	StandInSim* m_pStand;


};