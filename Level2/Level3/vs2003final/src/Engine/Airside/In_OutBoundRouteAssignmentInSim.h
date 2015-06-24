#pragma once
#include "../EngineDll.h"
#include "TaxiwayResourceManager.h"

#include <vector>

class CBoundRouteAssignment;
class CTimeRangeRouteAssignment;
class IntersectionNodeInSim;
class StandInSim;
class RunwayExitInSim;
class CAirportDatabase;
class AirsideFlightInSim;
class TakeoffQueueInSim;
class CIn_OutBoundRoute;
class IntersectionNodeInSimList;
class GroundRoute;
class FlightGroundRouteResourceManager;
class RunwayResourceManager;
class DeicePadInSim;
class StandLeadOutLineInSim;
class StandLeadInLineInSim;
class CRoutePriority;
class DeicepadGroup;
class DeicepadGroupEntry;
class COut_BoundRoute;
class FlightGroundRouteFinderStrategy;

enum BOUNDROUTETYPE;

class ENGINE_TRANSFER In_OutBoundRouteAssignmentInSim
{
public:
	In_OutBoundRouteAssignmentInSim(void);
	~In_OutBoundRouteAssignmentInSim(void);

	void Init(int nPrjID, CAirportDatabase* pAirportDB, TaxiwayResourceManager* pResManager,RunwayResourceManager * pRunwayResManager,FlightGroundRouteResourceManager* pGoundRouteResManager,IntersectionNodeInSimList* pNodeManager);

	void GetInboundRoute(AirsideFlightInSim* pFlight,RunwayExitInSim* pExit, StandInSim* pStand, FlightGroundRouteDirectSegList& vSegments);

	// Get outbound route
	// @parameters:
	// @pStartRes: start resource - could be StandInSim, DeicePadInSim, StartPositionInSim, TaxiInterruptLineInSim
	// @pSubStartRes: other referenced resource that related with pStartRes
	//  it is TaxiwayDirectSegInSim if pStartRes is a StartPositionInSim or TaxiInterruptLineInSim, otherwise ignored
	void GetOutboundRoute(AirsideFlightInSim* pFlight,TakeoffQueueInSim * pQueue,
		AirsideResource* pStartRes, FlightGroundRouteDirectSegInSim* pStartSeg, RunwayExitInSim* pTakeoffPos,
		FlightGroundRouteDirectSegList& vSegments);

	void GetCircuitBoundRoute(AirsideFlightInSim* pFlight,TakeoffQueueInSim * pQueue,
		RunwayExitInSim* pExit, RunwayExitInSim* pTakeoffPos,
		FlightGroundRouteDirectSegList& vSegments);

	void GetRouteToDeice();

// 	void GetRouteFStandTDeicepad(AirsideFlightInSim* pFlight, StandLeadOutLineInSim* pStandOut, DeicePadInSim* pDeicePad,FlightGroundRouteDirectSegList& vSegments);
	DeicepadGroupEntry* GetRouteFStandTDeicepadGroup(AirsideFlightInSim* pFlight, StandLeadOutLineInSim* pStandOut, DeicepadGroup* pGroup,FlightGroundRouteDirectSegList& vSegments);
	DeicepadGroupEntry* GetRouteToDeiceGroup(AirsideFlightInSim* pFlight,TaxiwayDirectSegInSim* pSeg, DeicepadGroup* pGroup,FlightGroundRouteDirectSegList& vSegments);

	
	void GetRouteFStandTStand(AirsideFlightInSim* pFlight, StandLeadOutLineInSim* pStandOut, StandLeadInLineInSim* pStandIn,FlightGroundRouteDirectSegList& vSegments);
	void GetRouteFDeiceTTakoff(AirsideFlightInSim* pFlight, DeicePadInSim* pDeicePad,RunwayExitInSim* pTakeoffPos, FlightGroundRouteDirectSegList& vSegments);

protected:
	void GetOutboundRouteFromRouteItem( COut_BoundRoute* pRoute, AirsideFlightInSim* pFlight, IntersectionNodeInSim * pNodeFrom, RunwayExitInSim* pTakeoffPos, TakeoffQueueInSim* pQueue, FlightGroundRouteDirectSegList &vSegments, FlightGroundRouteFinderStrategy* pUsingStrategy = NULL );


