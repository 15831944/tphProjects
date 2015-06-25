#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <inputs/MultiMobConstraint.h>
#include <inputs/Direction.h>
#include <inputs/paxtoresourcepoolmap.h>
#include <CommonData/ProcessorProp.h>
#include <results/procntry.h>

#include "mobile.h"
#include "ProcessorCapacity.h"
#include "ProcessorCapacityInSimulation.h"

class DependentProcessorList;
class CSubFlowList;
class CanServeProcList;
class ProcessorArray;
class MiscProcessorIDList;
class ProcessorDataElement;
class CFlowChannel;
class CSinglePaxTypeFlow;
class CARCportEngine;
class ProcessorQueueBase;

typedef long	MobileElemID;
typedef int		ProcessorIndex;
typedef std::pair<MobileElemID, ProcessorIndex> WaitingPair;

//Base class of all kind of processors.
class ENGINE_TRANSFER Processor : public ProcessorProp
{
    friend ProcessorList;

protected:
    //The index of the processor in the processor list, is set when link is established.
    int index;

    //The dynamic list of all passengers in transit through the processor.
    SortedMobileElementList occupants;

    // Object that controls all waiting line logic.
    ProcessorQueue* m_pQueue;

    // Flag indicating whether the processor will serve visitors.
    int visitorFlag;

    // Flags describing current state of processor.
    int available;

    // Flags describing current state of processor.
    int isOpen;

    // Flags describing current state of processor.
    int isClosing;

	//Recycle frequency
	std::map<int,int> m_RecycleFreq;

	// MultiMobConstraint that current can pass the processor
	// CMultiMobConstraint m_CanPassType;
	
	// MultiMobConstraint that current can inclusive pass the processor
	CMultiMobConstraint m_InclusivePassType;
	
	// MultiMobConstraint that current can exclusive pass the processor
	CMultiMobConstraint m_ExclusivePassType;

	// MultiMobConstraint that current can not pass the processor
	CMultiMobConstraint m_NotPassType;
	
    // Complete schedule of assignments for the day
    ProcessorRosterSchedule *assignmentSchedule;

    // Entry in Processor Hierarchies that will determine service time for various client types.
    ProcessorEntry *service;

    // Entry in Processor Hierarchies that will determine subsequent destination for various client types.
    ProcessorEntry* m_pTransition;
	ProcessorEntry* m_pTransitionGateDetail;
    ProcessorEntry* m_pStationTransition;

    //The list of gates that the processor leads to.
    //It is used for intelligent flow management to and from gates.
    GroupIndex *gateList;

    //Number of gates.
    int gateCount;

    ProcLogEntry logEntry;

	Terminal* m_pTerm;
	CARCportEngine *m_pEngine;
	
	bool m_bDisallowGroup;

	long m_lQueueCapacity;

	long m_lTerminateTime;
	
	int m_nCyclicFreq;
	///GroupSerTimeModIndex		Type
	//  0						Sum
	//  1						Average
	//  2						Minimum
	//  3						Maximum
	//  4						Random for Group
	int m_nGroupSerTimeModIndex;
	//
	int m_nGroupSerTimeModSum;	

	bool m_bWaitInQueueToOpen;
	CDirection m_direction;

	CDirection m_reclaimDirection;//find reclaim

	CDirection m_landsideDirection;

	// TLOS releted flag
	bool m_bIsTLOSInterestProc;
	CMobileElemConstraint m_currentServicePaxType; 
	// emergent flag
	bool m_bEmergentFlag;
	
	std::vector<Processor*>m_vSourceProcs;	
	
	CPaxToResourcePoolMap m_ResourcePoolMap;

	// (now, used only in conveyor system.) it means which processor casue me stop moving
	long m_lReason;	

	// ( currently ,only used in TLOS Created new processor dynamicly ) it means whether or not processor is dynamicly created
	bool m_bDynamicCreatedProc;

	std::vector<GroupIndex> m_vLinkedDestProcs;

	float m_fltServiceTimeModificationFactor; // value range =[0.0 , 1.0] [4/28/2004]

	std::vector<WaitingPair> m_vWaitingPax;//the momery of item should be new() from heap;

