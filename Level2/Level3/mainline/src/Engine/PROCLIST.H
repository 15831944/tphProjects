#ifndef PROCESSOR_LIST_H
#define PROCESSOR_LIST_H

#include "..\common\template.h" 
#include "..\common\dataset.h"
#include "..\common\elaptime.h"
#include "..\common\strdict.h"
#include "process.h"
#include "ProcGroupIdxTree.h"
#include <map>
#include <CommonData/BaseProcess.h>
#include "..\inputs\ProcessorFamily.h"
#include "..\inputs\ElevProcessorFamily.h"


#define START_PROCESSOR_INDEX		0x7fff
#define END_PROCESSOR_INDEX			0x7ffe
#define TO_GATE_PROCESS_INDEX		0x7ffd		
#define FROM_GATE_PROCESS_INDEX		0x7ffc

#define BAGGAGE_DEVICE_PROCEOR_INDEX		0x7ffb

#define MAX_POINTS 1024

enum ENUM_PROC_ID { PROC_ID_START, PROC_ID_END, PROC_ID_FROM_GATE, PROC_ID_TO_GATE, PROC_ID_BAGGAGE_DEVICE };

class Terminal;
class CTermPlanDoc;
class CPaxFlowConvertor;
class CAllPaxTypeFlow;
class CPaxDestDiagnoseInfo;
class ProcessorFamily;

class ENGINE_TRANSFER ProcessorList : public DataSet, public BaseProcessorList
{
protected:
    //Header of processor list.
    Processor* startProcessor;
	//Must follow startProcessor
	Processor* fromGateProcessor;
	//Must before endProcessor
	Processor* toGateProcessor;
    //Tail of processor list.
    Processor* endProcessor;

	Processor* baggageDeviceProcessor;
		
    //Processor list.
    ProcessorArray procList;

    //Number of processors in list.
    int m_procCount;


	Point* m_pServicePoints;
	Point* m_pQueue;	

	bool m_bDoOffset;
	DistanceUnit m_minX;
	DistanceUnit m_maxX;
	DistanceUnit m_minY;
	DistanceUnit m_maxY;

	ProcessorList* m_pDynamicCreatedProcsByTLOS;
	ProcessorList* m_pDynamicCreatedProcsPrototypes;
	//the map of processor's old index and new index ( in Processor )
	std::map<int,Processor*>m_mapProcListIdx;
	
	//the map of dynamicly created processor's old index and new index ( in Processor )
	std::map<int,Processor*>m_mapDynamicCreatedProcListIdx;

	bool m_bCanDynamicCreateProc;
	
	// following data is to avoid the same process in the sim loop
	ProcessorArray m_barrierList;    
    int m_nBarrierCount;
	
	ProcessorArray m_baggageProcList;
	ProcessorArray m_billBoardProcList;

	//Every node in the tree has a data which can tell you the group index of the ProcessorID of this node
	//so just search the tree , you can get the processorid's group, 
	//needless to travel all the proclist to find out its group index just like before.
	// the performance is improved considerable.
	CProcGroupIdxTree m_groupIdxTree;


	ProcessorID m_StartID;
	ProcessorID m_EndID;
	ProcessorID m_FromGateID;
	ProcessorID m_ToGateID;
	ProcessorID m_BaggageDeviceID;

protected:

	void DoOffset();

    // Determines entries in hierarchy of each processor's service time.
    void setServiceTimes (const ServiceTimeDatabase& serviceTimeDB);

    // Determines entries in hierarchy of each processor's pax flows.
	//initialize the processor's destination
	//paxFlowDB, comes from flow definition
	//pSubFlowList, comes from PROCESS definition
    void setPassengerFlows (const PassengerFlowDatabase& paxFlowDB, 
		const PassengerFlowDatabase& _gateDetailFlowDB, 
		const PassengerFlowDatabase& _stationPaxFlowDB,
		const CSubFlowList& pSubFlowList);

