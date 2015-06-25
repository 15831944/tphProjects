#pragma once

#include "../EngineDll.h"
//#include "common\elaptime.h"
#include "Inputs\flight.h"
#include "../../Results/AirsideFlightLogEntry.h"
#include "./CommonInSim.h"
//#include "AirsideResource.h"
//#include "../../Common/Point2008.h"
#include "AirsideMobElementWriteLogItem.h"
//#include "AirsideFlightState.h"
#include "../../Common/Pollygon2008.h"
#include "TempParkingInSim.h"
#include ".\FlightDeiceStrategyDecision.h"
#include "AirsideTowTruckInSim.h"
#include "../ARCportEngine.h"
#include "FlightPerformancesInSim.h"

class AirTrafficController;
class Clearance;
class ClearanceItem;

class CRoutePriority;
class StandInSim;
class LogicRunwayInSim;
class RunwayExitInSim;
class TaxiRouteInSim;
class OutputAirside;
class FlightPerformanceManager;
class FlightRouteInSim;
class VehicleRequestDispatcher;
class CFlightServiceInformation;
class LaneFltGroundRouteIntersectionInSim;
class TempParkingSegmentsInSim;
class CAirportDatabase;
class CArrivalETAOffsetList;
class AirsideSimConfig;
class AirsidePaxBusParkSpotInSim;
class AirsideVehicleInSim;
class VehicleServiceRequest;
class AirsideFlightDelay;
class AirsideConflictionDelayLog;
class DeiceVehicleServiceRequest;
class AirsideFollowMeCarInSim;
class TaxiwayDirectSegInSim;

class HoldPositionInSim;
class AirsideFlightLogItem;
class CTowOffStandAssignmentData;
class ARCEventLog;
class OnboardFlightInSim;
class FlightPerformancesInSim;
class AirsidePassengerBusContext;
class AirsidePassengerBusStrategy;
class CBagCartsParkingSpotInSim;
class AirsideFlightBaggageManager;

enum FLIGHT_PARKINGOP_TYPE
{
	ARR_PARKING = 0,
	INT_PARKING,
	DEP_PARKING,
};

enum FLIGHTTOWCRITERIATYPE
{
	NOTTOW =0,
	REPOSITIONFORDEP,
	FREEUPSTAND
};
enum FlightOperation
{
	ARRIVAL_OPERATION = 0,
	DEPARTURE_OPERATION

};
class ENGINE_TRANSFER FlightWakeUpCaller : public CAirsideObserver
{
public:
	void SetFlight(AirsideFlightInSim* pFlight){ m_pFlight = pFlight; }
	virtual void Update(const ElapsedTime& tTime);
protected:
	AirsideFlightInSim* m_pFlight;
};


class COpenDoorInfo
{
public:
	CPoint2008 mDoorPos; //door position
	CPoint2008 mGroundPos; //stair ground pos at dir 
	ElapsedTime mOpenTime; //open time
};


//Flight in Simulation	
class ENGINE_TRANSFER AirsideFlightInSim: public CAirsideMobileElement
{
protected:
	//the flight is take off
	//preflight is take off
	// the time that the flight's start preparing to take off, set while apply for take off time
	//according to the clearance criteria middle part
	//the take off flight comes into runway after the clear time, so, the flight maybe not arrive at runway and then prepare takeoff
	//it maybe wait a few seconds for getting proper separation with pre-flight in the air
	ElapsedTime m_eTOMTimeInRunway;

	//take off flight
	// the time that flight start moving to take off after prepare time
	ElapsedTime m_eTOStartPrepareTime;

	//the time of previous flight's landing or take off exit runway time
	ElapsedTime m_ePreFlightExitRunwayTime;


	DynamicClassStatic(AirsideFlightInSim)
	RegisterInherit(AirsideFlightInSim,CAirsideMobileElement)
	
	friend class AirTrafficController;
	friend class Push_TowOperationSpecInSim;

public:
	const static DistanceUnit defaultLength;
	const static DistanceUnit defaultWingSpan;
	const static DistanceUnit defaultHeight;
	const static DistanceUnit defaultWeight;
	const static DistanceUnit defaultCabinWidth;