	std::vector<WaitingPair> m_vWaitingPaxInDependentProc;

	std::vector<int> m_vGateFlowPairProcs;//parent of the gate processor
	std::vector<int> m_vReclaimFlowPairProcs;//parent of the reclaim processor
	std::vector<int> m_vLandsidePairProc;//parent of landside processor

	// backup
	bool m_bBackup;

	//when the proc has created by run simulation last time,it will set to true this time
	bool m_bNewCreatedTLOSProcFlag;
protected:
	
	//write integrated processor
    virtual int writeSpecialProcessor(ArctermFile& procFile) const {return TRUE;};
	
	//read integrated processor
	virtual int readSpecialProcessor(ArctermFile& procFile) {return TRUE;};
	
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;

	// base on the "occupants" which is count of group, return number of passengers.
	//int GetTotalNumberPassengerInThisProcessor() const;

	// when make available, notice source conveyor processor if necessary
	bool noticeSourceConveyor( const ElapsedTime& time );

public:
	virtual ProcessorQueueBase* GetQueue() const;

	bool getNewCreatedTLOSProcFlag(){return m_bNewCreatedTLOSProcFlag;}
	void setNewCreatedTLOSProcFlag(bool bFlag){m_bNewCreatedTLOSProcFlag = bFlag;}
	bool IsBackup(){return m_bBackup;}
	void SetBackup(bool bBackup){ m_bBackup = bBackup;}
	long GetTerminateTime(){ return m_lTerminateTime; }

	// add pax id and processor idx to the dest processor.
	void AddWaitingPair(const WaitingPair& _wait);

	void AddWaitingPairInDependentProcList(const WaitingPair& _wait);
	void AddWaitingPairInDependentProc(const WaitingPair& _wait);

	void AddGateFlowPairProcessor(int nIndex);
	void notifyGateFlowPair(const CMobileElemConstraint& mobileType);
	void AddReclaimFlowPairProcessor(int nIndex);
	void AddLandsideFacilityFlowPair(int nIndex);
	void notifyLandsideFacilityFlowPair(const CMobileElemConstraint& mobileType);
	void notifyReclaimFlowPair(const CMobileElemConstraint& mobileType);
	void clearGateAndReclaimFlowPair();
	
	static bool FindDestProcessors(const Terminal* _pTerminal, const Person* _person,
		const Processor* _curProc, const ProcessorDistribution* _pDistribution ,
		const CFlowChannel* _pInChannel,const ElapsedTime& _curTime, 
		const CSinglePaxTypeFlow* _pProcessSingleFlow, const ProcessorID* _pProcessCurID,
		ProcessorArray& _destProcs, bool& bOneToOneReason, bool& bOneXOneReason);
	bool FindDestProcessors(const Person* _person, const ProcessorDistribution* _pDistribution ,
		const CFlowChannel* _pInChannel,const ElapsedTime& _curTime,  
		const CSinglePaxTypeFlow* _pProcessSingleFlow, const ProcessorID* _pProcessCurID,
		ProcessorArray& _destProcs, bool& bOneToOneReason, bool& bOneXOneReason);

	// remove the processors which can not take more passengers from the dest processor list and add into different processor list for different 
	// reasons ( closed, exceedcapacity, not open )
	static void PruneInvalidDestProcs(Terminal* pTerm, Person* pPerson, ProcessorArray* _pSourceDestProcs, ProcessorArray* _pClosedDestProcs=NULL, ProcessorArray* _pExceedCapacityProcs=NULL, ProcessorArray* _pOtherReasonProcs=NULL);

	void clearLinkedDestProcs();
	int FindLinkedDestinationProcs(const ProcessorID& _groupID, ProcessorArray& _destProcs , bool& _bLevel0Matched) const;
	int GetLinkedDestCount() const { return (int)m_vLinkedDestProcs.size();}
	void SetDisallowGroupFlag( bool _bFlag ) { m_bDisallowGroup = _bFlag;	}
	bool GetDisallowGroupFlag()const { return m_bDisallowGroup ;	}
	
	void SetDynamicCreatedFlag( bool _bFlag ) { m_bDynamicCreatedProc = _bFlag;	}
	bool GetDynamicCreatedFlag()const { return m_bDynamicCreatedProc ;	}

