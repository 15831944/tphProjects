#pragma once
#include "landside/LandsideVehicleLogEntry.h"
#include "airside/ARCMobileElement.h"
#include "LandsideStretchInSim.h"
#include "Landside/VehicleLandSideDefine.h"
#include "./IDModel.h"
#include "core/SAgent.h"
#include "LandsideVehicleStateMovement.h"
#include "Results/paxentry.h"
#include "Landside/CLandSideVehicleType.h"
#include <common/HierachyName.h>
#include "MovingTrace.h"
#include "landside/BlockTag.h"
#include "Common/LandsideVehicleState.h"
#include "Landside/LandsideResidentRelatedContainer.h"
#include "core/State.h"


enum OpMoveType  //operation move in road
{
	_STOP,
	_MAXBREAK,
	_SLOWDOWN,
	_UNIFORM,
	_SLOWACC,
	_MAXACC,
	_OpMoveTypeCount,
};
class MoveOperationTable : public std::vector<double> 
{
public:
	void InitFromPerform(const MobileGroundPerform& perfom);
};


//////////////////////////////////////////////////////////////////////////
class CARCportEngine;
class CVehicleItemDetail;
class OutputLandside;
class MobLogEntry;
enum enumVehicleLandSideType; 
class CMobileElemConstraint;
class LandsideBusStationInSim;
class LandsideSimulation;
class ResidentVehicleRouteList;
class LandsidePaxVehicleInSim;
class IVehicleStateMovement;
class LandsideBaseBehavior;
class BlockTag;
class LandsideVehicleProperty;
class LandsideResidentVehicleInSim;
class LandsideVehicleServiceEndEvent;
class LandsidePublicExtBusInSim;
class LandsideTaxiInSim;
class LandsidePrivateVehicleInSim;
class CurbsideStrategyStateInSim;
class ParkinglotStrategyStateInSim;
class IParkingSpotInSim;

class landsideResourceQueue
{
public:
	landsideResourceQueue()
	{
		m_bWaitInResQueue = false;
		m_pRes = NULL;
		m_pCrosswalk = NULL;
	}	
	

	void SetWaitInQueueFlag(bool bFlag)
	{
		m_bWaitInResQueue = bFlag;
	}

	bool GetWaitInQueueFlag()const
	{
		return m_bWaitInResQueue;
	}

	void SetlandsideRes(LandsideLayoutObjectInSim* pRes)
	{
		m_pRes = pRes;
	}

	LandsideLayoutObjectInSim* GetLandsideRes()const {return m_pRes;}

	void SetCrosswalk(CCrossWalkInSim* pCrosswalk)
	{
		m_pCrosswalk = pCrosswalk;
	}

	CCrossWalkInSim* GetCrosswalk()const {return m_pCrosswalk;}

	bool bResourceQueue()const
	{
		if (m_pRes)
			return true;
		return false;
	}
	bool CrosswalkQueue()const
	{
		if (bResourceQueue())
			return true;

		if (m_pCrosswalk)
			return true;
		return false;
	}
	bool WaitInCrosswalkQueue(CCrossWalkInSim* pCrosswalk)const
	{
		if (m_bWaitInResQueue == false)
			return false;
		
		if (CrosswalkQueue() == false)
			return false;
		
		if (m_pCrosswalk != pCrosswalk)
			return false;

		return true;
	}

	bool WaitInResourceQueue(LandsideLayoutObjectInSim* pRes)const
	{
		if (m_bWaitInResQueue == false)
			return false;
		
		if (bResourceQueue() == false)
			return false;

		if (m_pRes != pRes)
			return false;
		
		return true;
	}

	void EnterResourceQueue(LandsideLayoutObjectInSim* pRes)
	{
		if (m_bWaitInResQueue == true)
			return;
		
		m_pRes = pRes;
	}

	void LeaveResourceQueue()
	{
		m_bWaitInResQueue = false;
		m_pRes = NULL;
	}

	void EnterCrosswalkQueue(CCrossWalkInSim* pCrosswalk)
	{
		if (m_bWaitInResQueue == true)
			return;
		
		m_pCrosswalk = pCrosswalk;
	}

	void LeaveCrosswalkQueue()
	{
		m_bWaitInResQueue = false;
		m_pCrosswalk = NULL;
	}
private:
	bool m_bWaitInResQueue;//check vehicle whether wait in resource queue
	LandsideLayoutObjectInSim* m_pRes; //which landside resource generate queue
	CCrossWalkInSim* m_pCrosswalk;
};

typedef std::vector<landsideResourceQueue*>  VehicleResourceQueue;
class LandsideVehicleInSim : public SAgent, public IStateMachine
{
public:
	//constructor
	//inherited classes
	virtual ~LandsideVehicleInSim();
	virtual LandsideResidentVehicleInSim *toResidentVehicle(){ return NULL; }
	virtual LandsidePublicExtBusInSim *   toPublicExBus(){ return NULL; }
	virtual LandsideTaxiInSim*			  toTaxi(){ return NULL; }
	virtual LandsidePrivateVehicleInSim*  toPrivate(){ return NULL; }

