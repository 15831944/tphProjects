#pragma once
#include "Goal_MoveInRoad.h"

#include <map>
//alg 2
//class Goal_MoveInRoad2: public LandsideVehicleGoal
//{
//public:
//	Goal_MoveInRoad2(LandsideVehicleInSim* pEnt, const LandsideLaneNodeList& path, bool bStopAtEnd, bool bChangeLane = false);
//	void Process(CARCportEngine* pEngine);
//	void Activate(CARCportEngine* pEngine);
//protected:	
//	bool ChangeLane(){ return false; }
//	void MoveInLane();
//
//	bool m_bChangeLane;
//	bool m_bStopAtEnd;
//	LandsideLaneNodeList m_rpath; //reversed path
//	//get steps at least dS long, return real steps len
//	void GetNextSteps(  const MobileState& initS, DistanceUnit dS, MovingTrace& mb)const;
//
//	LandsideLaneNodeInSim* m_plastNode;
//	const CPath2008* GetSegPath( LandsideLaneNodeInSim* pNode1, LandsideLaneNodeInSim* pNode2, CPath2008& subPath )const;
//	LandsideResourceInSim* GetSegPathResource( LandsideLaneNodeInSim* pNodePre, LandsideLaneNodeInSim* pNodeNext ) const;
//
//
//	StateInfo m_lastStateInfo;
//	LandsideVehicleInSim * GetConflictVehicle( const ARCPolygon& poly,LandsideResourceInSim* pRes );
//	//check conflict with vehicles
//	LandsideVehicleInSim* CheckVehicleConflict(const MovingTrace& nextpath, double dOffsetS, double dNextSpd );
//	//check conflict with free moving pax
//	PaxLandsideBehavior* CheckPaxConflict(const MovingTrace& nextpath,double dOffsetS,double dNextSpd);
//	//check conflict with cross walk
//	CCrossWalkInSim* CheckCrossWalkConflict(const MobileState&lastState,  double dNextSpd, double dOffsetS);
//	//check conflict with traffic light
//	LandsideIntersectLinkGroupInSim* CheckIntersectionConflict(double& dOffsetS);
//
//	void popPathNodes(int nNum);
//
//	void UpdatePathMap();
//	
//	const CPath2008* GetSegPath( LandsideLaneNodeInSim* pNode1, LandsideLaneNodeInSim* pNode2 )const;
//	
//	typedef std::pair<LandsideLaneNodeInSim*,  LandsideLaneNodeInSim*> NodePair;
//	typedef std::map<NodePair,CPath2008> PathMap;
//	PathMap m_pathMap;
//	
//};
//
