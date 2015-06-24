#pragma once
#include "landsidePath.h"
#include "State_LandsideVehicle.h"
#include "..\LandsideVehicleInSim.h"

class LandsideIntersectLinkGroupInSim;
class PaxLandsideBehavior;
class CCrossWalkInSim;
//////////////////////////////////atom goal , more like behavior tree actions////////////////////////////////////////
//arrival pax Vehicle Logic

enum ConflictEnum
{
	cf_none,
	cf_followingVehicle,
	cf_conflictVehicle,
	cf_cross,
	cf_intersection,
	cf_pax,
};

class ConflictInfo
{
public:
	LandsideVehicleInSim* pFollowingVehicle;
	LandsideVehicleInSim* pConflictVehicle;
	CCrossWalkInSim* pConflictCross ;
	LandsideIntersectLinkGroupInSim* pConflictIntersect ;
	PaxLandsideBehavior* pConflictPax;

	ConflictEnum nearestConflict;
	DistanceUnit sepToConflict;

	ConflictInfo();	
	bool isConflict()const{ return nearestConflict!=cf_none; }
	bool isSameConflict(const ConflictInfo& cf)const;
};

struct DelayInfo
{
	DistanceUnit distStartDelay;
	ConflictInfo conflictInfo;
	ElapsedTime timeStartDelay;
	LandsideResourceInSim* delayRes;
	OpMoveType operation;
	double dSpeedNeed;

	bool IsSameDelay(const DelayInfo& delay)const;
	bool IsDelay()const{ return conflictInfo.isConflict(); }
};



class State_MoveInRoad  : public State_LandsideVehicle<LandsideVehicleInSim>
{
public:
	State_MoveInRoad(LandsideVehicleInSim* pEnt);	
	virtual void Execute(CARCportEngine* pEngine);
	
	virtual void OnMoveOutRoute(CARCportEngine* _pEngine ){}
	void SetPath(const LandsideLaneNodeList& routePath);
protected:	
	LandsideLaneNodeList mRoutePath;	
	LandsidePath m_path;	

	void UpdateTotalPath();
	void DoMovementsInPath(CARCportEngine* _pEngine, const ElapsedTime& fromTime , double dS,double dStarSpd, double dEndSpd );
	bool IsOutRoute(double dDist) const;
	bool ChangeLane(CARCportEngine *_pEngine); //change lane logic
	bool MoveInLane2(CARCportEngine *_pEngine); 
	//do over pass front vehicle

	void RelealseCrossWalk();
	void LeaveCrosswalkQueue();

	void DoChangeLane(CARCportEngine* _pEngine, LandsideLaneInSim* pToLane , DistanceUnit dChangeLaneDist, const LandsideLaneNodeList& routepath);


	//delay report functions
	void ReportDelay(CARCportEngine* _pEngine,const DelayInfo& delay);
	void ReportEndDelay(CARCportEngine* _pEngine, const ElapsedTime& t);
	DelayInfo m_theDelayInfo;
	//end delay functions

	void UpdateConflictResolvePath();


	bool bCanChangeLane( ) const;
protected:	
	double m_dLastDistInRoute;  //position in path

	bool m_bChangeLane;
	bool m_bStopAtEnd;
	int m_nChangeLaneCount;
	ElapsedTime m_tLastChangeLane;
protected:
	//info to report delay 
	//StateInfo m_lastStateInfo;
	//void ReportDelay(CARCportEngine* _pEngine, StateInfo& thisStateInfo);
	void ReportResourceQueue(CARCportEngine* _pEngine, ConflictInfo& conflictInfo);
	void ReportConflictEnd(CARCportEngine* _pEngine, ConflictInfo& conflictInfo);
	void LeaveResourceQueueConflict(CARCportEngine* _pEngine);
	void ResourceQueueMovement(CARCportEngine* _pEngine,LandsideResourceInSim* pStartRes,LandsideResourceInSim* pEndRes); 

	void _addSeg( LandsideLaneNodeInSim* pNodePre ,LandsideLaneNodeInSim* pNodeNext );
	bool _isOutRoute() ;
	DistanceUnit getDecDist(double dSpeed);

	void _getConflictInfo(DistanceUnit distRad,
		ConflictInfo& conflictInfo);	
	//std::vector<MoveOperation> m_opList;  //operation list to test
	LandsideResourceInSim* m_pDestResource;

};


class State_MoveToDest : public State_MoveInRoad
{
public:
	State_MoveToDest(LandsideVehicleInSim* pEnt,LandsideLayoutObjectInSim* pDest);	
	virtual void Entry(CARCportEngine* pEngine);
	virtual void OnMoveOutRoute(CARCportEngine* _pEngine );
	virtual const char* getDesc()const{ return _T("Move to Destination"); }
protected:
	LandsideLayoutObjectInSim* m_pDestObject;
	LandsideLaneEntry* m_pLaneEntry;

};