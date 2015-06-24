#pragma once
#include "TaxiRouteInSim.h"

class ENGINE_TRANSFER TaxiToReleasePointRouteInSim: public TaxiRouteInSim
{
public:
	TaxiToReleasePointRouteInSim(AirsideMobileElementMode mode,AirsideResource* pSrcRes, AirsideResource* pDestRes);
	~TaxiToReleasePointRouteInSim(void);

	virtual DistanceUnit GetExitRouteDist(AirsideFlightInSim* pFlight);


	FlightGroundRouteDirectSegInSim* getLastSeg();
public:
	//get clearance 
	bool FindClearanceInConcern(AirsideFlightInSim * pFlight,ClearanceItem& lastItem, DistanceUnit radius, Clearance& newClearance);

	bool IsLockDirectionOfWholeRoute();

	//post segs going to move
	void SetPostRoute(const FlightGroundRouteDirectSegList& vpostsegs){ m_postSegs = vpostsegs; }
	const FlightGroundRouteDirectSegList& GetPostRoute()const{ return m_postSegs; }
	FlightGroundRouteDirectSegList& GetPostRoute(){ return m_postSegs; }
protected:
	virtual void PlanRoute(AirsideFlightInSim* pFlt,const ElapsedTime& t);
	FlightGroundRouteDirectSegList m_postSegs;
};