	void SetWaitInQueueToOpen(bool _bFlag) { m_bWaitInQueueToOpen = _bFlag;	}
	bool GetWaitInQueueToOpen(void){return m_bWaitInQueueToOpen;}
	
	void AddSourceProcs( std::vector<Processor*>& _AllNewSources );
	//void AddDestProcs( std::vector<Processor*>& _AllNewDests );

	void ClearAllSourceProcs(){ m_vSourceProcs.clear();	}
	//void ClearAllDestProcs(){ m_vDestProcs.clear();	}	

	GroupIndex * GetGateList() { return gateList ;}
	ProcessorQueue* GetProcessorQueue() { return m_pQueue; }
	void SetTLOSFlag( bool _bFlag ){ m_bIsTLOSInterestProc = _bFlag;	}
	void SetCurrentServicePaxType( const CMobileElemConstraint& _currentServicePaxType ) { m_currentServicePaxType = _currentServicePaxType; }

	bool GetTLOSFlag()const{ return m_bIsTLOSInterestProc;	}
	const CMobileElemConstraint& GetCurrentServicePaxType()const{ return m_currentServicePaxType;	}

	void setEmergentFlag( bool _flag ) { m_bEmergentFlag = _flag;}
	bool getEmergentFlag( void ) const { return m_bEmergentFlag; }
	
	bool CanLeadTo( const CMobileElemConstraint& _paxType,  const ProcessorID& _destProcID)const;
	CDirection& GetDirection() { return m_direction;	}
	void SetDirection( CDirection& _direction ){ m_direction = _direction;	}

	bool CanLeadToReclaim(const CMobileElemConstraint& _paxType,  const ProcessorID& _destProcID)const;
	CDirection& GetReclaimDrection(){return m_reclaimDirection;}
	void SetReclaimDirection(CDirection& _reclaimDirection){m_reclaimDirection = _reclaimDirection;}

	CDirection& GetLandsideDrection(){return m_landsideDirection;}


	void ClearGateAndReclaim();
	
	virtual void RefreshRelatedData( InputTerminal* _pInTerm ){}

    //Constructors.
    Processor();
	Processor( const Processor& _anotherPorc );
	Processor& operator = ( const Processor& _anotherPorc);
    //Destructors.
    virtual ~Processor();

	//virtual Point GetPipeExitPoint( int iCurFloor, CString& _curTime,Point& outPoint,TerminalMobElementBehavior *terminalMob) ;
    // Initializes service m_location(s).
    // It must test count according to type of processor.
    virtual void initServiceLocation (int pathCount, const Point *pointList);

    // Allocates and initializes fixed or non-fixed queue.
    virtual void initQueue (int fixed, int pathCount, const Point *pointList);

    virtual void initMiscData ( bool _bDisallowGroup, int visitors, int count, const GroupIndex *gates );

    virtual void initSpecificMisc (const MiscData *miscData) {};

    virtual int readMiscInfo (ArctermFile& dataFile) { return TRUE; }

    // Sets processor index and relative event index.
    virtual void setIndex (int ndx) { index = ndx; logEntry.setIndex (ndx); }

    // Acquire service m_location.
	virtual Point AcquireServiceLocation( Person* _pPerson  ){ return GetServiceLocation(); }

    // Returns processor's TransitionRow.
    virtual ProcessorDistribution *getNextDestinations(const CMobileElemConstraint &p_type, int _nInGateDetailMode );

    // Adds Person to the processor's occupants list.
    virtual void addPerson (Person *aPerson);

    // Removes Person from the processor's occupants list.
	virtual void removePerson (const Person *aPerson);

    // It is called by Person::processServerArrival
    // The different processor types serve clients in different ways.
    virtual void beginService (Person *aPerson, ElapsedTime curTime);
 
    // It resets the server to allow the processor to process the next client and releases the next Person waiting in its queue.   
    virtual void makeAvailable (const Person *aPerson, ElapsedTime availTime, bool _bNoDelay );

    //It updates Person's state based on current state & processor data
    virtual void getNextState (int& state,Person* _pPerson) const;

    //It sets Person's next destination based on state & processor data
    virtual void getNextLocation (Person *aPerson);

