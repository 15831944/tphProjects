#pragma once
#include "MobElementBehavior.h"
#include "Common/point.h"
#include "person.h"
/**
*	Class to provide Mobile Element in Terminal Mode's Movement logic.
	<TODO::>  Reconstruction class 'Person'. move associated code from 'Person' to this class.
*/

class Person;
class CARCportEngine;
class Terminal;
class Processor;
class MobLogEntry;
class ProcessorArray;
class CMobileElemConstraint;
class CPersonOnStation;
class CPaxDestDiagnoseInfo;
class CFLowChannelList;
class CAirsidePaxBusInSim;


class ENGINE_TRANSFER TerminalMobElementBehavior : public MobElementBehavior
{
	friend class Person;
	friend class Passenger;
	friend class PaxVisitor;
	friend class TurnaroundPassenger;
public:
	TerminalMobElementBehavior(Person* _pPerson);
	virtual ~TerminalMobElementBehavior(void);

	enum TRANSFER_STATE { TRANSFER_ARRIVAL ,TRANSFER_DEPARTURE };

	struct PipePointInformation 
	{
		Point pt;
		int m_nPrePipe;
		int m_nCurPipe;
	};

public:
	virtual int performanceMove(ElapsedTime p_time,bool bNoLog);
	BehaviorType getBehaviorType()const{ return MobElementBehavior::TerminalBehavior; }

	const ProcessorID * GetEntryProcID()const;
	bool  GetAvailbleEntryProcList(std::vector<Processor *>& vAvailableProc, int nFloor )const;
	bool  GetAvailbleEntryProcList(std::vector<Processor *>& vAvailableProc)const;

	Processor *TryToSelectLandsideEntryProcessor(const ElapsedTime& _timeToSelect);

	//int processTerminalModeMovement(ElapsedTime p_time,bool bNoLog);

	//process fire evacuation logic
	// that is , select shortest path to let person evacuate from current position	
	virtual void ProcessFireEvacuation( const ElapsedTime& _timeToEvacuation );


	void updateCheckedBagStateOnReclaim(const ElapsedTime& _curTime);

	virtual void processPersonOnStation(ElapsedTime p_time, bool& _executed, bool& _localResult);

	//It is state handling methods.
	virtual int processBirth (ElapsedTime p_time);

	virtual void processEntryAirside(ElapsedTime p_time);
	virtual void processEntryOnboard(ElapsedTime p_time);
	virtual void processEntryLandside(ElapsedTime p_time);

	virtual void processGeneralMovement (ElapsedTime p_time);
	void processGeneralMovementHelp (ElapsedTime p_time);

	virtual void processInConstraint (ElapsedTime p_time);

	virtual void processQueueMovement (ElapsedTime p_time);

	virtual void processQueueDeparture (ElapsedTime p_time);

	virtual void processServerArrival (ElapsedTime p_time);

	virtual int processServerDeparture (ElapsedTime p_time);

	virtual void processOutConstraint (ElapsedTime p_time);

	virtual void processGreeting(ElapsedTime p_time);

	void processBridge( ElapsedTime p_time );

	virtual void processMoveToInterestedEntryPoint (ElapsedTime p_time);

	int getNextProcessor (ElapsedTime& p_time);
	Processor *selectProcessor ( ElapsedTime _curTime, bool _bStationOnly = false, bool _bExcludeFlowBeltProc = false, bool bTryExcludeElevatorProc = false);
	Processor *selectBestProcessor (const ProcessorArray *p_procList);
	Processor *selectBestHoldArea (const ProcessorArray *p_procList);

	Processor* GetDirectGoToNextProc()const { return m_pDirectGoToNextProc;}
	void SetDirectGoToNextProc( Processor* _pProc ){ m_pDirectGoToNextProc = _pProc;}
	Processor* TryToSelectNextProcessor( const ElapsedTime& _timeToSelect ,bool _bStationOnly = false,  bool _bExcludeFlowBelt = false );

	int GetStopReason()const { return m_iStopReason;};
	void SetStopReason( int _iReason ) { m_iStopReason = _iReason;	};

	// before the person begin to avoid density, do something
	void doSomethingBeforeAvoid( void );

