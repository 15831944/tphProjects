#pragma once

#include "AirsideResource.h"
#include <map>

#include "../../Common/ARCVector.h"
#include "../../Common/point.h"

#include "./../../InputAirside/Runway.h"
#include "TaxiwayResource.h"
#include "HoldPositionInSim.h"
#include "QueueToTakeoffPos.h"
//#include "RunwaysController.h"
#include "AirsideObserver.h"

class RunwayInSim;
class Clearance;
class LogicRunwayInSim;
class FlightGroundRouteDirectSegInSim;
class CRunwayCrossSpecificationInSim;
class CWaveCrossingSpecificationItem;
class CWakeUpRunwayEvent;
class QueueToTakeoffPos;
class AirsideCircuitFlightInSim;

//Runway exit of the logic runway
class ENGINE_TRANSFER RunwayExitInSim 
{	
public: 
	//construct from input	
	RunwayExitInSim(const RunwayExit & rwExitInput,FlightGroundRouteDirectSegInSim* pSeg, LogicRunwayInSim * pLogicRunway);

public:
	
	double GetAngle()const;
	DistanceUnit GetExitRunwayPos()const;
	int GetID()const;
	int GetIntersectionID() const;
	
	DistanceUnit GetEnterTaxiwayPos()const;
	DistanceUnit GetEnterRunwayPos()const;
	DistanceUnit GetPosAtRunway()const;

	bool operator < (const RunwayExitInSim& rwExit)const;	

	FlightGroundRouteDirectSegInSim * GetRouteSeg(){ return m_pRouteDirSeg; }
	LogicRunwayInSim * GetLogicRunway()const{ return m_pLogicRunway; }

	bool IsLeft()const;

	bool CanServeFlight(AirsideFlightInSim *pFlight);

	bool IsQueueWaitingForCrossOtherRunwayFull(AirsideFlightInSim* pFlight);//will set queue full 
	bool CanHoldFlight(AirsideFlightInSim* pFlight);  //check this exit can hold more flight
	RunwayInSim* GetQueueWaitingRunway();
	
	//this methold to check the runway exit is full or not
	//used for wave crossing
	//if the runway exit queue cannot hold the flight,set the queue flag to true
	bool CheckRunwayExitFull(AirsideFlightInSim* pFlight);

	bool IsExistFailedExitFlight();
	void AddFaildExitFlight(AirsideFlightInSim* pFlight);

	const RunwayExit& GetExitInput(){ return m_ExitInput; }

	CPoint2008 getPos()const;
	bool getPath(CPath2008& fullPath, RUNWAY_MARK  port, DistanceUnit dDist = 0.0) const;
	bool getFilletPath(CPath2008& fullPath, RUNWAY_MARK  port, DistanceUnit dDist = 0.0) const;
	
	//check is there any flights goint to take off from this exit
	bool IsFlightsGoingToTakeoff()const;
	void RegTakeoffPos(AirsideFlightInSim* pFlt);
	void UnRegTakeoffPos(AirsideFlightInSim* pFlt);

	QueueToTakeoffPos* getTakeoffQueue();
protected:
	//input data
	RunwayExit m_ExitInput;	 
	LogicRunwayInSim * m_pLogicRunway;
	std::vector<AirsideFlightInSim*> m_vFailedExitFlights;		//to record the failed exit runway flights when the exit be occupied
	std::set<AirsideFlightInSim*> m_vRegtakeoffFlts; //
public:
	//Relate Date In Sim
	FlightGroundRouteDirectSegInSim* m_pRouteDirSeg;

};

class RunwayInSim;
class ClearanceItem;
class TakeoffQueueInSim;
class RunwaySegInSim;
//
class ENGINE_TRANSFER LogicRunwayInSim : public AirsideResource, public CAirsideSubject
{
public:
	typedef ref_ptr<LogicRunwayInSim> RefPtr;
	
public:
	LogicRunwayInSim(RunwayInSim * pRunway, RUNWAY_MARK porttype);
	ResourceType GetType()const{ return AirsideResource::ResType_LogicRunway; }
	virtual CString GetTypeName()const{ return _T("LogicRunway"); }
	virtual CString PrintResource()const ;
	void getDesc(ResourceDesc& resDesc);