	virtual ResidentVehiclePlan* getRoutePlan()=0;

	virtual bool InitBirth(CARCportEngine *_pEngine);
	void OnActive( CARCportEngine*pEngine );
	//virtual void Kill(LandsideSimulation*pEngine,const ElapsedTime&t);
	bool IsTerminated()const;
	void ShowError(const CString& sError, const CString& erroType);

	//vehicle properties
	int getType()const{ return 0; } //should be remove
	int getID()const;
	void SetProperty( LandsideVehicleProperty* pProperty );
	int getCapacity()const{ return m_nCapacity; }		
	double GetLength()const{ return m_dLength; }
	double GetHalfLength()const{ return m_dLength*0.5; }
	double GetWidth()const{ return m_dWidth; }
	const CHierachyName& getName()const{ return m_TypeName; }
	double getSpeed(LandsideResourceInSim* pRes,double dist = 0);
	//active 
	//virtual bool DefaultHandleSignal(SSignal*pmsg);
	//virtual bool GoalProcessToEnd( CARCportEngine *_pEngine );



	//functions relate with states 
	void initState(const MobileState& s);
	MobileState& getState();
	MobileState& getState(const ElapsedTime&t);
	const MobileState& getLastState()const;
	void UpdateState(const MobileState& state);	//add next steps

	//conflict resolve
	//ARCPolygon& getBBox(const ElapsedTime& t);
	
	virtual bool bCanWaitFor( const LandsideVehicleInSim* pOtherPax );

	//movements 
	virtual void StartMove(const ElapsedTime&t);
	void WaitForVehicle(LandsideVehicleInSim* pOtherPax);


	//functions  relate with on vehicle pax
	void AddOnPax(LandsideBaseBehavior* pPax);
	void RemoveOnPax(LandsideBaseBehavior*pPax,const ElapsedTime& t);
	bool IsPaxOnVehicle(int nId)const;
	virtual bool IsPaxsVehicle( int nPaxId ) const{ return false; }
	int GetOnVehiclePaxNum()const{ return m_vOnVehiclePax.size(); }
	virtual void NotifyPaxMoveInto(CARCportEngine*pEngine, LandsideBaseBehavior* pPassenger, const ElapsedTime& eTime);

	//minimal dist that 
	DistanceUnit GetSafeDistToFront()const;


	eLandsideVehicleState m_curMode;


	//virtual void flushState(const ElapsedTime& t);
	CCarChangeLaneModel& GetChangeLaneModel(){ return m_ChangeLane; }
	const CCarChangeLaneModel& GetChangeLaneModel()const{ return m_ChangeLane; };


	CPoint2008 getOffVehiclePos(const CPoint2008& nextPt)const;


	MobileGroundPerform mPerform;
	MoveOperationTable m_opTable;
	IDModel m_IDModel;


	DistanceUnit getDecDist( double dSpeed );
	//block tags
	bool isBlocked()const;
	void addBlock(BlockTag* block);
	bool removeBlock(BlockTag* block,const ElapsedTime& t); 
	bool hasBlock(BlockTag* block)const;


	void MoveToPos(MobileState state);

	void Continue();
	void StepTime(CARCportEngine* pEngine);

	virtual void OnStateChanged();

	
	virtual bool InitBirthInLayoutObject(LandsideLayoutObjectInSim* pObj,CARCportEngine* pEngine);
	virtual void SuccessProcessAtLayoutObject(LandsideLayoutObjectInSim* pObj,LandsideResourceInSim* pDetailRes, CARCportEngine* pEngine);
	virtual void SkipProcessLayoutObject(LandsideLayoutObjectInSim* pObj,CARCportEngine* pEngine);
	virtual void LeaveLayoutObject( LandsideLayoutObjectInSim* pObj, LandsideResourceInSim* pDetailRes, CARCportEngine* pEngine );
	virtual void ArrivalLayoutObject(LandsideLayoutObjectInSim* pObj,LandsideResourceInSim* pDetailRes, CARCportEngine* pEngine );

	virtual void SuccessBirthInLotSpot(LandsideParkingSpotInSim* plot, CARCportEngine* pEngine);
	virtual void SuccessParkInLotSpot(LandsideParkingSpotInSim* plot);
	virtual void SuccessParkInCurb(LandsideCurbSideInSim*pCurb,IParkingSpotInSim* spot);
	virtual void SuccessParkInBusStation(LandsideBusStationInSim* pStation, IParkingSpotInSim* spot);
	virtual void FailedParkInCurb(LandsideCurbSideInSim*pCurb,CARCportEngine* pEngine);


	virtual bool ProceedToNextFcObject(CARCportEngine* pEngine);

	virtual void ChangeVehicleState(IState* nextState);
	virtual void ChangeStateMoveToDest(CARCportEngine* pEngine=NULL); //change to move to dest state