	//other flight's max wingspan 
	DistanceUnit mOtherFlightMaxWingSpan;
	void SetOtherMaxWingSpan(double d){ mOtherFlightMaxWingSpan = d; }

public:
	explicit AirsideFlightInSim(CARCportEngine *pARCPortEngine, Flight* pFlt,int id);
	virtual ~AirsideFlightInSim(void);

	virtual void OnNotify( SimAgent* pAgent, SimMessage& msg ); 	//notify by other agents
	virtual bool IsActive()const;	


	virtual void InitLogEntry();
	void InitFlightInfo(CAirportDatabase *pAirportDatabase);
	void InitBirth(const CArrivalETAOffsetList& etaoffsets,const AirsideSimConfig& simconf);
	void InitStatus();
	void InitScheduleEndTime(const ElapsedTime& tEndTime);

	virtual RouteDirPath* getCurDirPath()const;
	TaxiwayDirectSegInSim* GetCurTaxiwayDirectSeg() const;

	//add logEntry to output
	void StartLogging();

	//AirTraffic Controller;
	AirTrafficController* GetAirTrafficController(){ return m_pATC; }
	void SetAirTrafficController(AirTrafficController * pATC){ m_pATC = pATC; }

	//ask ATC for next clearance
	virtual void WakeUp(const ElapsedTime& tTime);

	//runway wave up
	void RunwayWakeUp(const ElapsedTime& eTime);

	//Flight Input
	Flight* GetFlightInput()const{ return m_pflightInput; }

	//	
	ElapsedTime GetTime()const{ return m_curState.m_tTime; }
	CPoint2008 GetPosition()const{ return m_curState.m_pPosition; }
	double GetSpeed()const{ return  m_curState.m_vSpeed; }
	AirsideResource * GetResource()const { return m_curState.m_pResource; }
	AirsideMobileElementMode GetMode()const{ return m_curState.m_fltMode; }
	DistanceUnit GetDistInResource()const{ return m_curState.m_dist; }

	void SetTime(const ElapsedTime& tTime){ m_curState.m_tTime = tTime; }
	void SetPosition(const CPoint2008& pos){ m_curState.m_pPosition = pos; }
	void SetSpeed(double speed);
// 	void SetResource(AirsideResource * pRes ){ m_curState.m_pResource = pRes; }
	void SetDistInResource(DistanceUnit dist){ m_curState.m_dist = dist; }
	void SetMode(AirsideMobileElementMode mode){ m_curState.m_fltMode = mode; }

	//in stand, the cargo door position
	BOOL getCargoDoorPosition(CPoint2008 &ptCargoDoor);

	//Get 
	FlightRouteInSim * GetArrFlightPlan();
	FlightRouteInSim * GetDepFlightPlan();
	FlightRouteInSim * GetSTAR();
	FlightRouteInSim * GetSID();
	FlightRouteInSim* GetLandingCircuit();
	FlightRouteInSim* GetTakeoffCircuit();
	LogicRunwayInSim * GetLandingRunway();
	LogicRunwayInSim * GetAndAssignTakeoffRunway();
	

	TaxiRouteInSim* GetRouteToTempParking();
	TaxiRouteInSim* GetRouteToStand();
	TaxiRouteInSim* GetCircuitTaxiwayRoute();
	TaxiRouteInSim* GetAndAssignOutBoundRoute();
	TaxiRouteInSim* GetAndAssignCircuitOutBoundRoute();
	TaxiRouteInSim* GetAndAssignCircuitInBoundRoute();
	TaxiRouteInSim* GetOutBoundRoute()const{ return m_pOutBoundRoute; }

	TaxiRouteInSim* GetTowingRoute();
	TaxiRouteInSim* GetRouteToMeetingPoint();
	TaxiRouteInSim* GetRouteToAbandonPoint();