	virtual CPoint2008 GetDistancePoint(double dist)const;
	double GetPointDist(const CPoint2008& pt)const;

	RunwayInSim * GetRunwayInSim()const{ return m_pRunway; }	

	RunwayExitInSim * GetExitAt(int idx){ return &m_vExitsList[idx]; }
	int GetExitCount()const{ return static_cast<int>(m_vExitsList.size()); }

	RunwayExitInSim * GetExitByID(int Id);
	RunwayExitInSim* GetExitByRunwayExitDescription(const RunwayExitDescription& runwayDes);

	double GetEndDist()const;

	LogicRunwayInSim * GetOtherPort()const;

	ARCVector3 GetDirection()const;

	DistanceUnit GetLandingThreshold()const;
	//
	bool FindClearanceInConcern(AirsideFlightInSim * pFlight,  ClearanceItem& lastItem ,DistanceUnit radius, Clearance& newClearance);
	
	// set enter time of flight
	virtual void SetEnterTime(CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode);
	// set exit time of flight
	virtual void SetExitTime(CAirsideMobileElement * pFlight, const ElapsedTime& exitT);

	bool IsLeftPoint(const CPoint2008& pt);

	//lock 
	virtual void ReleaseLock(AirsideFlightInSim * pFlight,const ElapsedTime& tTime);
	virtual bool GetLock(AirsideFlightInSim * pFlight);
	virtual AirsideFlightInSim * GetLockedFlight();
	virtual bool TryLock(AirsideFlightInSim * pFlight);

	virtual const ElapsedTime GetLastReleaseTime()const;
	

	//add flight to the queueToTakeoffPos
	void AddFlightToTakeoffQueue(AirsideFlightInSim*pFlight, RunwayExitInSim* pTakeoffPos,const ElapsedTime& tTime );
	void RemoveFlightInQueue(AirsideFlightInSim* pFlight);
	bool IsFlightInQueueToTakeoffPos(AirsideFlightInSim* pFlight, RunwayExitInSim* pTakeoffPos);
	int GetAheadFlightCountInQueueToTakeoffPos(AirsideFlightInSim* pFlight);
	ElapsedTime GetFlightEnterQueueTime(AirsideFlightInSim* pFlight)const;
	bool IsFlightInQueueToRunway(AirsideFlightInSim* pFlight, LogicRunwayInSim* pRunway)const;

	//get the exit correspond to  the exit of the opposite logic runway
	RunwayExitInSim* GetSameNodeRunwayExitWith(RunwayExitInSim* pOppsiteRunwayExit);

	CPoint2008 GetMissApproachCheckPoint(CPoint2008 LastWaypointPos);
	CPoint2008 GetLowAndOverPoint(AirsideCircuitFlightInSim* pFlight,const CPoint2008& LastWaypointPos)const;
	CPoint2008 GetFlightTouchdownPoint(AirsideFlightInSim* pFlight);
	RunwayExitInSim* GetAvailableExit(AirsideFlightInSim* pFlight);

public:
    std::vector<RunwayExitInSim> m_vExitsList;
	RUNWAY_MARK getLogicRunwayType()const {return m_port;}
	
	IntersectionNodeInSimList& GetIntersectionList(){ return m_vIntersectionWithRunways; }

	int GetIntersectRunwayCount()const{ return static_cast<int>(m_vIntersectRunways.size()); }
	LogicRunwayInSim * GetIntersectRunway(int idx)const{ return (LogicRunwayInSim*)m_vIntersectRunways.at(idx); }
	void AddIntersectRunway(LogicRunwayInSim * pRunway){ m_vIntersectRunways.push_back(pRunway); }
 
	ElapsedTime GetNearestLandingTime(AirsideFlightInSim *pFlight)const;
	ElapsedTime GetNearestTakeoffTime(AirsideFlightInSim * pFlight);
	ElapsedTime GetNearestPositionTakeoffTime(AirsideFlightInSim * pFlight)const;	