    // Determines entries in hierarchy of each processor's assignments.
    void setProcAssignments (const ProcAssignDatabase& assignDB);
	
	// copy dependent sink's roster to its source
	// discarded by Benny, 20091214
	void CopySinkAssignToSource();
		
	Processor* CopyProcessor( Processor* _pPrototype , InputTerminal* _pTerm)const;



public:
	//clear the proc dynamic created flag,
	void ClearAllProcDynamicFlag();

	
	void SetDynamicCreateProcFlag( bool _bFlag ){ m_bCanDynamicCreateProc = _bFlag;	}
	bool GetDynamicCreateProcFlag()const { return m_bCanDynamicCreateProc ;	}

	Processor* DynamicCreateProcByTLOS( const ProcessorID* _pPrototype , InputTerminal* _pTerm)const;
	ProcessorList* GetDynamicCreatedProcs() { return m_pDynamicCreatedProcsByTLOS;	}
	ProcessorList* GetDynamicCreatedProcsPrototype(){ return m_pDynamicCreatedProcsPrototypes;	}
	std::map<int,Processor*>&GetMapOfProcIdx() { return m_mapProcListIdx;	}
	std::map<int,Processor*>&GetMapOfDynamicCreatedProcIdx() { return m_mapDynamicCreatedProcListIdx;	}

	//used by dynamic create processor ( just like copy processor)
	ProcessorID* GetNextProcessorID(const ProcessorID* _pID )const;
	void InitDynamicCreateProcList( InputTerminal* _pTerm );
	void ResetAllTLOSFlag();

	// Determines entries in hierarchy of each processor's misc data.
    void setMiscData (const MiscDatabase& miscData);

	// processor in the proclist is the only data the engine will interfaced. need to refreshed before
	// the engine start.
	void RefreshRelatedDatabase();

	//because the conveyor architecture is designed in "proxy patten ", so we must init a real performer before
	// engine use conveyor.
	void InitEveryConveyor (const MiscDatabase& miscData);


	bool DidOffset(){ return m_bDoOffset; }

	int GetNumFloors();

    //Default constructor
    ProcessorList( StringDictionary* _pStrDict );
	ProcessorList( StringDictionary* _pStrDict, InputFiles fileType);
    
    virtual ~ProcessorList();

    //Adds processor to list.
    void addProcessor (Processor *proc, bool _bAutoRebuildGroupIdxTree = true );
	//add dynamic created processor into container ,only using for temply stored proc,this method don't change the proc's Index
	void addDynamicCreatedPrototypesProcessor(Processor *proc, bool _bAutoRebuildGroupIdxTree = true );
    //Returns processor at index ndx.
    Processor *getProcessor (int ndx) const;

    //Returns processor with full ID matching passed string.
    Processor *getProcessor (const char *procName) const;

	//Returns processor with ID;
	Processor *getProcessor(const ProcessorID& procID) const;

    //Returns processor count.
    int getProcessorCount (void) const { return m_procCount; };
	void setProcessorCount( int _iCount ){ m_procCount = _iCount;	}

    // Returns count of a specific processor type.
    int getProcessorsOfType (int procType) const;

    // Fills passed array with all processor of the correct type.
    void getProcessorsOfType (int procType, ProcessorArray& list);

    // Performs a binary search for an entry belonging to id.
    int findProcessor (const ProcessorID& id) const;

    // Removes processor from the list.
    void removeProcessor (const Processor *proc, bool _bAutoRebuildGroupIdxTree = true);

    // Removes processor from the list.
    Processor *removeProcessor (int index, bool _bAutoRebuildGroupIdxTree = true);

    // Deletes processor from the list. 
    void deleteProcessor (const ProcessorID& id, bool _bAutoRebuildGroupIdxTree = true);

    //It resets index members of all processors in list.
    void setIndexes (void);

