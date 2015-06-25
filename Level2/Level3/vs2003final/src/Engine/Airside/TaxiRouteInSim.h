#pragma once

#include <Boost/tuple/tuple.hpp>
#include "CommonInSim.h"
#include "TaxiwayResourceManager.h"
#include "HoldInTaxiRouteInSim.h"

class AirsideFlightInSim;
class Clearance;
class ClearanceItem;

//////////////////////////////////////////////////////////////////////////
//a segment of taxi route,  could be also runway?
//////////////////////////////////////////////////////////////////////////
class ENGINE_TRANSFER TaxiRouteItemInSim
{
public:
	TaxiRouteItemInSim(AirsideResource * pRes, DistanceUnit distF, DistanceUnit distT);



	AirsideResource * m_pRes;   //could be taxiway or runway
	DistanceUnit m_distFrom;
	DistanceUnit m_distTo;

	bool operator==(const TaxiRouteItemInSim& oItem)const;

	AirsideResource * GetResource()const{ return m_pRes; }
	DistanceUnit GetDistFrom()const{ return m_distFrom; }
	DistanceUnit GetDistTo()const{ return m_distTo; }

	bool IsDistInItem(DistanceUnit dist)const;

};

//////////////////////////////////////////////////////////////////////////
//segment list of taxiways, runways, designed for taxiing of a specified flight 
//////////////////////////////////////////////////////////////////////////
class IntersectionNodeInSim;
class SimAgent;
class ENGINE_TRANSFER TaxiRouteInSim
{
public:
	//
	TaxiRouteInSim(AirsideMobileElementMode mode,AirsideResource* pOrign, AirsideResource* pDest);
	virtual ~TaxiRouteInSim();
	
	//add taxiway segments to this list
	void AddTaxiwaySegList(const FlightGroundRouteDirectSegList& segList,bool isCyclicRoute=false);
	//void AddTaxiwaySegsToTempParking(TaxiwayDirectSegList& segList);

	void AddItem(const TaxiRouteItemInSim& newItem,bool isCyclicRoute=false);
	AirsideMobileElementMode GetMode()const{ return m_mode; }

	std::vector<IntersectionNodeInSim*> GetRouteNodeList();
	IntersectionNodeInSim* GetLastNode();
	//
	int  GetItemCount()const{ return m_vItems.size(); }
	TaxiRouteItemInSim & ItemAt(int idx){ return m_vItems[idx]; }
	const TaxiRouteItemInSim& ItemAt(int idx)const{ return m_vItems[idx]; }
	TaxiRouteItemInSim GetItem(int idx)const{ return m_vItems[idx]; }
	
	void Clear(){ m_vItems.clear(); }
	
	std::vector<TaxiRouteItemInSim>& GetItemlist(){ return m_vItems; }


	//return the index of the res in this route
	int GetItemIndex(AirsideResource * pRes)const;

	//get clearance of this 
	virtual bool FindClearanceInConcern(AirsideFlightInSim * pFlight,ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance);

	//Is need lock the whole route
	virtual bool IsLockDirectionOfWholeRoute();

	//add clearance items to the next dist
	int AddTaxiRouteClearanceItems(AirsideFlightInSim* pFlight,  const DistanceUnit nextDist,double endSpd, double dAngle, ClearanceItem& lastClearanceItem, Clearance& newClearance );

	//
	bool CheckFlightConflictInResource(AirsideFlightInSim *pFlight,AirsideResource *pResouce,AirsideResource *pNextResource,ElapsedTime& nextEventTime);
	
	virtual void FlightExitRoute(AirsideFlightInSim* pFlight, const ElapsedTime& releaseTime);


	void GetItemIndexAndDistInRoute(const DistanceUnit& dist, int& nItemIdx, DistanceUnit& distInItem);
	DistanceUnit GetDistInRoute( const int& nItemIdx , const DistanceUnit& distInItem) const;
	
	//when the Route treat as a temp parking in sim
	bool FindClearanceAsTempParking(AirsideFlightInSim* pFlight, ClearanceItem& lastItem, Clearance& newClearance, double offsetAngle);
	
	//return the hold list of this flight
	FlightHoldListInTaxiRoute& GetHoldList(){ return m_vHoldList; }
	FiletPointListInTaxiRoute m_vFiletPts;

	DistanceUnit GetItemBeginDist(int idx)const{ return m_vdAccLength[idx]; }
	DistanceUnit GetItemEndDist(int idx)const{ return m_vdAccLength[idx+1]; }
	DistanceUnit GetEndDist()const{ return *m_vdAccLength.rbegin(); }