	StandInSim * GetOperationParkingStand();
	StandInSim * GetPlanedParkingStand(FLIGHT_PARKINGOP_TYPE type);
	RunwayExitInSim * GetRunwayExit();
	RunwayExitInSim * GetAndAssignTakeoffPosition();
	RunwayExitInSim * GetTakeoffPosition()const{ return m_pTakeoffPos; }
	TempParkingInSim* GetTemporaryParking(){ return m_pTemporaryParking.get();}
	CFlightServiceInformation *GetFlightServiceInformation(){ return m_pFlightServiceInfomation;}
	StandInSim* GetIntermediateParking();

	ElapsedTime GetBirthTime()const;
	ElapsedTime GetArrTime()const{ return m_arrTime; }
	ElapsedTime GetDepTime()const{ return m_depTime; }
	ElapsedTime GetEstimateStandTime();

	//Set
	void SetArrFlightPlan(FlightRouteInSim*pRoute);
	void SetDepFlightPlan(FlightRouteInSim*pRoute);
	void SetLandingRunway(LogicRunwayInSim* pRunway){ m_pLandingRunway = pRunway; }
	void SetTakeoffRunway(LogicRunwayInSim *pRunway){ m_pDepatrueRunway = pRunway; }
	void SetInBoundRoute(TaxiRouteInSim* pRoute);
	void SetOutBoundRoute(TaxiRouteInSim* pRoute);
	void SetParkingStand(StandInSim * pStand,FLIGHT_PARKINGOP_TYPE type);
	void SetTempParking(TempParkingInSim* pTempParking);
	void SetRunwayExit(RunwayExitInSim* pExit);
	void SetTakeoffPosition(RunwayExitInSim * pExit);
	void SetSTAR(FlightRouteInSim * pRoute);
	void SetSID(FlightRouteInSim * pRoute);
	void SetCircuit(FlightRouteInSim * pRoute);

	void SetArrTime(const ElapsedTime t){ m_arrTime = t; }
	void SetDepTime(const ElapsedTime t){ m_depTime = t; GetFlightInput()->NotifyCheckRealDepatureTime();GetFlightInput()->SetRealDepTime(t) ;}
	//
	void SetPerformance(FlightPerformanceManager* pPerform);
	FlightPerformancesInSim * GetPerformance()const;
	double GetRandomValue()const {return m_dRandom;}
	double GetMinSpeed(AirsideMobileElementMode mode);
	double GetMaxSpeed(AirsideMobileElementMode mode);
	double GetTakeoffPositionTime();
	double GetLiftoffSpeed();
	double GetTakeoffRoll();
	double GetPushbackSpeed();
	ElapsedTime GetUnhookTime();
	double GetTowingSpeed();
	double GetExitSpeed();

	//calculate time between two clearance item
	ElapsedTime GetTimeBetween(const ClearanceItem& item1, const ClearanceItem& item2)const;

	void FlushLog(const ElapsedTime& endTime);


	virtual	bool IsItinerantFlight(){ return false;}
	virtual bool IsCircuitFlight(){return false;}

	DistanceUnit GetIntersectionBuffer()const;

public:  //properties
	BOOL IsTransfer()const{ return m_pflightInput->isTurnaround(); }
	BOOL IsArrival()const{ return m_pflightInput->isArriving(); }
	BOOL IsDeparture()const{ return m_pflightInput->isDeparting();  }
	virtual BOOL IsThroughOut()const;
    ElapsedTime getServiceTime (FLIGHT_PARKINGOP_TYPE type) const ;
    virtual void getACType (char *p_str) const { strcpy (p_str, m_pflightInput->getLogEntry().acType); }
	int getArrGateIndex (void) const { return m_pflightInput->getLogEntry().nArrGate; }
	int getDepGateIndex (void) const { return m_pflightInput->getLogEntry().nDepGate; }
	void getFullID (char *p_str, char p_mode = 0) const{ m_pflightInput->getFullID(p_str,p_mode);}
	const ALTObjectID getArrStand(void)const { return m_pflightInput->getArrStand(); }
	const ALTObjectID getDepStand(void)const { return m_pflightInput->getDepStand(); }
	const ALTObjectID getStand()const;

	bool IsArrivingOperation();
	bool IsDepartingOperation();

