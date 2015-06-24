

#include "VehicleRoadIntersectionInSim.h"
class ENGINE_TRANSFER LaneTaxiwayIntersectionInSim :public VehicleRoadIntersectionInSim,public RouteResource
{
	friend class VehicleLaneInSim;
public:	
	LaneTaxiwayIntersectionInSim(VehicleLaneInSim * plane , TaxiwaySegInSim * pTaxiSeg , int idx);
	void SetHold1(DistanceUnit dist){ m_holdDist1 = dist; }
	void SetHold2(DistanceUnit dist){ m_holdDist2 = dist; }
	DistanceUnit GetHold1()const{ return m_holdDist1; }
	DistanceUnit GetHold2()const{ return m_holdDist2; }

	bool MakeValid();//for data not valid, which only set one hold

	DistanceUnit GetIntersectDistAtLane()const{ return m_intersectDistAtLane; }
	DistanceUnit GetIntersectDistAtTaxiwaySeg()const{ return m_intersectDistAtTaxiway; }

	void SetIntersectDistAtLane(const DistanceUnit& dist);
	void SetIntersectDistAtTaxiwaySeg(const DistanceUnit& dist){ m_intersectDistAtTaxiway = dist; }

	TaxiwaySegInSim * GetTaxiwaySegment()const{ return m_pTaxiSeg; }

//	AirsideFlightInSim * GetLatestFlightInSight(const DistanceUnit& radius,const ElapsedTime&tTime)const;

	//get flight dist away form this Intersection return false if not in the route
	//bool GetFlightDistToHere(AirsideFlightInSim* pFlight,const ElapsedTime& t, double& dist, ARCVector2& dir)const;
	//get flight dist 

	CPoint2008 GetPosition()const;

	//be lock for the mob
	bool IsLockedfor(ARCMobileElement* pMob)const;
	void OnFlightEnter(AirsideFlightInSim* pFlt,const ElapsedTime& t);
	void OnFlightExit(AirsideFlightInSim* pFlt,const ElapsedTime& t);
	virtual bool IsActiveFor(ARCMobileElement* pmob,const RouteDirPath* pPath)const{ return !IsLockedfor(pmob); }
	virtual bool bMayHaveConflict(const RouteDirPath *pPath)const{ return true; }
	virtual bool bLocked(ARCMobileElement* pmob,const RouteDirPath* pPath)const{ return IsLockedfor(pmob); };

	DistanceUnit getDistInStreth(VehicleStretchInSim* pStretch);
protected:
	DistanceUnit m_holdDist1;
	DistanceUnit m_holdDist2;
	DistanceUnit m_intersectDistAtLane;
	DistanceUnit m_intersectDistAtTaxiway;
	std::set<ARCMobileElement*> mvLockMobs;

	VehicleLaneInSim  * m_pLane;
	TaxiwaySegInSim * m_pTaxiSeg;
	int m_idx;
};