	bool GetPreRoute( IntersectionNodeInSim*& pStartNode, IntersectionNodeInSim* pNodeIn, int nHeadTaxiwayID, bool bHeadTaxiwayOK, AirsideFlightInSim* pFlight, FlightGroundRouteDirectSegList& vRouteItems, FlightGroundRouteFinderStrategy* pUsingStrategy = NULL );
	bool GetPostRoute( IntersectionNodeInSim* pStartNode, IntersectionNodeInSim* pEndNode, IntersectionNodeInSim * pNodeOut, int nTailTaxiwayID, bool bTailTaxiwayOK, AirsideFlightInSim* pFlight, FlightGroundRouteDirectSegList& vRouteItems, FlightGroundRouteFinderStrategy* pUsingStrategy = NULL );

	bool GetEntireRouteFromPriority( CRoutePriority* pPriority, AirsideFlightInSim* pFlight, IntersectionNodeInSim* pNodeIn, IntersectionNodeInSim * pNodeOut, FlightGroundRouteDirectSegList& vRouteItems, FlightGroundRouteFinderStrategy* pUsingStrategy = NULL );
	bool GetEntireOutboundRouteFromPriority( CRoutePriority* pPriority, AirsideFlightInSim* pFlight, IntersectionNodeInSim * pNodeFrom, IntersectionNodeInSim * pNodeTo, TakeoffQueueInSim* pQueue, FlightGroundRouteDirectSegList &vRouteItems, FlightGroundRouteFinderStrategy* pUsingStrategy = NULL );

private:
	CTimeRangeRouteAssignment* GetTimeRangeRouteAssignment(AirsideFlightInSim* pFlight,BOUNDROUTETYPE eType);
	IntersectionNodeInSim* GetRouteItemListInPriority( CRoutePriority* pPriority,
		FlightGroundRouteDirectSegList& vRouteItems,
// 		IntersectionNodeInSim*& pTailNode/* [out] */,
		int& nHeadTaxiwayID/* [out] */,
		int& nTailTaxiwayID/* [out] */ ) const;// throws CString which identify the error

	bool CheckRoutePriorityChangeCondition( AirsideFlightInSim* pFlight, CRoutePriority* pPriority, const FlightGroundRouteDirectSegList &vSegList );

	int GetRouteItemsFromGroundRoute( GroundRoute* pGroundRoute,
		int nPreTaxiwayID/* only taxiway, -1 if not exists */,
		FlightGroundRouteDirectSegList& vRouteItems/* [out] */,
		int& nTailIntersetionNodeID/* [out] */,
		int& nHeadTaxiwayjID/* [out] */,
		int& nTailTaxiwayID/* [out] */,
		bool& bTailIsRunway/* [out] */ ) const;
	bool IsValidRoute(FlightGroundRouteDirectSegList& vSegments, int nFromID, int nToID);

private:
	CBoundRouteAssignment* m_pInboundRouteAssignment;
	CBoundRouteAssignment* m_pOutboundRouteAssignment;
	CBoundRouteAssignment* m_pDeiceRouteAssigment;
	CBoundRouteAssignment* m_pPostDeiceRouteAssigment;

	TaxiwayResourceManager* m_pTaxiResManager;
	RunwayResourceManager *m_pRunwayResManager;
	IntersectionNodeInSimList* m_pIntersectionNodeManager;
	FlightGroundRouteResourceManager *m_pGroundResManager;

	class SegEntrySuitGourpEntry
	{
	public:
		SegEntrySuitGourpEntry(DeicepadGroupEntry*& pEntry);
		bool operator()(FlightGroundRouteDirectSegInSim* pSeg, DeicepadGroupEntry* pEntry);

	private:
		DeicepadGroupEntry*& m_pEntry;
	};

};