    // Returns TRUE if the processor can serve a client (implicating open).
    virtual int isAvailable (void) const { return (isOpen || isClosing) && available; }

    // Returns TRUE if the processor cannot serve a client.
    virtual int closing (void) const { return isClosing; }

	int isClosed( void ) const { return !isOpen && !isClosing;	}
	int isOpened( void ) const { return isOpen; }
	void setCloseFlag( void ) { isOpen = FALSE; isClosing = FALSE; }

    virtual void balanceQueues (ElapsedTime curTime);

    // Kills all passengers stranded in queue at end of simulation.
    // Returns a count of the stranded passengers.
    virtual int clearQueue (ElapsedTime curTime);

    //It updates queue to reflect Person leaving head.
    virtual void leaveQueue (const Person *aPerson, ElapsedTime p_time, bool _bNoDelay );

    //It starts p_person moving and relays advance event to next pax in line.
    virtual void moveThroughQueue (Person *p_person, ElapsedTime p_time);

    // It tests Person's position and Processor's state to see if Person can advance to service m_location and begin service.
    virtual void processPerson (Person *aPerson, ElapsedTime p_time, bool _bStation );

    //Only and always returns true.
    virtual int writeExtraFields (ArctermFile& miscDataFile) const{ return TRUE; }

    virtual void closeStation (ElapsedTime p_time);

	virtual int ReadProcessor(ArctermFile& procFile);

	// Clean all sim engine related data, for the new sim process
	virtual void CleanData();

	//change conveyor processor state
	virtual void synFlag();

	void SetEngine( CARCportEngine* _pEngine );
	void SetTerminal(Terminal *_pInTerm);
	CARCportEngine* getEngine();
	Terminal* GetTerminal();
	Terminal* GetTerminal() const;

    // Method used to set Processor's name.
    void init (const ProcessorID& Name);


    
    void initMisc (const MiscData *miscData);

    void appendLinkedDestProcs(const MiscProcessorIDList* _pLinkedDestIdList);
		

    //It establishes link to the appropriate entry in service time database.
    void setServiceTimeLink (const ServiceTimeDatabase& database);

    //It establishes link to the appropriate entry in transition state database.
    void setTransitionLink (const PassengerFlowDatabase& database, 
		const PassengerFlowDatabase& _gateDetailFlowDB, 
		const PassengerFlowDatabase& _stationPaxFlowDB,
		const CSubFlowList& pSubFlowList);

    //It establishes link to the appropriate entry in proc assignment database.
    void setAssignmentLink (const ProcAssignDatabase& database);

    // Returns processor's index in the processor list.
    int getIndex (void) const { return index; }


	//Returns the number of points in the queue
	int queueLength(void) const; 

	//Returns queue position
	Point queuePoint(int index) const;

	//Returns queue as Path
	Path* queuePath();	

	//Returns TRUE if queue is fixed
	BOOL queueIsFixed(void) const;

    //Returns complete schedule of assignments for a day.
    ProcessorRosterSchedule *getSchedule (void) const { return assignmentSchedule; }

	ProcessorDistribution* getNextStation(const CMobileElemConstraint& _type);

    //If offers service, it returns the time required to service the client.
    long getServiceTime (Person* person, const ElapsedTime& curTime ) const ;

    long getServiceTime(const CMobileElemConstraint &type,const ElapsedTime& curTime ) const;
	// Remove all Persons from occupants list;
	virtual int clearPerson( const ElapsedTime& _time );

    //Sets processor available to offer service.
    void setAvail (int p_flag = 1) { available = p_flag; }

    //Returns flag available.
    int getAvail (void) { return available; }

    // Process opening for service.
    void open ( ProcessorRoster* _pAss = NULL );

    // Process shutdown event (must complete service of queue first).

     void close ( ProcessorRoster* _pAss = NULL );

    // It informs any source HoldingAreas that the processor has a new assignment, allowing them to release any clients that can use it.
	void ClearSequentialQueue();
    void notifyAssignmentChange (ElapsedTime time) const;

    // Returns true if it can serve visitors, false otherwise.
    int acceptsVisitors (void) const { return visitorFlag; }
    
    int leadsToGate (int index) const;

