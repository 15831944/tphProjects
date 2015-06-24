#pragma once

class IntersectionNodeInSim;
class AirsideFlightInSim;
class TaxiRouteInSim;

//the flag position to indicate flight enter or exit the node
class ENGINE_TRANSFER HoldInTaxiRoute
{
public:
	enum TYPE{ NONE,ENTRYNODE, EXITNODE_BUFFER, EXITNODE };
	DistanceUnit m_dDistInRoute;
	IntersectionNodeInSim* m_pNode;
	TYPE m_hHoldType;	
	int m_nHoldId;
	bool m_bCrossed; //be crossed by flight

	bool IsRunwayHold()const;
	bool IsTakeoffPosHold(RunwayExitInSim* pExit)const;
	FlightGroundRouteDirectSegInSim* m_pDirSeg; //at seg;
	FlightGroundRouteDirectSegInSim* mLinkDirSeg; //enter or exit seg when enter/exit node

	bool m_bIsRunwayExit;
public:
	HoldInTaxiRoute();
	HoldInTaxiRoute(TYPE holdType,const DistanceUnit&dist, IntersectionNodeInSim* pNode, FlightGroundRouteDirectSegInSim* pSeg,int nHoldID );
	void SetLinkDirSeg(FlightGroundRouteDirectSegInSim* pSeg){ mLinkDirSeg = pSeg; }
	bool operator <(const HoldInTaxiRoute& otherHold)const
	{
		return m_dDistInRoute < otherHold.m_dDistInRoute;
	}
};

//////////////////////////////////////////////////////////////////////////
// Hold in TaxiRoute  of Such Flight
//////////////////////////////////////////////////////////////////////////
class ENGINE_TRANSFER FlightHoldListInTaxiRoute
{
public:
	FlightHoldListInTaxiRoute():m_pFlight(NULL){}
	void Init(TaxiRouteInSim& theRoute,AirsideFlightInSim*pFlight,bool bLanding=false);
	int GetCount()const{ return (int)m_vHoldList.size(); }

	HoldInTaxiRoute& ItemAt(int idx){ return m_vHoldList[idx]; }
	const HoldInTaxiRoute& ItemAt(int idx)const{ return m_vHoldList[idx]; }
	HoldInTaxiRoute* GetEntryHold(IntersectionNodeInSim* pNode,bool bLast=false)const;
	HoldInTaxiRoute* GetExitHold(IntersectionNodeInSim* pNode)const;

	HoldInTaxiRoute* GetExitHold(const HoldInTaxiRoute& entryHold)const;


	HoldInTaxiRoute* GetLastEntryHold()const;
	HoldInTaxiRoute* GetLastRunwayEntryHold()const;
	HoldInTaxiRoute* GetFirstEntryHold()const;
	HoldInTaxiRoute* GetFirstExitHold()const;
	

	std::vector<HoldInTaxiRoute> GetHoldFromTo(const DistanceUnit& distF, const DistanceUnit& distT )const;
	int GetHoldUntil(const DistanceUnit& distEnd)const;

	void SetNodeExitTime(const DistanceUnit& distInRoute, AirsideFlightInSim* pFlight, const ElapsedTime& exitTime);

	std::vector<HoldInTaxiRoute> GetNextRouteNodeHoldList( DistanceUnit distInRoute)const;
	HoldInTaxiRoute* GetNextNodeEntryHold(DistanceUnit distInRoute)const;
	std::vector<HoldInTaxiRoute*> GetNextRunwayEntryHoldList(DistanceUnit distInRoute)const;

	HoldInTaxiRoute* IsDistInNoParkingNodeRange(const DistanceUnit& dist)const;
	HoldInTaxiRoute* GetWaitHold(const DistanceUnit& dist )const; //null if can wait at the dist in route