    // Creates logical start and end processors.
    void addStartAndEndProcessors (void);

	// Create @from_gate and @to_gate processord
	void addFromAndToGateProcessors(void);

    //It fills passed Array with all processors in the passed group.
    //Each processor must be able to serve type and must lead to gateIndex.
    //The openFlag indicates whether or not to include closed processors.
    void buildArray (const ProcessorID *groupID, ProcessorArray *pProcArray,
					 const CMobileElemConstraint& type, int gateIndex, bool _bNeedCheckLeadGate, 
					 int bagIndex,bool _bNeedCheckLeadReclaim, int openFlag, const Processor *curProc,
					 ElapsedTime _currentTime,  CARCportEngine* pEngine, bool& _bRosterReason,int _iDestFloor, 
					 CPaxDestDiagnoseInfo* _pInfo,Person* pPerson, BOOL bInSubFlow=FALSE, std::vector<ALTObjectID> vAltLandsideSelectedProc = std::vector<ALTObjectID>() ) const;
 
    // Returns FALSE if a processor in the list has a matching id.
    int isUnique (const ProcessorID& id) const;

    //It adds each base group name to strDict.
    void getAllGroupNames (StringList& strDict, int type,BOOL bGateStandOnly=FALSE) const;

    //It adds each processor's full name to strDict.
    void getAllFullNames (StringDictionary& p_dict, int p_type = -1) const;

    //It adds name at 'level' of each group to strDict.
    //The procType specifies the processor class of interest.
    void getGroupNames (StringList& strDict, const GroupIndex& group, int level, int procType,BOOL bGateStandOnly=FALSE) const;

    // Adds all group member names of the correct type to strDict
    void getMemberNames (const ProcessorID& pID, StringList& strDict, int type,BOOL bGateStandOnly=FALSE) const;

    //It gets processor id string of base group. The input parameter "str" will hold the returned id.
    void getProcessorID (const GroupIndex& group, char *str) const;

    //It gets processor id string of processor at index. The input parameter "name" will hold the returned index.
    void getNameAt (int index, char* name) const;

    // search processor's group in the group index tree
    GroupIndex getGroupIndex (const ProcessorID& pID) const;

	// add by bill
	ProcessorFamily* getFamilyIndex(const ProcessorID& pID) const;

	// Binary search to any processor with matching ID and then seek backwards and forwards to get complete range.
    // The group index contains start and end index of any group of processors.
    // Note the special tests for START and END, plus ALL PROCESSORS.
	GroupIndex getGroupIndexOriginal (const ProcessorID& pID) const;

    // Returns ProcessorID with highest group designation that contains only the single processor passed.
    ProcessorID getHighestSingleGroup (const Processor *proc) const;

    // Scans processor list for passengers left in hold areas or queues.
    void clearPassengers (ElapsedTime curTime);
     
    void writeProcLog (void);
     
    void updateProcLog (void);

    // Removes all processors from the list
    virtual void clear (void);

    virtual void readData (ArctermFile& p_file);

	virtual void readObsoleteData ( ArctermFile& p_file );

    virtual void writeData (ArctermFile& p_file) const;

    virtual const char *getTitle (void) const { return "Processor Layout File"; }

    virtual const char *getHeaders (void) const { return "Name,Type,Location,,,In Constraint,,,Queue,,,Out Constraint,,,Fixed Q"; }

    // Reads one processor from a CSV file
    Processor *readProcessor (ArctermFile& procFile, bool _bFirst = false );
	//2.2 version
	Processor *readOldProcessorVer22 (ArctermFile& procFile, bool _bFirst = false );
    // Write miscellaneous data from CSV file
    int writeMiscDataFile (const char *filename) const;

    // Echoes the name of each processor to procEchoFile
    int echoProcessorNames (const char *procEchoFile) const;

	BOOL IsValidProcessorName(const char* name);

	BOOL RenameProcessor(Processor* old,const char* newName);