	// after avoid density, do something
	void doSomethingAfterAvoid( const ElapsedTime& _time );

	// check the area if is full
	bool checkAreaIsFull( const CArea* _pArea ) const;

	//	bool FindGreetingProcFromDestination( std::vector<ProcessorID>& _vGreetingPlace  , ProcessorArray &canSelectProc,long _lAttachedID ,bool _bStationOnly,ElapsedTime _curTime );
	// yes for dep, no for others.
	virtual bool NeedCheckLeadToGate() const;
	virtual bool NeddCheckLeadToReclaim()const;
	
	void ProcessError(ElapsedTime _curTime, const CString& szMobType, const CString& strProcNameTmp);
public:
	static void SetFireEvacuationTime( const ElapsedTime& _time ) { m_timeFireEvacuation = _time;	}
	CString ClacTimeString(const ElapsedTime& _curTime);

	CString getPersonErrorMsg( void ) const;

//stick this person when dest processor exceed its capacity.
	virtual bool StickForDestProcsOverload(const ElapsedTime& _curTime);
	virtual bool StickNonPaxDestProcsOverload(const ElapsedTime& _curTime);
	virtual bool StickVisitorListForDestProcesOverload(const ElapsedTime& _curTime);
	
	//check need sync
	virtual bool ProcessPassengerSync(const ElapsedTime& _curTime);

	void leaveHoldArea (ElapsedTime releaseTime);
	
	void leaveLineProc ( const ElapsedTime& releaseTime);

	//SelectNextProcessor() try to call selectProcessor() and restore person's info. 
	//if you have modified selectProcessor(),you should change this function properly.
	Processor* SelectNextProcessor(ProcessorDistribution* _pFlowList, const ElapsedTime& p_time);

	void processBillboard (ElapsedTime& p_time);

	void changeQueue (Processor *newProc, ElapsedTime p_time);

	// To collect bags from carosel (called by baggage procs).
	// If all bags found, schedule departure from m_pProcessor.
	void pickupBag ( std::vector<Person*>_vBags, const ElapsedTime& _pickAtTime);

	// test if i must stay at this stage
	bool PersonShouldWaitOnThisStage( int _iStageID, const ElapsedTime& _currentTime  );
	void SetPersonOnStageFlag( int _iStageID, bool _bWait );

	bool FindGreetingProcFromDestination( std::vector<ProcessorID>& _vGreetingPlace  , ProcessorArray &canSelectProc,long _lAttachedID ,bool _bStationOnly,ElapsedTime _curTime );
	bool ifGroupOccupyHeadsOfQueue();

	const CFLowChannelList& GetFlowChannelList(){return m_FlowChannelList;}

	const Point& getNextHoldAreaPoint(void) { return m_nextHoldAiearPoint;}

	bool getNextHoldAreaPoTag(void) {return m_nextHoldAiearPoTag;}
	void setNextHoldAreaPoTag(bool _tag) { m_nextHoldAiearPoTag = _tag;}
	ProcessorDistribution * getDistribution(){return m_pFlowList;}
	ProcessorDistribution * getProDistribution(){return m_pPreFlowList;}

	void setGroupAttachLogIndex( long _lLogIdx ) { m_lGroupAttachLogIndex = _lLogIdx;}
	long getGroupAttachLogIndex( void ) const { return m_lGroupAttachLogIndex;	}

	bool IfPersonInStationSystem()const { return m_bPersonInStationSystem;}
	void SetPersonInStationSystem( bool _bFlag ){ m_bPersonInStationSystem = _bFlag;}	

	void DisablePersonOnStation(){ m_pPersonOnStation = NULL;	}

	bool IfAlreadyEvacuate()const{ return m_bHasProcessedFireEvacuation;	}
	void SetFireEvacuateFlag( bool _bFlag ){ m_bHasProcessedFireEvacuation = _bFlag;	}

	int GetSourceFloor()const { return m_iSourceFloor;	}
	int GetDestFloor()const { return m_iDestFloor;	}