	bool IsLastEntryRunwayHold()const;
public:
	std::vector<HoldInTaxiRoute> m_vHoldList;
	AirsideFlightInSim* m_pFlight;
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
class ENGINE_TRANSFER FiletPointInTaxiRoute
{
public:
	enum emFiletPointType{ FiletBegin,FiletEnd };
	FiletPointInTaxiRoute(const DistanceUnit& dist, emFiletPointType ftype){ m_distInRoute = dist; m_type = ftype; }
	DistanceUnit GetDistInRoute()const{ return m_distInRoute ;}
	void SetDistInRoute(const DistanceUnit& distInRoute){ m_distInRoute = distInRoute;  }
	DistanceUnit m_distInRoute;
	bool operator <(const FiletPointInTaxiRoute& otherFilet)const{ 
		return m_distInRoute < otherFilet.m_distInRoute;
	}

	emFiletPointType m_type;
	
};


class ENGINE_TRANSFER FiletPointListInTaxiRoute
{
public:
	void Init(TaxiRouteInSim& theRoute);
	std::vector<FiletPointInTaxiRoute> GetFiletPointsBetween(const DistanceUnit& distF, const DistanceUnit& distT)const;
	FiletPointInTaxiRoute* IsDistInFilet(const DistanceUnit& dist)const;
	FiletPointInTaxiRoute *GetNextEndPoint(int idx)const;

protected:
	std::vector<FiletPointInTaxiRoute> m_vFiletPoints;
};

//////////////////////////////////////////////////////////////////////////
//Vehicle Stretch Notify point ,which  notify vehicle at the place
//////////////////////////////////////////////////////////////////////////
class LaneFltGroundRouteIntersectionInSim;
class ENGINE_TRANSFER  LaneIntersectionNotifyPoint
{
public:
	enum TYPE{ ENTRY_POINT, EXIT_POINT };
	LaneIntersectionNotifyPoint(DistanceUnit dist,LaneFltGroundRouteIntersectionInSim* pIntersection, TYPE t);
	DistanceUnit mDistInRoute;
	LaneFltGroundRouteIntersectionInSim* mpNode;
	TYPE m_type;
	bool operator < (const  LaneIntersectionNotifyPoint& other){ return mDistInRoute <other.mDistInRoute; }
	void OnFlightAt(AirsideFlightInSim* pFlt, const ElapsedTime& t);
	
};


class LaneIntersectionNotifyPointList
{
public:
	LaneIntersectionNotifyPointList(){ mpFlight = NULL; }
	void Init(TaxiRouteInSim& theRoute, AirsideFlightInSim*pFlight);


	void clearFlightLocks(AirsideFlightInSim* pFlight, const ElapsedTime& tTime);

	std::vector< LaneIntersectionNotifyPoint> GetNotifyPointsBetween(const DistanceUnit& distF, const DistanceUnit& distT)const;
	std::vector< LaneIntersectionNotifyPoint> GetNotifyPointsUntil(const DistanceUnit& distT)const;
	std::vector< LaneIntersectionNotifyPoint> mvPoints;

	AirsideFlightInSim* mpFlight;

};

//////////////////////////////////////////////////////////////////////////
//Taxi Interrupt Line Pos in Route
//////////////////////////////////////////////////////////////////////////
class FlightInterruptPosInRoute
{
public:
	FlightInterruptPosInRoute(const DistanceUnit& dist, TaxiInterruptLineInSim* mpLine,int nStartNodeId){
		mpInterruptLine = mpLine;
		m_distInRoute = dist;
		m_startNodeId = nStartNodeId;
	}
	TaxiInterruptLineInSim* mpInterruptLine;
	DistanceUnit m_distInRoute; //flight wait dist in the route
	int m_startNodeId;
	bool operator < (const FlightInterruptPosInRoute& other)const{
		return m_distInRoute < other.m_distInRoute;
	}
};


class FlightInterruptPosListInRoute
{
public:
	FlightInterruptPosListInRoute(){
		mpFlight = NULL;
	}
	void Init(TaxiRouteInSim& theRoute,AirsideFlightInSim*pFlight);

	int GetCount()const{ return (int)m_vData.size();}
	FlightInterruptPosInRoute ItemAt(int idx)const{ return m_vData.at(idx); }
	AirsideFlightInSim* mpFlight;
	std::vector<FlightInterruptPosInRoute> m_vData;

	int GetLineIdx(TaxiInterruptLineInSim* pLine)const;

	std::vector< FlightInterruptPosInRoute> GetNextItems(const DistanceUnit& distF )const;

};