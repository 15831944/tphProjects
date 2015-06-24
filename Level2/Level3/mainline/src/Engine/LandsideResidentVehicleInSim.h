#pragma once
#include "LandsideVehicleInSim.h"
#include "LandsideVehicleServiceEndEvent.h"
#include "Landside\State_LandsideVehicle.h"

class ResidenRelatedtVehicleTypeData;
class ResidentRelatedVehicleTypePlan;
class ResidentVehicleRoute;
class LandsideVehicleProperty;
class RensidentVehicleDeployorInSim;

class LandsideFacilityObject;

enum eServiceState
{
	_waitOnbase,
	_preService,  //move to service start 
	_inService,     //in serivce loop
	_endOfService,  //end service go to homebase
};

class LandsideResidentVehicleInSim : public LandsideVehicleInSim
{
public:
	LandsideResidentVehicleInSim( RensidentVehicleDeployorInSim* pVehicleType);
	LandsideResidentVehicleInSim *toResidentVehicle(){ return this; }
	virtual ResidentVehiclePlan* getRoutePlan();

	virtual bool ProceedToNextFcObject(CARCportEngine* pEngine);
	virtual void ArrivalLayoutObject(LandsideLayoutObjectInSim* pObj,LandsideResourceInSim* pDetailRes, CARCportEngine* pEngine );
	virtual void SuccessParkInBusStation( LandsideBusStationInSim* pStation ,LaneParkingSpot* spot);
	bool BeginServicePlan(ResidentRelatedVehicleTypePlan* pPlan,LandsideSimulation* pSim );// deploy to start service
	
	bool IsWaitOnBase()const;

	//check the bus could take the passenger to his destination
	//the parameter should be a bus station or parking lot name
	bool CouldTakeToStation(ALTObjectID altDesination);

	void NotifyPaxMoveInto(CARCportEngine*pEngine, LandsideBaseBehavior* pPassenger, const ElapsedTime& eTime);

	bool IsAvailable() const{ return m_bAvaible; }
	void SetAvailable(bool b){ m_bAvaible = b; }

	void SetPaxGetoffDoneTag(bool bGetoffDone);
	bool GetPaxGetoffDoneTag()const;
	//-------------------------------------------------------------
	//Summary:
	//		check capacity whether exceed
	//-------------------------------------------------------------
	bool CapacityAvailable(PaxLandsideBehavior *pBehavior)const;

	//is the station the service start station?
	bool IsServiceStart(LandsideBusStationInSim* pStation);
	void PaxTakeOffTheVehicle(LandsideBusStationInSim* pBus ,ElapsedTime& eEventTime , CARCportEngine* pEngine);
	void GeneratePaxGetOffDoneTag(const ElapsedTime& eEventTime);
	ResidentRelatedVehicleTypePlan* GetCurrentPlan()const{ return m_pCurPlan; }
	void SetCurrentPlan(ResidentRelatedVehicleTypePlan*pPlan){ m_pCurPlan = pPlan; }

	bool IsAllPassengersReady();
	virtual void SuccessBirthInLotSpot(LandsideParkingSpotInSim* plot, CARCportEngine* pEngine);
	virtual void SuccessParkInLotSpot(LandsideParkingSpotInSim* plot);
	ElapsedTime GetTurnAroundTime()const;

	eServiceState GetServiceState()const{ return m_serviceState; }
	void SetServiceState(eServiceState s){ m_serviceState = s;  }

	bool InitBirth(CARCportEngine *_pEngine);
protected:
	//get the item object
	void RetrieveStoppedBusStation( LandsideSimulation* pSim);

	eServiceState m_serviceState;
	bool m_bAvaible;

	LandsideLayoutObjectInSim* _getLayoutObject(LandsideFacilityObject& obj, CARCportEngine* pEngine);


protected:
	RensidentVehicleDeployorInSim* m_pDeployer;
	ResidentRelatedVehicleTypePlan* m_pCurPlan; //service plan

	std::vector<LandsideBusStationInSim* > m_vBusStationCouldLeadTo;
	bool m_bPaxGetoffDone;//passenger get on vehicle need check first
};

//////////////////////////////////////////////////////////////////////////
class State_ProcessAtBusStationResidentVehicle : public State_LandsideVehicle<LandsideResidentVehicleInSim>
{
public:
	State_ProcessAtBusStationResidentVehicle(LandsideResidentVehicleInSim* pV,LandsideBusStationInSim* pStation,LaneParkingSpot* spot);

	virtual const char* getDesc()const{ return _T("Parking in Bus Station"); }
	virtual void Entry(CARCportEngine* pEngine);
	virtual void Execute(CARCportEngine* pEngine);	
protected:
	LandsideBusStationInSim *m_pBusStation;
	LaneParkingSpot* m_spot;

	State_ServiceTimer m_minServiceTimer;  //min service timer
	State_ServiceTimer m_headwayTimer;//timer keep headway,
	State_ServiceTimer m_allPaxGetOffTimer;

	bool m_bIsServieStartStation;
	//return true if it end of service loop
	bool CheckEndOfServiceLoop(CARCportEngine* pEngine);

};

//////////////////////////////////////////////////////////////////////////
class State_ParkingInHomeBaseResidentVehicle :public State_LandsideVehicle<LandsideResidentVehicleInSim>
{
public:
	State_ParkingInHomeBaseResidentVehicle(LandsideResidentVehicleInSim* pV,LandsideParkingSpotInSim * pspot,bool bBirth = false)
	:State_LandsideVehicle<LandsideResidentVehicleInSim>(pV){ m_spot = pspot; m_birthInLot = bBirth;  }
protected:
	virtual const char* getDesc()const{ return _T("Parking in Home Base"); }
	virtual void Entry(CARCportEngine* pEngine);
	virtual void Execute(CARCportEngine* pEngine);
protected:
	LandsideParkingSpotInSim * m_spot;
	State_ServiceTimer m_turnAroundTimer;
	bool m_birthInLot;
};