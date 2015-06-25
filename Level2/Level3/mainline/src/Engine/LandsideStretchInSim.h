#pragma once
#include "common/ARCPath.h"
#include "core/SAgent.h"
#include "..\Landside\IntersectionLinkGroupLogEntry.h"
#include "Common\ALTObjectID.h"
#include "Landside/IntersectionTrafficCtrlNode.h"
#include "Landside\LandsideIntersectionNode.h"
#include "Results\OutputLandside.h"
#include "LandsideResourceInSim.h"
#include "LandsideLayoutObjectInSim.h"

//////////////////////////////////////////////////////////////////////////
class LandsideLaneInSim;
class LandsideStretchInSim;
class LandsideLaneEntry;
class LandsideLaneExit;
class LandsideStretchSegmentInSim;


//node link two resource
class LandsideLaneNodeInSim 
{
public: 	
	LandsideLaneNodeInSim(){ mpLane = NULL; m_Index = -1; }

	virtual LandsideLaneEntry* toEntry(){ return NULL; }
	virtual LandsideLaneExit* toExit(){ return NULL; }

	virtual LandsideResourceInSim* getFromRes()const=0;
	virtual LandsideResourceInSim* getToRes()const=0;

	void SetPosition(LandsideLaneInSim* plane , double distInlane);
	void SetPosition(LandsideLaneInSim* plane, const CPoint2008& pt);

	void Offset(double dist);

	const CPoint2008& GetPosition()const{ return m_pos; }
	const ARCVector3& GetDir()const{ return m_dir; }
 
	LandsideLaneInSim* mpLane;
	double m_distInlane;
	CPoint2008 m_pos;//position
	ARCVector3 m_dir;//direction

	int m_Index; //index in graph will be set after add to graph
};


class LandsideLaneNodeList : public std::vector<LandsideLaneNodeInSim*>
{
public:
	void GetLaneEntryList(std::vector<LandsideLaneEntry*>& vEntries);
	void GetLaneExitList(std::vector<LandsideLaneExit*>& vExits);
	void DeleteClear();
	LandsideLaneNodeInSim* GetNode(int idx);
};

class LandsideLaneEntry : public LandsideLaneNodeInSim
{
public:
	LandsideLaneEntry(){ mpFromRes = NULL; }
	virtual LandsideResourceInSim* getFromRes()const{ return mpFromRes; }
	virtual LandsideResourceInSim* getToRes()const;


	bool IsLaneEntry()const { return true; }
	virtual LandsideLaneEntry* toEntry(){ return this; }
	void SetFromRes(LandsideResourceInSim* pRes){ mpFromRes = pRes; }
protected:
	LandsideResourceInSim* mpFromRes;	
};


class LandsideLaneExit : public LandsideLaneNodeInSim
{
public:
	LandsideLaneExit(){ mpToRes = NULL; }
	virtual bool IsLaneDirectToCurb()const;;

	virtual LandsideResourceInSim* getFromRes()const;
	virtual LandsideResourceInSim* getToRes()const;

	virtual LandsideLaneExit* toExit(){ return this; }
	void SetToRes(LandsideResourceInSim* pRes){ mpToRes = pRes; }
protected:
	LandsideResourceInSim* mpToRes;
};



class LaneNodeLinkageInSim
{
public:
	virtual LandsideLaneNodeInSim* getFromNode()const=0;
	virtual LandsideLaneNodeInSim* getToNode()const=0;
	virtual double getDistance()const=0;
};




class CCrossWalkInSim;
class LandsideStretchLaneInSim;
class LandsideRoundaboutLaneInSim;
typedef std::vector<LandsideLaneExit*> LandsideLaneExitList;

class LandsideLaneInSim :public LandsideResourceInSim
{
public:
	~LandsideLaneInSim(){ Clear(); }
	LandsideLaneInSim* toLane(){ return this; }
	virtual LandsideStretchLaneInSim* toStretchLane(){ return NULL; }
	virtual LandsideRoundaboutLaneInSim* toAboutLane(){ return NULL; }

	virtual LandsideLaneInSim* getLeft()const=0;
	virtual LandsideLaneInSim* getRight()const=0;
	virtual bool IsSameDirToLane(LandsideLaneInSim* plane)const=0;
	virtual CPath2008 GetSubPathDist(DistanceUnit distF, DistanceUnit distT)const=0;
	
	virtual LandsideLaneExitList GetLaneExitsAfterDist(double dist)const=0;
	virtual LandsideLaneEntry* GetPreLaneEntry(double dist)const{ return NULL; }

	//is the lane empty in the dist range
	bool isSpaceEmpty(LandsideVehicleInSim* pV, DistanceUnit distF, DistanceUnit distT)const;


	virtual	CPoint2008 GetDistPoint(DistanceUnit distF)const;
	virtual ARCVector3 GetDistDir(DistanceUnit dist)const;
	CPath2008 GetPathLeft(DistanceUnit distF)const;
	DistanceUnit GetPointDist(const CPoint2008& pos)const;
	DistanceUnit GetLength()const{ return m_dLength; }
	
	
	void AddLaneNode(LandsideLaneNodeInSim* pNode);	
	void sortEntryExits();

	//get ahead vehicle and return the separation 		
	LandsideVehicleInSim* GetBehindVehicle(LandsideVehicleInSim* mpVhicle, double distInLane);
	