	void GenerateArrivalCloseDoorEvent();
	void GenerateDepartureCloseDoorEvent();

	OnboardFlightInSim* GetOnboardFlight();
public: // statues
	const AirsideFlightState& GetPreState()const{ return m_preState; }
	AirsideFlightState& GetCurState(){ return m_curState; }
	const AirsideFlightState& GetCurState()const{ return m_curState; }

	virtual ElapsedTime getArrTime (void) const { return m_pflightInput->getArrTime(); }
	virtual ElapsedTime getDepTime (void) const { return m_pflightInput->getDepTime(); }
	ElapsedTime getPlanParkingTimeInAirport() const;

	//update flight state and write log
	virtual void UpdateState(const AirsideFlightState& fltState, bool bInNode );
//
public:
	virtual int fits(const FlightConstraint& _fltcons) const;
	AirsideFlightInSim& ChangeToArrival();
	AirsideFlightInSim& ChangeToDeparture(); 
	CString GetCurrentFlightID()const;

	//------------------------------------------------------------
	// 
	// access m_pPreferredRoutePriority setting
	CRoutePriority* GetPreferredRoutePriority() const { return m_pPreferredRoutePriority; }
	void SetPreferredRoutePriority(CRoutePriority* val) { m_pPreferredRoutePriority = val; }
	// check whether the passed in flight is on the same preferred route
	bool IsOnSamePreferredRoute(const AirsideFlightInSim* pOtherFlight, CRoutePriority* pExtRoutePriority = NULL) const;
	// pExtRoutePriority : extern CRoutePriority, if not NULL, use this to decide, else use m_pPreferredRoutePriority
	std::vector<AirsideFlightInSim* > FindFlightsOnDiffPreferredRoute( const std::vector<AirsideFlightInSim* >& vecFlights, CRoutePriority* pExtRoutePriority = NULL ) const;
	//------------------------------------------------------------

public:
	void SetOutput(OutputAirside * pOut){ m_pOutput = pOut; }
	void PerformClearance(const Clearance& clearance);
	virtual void PerformClearanceItem(const ClearanceItem& clearanceItem);

	void SetServiceRequestDispatcher(VehicleRequestDispatcher* pDispatcher){ m_pServiceRequestDispatcher = pDispatcher;}
	VehicleRequestDispatcher* GetServiceRequestDispatcher()const{ return m_pServiceRequestDispatcher; }

	CFlightServiceInformation* GetServicingInformation() { return m_pFlightServiceInfomation;}
	void SetFlightServiceInformation( CFlightServiceInformation* pInfo) { m_pFlightServiceInfomation = pInfo;}


	AirsidePaxBusParkSpotInSim * GetPaxBusParking(bool bArrival);

	void SetArrivalPaxBusParking(AirsidePaxBusParkSpotInSim * pPaxParking){ m_pArrivalPaxParkingInSim = pPaxParking;}
	void SetDepPaxBusParking(AirsidePaxBusParkSpotInSim * pPaxParking){ m_pDepPaxParkingInSim = pPaxParking;}




	bool IsGoingToIntersectLane(LaneFltGroundRouteIntersectionInSim& );
	TaxiRouteInSim * GetCurrentTaxiRoute();

	void AllVehicleFinishedServicing(const ElapsedTime& dT);

	//if the flight in the air, calculate the estimate time before landing
	bool GetEstimateLandingTime(ElapsedTime& estimateTime);

	virtual TYPE GetType() { return AIRSIDE_FLIGHT; }

	double GetWingspan()const;
	double GetWeight()const;
	virtual double GetLength()const;
	double GetCabinWidth()const;

	virtual double getCurSpeed()const{ return m_curState.m_vSpeed; }	
	virtual ElapsedTime getCurTime()const{ return m_curState.m_tTime; };
	virtual DistanceUnit getCurSeparation()const;//the separation other mobile need to keep;
	virtual DistanceUnit getCurDistInDirPath()const{ return GetDistInResource(); } 

	CPoint2008 GetPosition(const ElapsedTime t)const;

	//void IncreaseServicePoint(int nCount = 1);
	//void DecreaseServicePoint(int nCount = 1);
	bool IsAllVehicleServiced();