	//set ,get wait in linked area flag
	bool IfWaitInLinkedArea()const { return m_bWaitInLinkedArea; }
	void SetWaitInLinkedAreaFlag( bool _bFlag ){ m_bWaitInLinkedArea = _bFlag;	}
	void SetBackToProc( Processor* _pProc ){ m_pBackToProcFromWaitArea = _pProc; }
	Processor* GetBackToProc(){ return m_pBackToProcFromWaitArea;	}

	ElapsedTime getEntryQueueTime( void ) const { return m_timeEntryQueueTime; }
	void setEntryQueueTime( const ElapsedTime& _time ) { m_timeEntryQueueTime = _time; }

	void SetEntryBagDeviceTime( const ElapsedTime& _time ){ m_timeEntryBagProc = _time;	};
	ElapsedTime GetEntryBagDeviceTime()const { return m_timeEntryBagProc;	}

	void SetDestinationSlot( int _idx ) { m_iDestinationSlot = _idx; }
	int GetDestinationSlot( void ) const { return m_iDestinationSlot; }

	//  [5/18/2004]
	void SetSorterEntryExitProcs( int _idx ) { m_iSorterEntryExitProcs = _idx; }
	int GetSorterEntryExitProcs(){ return m_iSorterEntryExitProcs; }

	//congestion area
	void SetOldStartPoint(const Point& pt) { m_ptOldStart = pt;	}
	const Point& GetOldStartPoint() const { return m_ptOldStart; }
	//
	void SetOldDestPoint(const Point& pt) { m_ptOldDest = pt; }
	const Point& GetOldDestPoint() const { return m_ptOldDest; }
	//
	void SetOldLocation(const Point& pt) { m_ptOldLocation = pt; }
	const Point& GetOldLocation() const { return m_ptOldLocation; }
	//
	void SetOldSpeed(DistanceUnit nSpeed) { m_nOldSpeed = nSpeed; }
	DistanceUnit GetOldSpeed() const { return m_nOldSpeed; }
	//
	//void SetPipePointList(const std::vector<Point>& vPPList) { m_vPipePointList = vPPList; }
	//std::vector<Point>& GetPipePointList() { return m_vPipePointList; }
	//end congestion area operate

	CPaxDestDiagnoseInfo* GetPaxDestDiagnoseInfo() const { return m_pDiagnoseInfo; }
	CPersonOnStation* GetPersonOnStation() const { return m_pPersonOnStation; }

	void SetTransferTypeState(TRANSFER_STATE _state) { m_TransferState = _state;}
	TRANSFER_STATE GetTransferTypeState() { return m_TransferState;}

	//Checks if a person is waiting for service.
	int isWait (void) const { return m_nWaiting; }

	//Check if a person is stranded.
	int isStranded (void) const { return m_nStranded; }

	//Releases a person by set "stranded" false.
	void release (void) { m_nStranded = FALSE; }

	//Releases a person by set "stranded" false and set wait in stage flag as false.
	void release ( int _iStageID );


	//Queue interface methods : returns path index.
	int getCurrentIndex (void) const { return m_nCurrentIndex; }

	//Set next destination index.
	void setNextIndex (int newIndex) { m_nNextIndex = newIndex; }

	//Retruns next destination index.
	int getNextIndex (void) const { return m_nNextIndex; }

	//Resets path index.
	void updateIndex (void);

	//Returns current segment of queue's path.
	int getQueuePathSegmentIndex (void) const { return m_nQueuePathSegmentIndex; }

	//Sets current segment of queue's path.
	void setQueuePathSegmentIndex (int _nQueuePathSegmentIndex) { m_nQueuePathSegmentIndex = _nQueuePathSegmentIndex; }

	//Shorts current segment of queue's path
	void decQueuePathSegmentIndex (void) { m_nQueuePathSegmentIndex--; }

	//Sets person waiting state.
	void wait (int waitState) { m_nWaiting = waitState; }

	//Sets the speed that a person moves m_in queue.
	//A person's speed could not exceed DistanceUnit.
	void setQueueSpeed (DistanceUnit p_max) { if (p_max) m_pPerson->setSpeed(MIN(m_desiredSpeed,p_max));}

	// check whether the Person can jump to the Processor _proc
	// make sure the _proc is at the same level of the person's current processor in flow
	// that means it must be 'JUMP'
	bool canJumpToProcessor(Processor* _proc);

