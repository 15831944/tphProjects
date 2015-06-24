#pragma once
#include "..\core\SGoal.h"
#include "..\LandsideVehicleInSim.h"
#include "Goal_LandsideVehicle.h"
#include "landsidePath.h"
#include "State_MoveInRoad.h"

class LandsideIntersectLinkGroupInSim;
class PaxLandsideBehavior;
//////////////////////////////////atom goal , more like behavior tree actions////////////////////////////////////////
//arrival pax Vehicle Logic



class Goal_MoveInRoad  : public LandsideVehicleGoal
{
public:
	Goal_MoveInRoad(LandsideVehicleInSim* pEnt, LandsideResourceInSim* pDestResource,const LandsideLaneNodeList& path, bool bStopAtEnd, bool bChangeLane = false);	
	void Process(CARCportEngine* pEngine);
	virtual void Activate(CARCportEngine* pEngine);


protected:	
	LandsideLaneNodeList mRoutePath;	
	LandsidePath m_path;	

	void UpdateTotalPath();
	void DoMovementsInPath(CARCportEngine* _pEngine, const ElapsedTime& fromTime , double dS,double dStarSpd, double dEndSpd );
	bool IsOutRoute(double dDist) const;
	bool ChangeLane(CARCportEngine *_pEngine); //change lane logic
	bool MoveInLane(CARCportEngine *_pEngine); //step move in road	
	bool MoveInLane2(CARCportEngine *_pEngine); 
	//do over pass front vehicle
	bool OverPass(LandsideVehicleInSim* pFront, CARCportEngine *_pEngine);

	//conflict
	LandsideVehicleInSim* CheckHeadVehicle(double dOffsetS, double dNextSpd);
	LandsideVehicleInSim* CheckVehicleConflict( double dOffsetS, double dNextSpd );
	CCrossWalkInSim* CheckCrossWalkConflict(double dOffsetS, double dNextSpd, const ElapsedTime& eTime);
	LandsideIntersectLinkGroupInSim* CheckIntersectionConflict(double dOffsetS, double dNextSpd);


	void RelealseCrossWalk();
	void LeaveCrosswalkQueue();

	void DoChangeLane(CARCportEngine* _pEngine, LandsideLaneInSim* pToLane , DistanceUnit dChangeLaneDist, const LandsideLaneNodeList& routepath);


	//delay report functions
	void ReportDelay(CARCportEngine* _pEngine,const DelayInfo& delay);
	void ReportEndDelay(CARCportEngine* _pEngine, const ElapsedTime& t);
	DelayInfo m_theDelayInfo;
	//end delay functions

private:	
	double m_dLastDistInRoute;  //position in path

	bool m_bChangeLane;
	bool m_bStopAtEnd;

	LandsideResourceInSim* m_pDestResource;
	ElapsedTime m_tLastChangeLane;
	int m_nChangeLaneCount;


	bool bCanChangeLane()const;
public:
	BlockTag m_blockInRoad;
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

	void _getConflictInfo(DistanceUnit distRad, ConflictInfo& conflictInfo);	
	//std::vector<MoveOperation> m_opList;  //operation list to test
};

//vehicle birth at entry node
class Goal_BirthAtEntryNode: public LandsideVehicleGoal
{
public:
	Goal_BirthAtEntryNode(LandsideVehicleInSim* pEnt,LandsideExternalNodeInSim* pNode)
		:LandsideVehicleGoal(pEnt)
	{
		m_sDesc = "Goal_BirthAtEntryNode";
		m_pNode = pNode;
	}
	void Process(CARCportEngine* pEngine);
	LandsideExternalNodeInSim* m_pNode;
};


class Goal_ParkingExternalNode : public LandsideVehicleSequenceGoal
{
public:
	Goal_ParkingExternalNode(LandsideVehicleInSim* pExt,LandsideExternalNodeInSim* pNode)
		:LandsideVehicleSequenceGoal(pExt)
	{
		m_sDesc = "Goal_MoveAtExitNode";
		m_pNode = pNode;
	}
	void Process(CARCportEngine* pEngine);
	LandsideExternalNodeInSim* m_pNode;
};

///////////////////Composite Goals ,Organize the flow,  more like behavior tree : sequence node///////////////////////////////////////////////////////
class Goal_MoveToRes : public LandsideVehicleSequenceGoal
{
public:
	Goal_MoveToRes(LandsideVehicleInSim* pEnt, LandsideResourceInSim* pDest);
	virtual ~Goal_MoveToRes(){ delete m_pLaneEntry; }
	void Activate(CARCportEngine* pEngine);
protected:
	LandsideResourceInSim* mpDest;
	LandsideLaneEntry* m_pLaneEntry; //for new dynamic lane entry
};