	int GetUID()const{ return m_id; }

	AirsideFlightLogEntry& GetLogEntry(){ return m_LogEntry; } 
	const AirsideFlightLogEntry& GetLogEntry()const{ return m_LogEntry; }

	CPoint2008 GetEntrySystemPos();
	

	bool IsGeneralVehicleAndPaxbusServiceCompleted();	//only general vehicle and pax bus service finished
	bool IsAllVehicleServiceExceptTowingCompleted(); //general vehicle,pax bus and deice service finished

	bool IsWaitingForTowTruck();
	bool IsTowingServiceCompleted(); //for towing service
	void SetTowingServiceCompleted(); // to mark the towing service completed
	void SetTowingServiceVehicle(AirsideTowTruckInSim* pTowTruck); // tell flight which tow truck service it
	AirsideTowTruckInSim* GetTowingServiceVehicle(){ return m_pTowingServiceVehicle; }

	void AddVehicleServiceRequest(VehicleServiceRequest *);
	void VehicleServiceComplete(AirsideVehicleInSim *pVehicle);
	//the request the vehicle is at 
	VehicleServiceRequest* getServiceRequest(AirsideVehicleInSim* pVehicle);
	VehicleServiceRequest* getGeneralServiceRequest(int nVelTypeID);
	DeiceVehicleServiceRequest* getDeiceServiceRequest();
	void RemoveServiceReques(VehicleServiceRequest* pRequest);

	DistanceUnit GetTouchDownDistAtRuwnay(LogicRunwayInSim* pRunway);
	void WakeupAllPaxBusCompleteService();

	CBagCartsParkingSpotInSim * getArrivalBagCartsParkingSpot() const;
	void setArrivalBagCartsParkingSpot(CBagCartsParkingSpotInSim * pSpotInSim);
	CBagCartsParkingSpotInSim * getDeparturelBagCartsParkingSpot() const;
	void setDeparturelBagCartsParkingSpot(CBagCartsParkingSpotInSim * pSpotInSim);

public:
	void WritePureLog(AirsideMobElementWriteLogItem* logItem);
	virtual void WriteLog( bool bEnterRes = false);

	// get flight speed according to mode
	// NOTE: different to GetSpeed, this method does not return current speed of the flight,
	// it just returns the default speed of the flight
	double GetSpeedByMode(AirsideMobileElementMode mobileMode) const;
	// return whether the speed of current flight is constrained by other flight
	// ignore the case of constrained by TaxiSpeedConstriants
	bool IsSpeedConstrainedByOtherFlight();

	// get the taxiway constrained speed on current taxiway segment
	// according to TaxiwaySpeedConstraintsInSim
	// dSpeed is the result
	// return true is succeed, else false
	bool GetTaxiwayConstrainedSpeed(double& dSpeed) const;

	void WritePathLog( const  MobileGroundPerform& groundPerform, const CPath2008& path,AirsideFlightState startState,const ClearanceItem& item);
	void  WriteMissApproachOnRunwayPathLog(const CPath2008& path,AirsideFlightState startState,const ElapsedTime&  endTime, const double& endSpd);
	FlightWakeUpCaller& GetWakeUpCaller(){ return m_wakeupCaller; }

	//set current delay to Flight if different than the old delay , it will write delay to log
	void SetDelayed(AirsideFlightDelay* pDelay);
	OutputAirside* GetOutput(){ return m_pOutput;}

	CString getArrGateInSim (void) ;

	CString getDepGateInSim (void);

	void setArrGateInSim (const CString& strID);
	void setDepGateInSim (const CString& strID);
	bool WaitingInHold(ElapsedTime tHoldTime = -1L);

	//get the flight polygon position
	CPollygon2008 GetFlightPosition(const ElapsedTime& eTime);

	//get the time of flight arrive at the resource
	ElapsedTime GetFlightEstimateTime(AirsideResource *pResource,DistanceUnit dDistance);

	//release all resource lock and exit running system
	void Terminate(const ElapsedTime& t);