	void SetPath(const CPath2008& path );
	const CPath2008& GetPath()const{ return m_path; }
public:
	LandsideLaneNodeList mvEntryExits;
protected:
	virtual void Clear();	
protected:
	CPath2008 m_path;
	DistanceUnit m_dLength;	
};


//stretch lane 
class LandsideStretchLaneInSim : public LandsideLaneInSim
{
public:
	LandsideStretchLaneInSim(LandsideStretchInSim* pStretch, int idx);
	virtual LandsideLayoutObjectInSim* getLayoutObject()const;
	virtual LandsideStretchLaneInSim* toStretchLane(){ return this; }
	virtual StretchSpeedControlData* GetSpeedControl(double dist)const; 
	virtual CString print() const;
	int GetLaneIndex()const{ return m_Idx; }

	//lane entry exits
	void SetFirstLaneEntry(LandsideLaneEntry* pEntry){ mpFirstEntry = pEntry; }
	void SetLastLaneExit(LandsideLaneExit* pExit){ mpLastExit = pExit; } 
	LandsideLaneEntry* GetFirstLaneEntry()const;
	LandsideLaneExit* GetLastLaneExit()const;

	void OnVehicleEnter( LandsideVehicleInSim* pVehicle, DistanceUnit enterDist, const ElapsedTime& t );

	virtual LandsideLaneEntry* GetPreLaneEntry(double dist)const;

	//is the lane empty in the dist range
	//bool isSpaceEmpty(LandsideVehicleInSim* pV, DistanceUnit distF, DistanceUnit distT)const;
	void SetHasParkingSpot(bool b){ m_bHasParkingSpot = b; }
	bool HasParkingSpot()const{ return m_bHasParkingSpot; }

	//cross walks in this lane
	struct CrossWalkInlane
	{
		CCrossWalkInSim* pCross;
		DistanceUnit distInlane;
		bool operator<(const CrossWalkInlane& other)const{ return distInlane< other.distInlane; }
	};
	void AddCross(CCrossWalkInSim* pCross,const CPoint2008& crosspos);
	CCrossWalkInSim* GetAheadCross(LandsideVehicleInSim* pVeh, double dfromDist, double& dCrossDist)const;
	void ReleaseCrossOcys(LandsideVehicleInSim* pVeh,double dDist);

	void LeaveCrosswalkQueue(LandsideVehicleInSim* pVeh,double dDist);

	//
	virtual LandsideLaneInSim* getLeft()const;
	virtual LandsideLaneInSim* getRight()const;
	virtual CPath2008 GetSubPathDist(DistanceUnit distF, DistanceUnit distT)const;

	virtual bool IsSameDirToLane(LandsideLaneInSim* plane)const;	

	bool IsPositiveDir()const;

	LandsideLaneEntry* GetNextLaneEntry(double dist)const;
	LandsideLaneExit* GetNextLaneExit(double dist)const;
	//get exits in this Lane;
	LandsideLaneExitList GetLaneExitsAfterEntry(LandsideLaneEntry* pEntry)const;
	//get exits of other lanes for change lanes
	LandsideLaneExitList GetOtherLaneExitsAfterEntry(LandsideLaneEntry* pEntry)const;

	virtual LandsideLaneExitList GetLaneExitsAfterDist(double dist)const;
	LandsideLaneEntry* GetNextLaneEntry(LandsideLaneEntry* pEntry)const;

	void AddLaneSeg(LandsideStretchSegmentInSim* pobj);


	virtual LandsideVehicleInSim* GetAheadVehicle(LandsideVehicleInSim* mpVehicle, DistanceUnit distInLane, DistanceUnit& aheadDist);

protected:
	LandsideStretchInSim* mpParent;
	int m_Idx;
	bool m_bHasParkingSpot;
	typedef std::vector<CrossWalkInlane> CrossWalkInlaneList;
	CrossWalkInlaneList m_crosswalks;
	LandsideLaneEntry* mpFirstEntry;
	LandsideLaneExit* mpLastExit;

	std::vector<LandsideStretchSegmentInSim*> m_vStretchSegment;

	virtual void Clear();

};

class LandsideStretch;

class StretchSpeedControlData;
class LandsideStretchInSim : public LandsideLayoutObjectInSim, public LandsideResourceInSim
{
public:
	LandsideStretchInSim(LandsideStretch* pStretch);	
	virtual LandsideResourceInSim* IsResource(){ return this;}
	virtual LandsideLayoutObjectInSim* getLayoutObject()const;
	virtual CString print()const;


	//find the nearest lane to the Point return the Lane and the dist in the lane
	LandsideStretchLaneInSim* GetNearestLane(const CPoint2008& pos, DistanceUnit& distInlane )const;

	void AddCrossWalk(CCrossWalkInSim* pCross);
	//lane s
	int GetLaneCount()const{ return m_vLanes.getCount(); }
	LandsideStretchLaneInSim* GetLane(int idx)const;


	virtual void InitRelateWithOtherObject(LandsideResourceManager* pRes);
	virtual void InitGraphNodes(LandsideRouteGraph* pGraph)const;
	virtual void InitGraphLink(LandsideRouteGraph* pGraph)const;    
	CPath2008 m_Path;
protected:	
	UnsortedContainer<LandsideStretchLaneInSim>  m_vLanes;
};