	bool IsRunwayClear(AirsideFlightInSim* pFlight)const;
	void AddFlightOnRunway(AirsideFlightInSim* pFlight);
	void RemoveLeftFlight(AirsideFlightInSim * pFlight);

	void RemoveLeftFlight(AirsideFlightInSim* pFlight, const ElapsedTime& t);

	//int GetTakeoffQueueCount()const{ return static_cast<int>(m_vTakeoffQueses.size()); }
	//TakeoffQueueInSim * GetTakeoffQueue(int idx)const { return m_vTakeoffQueses[idx]; }
	int GetDepartureQueueLength()const;
	const int GetTakeoffPostionQueueLength(RunwayExitInSim* pExit);
	ElapsedTime GetDepartureQueueWaitingTime(const ElapsedTime& curTime)const; // average  waiting time of flights in Queue
	QueueToTakeoffPosList& GetQueueList(){ return m_vQueueList; }

	void AddIntersectionNodeInSim(IntersectionNodeInSim* pIntersectionNodeInSim);

	void GetIntersectRunways(std::vector<std::pair<int,IntersectionNodeInSim *> >& vRunwayID);
 	void GetIntersectNodeInRange(double dMinDist,double dMaxDist, std::vector<IntersectionNodeInSim *>& vRunwayIntNode);

	//void AddFlightOccupyRunwayInfo(AirsideFlightInSim* pFlight, const CRunwaySystem::RunwayOccupyInfoList& vOccupyInfo);
	//CRunwaySystem::RunwayOccupyInfoList GetFlightOccupyRunwayInfo(AirsideFlightInSim* pFlight);
	
	void SetHasMissedApproachFlight();
	bool HasHasMissedApproachFlight(){ return m_bHasMissedApproachFlight; }

	CString GetMarkName();
protected:
	//std::vector<TakeoffQueueInSim *> m_vTakeoffQueses;
	QueueToTakeoffPosList m_vQueueList;
	
	RunwayInSim *  m_pRunway;
	RUNWAY_MARK m_port;
	std::vector<LogicRunwayInSim* > m_vIntersectRunways;	
	//std::map<AirsideFlightInSim*,CRunwaySystem::RunwayOccupyInfoList> m_mapFlightOccupyRunwayInfo;
	IntersectionNodeInSimList m_vIntersectionWithRunways;

	//flights occupied runway;
	std::vector<AirsideFlightInSim*> m_vOccupiedFlights;
	bool m_bHasMissedApproachFlight;

	
};


class ENGINE_TRANSFER RunwayInSim : public FlightGroundRouteInSim, public Referenced
{
public:
	enum RunwayState
	{
		RunwayState_Landing = 0,
		RunwayState_TakeOff,
		RunwayState_WaveCross
	};


friend class RunwayResourceManager;
public:
	typedef ref_ptr<RunwayInSim> RefPtr;

public:
	LogicRunwayInSim * GetLogicRunway1(){ return m_LogicRwy1.get(); }
	LogicRunwayInSim * GetLogicRunway2(){ return m_LogicRwy2.get(); }
	
	explicit RunwayInSim(Runway* pRunway, double dMissApproachDist);
	~RunwayInSim();
public:
	//init from a runway input
	Runway * GetRunwayInput()const{ return (Runway*)m_RunwayInput.get(); }	

public:
	RUNWAY_MARK GetActivePort()const { return m_activePort; }
	void SetActivePort(AirsideFlightInSim * pFlight, RUNWAY_MARK port){ m_pLockFlight = pFlight, m_activePort = port; }
	AirsideFlightInSim * GetLockFlight(){ return m_pLockFlight; }
	void SetLockFlight(AirsideFlightInSim * pFlight){ m_pLockFlight = pFlight; }

	bool bCanFlightCross(AirsideFlightInSim* pFlight,const ElapsedTime& eTime)const;

	bool InitSegments(IntersectionNodeInSimList& NodeList,const HoldShortLinePropList&  );
	