	void PushChannelToList(CFlowChannel* pChannel);
	CFlowChannel* PopChannelFromList();	

	void SetAirsideFlight(AirsideFlightInSim* flight) { m_pFlightAirside = flight;}
	AirsideFlightInSim* GetAirsideFlight() { return m_pFlightAirside;}

	BOOL HasBusServer() {return m_bhasBusServer;} ;
	void HasBusServer(BOOL _ser) { m_bhasBusServer = _ser;} ;
	BOOL HasBusForThisFlight(int _id,HoldingArea* pHold = NULL) ;

	BOOL HasArrivedFixQueue() { return m_IsArrivedFixQueue;}
	void HasArrivedFixQueue(BOOL _res) {  m_IsArrivedFixQueue = _res;}

    BOOL GetFromLandSide() const { return m_bFromLandSide; }
    void SetFromLandSide(BOOL b) { m_bFromLandSide = b; }

	BridgeConnector* GetBridgeConnector(Processor* pProcessor);

	friend stdostream& operator << (stdostream& s, const TerminalMobElementBehavior& p);

	void SetAirsideBus(CAirsidePaxBusInSim* _vehicle) { m_bus = _vehicle;}
	CAirsidePaxBusInSim* GetAirsideBus() { return m_bus ;}

	void SetInfoFromLeader(Person* pLeader);

	void getFollowersAndVisitorsOccupancyInfo(ProcessorOccupantsState& snapshot);

	ProcessorDistribution* GetProcessorDist() { return m_pFlowList;}

	//Returns m_pProcessor index : accessor.
	int processorNum (void) const;

	//Returns m_pProcessor.
	Processor *getProcessor (void) { return m_pProcessor; }
	void setProcessor( Processor* _proc ) { m_pProcessor = _proc; }

	void SetDestStation( int _nIndex );

	bool IsWalkOnMovingSideWalk(){ return m_bWalkOnMovingSideWalk; }
	void SetWalkOnMovingSideWalk( bool _bWalkOnMovingSideWalk ){ m_bWalkOnMovingSideWalk = _bWalkOnMovingSideWalk; }

	DistanceUnit GetCurMovingSidewalkGap(){ return m_dCurMovingSidewalkGap;	}

	void SetCurMovingSidewalkGap( DistanceUnit _dGap ){ m_dCurMovingSidewalkGap = _dGap; }

	int GetInGateDetailMode() { return m_nInGateDetailMode; }


	int GetPosIdxOnBaggageProc( )const { return m_iPosIdxOnBaggageProc;	};
	void SetPosIdxOnBaggageProc( int _iPosIdx ){ m_iPosIdxOnBaggageProc = _iPosIdx;};

	void SetGerentGreetEventOrWaitFlag( bool _flag ){ m_bGerentGreetEventOrWait = _flag;	};

	void SetEntryPoint(const Point& point ) { Entry_point = point ;} ;
	Point GetEntryPoint() {return Entry_point ;} ;
	//void SetNextHoldAreaValue(bool HoldAreaPoTag,const Point &HoldareaPoint);
	
	void kill (ElapsedTime killTime);

protected:
	void writeLogEntry (ElapsedTime time, bool _bBackup, bool bOffset = true );;
	void generateEvent (ElapsedTime eventTime,bool bNoLog );
	virtual void writeLog (ElapsedTime time, bool _bBackup, bool bOffset = true );

	CARCportEngine *getEngine();

	// Dumps all tracks to the log and updates ElementDescription record.
	// It is called at death of passenger.
	// It must repeat once for each member of group
	virtual void flushLog (ElapsedTime p_time, bool bmissflight = false);

	void ReleaseResource(const ElapsedTime& _congeeTime);


	virtual CString GetPersonContextInfo()const;

	// get dest index.
	int GetDestStation( ElapsedTime p_time, Processor* _pProcessor );

	//Get elevator destination processor()
	Processor*  TryToGetElevatorDestProc( ElapsedTime p_time, Processor* _pProcessor );

	// output: _pProc 
	virtual bool HandleGreetingLogic( Processor** _pProc, bool _bStationOnly, ElapsedTime _curTime){ ASSERT( 0 );return false;	}	