	//cross runway queue
	void EnterQueueToRunwayHold(HoldPositionInSim* pQueue, bool bMoveToHead);
	void QuitCrossRunwayQueue();
	HoldPositionInSim* m_pCrossRunwayHold; //not null, flight in the cross runway queue
	HoldPositionInSim* GetWaitRunwayHold()const{ return m_pCrossRunwayHold; }
	bool IsInCrossRunwayQueue()const{ return  GetWaitRunwayHold()!=NULL; }


	//deice 
	inline const FlightDeiceStrategyDecision& GetDeiceDecision()const{ return m_deiceDecision; }
	inline FlightDeiceStrategyDecision& GetDeiceDecision(){ return m_deiceDecision; }
	//inline const FlightDeiceRequirment& GetDeiceRequirment()const{ return m_deiceRequirment; }
	//inline FlightDeiceRequirment& GetDeiceRequirment(){ return m_deiceRequirment; }
	void SetReadyForDeice(bool b){ mbReadyForDeice = b; }
	bool bReadyForDeice()const { return mbReadyForDeice; }
	bool mbReadyForDeice;

	void SetParkingAtDeice(bool parking){mbDeicParking = parking;}
	bool mbDeicParking;

	void ApplyForDeiceSerive(const ElapsedTime& tTime);  //ask for deice operations
	void ApplyForTowTruckService(StandInSim* pCurStand,const ElapsedTime& tTime);
	void SetTowingRoute(TaxiRouteInSim* pTowingRoute);
	void SetBeginTowingService();

	void SetBeginFollowMeCarService();
	void SetFollowMeCarServiceCompleted();

	void LogRunwayOperationLog(bool bEntryOrExit, ClearanceItem& clearanceItem);

protected:

	void SaveState(){ m_preState = m_curState; }
	
	//generate next event, if have write log event, generate write log event
	//else ,create a find clearance event
	void GenerateNextEvent();
	
	//create a write log event, if have no write log event,generate a new find clearance event
	void GenerateWriteLogEvent();

protected:
	void ApplyForVehicleService();

	void ApplyForPaxBusService(bool bArrival);
	void ApplyForFollowMeCarService(StandInSim* pDest);
	void ApplyForBaggageTrainService(FlightOperation enumOperation);

public:
	//the 3 parameters are  temporary used for take off
	ElapsedTime getTOStartPrepareTime() const { return m_eTOStartPrepareTime; }
	void setTOStartPrepareTime(ElapsedTime val) { m_eTOStartPrepareTime = val; }
	ElapsedTime getRealOpTimeOnRunway() const { return m_eTOMTimeInRunway; }
	void setRealOpTimeOnRunway(ElapsedTime val) { m_eTOMTimeInRunway = val; }
	ElapsedTime getPreFlightExitRunwayTime() const { return m_ePreFlightExitRunwayTime; }
	void setPreFlightExitRunwayTime(ElapsedTime val) { m_ePreFlightExitRunwayTime = val; }
	ElapsedTime getEngineStartTime();


	void getDesc(ARCMobileDesc& desc)const;
	//delay return ture if start a new delay
	bool StartDelay(ClearanceItem& startItem, ARCMobileElement* pConflictMobile,FlightConflict::ActionType action ,FlightConflict::LocationType conflictLocation,FltDelayReason emFltDelayReason,const CString& sDelayReason);  //indicate the delay started
	void EndDelay(ClearanceItem& endItem); //indicate the delay end;
	void LogEventItem(ARCEventLog* pLog); //add the delay log done list
	const AirsideConflictionDelayLog* GetCurDelayLog() const;	//get current delay log
	bool GetStoppedStartTime(ElapsedTime& stoppedTime) const; // start time of current STOP, if not STOP, return false

	void SetTowOffData(CTowOffStandAssignmentData* pData);
	CTowOffStandAssignmentData* GetTowOffData();
	FLIGHTTOWCRITERIATYPE GetTowOffStandType();

	//tag for flight whether wait for passenger to take on flight
	void SetWaitPassengerTag(bool bWait);
	bool GetWaitPassengerTag()const;