	//the event send out to notify fix service time end
	//bool IsServiceTimeEnd()const;
	//virtual void OnServiceTimeEnd(CARCportEngine* pEngine, const ElapsedTime& eEventTime);
	//void NewServiceTimeEndEvent(const ElapsedTime& endTime);  //create a new service time countdown
	//LandsideVehicleServiceEndEvent* m_pServiceEndEvt;
	
	const IDModel& getIDModel()const{ return m_IDModel; }

	//resource queue
	bool GetWaitResourceQueueFlag()const;
	landsideResourceQueue* GetLastResourceQueue()const;
	bool WaitInResourceQueue(LandsideLayoutObjectInSim* pRes)const;
	bool WaitInCrosswalkQueue(CCrossWalkInSim* pCrosswalk)const;
	CPath2008& getSpanPath(){ return m_tPath; }

	void EnterResourceQueue(LandsideLayoutObjectInSim* pRes);
	void LeaveResourceQueue(LandsideLayoutObjectInSim* pRes);

	void EnterCrosswalkQueue(CCrossWalkInSim* pCrosswalk);
	void LeaveCrosswalkQueue(CCrossWalkInSim* pCrosswalk);

	void ClearResourceQueue();



	void MoveToExtNode(LandsideExternalNodeInSim* pNode);
	LandsideLayoutObjectInSim* getDestLayoutObject();

	inline CurbsideStrategyStateInSim* getCurbStragy(){ return m_pCurbStratgy; }
	inline ParkinglotStrategyStateInSim* getParklotStragy(){ return m_pParklotStratgy; }
	CurbsideStrategyStateInSim* BeginCurbStrategy(const ALTObjectID& curbName, CARCportEngine* pEngine);
	ParkinglotStrategyStateInSim* BeginLotStrategy(const ALTObjectID& lotName, CARCportEngine* pEngine);
	void EndCurbStrategy();
	void EndLotStrategy();


	bool bToSameDest()const{ return m_pCurLayoutObject == m_pPreviousLayoutObject; }

	//state is in loading pax;
	bool IsLoadingPax()const{ return m_bLoadingPax ;}
	void SetLoadingPax(bool b){ m_bLoadingPax = b; }



	void DoParkingToSpot(IParkingSpotInSim* pSpot);
protected:	
	LandsideVehicleInSim();	
	void ChangeDest(LandsideLayoutObjectInSim* pdest);
	virtual void OnTerminate(CARCportEngine*pEngine);

	int m_id;
	CHierachyName m_TypeName;

	//make the vehicle stop until all the block tag removed
	std::vector<BlockTag*> m_vBlockTags; 
	//SGoalBase* m_pGoal;

	CCarChangeLaneModel m_ChangeLane;
	
	//wait loop
	const LandsideVehicleInSim* FindEndMovablePax( const LandsideVehicleInSim* pBreakPax ) const;
	LandsideVehicleInSim* m_pWaitVeh;

	//state 
	MobileState m_lastState;  //last logged state
	//bbox at time;
	//ARCPolygon m_tBBox;	
	CPath2008 m_tPath;


	bool	m_bInit;

	double	m_dLength;
	double	m_dWidth;
	double	m_dHeight;
	int		m_nCapacity;		//pax capacity

	LandsideVehicleLogEntry m_logEntry;

	std::vector<LandsideBaseBehavior*> m_vOnVehiclePax; //the passengers on the vehicle

	//remember the previous speed
	struct ResourceSpeed
	{
		LandsideResourceInSim* pRes;
		double dSpeed;
		StretchSpeedControlData* pSpdCtrl;
		ResourceSpeed(){ pRes  = NULL ; pSpdCtrl = NULL; }
	};
	ResourceSpeed m_pairResourceSpeed;
	//std::pair<LandsideResourceInSim*, double> m_pairResourceSpeed;

	void flushState( const ElapsedTime& t );

	VehicleResourceQueue m_repResourceQueue;
	//write state to log;
	int WriteLog(const MobileState& state);
	//flush pax log
	virtual void FlushOnVehiclePaxLog(const ElapsedTime& t);
	//flush log
	void FlushLogEntry(LandsideSimulation*pEngine,const ElapsedTime&t );


	//v
	void SetCurbStragy(CurbsideStrategyStateInSim* pS);
	void SetParkLotStragy(ParkinglotStrategyStateInSim* pS);

	ResidentVehicleRoute* getNextRouteItem(ResidentVehicleRouteList* flow, CARCportEngine* pEngine);

private:
	LandsideLayoutObjectInSim* m_pCurLayoutObject;
	LandsideLayoutObjectInSim* m_pPreviousLayoutObject;

	CurbsideStrategyStateInSim* m_pCurbStratgy;
	ParkinglotStrategyStateInSim* m_pParklotStratgy;

	//LandsideResourceInSim* m_pParkPlace;
	bool m_bLoadingPax;

public:
	static int nLastVehicleId;
};

typedef std::vector<LandsideVehicleInSim*> LandsideVehicleList;