	// wait for resource 
	// if need to wait, send a resource request to pool, return true
	// else return false
	virtual bool waitForResourceIfNeed( const ElapsedTime& _time );
	Terminal *getTerminal();

	//test if the baggage proc is suitable here
	bool IfBaggageProcAllowed( const ProcessorArray *nextGroup , ProcessorDistribution* _pDistritution) const;

	int noExceptions (ProcessorArray *p_array);
private:
	Point& getLocation();
	void setState (short newState);

	//test if there is any baggage proc in the dest distribution
	bool IfBaggageExistInDist( ProcessorDistribution* _pDistritution) const;


	// base on what the next processor, creat the log for the passenger.
	void ProcessPipe( Processor* _pNextProc, ElapsedTime& _curTime, CFlowItemEx* _pPrevFlowItem );
	
	//passenger walk from linking holding area should auto pipe
	void ProcessHoldingAreaPipe(Processor* _pNextProc, ElapsedTime& _curTime);

	bool FindBillboardLinkedProcessorFromDestination(Processor **destProc,ElapsedTime _curTime, bool _bStationOnly);
	int hasClearPath (const ElapsedTime& _curTime);
	void AvoidFixedQueueIfNecessary( ProcessorArray* _pBarrierList , ProcessorArray* _pNewCreateBarrier );


	// filter out all the processors which is a flow belt processor( a subtype of conveyor processor )
	void FilterOutFlowBeltProcs( ProcessorArray* _pDestProcs );


	// filter out processor which is not satisfied with flow condition defined in pax flow
	void FilterOutProcessorByFlowCondition( ProcessorArray* _pDestProcs,const ElapsedTime& _curTime );

	// Keep only integrated station in the processor array.
	void KeepOnlyStation( ProcessorArray* _pDestProcs );

	//void WritePipeLogs( std::vector<Point>& _vPointList, ElapsedTime& _eventTime );
	void WritePipeLogs( PTONSIDEWALK& _vPointList, ElapsedTime& _eventTime, bool _bNeedCheckEvacuation );


	//choose the holdingArea linkage
	Processor* GetHoldAreaLinkageFromType(const Person *_pPerson,const ProcessorArray&_pArray);

	// walk from source point to destination point by pipe
	void WalkAlongShortestPathForEvac( const Point& _srcPoint, const Point& _desPoint, ElapsedTime& _curTime );


	Point GetPipeExitPoint( Processor* _pNextProc,int iCurFloor, CString& _curTime,Point& outPoint, CFlowItemEx* _pFlowItem = NULL) ;


	// walk from location to next process by pipe
	void WalkAlongShortestPath( Processor* _pNextProc, const Point &point,ElapsedTime& _curTime);



	bool HandleLoaderHoldLogic( Processor** _nextproc, const ElapsedTime& _curTime );
	bool HandleSorterLogic(Processor** _nextproc, const ElapsedTime& _curTime );

	bool FindAllPossibleDestinationProcs(std::vector<ProcessorID>& _vSearchProc,ProcessorArray &canSelectProc ,bool _bStationOnly,ElapsedTime _curTime,bool &bGerentGreetEventOrWait );

	
	virtual void ClearBeforeThrowException( const ElapsedTime& _currentTime );

	// check if need to generate avoid event
	bool needToAvoidDensity( void ) const;

	// if _eCurTime > FlightDepTime - _eDeltaTime
	bool IsLateForDepFlight( ElapsedTime _eCurTime, long _lMinutes );

	//--------------------------------------------------------------------------------------------
	//Summary:
	//		passenger enter from terminal to onboard mode need add passenger to onboard flight and 
	//		check all passenger whether enter onboard flight
	//--------------------------------------------------------------------------------------------
	bool EnterFromTerminalModeToOtherMode();
	//--------------------------------------------------------------------------------------
	//Summary:
	//		passenger from onboard to enter terminal and check door whether close
	//Parameters:
	//		tTime[in]: close door time
	//--------------------------------------------------------------------------------------
	void EnterFromOnboardModeToTerminalMode(const ElapsedTime& tTime);