	void AddPaxCount(int nCount);
	void AddPaxInFlight();
	int GetPaxCount();
	void DecreasePaxNumber(const ElapsedTime& tTime);

	void WriteStairsLog(const ElapsedTime& tTime);
	bool GetOpenDoorAndStairGroundPostions(std::vector< std::pair<CPoint2008, CPoint2008> >& vPoints );
	void SetShowStair(const ElapsedTime&  tTime);
	void OpenDoors(const ElapsedTime&  tTime);

	AirsideFollowMeCarInSim* GetServiceFollowMeCar();

	void ConnectBridge(const ElapsedTime&  tTime);
	
	std::vector<COpenDoorInfo> m_vOpenDoors; //opened doors	
	BOOL HasBrrdgeServer(BOOL _arrival);

	CPath2008 GetFilletPathFromTaxiwayToRwyport(RUNWAY_MARK  port);

	StandInSim * GetArrParkingStand() const { return m_pArrParkingStand; }
	StandInSim * GetDepParkingStand() const { return m_pDepParkingStand; }

	//when turnaround flight change state from arrival to departure, need reset vehicle service
	void ResetVehicleService(){m_bApplyFroServiceRequest = false;}

	CARCportEngine *GetEngine(){return m_pARCPortEngine;}


	BOOL hasPaxBusService(char mode)const;
	bool DeparturePaxBusService();
	bool ArrivalPaxBusService();

	bool NeedGenerateBoardingCall(const ElapsedTime& eTime);
	void GenerateBoardingCall();

	AirsideFlightBaggageManager *getBaggageManager();

protected:
	std::vector<VehicleServiceRequest *> m_vServiceRequest;

//	CPaxBusParkingInSim * m_pArrivalPaxParkingInSim;
//	CPaxBusParkingInSim * m_pDepPaxParkingInSim;
	AirsidePaxBusParkSpotInSim* m_pArrivalPaxParkingInSim;
	AirsidePaxBusParkSpotInSim * m_pDepPaxParkingInSim;

	int m_id;
	ElapsedTime m_tScheduleEndTime;

	//flight state
	AirsideFlightState m_preState;
	AirsideFlightState m_curState;

	//ElapsedTime m_tDelay;     //accurate delay time;
	//FltDelayReason m_nDelayRe;  //delay reason

	//
	
	//Assignment
	AirTrafficController *m_pATC;        // ATC

	FlightRouteInSim * m_pArrivalPlan;
	FlightRouteInSim * m_pSTAR;
	LogicRunwayInSim * m_pLandingRunway;
	RunwayExitInSim * m_pRunwayExit;
	StandInSim * m_pArrParkingStand;
	StandInSim * m_pDepParkingStand;
	StandInSim* m_pIntermediateParking;
	TaxiRouteInSim * m_pOutBoundRoute;
	TaxiRouteInSim* m_pCircuitBoundRoute;
	RunwayExitInSim * m_pTakeoffPos;
	LogicRunwayInSim * m_pDepatrueRunway;
	FlightRouteInSim * m_pSID;
	FlightRouteInSim * m_pDeparturePlan;
	//circuit flight route for take off and landing
	FlightRouteInSim* m_pLandingCircuitRoute;
	FlightRouteInSim* m_pTakeoffCircutRoute;
	//////////////////////////////////////////////////
	TempParkingInSim::RefPtr m_pTemporaryParking;

	TaxiRouteInSim* m_pRouteToStand;
	TaxiRouteInSim* m_pRouteToTempParking;
	TaxiRouteInSim* m_pRouteInQueue;

	TaxiRouteInSim* m_pTowingRoute;
	TaxiRouteInSim* m_pRouteToMeetingPoint;
	TaxiRouteInSim* m_pRouteToAbandonPoint;

	AirsideTowTruckInSim* m_pTowingServiceVehicle;
	AirsideFollowMeCarInSim* m_pServiceFollowMeCar;

	ElapsedTime m_arrTime;
	ElapsedTime m_depTime;
	ElapsedTime m_tToStandDelayTime;
	bool m_bTowTruckServiced;