	virtual DistanceUnit GetExitRouteDist(AirsideFlightInSim* pFlight);
	virtual HoldInTaxiRoute* GetlastEntryHold();

	void GetTaxiInterruptLinesInRoute(std::vector<TaxiInterruptLineInSim*>& vInterruptLines);
	void GetTaxiStartPositionsInRoute(std::vector<StartPositionInSim*>& vStartPositions);
	const LaneIntersectionNotifyPointList& GetLaneNotifyPtList()const{ return m_LaneNotifyPtList; }
	void SetSpeed(double dSped){ m_dTaxiSpd = dSped; }
	RouteDirPathList getNextPathList(int nBeginIdx)const;

	AirsideMeetingPointInSim* GetFirstMeetingPointInRoute(const std::vector<ALTObjectID>& vMeetingPoints);
	bool GetSubRouteToMeetingPoint(const AirsideMeetingPointInSim* pMeetingPoint,  FlightGroundRouteDirectSegList& segList);

	// get the flight speed on the taxiway segment on which the flight is moving
	// NOTE: 1. the flight must be on the route now.
	//       2. different to GetFlightOnRouteSpeed, the method cares the taxiway speed constraints
	//          which is defined to some taxiway segments
	double GetFlightOnTaxiwaySegSpeed(const AirsideFlightInSim* pFlight) const;

	void InitRoute(AirsideFlightInSim* pFlight,const ElapsedTime& t);

	//check conflictions
	bool IsRouteValide(AirsideFlightInSim* pFlt)const;
protected:
	//calculate the hold list and the fillets points
	void UpdateData();	
	std::vector<DistanceUnit> m_vdAccLength;
	int m_nCurItemIdxInRoute;  //curdist in route  for flight remember curd ist in route
	void updateCurItemIndex(int nidx){ m_nCurItemIdxInRoute = nidx; }

	virtual void PlanRoute(AirsideFlightInSim* pFlt,const ElapsedTime& t); //planning route for resolve conflict

	void TravelRunwaySegInRoute(AirsideFlightInSim* pFlight,TaxiRouteInSim& theRoute,double taxiSpd, const DistanceUnit& curDist, ClearanceItem& lastClearanceItem, Clearance& newClearance ,const ElapsedTime& supEndTime );
	bool IsNextResourceIsRunwaySegment(TaxiRouteInSim& theRoute,const DistanceUnit& curDist);
	
	AirsideMobileElementMode m_mode;
	double m_dTaxiSpd;
	std::vector<TaxiRouteItemInSim> m_vItems;
	FlightHoldListInTaxiRoute m_vHoldList;
	LaneIntersectionNotifyPointList m_LaneNotifyPtList;
	FlightInterruptPosListInRoute m_InterruptLineList;

	//check conflict with lead mob. return  <endDist, end speed, watch agent,next mob dist>
	boost::tuple<DistanceUnit,double, SimAgent*,DistanceUnit> checkConflictWithLeadMobile(AirsideFlightInSim* pFlight,DistanceUnit mCurDistInRoute,DistanceUnit dRadOfConcern);
	//check conflict with the next hold bool have conflict returns <endDist, end speed, watch agent(hold),next hold dist>
	boost::tuple<DistanceUnit,double, SimAgent*,DistanceUnit> checkConflictWithNextHold(AirsideFlightInSim* mpFlight, ARCMobileElement* pleadFlight,DistanceUnit mCurDistInRoute);

	//check conflict with the interrupt line returns < endDist, end speed, Interrupt line,wait dist>
	boost::tuple<DistanceUnit,double , TaxiInterruptLineInSim*, DistanceUnit, ElapsedTime> checkConflictWithInterruptLine(AirsideFlightInSim*mpFligt, DistanceUnit mCurDistInRoute);

	// get the entry hold that the flight is about to enter at this moment,
	// it means the flight is now at the position near the hold,
	// NULL if not found
	// -- refer to checkConflictWithNextHold for more details
	bool IsFlightToEnterHold(AirsideFlightInSim* mpFlight, DistanceUnit mCurDistInRoute);
	
	//ElapsedTime GetFlightOcupancyRunwayTime(std::vector<HoldInTaxiRoute> vHolds,AirsideFlightInSim* pFlight)const;
public:
	//Original Resource, Dest Resource
	AirsideResource* mpResOrig;
	AirsideResource* mpResDest;

private:
	// Enter this hold but not cross or just cross the node
	HoldInTaxiRoute* m_pLastAcrossHold;
};
