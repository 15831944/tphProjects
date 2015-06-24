#pragma once
#include "LandsideVehicleStateMovement.h"
#include "LandsideVehicleInSim.h"
//////////////////////////////////////////////////////////////////////////
class LandsideVehicleMoveInRoad : public IVehicleStateMovement
{
public:
	LandsideVehicleMoveInRoad(LandsideVehicleInSim*pVehicle);
	bool SetOrignDest(LandsideResourceInSim* pOrign, LandsideResourceInSim* pDest,CARCportEngine *_pEngine );
	bool SetOrignDest(LandsideLaneNodeInSim* pOrign, LandsideResourceInSim* pDest,CARCportEngine *_pEngine );	

	virtual IVehicleStateMovement* proceedTo(CARCportEngine *_pEngine);
	virtual int getState()const{ return _moveOnRoad; }

	IVehicleStateMovement * gotoNextState(CARCportEngine *_pEngine);

	//return whether a block ahead in the radius of concern
	bool GetAheadBlock(double& distToHead, double& dAheadSpd);
	LandsideVehicleInSim * LandsideVehicleMoveInRoad::GetAheadVehicle(LandsideResourceInSim* pRes, double& dMinDist )const;
protected:
	LandsideResourceInSim* mpOrign;
	LandsideResourceInSim* mpDest;
	LandsideLaneNodeList mRoutePath;

	void UpdateTotalPath();
	//
	ARCPath3 mTotalPath;
	std::vector<int> nSegChangePtIndex;  //which point it will change seg,
	std::vector<double> nSegChangeDist; //at which dist will change seg
	int GetSegIndex(double dDist)const;	
	const ARCPath3* GetSegPath(int nSegIndex, ARCPath3& path)const; //possible return input path 
	virtual double GetSegStartDistInRes(int nSegIndex)const;	
	virtual LandsideResourceInSim* GetSegResource(int nSegIndex)const;
	
protected: //move in path
	//return true if done current state 
	//void translate(double dAcc, const ElapsedTime& dt); //moving
	bool IsOutRoute()const;
	void DoMovementsInPath( double dEndSpd,double dS,MobileState lastmvPt ,const ElapsedTime& dEndT);	
	double m_dLastDistInRoute;  //position in path
protected:
	//bool ChangeLane(CARCportEngine *_pEngine);
	bool MoveInLane(CARCportEngine *_pEngine); //return true if moving	

	//bool ChangeToLane(LandsideLaneInSim* pFromLane, LandsideLaneInSim* pToLane, CARCportEngine *_pEngine,bool bLeft);
	
	LandsideVehicleInSim * CheckConflict(const ARCPolygon& poly, const ARCVector3& dir, LandsideResourceInSim* pRes );
	virtual LandsideVehicleInSim* CheckNextConflict(const ARCPolygon& poly, const ARCVector3& dir);
};
//
//////////////////////////////////////////////////////////////////////////
class LandsideCurbSideInSim;
class LandsideVehicleMoveInCurbside : public LandsideVehicleMoveInRoad
{
public:
	LandsideVehicleMoveInCurbside(LandsideVehicleInSim*pVehicle, LandsideCurbSideInSim* pCurb,LandsideLaneNodeInSim* pFromNode);

	virtual IVehicleStateMovement* proceedTo(CARCportEngine *_pEngine);	
	virtual int getState()const{ return _moveOnCurbside; }
	//virtual bool InResource(LandsideResourceInSim* pRes)const;
	virtual LandsideVehicleInSim* CheckNextConflict(const ARCPolygon& poly, const ARCVector3& dir);
	virtual LandsideResourceInSim* GetSegResource(int nSegIndex)const;

	LandsideCurbSideInSim* m_pCurbside;

	bool bReadyToLeave()const;
	void doPickDrop(const ElapsedTime& t, CARCportEngine* _pEngine);

	//void InitMove(const MovePointInRoad& mvp);
protected:
	

};