	// Calculate walk length when elements walk along user defined pipe.
	double CalculateWalkLengthOfUserPipe(const Point& ptFrom, const Point& ptTo, const std::vector<int>& vPipeList);

protected:
	Person* m_pPerson;
	bool m_bInBillboardLinkedproc;

	// Current location within the terminal.
	Processor* m_pProcessor;

	// Current location within the terminal. 
	//Processor* m_pDestStation;

    // current distribution of transition flows for this Person.
    ProcessorDistribution* m_pFlowList;
	// pre distribution of transition flows for this Person. used when person must avoid fixed queue
	ProcessorDistribution* m_pPreFlowList;

    // moving / non-moving flag
    int m_nWaiting;

    // moving / non-moving flag
    int m_nStranded;

	// Queueing status variables.
	// Current index m_in queue (0 == HEAD).
	int m_nCurrentIndex;              

	// Queueing status variables.
	// Destination index.
	int m_nNextIndex;              

	// Queueing status variables.
	// Current segment of the queue's path.
	int m_nQueuePathSegmentIndex;


   // Queueing status variables.
    // Freemoving speed, needed for queuing.
    DistanceUnit m_desiredSpeed;  

    //Used for multiple calls to processGeneralMovement due to barriers
    int m_nAvoidingBarrier;

    //Used for multiple calls to processGeneralMovement due to barriers
    int m_nNextState;

    //Used for multiple calls to processGeneralMovement due to barriers
    Point m_nextPoint;

	Terminal* m_pTerm;



	//congestion area
	Point m_ptOldStart;
	Point m_ptOldDest;
	DistanceUnit m_nOldSpeed;
	Point m_ptOldLocation;

	std::vector<PipePointInformation> m_vPipePointList;


	CPersonOnStation* m_pPersonOnStation;

	bool m_bWalkOnMovingSideWalk;	// walk on the moving side walk?

	DistanceUnit m_dCurMovingSidewalkGap;	// Set at getservice location, used on beginService;
	
	int m_nInGateDetailMode;	// default is NORMAL_MODE. it means go to the detail gate mode.

	int m_iPosIdxOnBaggageProc; // pos indx in baggage proc
	ElapsedTime m_timeEntryBagProc;// time when bag or pax entry baggage processor

	std::vector<Person*> m_vBags;// all my bags


	bool m_bGerentGreetEventOrWait;
	Person* m_pWantToGreetPerson;

	int m_iNumberOfPassedGateProc;

	int m_iNumberOfPassedReclaimProc;
	// first time entry in processor Queue ( using by TLOS logic )
	ElapsedTime m_timeEntryQueueTime;

	//whether or not person should stay at stage
	std::map<int,bool> m_mapWaitOnStage;


	//wait in linked area flag
	bool m_bWaitInLinkedArea;
	Processor* m_pBackToProcFromWaitArea;

	//the floor person current on
	int m_iSourceFloor;

	// the floor person want to go to
	int m_iDestFloor;

	// if he has already evacuated 
	bool m_bHasProcessedFireEvacuation;

	// the fire evacuation time 
	static ElapsedTime m_timeFireEvacuation;

	// flag to decide if person is now in station system
	// used by fire evacuation logic only
	bool m_bPersonInStationSystem;

	// the index of conveyor's slots
	int m_iDestinationSlot;

	// the index of entry processor/exit processor, used by CONVEYOR - SORTER [5/18/2004]
	// you should set it as -1 after used.
	int m_iSorterEntryExitProcs;

	// when person get next processor, let the next proc directtly as m_pDirectGoToNextProc
	// then person will directly go to that processor
	Processor* m_pDirectGoToNextProc;

	// the flag of the person if has been service by resource;
	bool m_bHaveServiceByResource;
	//////////////////////////////////////////////////////////////////////////
	// diagnose info for pax flow
	CPaxDestDiagnoseInfo* m_pDiagnoseInfo;
	
	// group attach log index
	long m_lGroupAttachLogIndex;

	// currently, it is used only in conveyor system. it means which processor( maybe indirectly) make him stop
	int m_iStopReason;