	void RefreshAllStationLocatonData();

	void GetInbridgeOfGate( int iGateIdx , std::vector<int>& _vInbridge ) const;
	void GetHoldingAraOfGate( int iGateIdx , std::vector<int>& _vHoldingArea ) const;

	// Clean all the data related to sim engine
	void CleanData();

	void InitAllBaggageProcData();

	void InitAllBillBoardProcData();
	void ClearGateAndReclaimData();

	int GetAssignedBaggageProcIdx( const CMobileElemConstraint& type )const;

	void InitAllProcDirection();

	// init all processor's available = isOpen = TRUE;
	void InitProcsOpenAvaiFlag();
	
	void GetLinkedWaitAreas( const Processor* _pProc, std::vector<Processor*>& _vWaitAreas );
	
	Processor* GetWaitArea( const ProcessorID* _pProcID );

	//find the nearest opened proc and let person go to that proc and wait for opening
	Processor* GetNearestOpenedProc(Processor *curProc,  const ProcessorID* _pProcGroupID, const ElapsedTime& _curTime, Person *_pPerson, ElapsedTime& _openTime ,bool& _resultType );//const CMobileElemConstraint& _paxType, ElapsedTime& _openTime ,bool& _resultType );
	
	//depend source processor
	Processor* GetNearestOpenedDependSourceProc(Processor *curProc,  const ProcessorID* _pProcGroupID, const ElapsedTime& _curTime, Person *_pPerson, ElapsedTime& _openTime ,bool& _resultType );//const CMobileElemConstraint& _paxType, ElapsedTime& _openTime ,bool& _resultType );

	bool IfAllBaggageProcDataValid();

	// init elevator's necessary data, such as every floor's height....etc
	bool InitAllElevatorData( std::vector<double>& _vFloorsAltitude ,Terminal* _pTerm );
	void FlushElevatorLog();
	void FlushBridgeLog();
	void FlushRetailProcess();

	//clear station moving train
	void ClearStationMovingTrain();
	
	void InitQueueTraceFlag();
	
	// build source / dest processor of every conveyor processor( or related with conveyor processor in pax flow )
	void InitProcessorSourceDestProcessors( CPaxFlowConvertor* _pConvertor, CAllPaxTypeFlow* _pAllFlow );
	
	// init processor's resource pool data
	void initResourcePoolData( void );		
	// add all dynamic created processors (TLOS) into current proclist, and update index
	void AddAllDynamicCreatedProcIntoList();

	// put the barriers list into _list
	void GetBarriersList( ProcessorArray& _list );

	void GetBaggateProcessorList(ProcessorArray& _list );

	void GetBillboardProcessorList(ProcessorArray& _list);

	// build the group index tree to prepare retrieve processor's group efficiently
	void BuildProcGroupIdxTree();
	int FindWorthWaitedProcs(const ProcessorID *groupID, const ElapsedTime& _curTime, const CMobileElemConstraint& paxType , int gateIndex);

	
	// get all processors by type
	virtual void GetProcessorsByType(std::vector<BaseProcessor*>& vProcs, int ProcType);

private:
	// init baggage proc's barrier and bag moving path if file version is old
	void InitOldBaggageProcData();
	bool CheckStairProcessor( const Processor* _pCurrentProc, const Processor* _pDestProc )const;

	bool CanLeadToLandsideObject(Processor *pProc,Person* pPerson, CARCportEngine* pEngine,ElapsedTime _currentTime, CString& strErrorInfo) const;
	bool CanLeadToIndividualConvergerObject(Processor *pProc,Person* pPerson, CARCportEngine* pEngine,ElapsedTime _currentTime, CString& strErrorInfo)const;
	bool CanLeadToTerminalProcessor(const Processor *curpProc,  Processor *pNextProc,std::vector<ALTObjectID> vAltLandsideSelectedProc, CARCportEngine* pEngine)const;
};


#endif































































