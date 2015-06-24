#pragma once
#include "LandsideStretchInSim.h"
#include <common/containr.h>

class LandsideRoundaboutInSim;
class LandsideRoundaboutLaneInSim : public LandsideLaneInSim
{
public:
	LandsideRoundaboutLaneInSim(LandsideRoundaboutInSim* pabout, int idx);
	virtual LandsideLayoutObjectInSim* getLayoutObject()const;
	virtual LandsideLaneInSim* getLeft()const;
	virtual LandsideLaneInSim* getRight()const;
	virtual bool IsSameDirToLane(LandsideLaneInSim* plane)const;
	virtual CString print()const;
	virtual CPath2008 GetSubPathDist(DistanceUnit distF, DistanceUnit distT)const;
	virtual CPoint2008 GetDistPoint(DistanceUnit distF)const;
	virtual ARCVector3 GetDistDir(DistanceUnit dist)const;

	LandsideLaneExitList GetLaneExitsAfterDist(double) const;

	LandsideRoundaboutInSim* getAbout()const{ return m_pParent; }

	virtual LandsideVehicleInSim* GetAheadVehicle(LandsideVehicleInSim* mpVehicle, DistanceUnit distInLane, DistanceUnit& aheadDist);
	//virtual LandsideVehicleInSim* CheckSpaceConflict(LandsideVehicleInSim* pVeh, const ARCPolygon& poly);
	virtual LandsideVehicleInSim* CheckPathConflict(LandsideVehicleInSim* pVeh, const CPath2008& path, DistanceUnit& dist);

	virtual void OnVehicleEnter(LandsideVehicleInSim* pVehicle,DistanceUnit dist, const ElapsedTime& t);
	virtual void OnVehicleExit(LandsideVehicleInSim* pVehicle,const ElapsedTime& t);
protected:
	LandsideRoundaboutInSim* m_pParent; //
	DistanceUnit m_dPathlen;
	int m_idx; // index from inner to outer
};

class LandsideAboutLaneLink : public LaneNodeLinkageInSim
{
	friend class LandsideRoundaboutInSim;
public:
	virtual LandsideLaneNodeInSim* getFromNode()const{ return pFrom; }
	virtual LandsideLaneNodeInSim* getToNode()const{ return pTo; }
	virtual double getDistance()const{ return m_dist; }

protected:
	LandsideLaneNodeInSim* pFrom;
	LandsideLaneNodeInSim* pTo;
	CPath2008 m_path;
	DistanceUnit m_dist;
};

class LandsideRoundabout;
class LandsideResourceManager;
class LandsideRoundaboutInSim : public LandsideLayoutObjectInSim, public LandsideResourceInSim
{
public:
	LandsideRoundaboutInSim(LandsideRoundabout* pabout);
	void InitRelateWithOtherObject(LandsideResourceManager* allRes); //initial relation with other stretch
	virtual void InitGraphNodes(LandsideRouteGraph* pGraph)const;
	virtual void InitGraphLink(LandsideRouteGraph* pGraph)const;
	virtual LandsideLayoutObjectInSim* getLayoutObject()const;

	BOOL IsClockWise()const;
	LandsideRoundaboutLaneInSim* getLane(int idx)const;
	virtual CString print()const;
	virtual LandsideRoundaboutInSim* toAbout(){ return this; }
	LandsideResourceInSim* IsResource(){ return this; }

	bool GetLinkPath(LandsideLaneNodeInSim* pFrom, LandsideLaneNodeInSim* pTo, CPath2008& path)const;
	//virtual LandsideVehicleInSim* CheckSpaceConflict(LandsideVehicleInSim* pVeh, const ARCPolygon& poly);
	virtual LandsideVehicleInSim* CheckPathConflict(LandsideVehicleInSim* pVeh, const CPath2008& path, DistanceUnit& dist);
protected:
	UnsortedContainer<LandsideRoundaboutLaneInSim> m_vLanes;
	UnsortedContainer<LandsideAboutLaneLink> m_vLinks;

};