	//init runway wave crossing settings
	void SetWaveCrossSpecification(CWaveCrossingSpecificationItem* pWaveCrossSettingItem );
	CRunwayCrossSpecificationInSim* GetWaveCrossSpecification();

	RunwaySegInSim * GetSegment(int idx);
	int GetSegmentCount ()const{ return static_cast<int>(m_vSegments.size()); }	
	void GetRunwayDirectSegmentList(int nIntersectNodeIDFrom, int nIntersectNodeIDTo, FlightGroundRouteDirectSegList& taxiwayDirectSegList);

	void AddTaxiwayHoldPosition(HoldPositionInSim *pHoldPosition);
	//have ,pointer
	//not ,return NULL
	HoldPositionInSim* GetHoldPosition(int nHoldPosID)const;
	HoldPositionInSim* GetHoldPosition(int nTaxiID, int nNodeID )const;

	void CheckWaveCross(AirsideFlightInSim *pFlight,const ElapsedTime& eTime);
	void NotifyWaveCross(AirsideFlightInSim *pFlight,const ElapsedTime& eTime);
	//get next flight after the give flight
	std::vector<AirsideFlightInSim *>GetNextFlightAfter(AirsideFlightInSim *pCurFlight);
	
	//set runway's state
	void SetRunwayState(RunwayState runwayState);
	RunwayState GetRunwayState() const;


	//Add Waiting runway flight, used by delay by cross runway
	void AddWaitingFlight(AirsideFlightInSim *pFlight);

	//void AddWaveCrossFlight(AirsideFlightInSim *pFlight);

	void RemoveWaveCrossFlight(AirsideFlightInSim *pFlight); //can 
	
	//wake up the waiting flight which delay by wave cross
	void WakeUpWaitingFlights(const ElapsedTime& eTime);

	//remove the flight's occupancy information
	void RemoverRunwayOccupancyInfo(AirsideFlightInSim *pFlight);

	//wake up runway landing operation, 
	void WakeUpRunwayLanding(const ElapsedTime& eEventTime);

	ElapsedTime GetLandingSafeTime() const { return m_eLandingSafeTime; }
	void SetLandingSafeTime(ElapsedTime val) { m_eLandingSafeTime = val; }

	int GetRunwayID();

	double GetMissApproachDistanceToRunway(){ return m_dMissApproachDistance; }

	void SetWakeupRunwayEvent(CWakeUpRunwayEvent *pWakeUpRunwayEvent);
	CWakeUpRunwayEvent * GetWakeupRunwayEvent();

	void AddWaveCrossHold(HoldPositionInSim *pHold, int pWaveCrossCount);
protected:
	CWakeUpRunwayEvent *m_pWakeUpRunwayEvent;
	RunwayState m_runwayState;
	RUNWAY_MARK m_activePort;
	AirsideFlightInSim * m_pLockFlight;

	//the runway's safe time to wave cross,the time of the flight cross runway should be less than this time.
	ElapsedTime m_eLandingSafeTime;

protected:	
	Runway::RefPtr  m_RunwayInput;

	LogicRunwayInSim::RefPtr m_LogicRwy1;
	LogicRunwayInSim::RefPtr m_LogicRwy2;

	double m_dMissApproachDistance;

	std::vector<RunwaySegInSim*>  m_vSegments;  //sequence of segments of the taxiway;	

	//the hold positions, which belong to taxiway. NOTE:Runway has no hold position.
	std::vector<HoldPositionInSim *> m_vTaxiwayHodPostions;

	//the list runway cross specification
	std::vector<CRunwayCrossSpecificationInSim*> m_vWaveCrossSettings;
	
	//waiting flight list, delay by cross runway
	std::vector<AirsideFlightInSim *> m_vWaitingFlight;

	//the flight list need to pass the runway in one case
	//std::vector<AirsideFlightInSim *> m_vWaveCrossFlight;

	//it store the hold that need to do wave cross
	std::vector<HoldPositionInSim *> m_vWaveCrossHold;

};