	//It scans gateList for person's gate.
	// Returns true if gate index found in gate list, false otherwise.
	bool AttachToGate (int index) const;

    // Returns true if the processor can serve the passed pax type.
	// 
	// MUST KNOW : for Exclusive roster rules' presence, the bool returned value is not reliable.
	//             you should call this method for all 'Possible Processors' iteratively
	//             by passing the same in a CanServeProcList which should not be NULL
	// GOOD LUCK !!!
	// Example code : bool Person::canGotoTheProcessor(Processor* _proc)
	// virtual method
    virtual int canServe (const CMobileElemConstraint& paxType,   CanServeProcList* _canServeProcList = NULL) const;

    // Checks to see if a person with id of p_ndx is currently at processor.
    MobileElement *findElement (long p_ndx) const;
	MobileElement *fineVisitor(long p_ndx) const;

    // Add Person to the list of people approaching the queue
    void addToQueue (Person *aPerson, ElapsedTime currentTime);

    // Returns the current number of persons waiting in the queue.
    int getQueueLength (void) const;

    // Returns the current number of Persons both waiting in and heading towards the queue.
    int getTotalQueueLength (void) const;

    // Returns the current number of groups both waiting in and heading towards the queue.
    int getTotalGroupCount (void) const;

    // Add Person to the tail of the waiting queue.
    // All Persons approaching the queue are informed of the change in the current m_location of the queue's tail position.
    void arriveAtQueue (Person *aPerson, ElapsedTime p_time, bool _bStation );

    //It updates queue to reflect Person moving forwards.
    void notifyQueueAdvance (Person *aPerson, ElapsedTime p_time, bool _bStation );

    int writeProcessor (ArctermFile& procFile) const;
	

    int writeMiscData (ArctermFile& miscDataFile) const;

    void writeLogEvent (const Person* aPerson, ElapsedTime time, int eventType, long _lLoad = -1 );

    int echoApproach (void) const;

    //Retruns logEntry.
    ProcLogEntry &getLogEntry (void){ return logEntry; }

    virtual void writeLog (void);
	virtual void sortLog  ( void );
    virtual void flushLog (void);

    void openStation (ElapsedTime p_time);

    double distance (Processor *p_proc) { return (getServicePoint (0)).distance (p_proc->getServicePoint(0)); }

	// check the destination processor which the person want to go is exceed capacity or not
	bool IsDestProcExceedCapacity(Person* pPerson , const ElapsedTime& _curTime );

	
	// check if this processor can serve current person 
	// according to processor's current occupancy state and it's capacity attributes.
	virtual bool isVacant( Person* pPerson  = NULL) const;

	virtual bool isPaxSkipVacant(Person* pPerson, const ElapsedTime& _curTime)const {return true;}
	virtual bool isQueueLengthVacant(Person* pPerson)const {return true;}
	int GetCyclicFreq() const;
	
	//check recycle frequency
	bool CheckRecycleFrequency(Person* pPerson);
	void AddRecycleFrequency(Person* pPerson);
	void ClearRecycleFrequency();

	void GetQueueTailPoint( Person* _pPerson );


	// Get Entry Point of the processor,
	// might be inconstraint, or service m_location.
	virtual Point GetEntryPoint(const CString& _strMobType,int _iCurFloor, const CString& _strTime);

	//check the proc is the same floor with the icurFloor;
    virtual bool CheckIfOnTheSameFloor(int _iCurFloor);
	// Get Exit Point of the processor,
	// might be outconstraint, or service m_location.
	Point GetExitPoint();

	// check if there is any destination start from this processor
	bool HasDestination( const CMobileElemConstraint& _type, int _nInGateDetailMode ) const;

	ProcessorEntry* GetNormalDestDistribution() { return  m_pTransition;};
	void SetNormalDestDistribution( ProcessorEntry* _pDistribution ){ m_pTransition = _pDistribution;	};
	
	SortedMobileElementList* GetOccupants(){ return &occupants;	}
	
	//init reource pool data
	void initResourcePool( void );

	// get best pool by pax type
	CResourcePool* getBestResourcePoolByType( const CMobileElemConstraint& _mob, long& _lServiceTime );