	//
	FlightPerformancesInSim* m_pFlightPerformanceManager;

	VehicleRequestDispatcher* m_pServiceRequestDispatcher;

	//input
	Flight* m_pflightInput;

	//output
	AirsideFlightLogEntry m_LogEntry;
	AirsideFlightLogItem* m_pReportLogItem;
	OutputAirside * m_pOutput;	
	void FlushLogToFile();
	//flight service info
	ElapsedTime ignorLogTime;
	int ignorLogCount;

	bool m_bApplyFroServiceRequest;
	bool m_bApplyFrPaxBusServiceRequestArrival;
	bool m_bApplyFrPaxBusServiceRequestDeparture;

	bool m_bApplyTrainServiceRequestArrival;
	bool m_bApplyTrainServiceRequestDeparture;

	bool m_bTowingToReleasePoint;
	bool m_bFollowMeCarServicing;
	CFlightServiceInformation *m_pFlightServiceInfomation;
	
	//flight properties

	//int m_nServicePointCount;
	
	FlightWakeUpCaller m_wakeupCaller;

	CHAR m_curFlightType; //is arrival or departure "A " for arrival ,'D' for departure
	
	//current delay of flight
	AirsideFlightDelay* m_pCurrentDelay; //the old delay system need to delete
	AirsideConflictionDelayLog* mpCurrentDelayLog;

	CString m_strActualArrGate;
	CString m_strActualDepGate;
	///////flight performance///////////////
	//double m_tTakeoffPositionTime;
	//double m_dLiftOffSpeed;
	//double m_dTakeOffRoll;
	//double m_dPushbackSpd;
	//ElapsedTime m_tUnhookTime;
	//double m_dTowingSpeed;

	//FlightDeiceRequirment m_deiceRequirment;
	FlightDeiceStrategyDecision m_deiceDecision;
	CTowOffStandAssignmentData* m_pTowoffData;

	//log items
	AirsideMobElementWriteLogItemList m_lstWirteLog;
	double m_dRandom;	
	int m_nPaxCountInFlight;
	bool m_bWaitPaxTakeOn;
	bool m_bStairShowState;
//	ElapsedTime m_tEngineStartTime;

	// preferred route priority
	CRoutePriority* m_pPreferredRoutePriority;

	CARCportEngine *m_pARCPortEngine;

	//intermediate stand
	bool m_bTowingToIntStand;//check flight whether has intermediate operation
	bool m_bTowingToDepStand;//check flight whether need towing to dep stand

	bool m_bMoveToIntStand;//check flight move from arr sand to int stand
	bool m_bMoveToDepStand;//check flight move from int stand to dep stand

	AirsidePassengerBusContext* m_pPaxBusContext;
	AirsidePassengerBusStrategy* m_pArrPaxBusStrategy;
	AirsidePassengerBusStrategy* m_pDepPaxBusStrategy;


	CBagCartsParkingSpotInSim *m_pArrivalBagCartsParkingSpot;
	CBagCartsParkingSpotInSim *m_pDeparturelBagCartsParkingSpot;
	AirsideFlightBaggageManager *m_pBaggageManager;


public:
	bool NeedMoveToIntStand()const;
	bool NeedTowingToDepStand()const;
	void ApplyTowingToIntStand();
	void ApplyTowingToDepStand();

	void TowingToIntStand();
	void TowingToDepStand();
	bool HasTowingToIntStand()const;
	bool HasTowingToDepStand()const;

	//-----------------------------------------------------------------------------
	//Summary:
	//		Passenger bus work methods
	//-----------------------------------------------------------------------------
	void PassengerBusArrive(bool bArrival,const ElapsedTime& time,CAirsidePaxBusInSim* pPaxBus);
	void PassengerBusLeave(bool bArrival,CAirsidePaxBusInSim* pPaxBus);
	void FlightArriveStand(bool bArrival,const ElapsedTime& time);
	void FlightLeaveStand(bool bArrival);
	CAirsidePaxBusInSim* GetAvailablePassengerBus(Person* pPerson,bool bArrival);
private:
	void InitPaxBusContext(bool bArrival);
};