	// the information stored the Holdarea, line, baggage proc's Entry point
	// if the holdarea proc don't have inConstraint, select one of the service location point randomly as the proc's entry point,
	//the information need to keep down in order to used when the pax arrive at holdarea.
	//bool m_nextHoldAiearPoTag; indicated need to use the entry point or not
	//Point m_nextHoldAiearPoint; stored the entry point inforamtion
	//reference to void Processor::getNextLocation (Person *aPerson)
	Point m_nextHoldAiearPoint;
	bool m_nextHoldAiearPoTag;



	//stack for FlowChannel ,  use for 1x1
	CFLowChannelList m_FlowChannelList;

	//billboard linked processor
	std::vector<ProcessorID> m_vNeedVisitLinkedProcessor;
    
	//the bus which person take on
	CAirsidePaxBusInSim* m_bus ;

	//for person if arr bridge or dep bridge
	ArrDepBridgeState m_emBridgeState;

	AirsideFlightInSim* m_pFlightAirside;

	BOOL m_bhasBusServer;//if has pax bus server 
	BOOL m_IsWalkOnBridge ; // check if pax walk through the bridge ,not bus 
	BOOL m_IsArrivedFixQueue ; // check if the person has arrived at the Fixqueue ;
    BOOL m_bFromLandSide; // true if this pax from landside, used by terminal processBirth().
    Point Entry_point ;

	TRANSFER_STATE m_TransferState;

	//Current coordinates
	Point location;

	//Current destination, used for
	Point m_ptDestination;      

	//pure location( no offset )
	Point pure_location;

//Mobile behavior in terminal position interface
public:
	//Returns current coordinates
	const Point& getPoint (void) const { return location; }

	//Returns current destination.
	const Point& getDest (void) const { return m_ptDestination; }

	// get pure location
	const Point& getPureLocation( void ) const { return pure_location; }

	ElapsedTime moveTime (void) const;

	ElapsedTime moveTime( DistanceUnit _dExtraSpeed, bool _bExtra ) const;

	//Sets Element's destination
	virtual void setDestination (Point p);
	//for non passenger follower relative position
	void SetRelativePosition(const Point& paxDirection,const Point& paxLocation);

	virtual void setLocation( const Point& _ptLocation ){ location = _ptLocation;	}

	inline void SetWalkOnBridge(BOOL b){ m_IsWalkOnBridge = b; }
	inline BOOL IsWalkOnBridge()const{ return m_IsWalkOnBridge; }
	ArrDepBridgeState getBridgeState()const{ return m_emBridgeState;}
	void setBridgeState(ArrDepBridgeState state){ m_emBridgeState = state; }

	Processor * getLastTerminalProc() const { return m_pLastTerminalProc; }
	void setLastTerminalProc(Processor * pProc) { m_pLastTerminalProc = pProc; }

    int getEntryPointCorner() const { return m_entryPointCorner; }
    void setEntryPointCorner(int val) { m_entryPointCorner = val; }

public:
	int inElevotor;

	// when fire, the person is in pipe.
	bool m_bEvacuationWhenInPipe;

	// flag of when closing, can serve for this person
	bool m_bCanServeWhenClosing;

	// area intersect point list
	INTERSECTPOINTLIST m_intersectPointList;

	// when the person avoid area, it avoid direction
	int m_iAvoidDirection;

	bool m_bUserPipes;

	//the loader proc is not allowed group,so the pax pass through the loader processor
	//need to divide the group to leader and follower,if the loader is exceed its capacity
	// then,need to set the follower to the linked hold area processor.
	bool m_bNeedLoaderToHoldArea;

	Processor *m_pLoaderProc;

	bool m_bIsArrivalDoor;//for write log for pax arrival door height

	int first ;

	void setLandsideSelectedProc(const std::vector<ALTObjectID>& altLandsideSelectedProc);
	const std::vector<ALTObjectID>& GetLandsideSelectedProc()const;
private:
	//int m_nBridgeIndex;

	std::vector<ALTObjectID> m_vAltLandsideSelectedProc;

	//the last terminal processor
	//will be set value when process to END processor
	//uses for arrival passenger
	Processor *m_pLastTerminalProc;

    // for fixed queue only, the entry point's corner index of queue.
    int m_entryPointCorner;
};