	// notice processor release pax
	bool noticeReleasePax( MobileElement* _mobElem, const ElapsedTime& _time );

	virtual long GetStopReason()const { return m_lReason; };
	Processor* GetRandomDestProc(Person* _pPerson);
	
	virtual bool IsWantSettingClosing(CMobileElemConstraint& _constrain){return (!available + occupants.getCount()) || !available;}

	virtual Processor* CreateNewProc();
	virtual Processor* CopyProcessor(const ProcessorID& _newPorcID, Terminal* _pTerm, const int _nFloor, const int _nFloorCountk, const int _nDestFloorTotal);
	virtual bool PrepareCopy(Processor* _pDestProc, const int _nFloor, const int _nDestFloorTotal);
	virtual bool CopyServiceLocation(Processor* _pDestProc, const int _nFloor, const int _nFloorCount);
	virtual bool CopyInConstraint(Processor* _pDestProc);
	virtual bool CopyOutconstraint(Processor* _pDestProc);
	virtual bool CopyQueue(Processor* _pDestProc);
	virtual bool CopyOtherData(Processor* _pDestProc);
protected:	
	bool releasePusher( const ElapsedTime& time );
	bool releaseSimpleConveyor( const ElapsedTime& time );
	void DeleteMyTransition(ProcessorEntry** _ppTransition);

	void InitTransition(ProcessorEntry** _ppTransition, 
		std::vector<ProcessorDataElement*>& _vData,
		const PassengerFlowDatabase& database,
		const CSubFlowList& pSubFlowList);

	//public:
	//void InitTransition(ProcessorID , 
	//		const PassengerFlowDatabase& database,
	//		const CSubFlowList& pSubFlowList);


protected:
	void addConstraintToPaxTypeList( CMultiMobConstraint* _pSrcTypeList, CMultiMobConstraint& _destTypeList );
	void removeConstraintFromPaxTypeList( CMultiMobConstraint* _pSrcTypeList, CMultiMobConstraint& _destTypeList );
	void setPersonFlagWhenClose( ProcessorRoster* _pCloseAss );
	// do match in m_NotPassType, m_ExclusivePassType, m_InclusivePassType, 
	// and updata _canServeProcList if need
	int doMatch( const CMobileElemConstraint& paxType, CanServeProcList* _canServeProcList, bool _bIgnoreStar ) const;

	// wake up the passenger in previous processor who was stuck because the exceed the capacity in this processor
	void WakeupWaitingPaxInPrevProc(  const ElapsedTime& time );

	void WakeupWaitingPaxInDependentProc(  const ElapsedTime& time );


public:
	//For Processor Capacity attributes
	void InitProcessorCapacityInSim();

	ProcessorCapacity m_ProcessorCapacity;
	ProcessorCapacityInSimulation m_ProcessorCapacityInSim;
   

	// dependent processors(stop new service if all dependent processors are occupied)
	DependentProcessorList * m_pDependentProcList;
	void initDependentProcList(const bool bUseDependentProc, const MiscProcessorIDList* _pDependentProcList);

	bool isDependentProcUsed() const ;
	bool isAllDependentProcsOccupied(Person* _person) const;

	virtual void UpdateFloorIndex(const FloorChangeMap& changMap);
public:

//
	Point m_min;
	Point m_max;
	virtual void UpdateMinMax();
protected :
   void initEntryPoint(std::vector<int>& points) ;
   void ResetEntryPoint() ;
};



class ENGINE_TRANSFER TerminalProcessor : public Processor
{
	
};

//A container conatains the sorted processors.
class ProcessorArray : public SortedContainer<Processor>
{
public:

    // default array size 8, subscript 0, delta (inc) 8
    ProcessorArray () : SortedContainer<Processor> (8, 0, 8) {};

	ProcessorArray ( const ProcessorArray& _arrary );

    virtual ~ProcessorArray () {};
	
	// From this array get one processor which has one to one relationship to the _pSourceProcessor
	Processor* GetOneToOneProcessor( const ProcessorID* _pSourceID, int _nDestIdLength ) const;



};
void GetMinMax(Point& min, Point& max, const Point& newPoint);
void GetMinMax(Point& min, Point& max, const Path& newPath );
#endif
