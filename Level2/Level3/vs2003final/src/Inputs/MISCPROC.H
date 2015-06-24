#ifndef MISC_PROCESSOR_DATA_ELEMENTS_H
#define MISC_PROCESSOR_DATA_ELEMENTS_H


#include "common\template.h"
#include "common\containr.h"
#include "Common\ALTObjectID.h"
#include "inputs\procdb.h"
#include "engine\conveyor.h"

#include "CapacityAttributes.h"
#include "PROCIDList.h"

#define USE_NOTHING				0
#define USE_PIPE_SYSTEM			1
#define USE_USER_SELECTED_PIPES	2

typedef std::vector<int>PIPES;

class INPUTS_TRANSFER MiscProcessorIDWithOne2OneFlag : public ProcessorID
{
private:
	bool m_bOne2OneFlag;
	// Route condition
	int m_iTypeOfUsingPipe;	// m_iTypeOfUsingPipe = 0 use nothing
							// m_iTypeOfUsingPipe = 1 use pipe system
							// m_iTypeOfUsingPipe = 2 use user selected pipes
	PIPES m_vUsedPipes;
public:
	// construct
	MiscProcessorIDWithOne2OneFlag() { m_bOne2OneFlag = false; m_iTypeOfUsingPipe = USE_NOTHING; }
	MiscProcessorIDWithOne2OneFlag( ProcessorID& entry, bool bFlag = false)
		:ProcessorID(entry)	{ m_bOne2OneFlag = bFlag; m_iTypeOfUsingPipe = USE_NOTHING; }
	MiscProcessorIDWithOne2OneFlag( MiscProcessorIDWithOne2OneFlag& entry ){ *this = entry; }
	virtual ~MiscProcessorIDWithOne2OneFlag() {};
	
	// get and set
	bool getOne2OneFlag() const { return m_bOne2OneFlag;}
	void setOne2OneFlag( bool _bFlag ) { m_bOne2OneFlag = _bFlag; }

	// Route condition
	void SetTypeOfUsingPipe( int _iType ) { m_iTypeOfUsingPipe = _iType; };
	int GetTypeOfUsingPipe() const { return m_iTypeOfUsingPipe;	};

	void AddUsedPipe(int _iIdxOfPipe ){ m_vUsedPipes .push_back( _iIdxOfPipe );};
	void ClearAllPipe(){ m_vUsedPipes.clear();};
	bool IfUseThisPipe(int nCurPipeIndex)const;
	PIPES& GetPipeVector() { return m_vUsedPipes;};
	int GetUsedPipeCount() const { return m_vUsedPipes.size();	};
	int GetPipeAt( int _iIdx ) const ;

	// read and write
	int readData (ArctermFile& file);
    int writeData (ArctermFile& file) const;
};




class INPUTS_TRANSFER MiscProcessorIDList : public ProcessorIDList
{
public:
	MiscProcessorIDList(){};
	virtual ~MiscProcessorIDList() {};

	// read and wite 
	virtual void readIDList (ArctermFile& p_file, InputTerminal* _pInTerm ,bool _bWithOneToOne = true );
	virtual void writeIDList (ArctermFile& p_file) const;
private:
	void readIDListWithPipe(char* pBuf, MiscProcessorIDWithOne2OneFlag* procId);

};
//////////////////////////////////////////////////////////////////////////
class INPUTS_TRANSFER MiscProcessorIDWithLevel : public ProcessorID
{
private:
	int  m_level;
	
public:
	// construct
	MiscProcessorIDWithLevel() { m_level = 1;}
	MiscProcessorIDWithLevel( ProcessorID& entry, bool bFlag = false):ProcessorID(entry)	{ m_level = bFlag; }
	MiscProcessorIDWithLevel( MiscProcessorIDWithLevel& entry ){ *this = entry; }
	virtual ~MiscProcessorIDWithLevel() {};
	
	
	// get and set
	int GetLevel() const { return m_level;}
	void SetLevel( int nLevel ) { m_level = nLevel;	}
	
};
//////////////////////////////////////////////////////////////////////////

class INPUTS_TRANSFER MiscProcessorIDListWithLevel : public ProcessorIDList
{
public:
	MiscProcessorIDListWithLevel(){};
	virtual ~MiscProcessorIDListWithLevel() {};
	
	// read and wite 
	virtual void readIDList (ArctermFile& p_file, InputTerminal* _pInTerm ,bool _bWithOneToOne = true );
    virtual void writeIDList (ArctermFile& p_file) const;
private:
	void readObsoleteIDList24 (ArctermFile& p_file, InputTerminal* _pInTerm,bool _bWithOneToOne );
		
};

//////////////////////////////////////////////////////////////////////////
class INPUTS_TRANSFER MiscData
{
protected:

	bool m_bDisallowGroup;
    int visitorFlag;
    ProcessorIDList *gateList;
	ALTObjectIDList *curbSideList;//add by Colin 2011/4/15  be used when define parkinglot behavior
	ALTObjectIDList *curbSideListAdd;
	ALTObjectIDList *curbSideListDelete;
	MiscProcessorIDList *m_pLinkedDestList;
	InputTerminal* m_pInTerm;
	long m_lQueueCapacity;
	long m_lTerminateTime;
	int m_nCyclicFreq;
	MiscProcessorIDList* m_pWaitAreaList;		// add by bird 2003/7/15  for each processor , can assign wait area processor or line processor
	int m_nGroupSerTimeModIndex;
	int m_nGroupSerTimeModSum;
	bool m_bWaitInQueue; // tag of Wait In Queue To Open

	CapacityAttributes *m_pCapacityAttributes;

	bool m_bUseDependentProc;
	MiscProcessorIDList* m_pDependentProcList;
	MiscProcessorIDList* m_pBridgeConnectorList;



public:
	void SetInputTerminal( InputTerminal* _pInTerm ){ m_pInTerm = _pInTerm; }

    MiscData ();
    MiscData (const MiscData& entry) 
	{ 
		*gateList = *(entry.gateList);
		*curbSideList = *(entry.curbSideList);
		*curbSideListAdd = *(entry.curbSideListAdd);
		*curbSideListDelete = *(entry.curbSideListDelete);
        visitorFlag = entry.visitorFlag; 
		m_bWaitInQueue = entry.m_bWaitInQueue;
		m_pInTerm = NULL; 
		*m_pWaitAreaList = *( entry.m_pWaitAreaList);
		*m_pLinkedDestList = *(entry.m_pLinkedDestList);
		m_nGroupSerTimeModIndex = 0;
		m_nGroupSerTimeModSum = 100;

		m_bUseDependentProc = entry.m_bUseDependentProc;
		*m_pDependentProcList = *(entry.m_pDependentProcList);
		*m_pBridgeConnectorList = *(entry.m_pBridgeConnectorList);
	}
    virtual ~MiscData () 
	{ 
		delete gateList; gateList = NULL; 
		delete curbSideList; curbSideList=NULL;
		delete curbSideListAdd; curbSideListAdd=NULL;
		delete curbSideListDelete; curbSideListDelete=NULL;
		delete m_pWaitAreaList; m_pWaitAreaList = NULL;
		delete m_pLinkedDestList; m_pLinkedDestList=NULL;
		delete m_pCapacityAttributes; m_pCapacityAttributes = NULL;
		delete m_pDependentProcList; m_pDependentProcList = NULL;
		delete m_pBridgeConnectorList; m_pBridgeConnectorList = NULL;
	}

	bool GetDisallowGroupFlag() const{ return m_bDisallowGroup; }
	void SetDisallowGroupFlag( bool _bDisallowGroup ){ m_bDisallowGroup = _bDisallowGroup; }
    
	bool GetWaitInQueue() const{return m_bWaitInQueue;}
	void SetWaitInQueue(bool _bBool){m_bWaitInQueue = _bBool;}

	CapacityAttributes* GetCapacityAttributes( void ) const {	return m_pCapacityAttributes; }
	void SetQueueCapacity(long _iNewCapacity);
	long GetQueueCapacity() const;

	void SetTerminateTime(long _iNewTerminateTime);
	long GetTerminateTime() const;

	void SetGroupSerTimeModIndex(int _Index ){ m_nGroupSerTimeModIndex = _Index; };
	int  GetGroupSerTimeModIndex() const { return m_nGroupSerTimeModIndex; };

	void SetGroupSerTimeModSum(int _SumPercent ){ m_nGroupSerTimeModSum = _SumPercent; }
	int  GetGroupSerTimeModSum() const { return m_nGroupSerTimeModSum; }
	
	void SetCyclicFreq( int _nCyclicFreq );
	int GetCyclicFreq() const;

    void setVisitors (int flag) { visitorFlag = flag; }
    int allowsVisitors (void) const { return visitorFlag; }

    void setGateList (const ProcessorIDList *p_ids) { *gateList = *p_ids; }
	void setLinkedDestList(const MiscProcessorIDList *p_ids) { *m_pLinkedDestList = *p_ids ;}
    ProcessorIDList *getGateList (void) const { return gateList; }
	MiscProcessorIDList * getLinkedDestList(void) const { return m_pLinkedDestList;} 

// 	void setCurbsideList(const ALTObjectIDList *p_ids){*curbSideList = *p_ids;}
// 	ALTObjectIDList *getCurbSideList() const {return curbSideList;}

	
	void setWaitAreaList( const MiscProcessorIDList* p_ids ) { *m_pWaitAreaList = *p_ids;}
	MiscProcessorIDList *getWaitAreaList( void ) { return m_pWaitAreaList;	}

    static char *getProcTypeName (int type) { return SHORT_PROC_NAMES[type]; }
    const char *getProcTypeName (void) const { return SHORT_PROC_NAMES[getType()]; }
    virtual int getType (void) const { return PointProc; }

	MiscProcessorIDList * getDependentProcList(void) const{ return m_pDependentProcList; }
	void setDependentProcList(const MiscProcessorIDList* p_ids){ *m_pDependentProcList = *p_ids; }
	bool getUseDependentProcFlag() const{ return m_bUseDependentProc; }
	void setUseDependentProcFlag(bool bUseDependentProc){ m_bUseDependentProc = bUseDependentProc; } 

	void setBridgeConnectorList(const MiscProcessorIDList *p_ids) { *m_pBridgeConnectorList = *p_ids ;}
	MiscProcessorIDList * getBridgeConnectorList(){ return m_pBridgeConnectorList; }

    // returns 1 if ID list is blanked by removal
    virtual int removeID (const ProcessorID& id)
    { 
		if (gateList) gateList->remove (id,m_pInTerm); 
		if (m_pWaitAreaList) m_pWaitAreaList->remove(id,m_pInTerm);
		if (m_pLinkedDestList) m_pLinkedDestList->remove(id,m_pInTerm);
		return 0; 
	}
    virtual void replaceID (const ProcessorID& p_old, const ProcessorID& p_new)
    { 
		if (gateList) gateList->replace (p_old, p_new,m_pInTerm); 
		if (m_pWaitAreaList) m_pWaitAreaList->replace( p_old, p_new,m_pInTerm);
		if (m_pLinkedDestList) m_pLinkedDestList->replace(p_old, p_new,m_pInTerm);
	}

    void readMiscData (ArctermFile& p_file);
	void readObsoleteMiscData(ArctermFile& p_file);
    void writeMiscData (ArctermFile& p_file) const;

    virtual void readSpecificData ( ArctermFile& p_file ) {};
    virtual void writeSpecificData (ArctermFile& p_file) const {};

	virtual bool IfUsedThisProcssor( const ProcessorID& _procID ,InputTerminal*_pTerm,std::vector<CString>& _stringVec,const ProcessorID& _entryID)const;


	//add by Colin,2011/4/21
	void readParkingLotBehaviorDB(int p_parentID);//
	void readCurbSideListDB(int p_parentID);
	int getCurbSideCount();
	void saveDataToDB(int parentID);
	void readDataFromDB(int parentID);
	void saveCurbSideListToDB(int parentID);
	void readCurbSideListFromDB(int parentID);
	void addCurbSide(const ALTObjectID &p_item);
	void delCurbSide(int nIndex);
	bool existCurbSide(const ALTObjectID &p_item);
	ALTObjectID &getCurbSide(int nIndex)const;
private:
	void readObsoleteMiscData24 (ArctermFile& p_file);
	void readObsoleteMiscData25 (ArctermFile& p_file);
	void readObsoleteMiscData26(ArctermFile& p_file);
    void readObsoleteMiscData271(ArctermFile&p_file);
    void readObsoleteMiscData28(ArctermFile&p_file);
	void readObsoleteMiscData29(ArctermFile&p_file);
	void readObsoleteMiscData30(ArctermFile&p_file);
	void readObsoleteMiscData31(ArctermFile&p_file);
	void readObsoleteMiscData32(ArctermFile&p_file);
	void readObsoleteMiscData33(ArctermFile&p_file);
	void readObsoleteMiscData34(ArctermFile&p_file);
	void readObsoleteMiscData35(ArctermFile&p_file);
	void readObsoleteMiscData37(ArctermFile&p_file);
	void ConvertCapacityIssueIfNeed(float fFileVersion);
	//void readObsoleteMiscData32(ArctermFile&p_file)/*3.2 */;



};

class INPUTS_TRANSFER MiscLineData : public MiscData
{
public:
    virtual int getType (void) const { return LineProc; }
};

class INPUTS_TRANSFER MiscDependentData : public MiscData
{
protected:
    ProcessorIDList *dependentList;

public:
    MiscDependentData() { dependentList = CreateDependentList(); dependentList->OwnsElements( 1 ); }
    virtual ~MiscDependentData() { delete dependentList; }

    void setDependentList (const ProcessorIDList *p_ids) { *dependentList = *p_ids; }
    ProcessorIDList *getDependentList (void) const { return dependentList; }

	virtual ProcessorIDList * CreateDependentList(void)
	{
		dependentList = new MiscProcessorIDList;
		return dependentList;
	} 
	
    virtual int removeID (const ProcessorID& id)
    {
        if (dependentList) dependentList->remove (id,m_pInTerm);
        return MiscData::removeID(id);
    }

    virtual void replaceID (const ProcessorID& p_old, const ProcessorID& p_new)
    { 
        if (dependentList) dependentList->replace (p_old, p_new,m_pInTerm); 
		MiscData::replaceID(p_old,p_new);
	}

    virtual void readSpecificData (ArctermFile& p_file);
    virtual void writeSpecificData (ArctermFile& p_file) const;
	virtual bool IfUsedThisProcssor( const ProcessorID& _procID ,InputTerminal*_pTerm,std::vector<CString>& _stringVec,const ProcessorID& _entryID)const;
		
};

class INPUTS_TRANSFER MiscDepSourceData : public MiscDependentData
{
public:
    virtual ~MiscDepSourceData() {};
    virtual int getType (void) const { return DepSourceProc; }
};

class INPUTS_TRANSFER MiscDepSinkData : public MiscDependentData
{
public:
	MiscDepSinkData() { dependentList = CreateDependentList();}
    virtual ~MiscDepSinkData() {};
    virtual int getType (void) const { return DepSinkProc; }
    virtual void readSpecificData (ArctermFile& p_file);
	virtual ProcessorIDList * CreateDependentList(void)
	{
		dependentList = new MiscProcessorIDListWithLevel;
		return dependentList;	
	}
	
};


class INPUTS_TRANSFER MiscGateData : public MiscData
{
protected:
    ProcessorIDList *holdAreaList;

public:
    MiscGateData() { holdAreaList = new MiscProcessorIDList; holdAreaList->ownsElements( 1 ); }
    virtual ~MiscGateData() { delete holdAreaList; }


    void setHoldAreaList (const ProcessorIDList *p_ids) { *holdAreaList = *p_ids; }
    ProcessorIDList *getHoldAreaList (void) const { return holdAreaList; }
	virtual bool IfUsedThisProcssor( const ProcessorID& _procID ,InputTerminal*_pTerm,std::vector<CString>& _stringVec,const ProcessorID& _entryID)const;

    virtual int removeID (const ProcessorID& id)
    {
        if (holdAreaList) holdAreaList->remove (id,m_pInTerm);
        return MiscData::removeID(id);
    }
    virtual void replaceID (const ProcessorID& p_old, const ProcessorID& p_new)
    { 
        if (holdAreaList) holdAreaList->replace (p_old, p_new,m_pInTerm); 
		MiscData::replaceID(p_old,p_new);
	}

    virtual int getType (void) const { return GateProc; }
    virtual void readSpecificData (ArctermFile& p_file);
    virtual void writeSpecificData (ArctermFile& p_file) const;
};

class INPUTS_TRANSFER MiscFloorChangeData : public MiscData
{
protected:
	ProcessorIDList *connectionList;

public:
    MiscFloorChangeData() { connectionList = new MiscProcessorIDList; connectionList->ownsElements( 1 );}
    virtual ~MiscFloorChangeData() { delete connectionList; }


    void setConnectionList (const ProcessorIDList *p_ids) { *connectionList = *p_ids; }
    ProcessorIDList *getConnections (void) const { return connectionList; }

    virtual int removeID (const ProcessorID& id)
    {
        if (connectionList) connectionList->remove (id,m_pInTerm);
        return MiscData::removeID(id);
    }
    virtual void replaceID (const ProcessorID& p_old, const ProcessorID& p_new)
    { 
        if (connectionList) connectionList->replace (p_old, p_new,m_pInTerm); 
		MiscData::replaceID(p_old, p_new);
	}

    virtual int getType (void) const { return FloorChangeProc; }
    virtual void readSpecificData (ArctermFile& p_file);
    virtual void writeSpecificData (ArctermFile& p_file) const;
	virtual bool IfUsedThisProcssor( const ProcessorID& _procID ,InputTerminal*_pTerm,std::vector<CString>& _stringVec,const ProcessorID& _entryID)const;
};

class INPUTS_TRANSFER MiscHoldAreaData : public MiscData
{
protected:
    float radius;
    ProcessorIDList *sourceProcList;
    ProcessorIDList *destProcList;
	int m_nStageNumber;			// 0 for no stage, 1 - N is the range of stage number,
								// 1 is the closest Holding area to the gate.
	bool m_bApplyServiceTimeAfterGreeting;		// if yes, use service time.

	//if the capacity is full, the hold area will service the passenger in the hold
	// need to define the capacity, if no limit, error message will show up and stop engin
	//

	//the flag marks the service after full feature
	bool m_bCorralService;
	//wait the time since the last passenger entry, corral processor start service the passenger 
	//the time is seconds
	int  m_nTimeToService;

public:
    MiscHoldAreaData() ;
    virtual ~MiscHoldAreaData() { delete sourceProcList;
        delete destProcList; }


	void SetStageNumber( int _nStageNumber ){ m_nStageNumber = _nStageNumber; }
	int GetStageNumber(){ return m_nStageNumber; }

    void setRadius (float rad) { radius = rad; }
    float getRadius (void) const { return radius; }

    void setSourceList (const ProcessorIDList *p_ids) { *sourceProcList = *p_ids; }
    ProcessorIDList *getSourceList (void) const { return sourceProcList; }

    void setDestList (const ProcessorIDList *p_ids) { *destProcList = *p_ids; }
    ProcessorIDList *getDestList (void) const { return destProcList; }

	void setApplyServiceTimeAfterGreeting( bool _bApplyServiceTimeAfterGreeting ){ m_bApplyServiceTimeAfterGreeting = _bApplyServiceTimeAfterGreeting; }
	bool getApplyServiceTimeAfterGreeting(){ return m_bApplyServiceTimeAfterGreeting; }
	
	bool IsCorralService() const { return m_bCorralService; }
	void setCorralService(bool bEnable) { m_bCorralService = bEnable; }

	int getTimeToService() const { return m_nTimeToService; }
	void setTimeToService(int nCount) { m_nTimeToService = nCount; }


    virtual int removeID (const ProcessorID& id)
    {
        if (sourceProcList) sourceProcList->remove (id,m_pInTerm);
        if (destProcList) destProcList->remove (id,m_pInTerm);
        return MiscData::removeID(id);
    }
    virtual void replaceID (const ProcessorID& p_old, const ProcessorID& p_new)
    { 
        if (sourceProcList) sourceProcList->replace (p_old, p_new,m_pInTerm);
        if (destProcList) destProcList->replace (p_old, p_new,m_pInTerm); 
		MiscData::replaceID(p_old, p_new);
	}

    virtual int getType (void) const { return HoldAreaProc; }
    virtual void readSpecificData (ArctermFile& p_file);
    virtual void writeSpecificData (ArctermFile& p_file) const;
	virtual bool IfUsedThisProcssor( const ProcessorID& _procID,InputTerminal*_pTerm,std::vector<CString>& _stringVec,const ProcessorID& _entryID)const;
};

class INPUTS_TRANSFER MiscBaggageData : public MiscData
{
protected:
    int capacity;
    ProcessorIDList *bypassProcessor;
    ProcessorIDList *m_pHoldAreaList;

public:
    MiscBaggageData();
    virtual ~MiscBaggageData() { delete bypassProcessor;delete m_pHoldAreaList;}


    void setCapacity (int maxBags) { capacity = maxBags; }
    int getCapacity (void) const { return capacity; }

    void setBypassList (const ProcessorIDList *p_ids) { *bypassProcessor = *p_ids; }
    ProcessorIDList *getBypassList (void) const { return bypassProcessor; }
	ProcessorIDList *getHoldAreaList (void) const { return m_pHoldAreaList; }	
	virtual bool IfUsedThisProcssor( const ProcessorID& _procID ,InputTerminal*_pTerm,std::vector<CString>& _stringVec,const ProcessorID& _entryID)const;

    virtual int removeID (const ProcessorID& id)
    {
        if (bypassProcessor) bypassProcessor->remove (id,m_pInTerm);
        return MiscData::removeID(id);
    }
    virtual void replaceID (const ProcessorID& p_old, const ProcessorID& p_new)
    { 
        if (bypassProcessor) bypassProcessor->replace (p_old, p_new,m_pInTerm); 
		MiscData::replaceID(p_old, p_new);
	}

    virtual int getType (void) const { return BaggageProc; }
    virtual void readSpecificData (ArctermFile& p_file);
    virtual void writeSpecificData (ArctermFile& p_file) const;
};

class INPUTS_TRANSFER MiscMovingSideWalkData : public MiscData
{
public:
    virtual int getType (void) const { return MovingSideWalkProc; }
};

class INPUTS_TRANSFER MiscConveyorData : public MiscData
{
	SUBCONVEYORTYPE m_enumSubType;
	ElapsedTime m_serviceTime;			// for scanner
	ElapsedTime m_randomTime;			// for pusher
	ElapsedTime m_scheduleTime;			// for pusher
	DistanceUnit m_dSpeed;

	ProcessorIDList *m_pExitBeltList;	// for sorter

	bool m_bPusherReleasedByFull;
	bool m_bPusherReleasedByNextAvail;
	bool m_bPusherReleasedByMaxTime;
	bool m_bPusherReleasedBySchedPick;
	
	bool m_bReversible;

public:
	static const double DefaultSpeed;
	
	MiscConveyorData();
	virtual ~MiscConveyorData();

	void setExitBeltList (const ProcessorIDList *p_ids) { *m_pExitBeltList = *p_ids; }
    ProcessorIDList *getExitBeltList (void) const { return m_pExitBeltList; }	

    virtual int getType (void) const { return ConveyorProc; }
	void SetSubType( SUBCONVEYORTYPE _enumType ) { m_enumSubType = _enumType;	}
	SUBCONVEYORTYPE GetSubType()const { return m_enumSubType;	}
	
	void SetServiceTime( ElapsedTime _serviceTime ){ m_serviceTime = _serviceTime; }
	ElapsedTime GetServiceTime()const{ return m_serviceTime; }
	void SetRandomAmountTime( ElapsedTime _randomTime ){ m_randomTime = _randomTime; }
	ElapsedTime GetRandomAmountTime()const{ return m_randomTime; }
	void SetScheduleTime( ElapsedTime _scheduleTime ){ m_scheduleTime = _scheduleTime; }
	ElapsedTime GetScheduleTime()const{ return m_scheduleTime; }

	void SetSpeed( DistanceUnit _dSpeed ){ m_dSpeed = _dSpeed; }
	DistanceUnit GetSpeed()const{ return m_dSpeed; }

	void SetPusherReleasedByFull( bool _bPusherReleasedByFull ){ m_bPusherReleasedByFull = _bPusherReleasedByFull; }
	void SetPusherReleasedByNextAvail( bool _bPusherReleasedByNextAvail ){ m_bPusherReleasedByNextAvail = _bPusherReleasedByNextAvail; }
	void SetPusherReleasedByMaxTime( bool _bPusherReleasedByMaxTime ){ m_bPusherReleasedByMaxTime = _bPusherReleasedByMaxTime; }
	void SetPusherReleasedBySchedPick( bool _bPusherReleasedBySchedPick ){ m_bPusherReleasedBySchedPick = _bPusherReleasedBySchedPick; }

	bool GetPusherReleasedByFull()const{ return m_bPusherReleasedByFull; }
	bool GetPusherReleasedByNextAvail()const{ return m_bPusherReleasedByNextAvail; }
	bool GetPusherReleasedByMaxTime()const{ return m_bPusherReleasedByMaxTime; }
	bool GetPusherReleasedBySchedPick()const{ return m_bPusherReleasedBySchedPick; }

	void SetReversible( bool _bReversible ){ m_bReversible = _bReversible; }
	bool GetReversible(){ return m_bReversible; }

    virtual void readSpecificData (ArctermFile& p_file);
    virtual void writeSpecificData (ArctermFile& p_file) const;
	virtual bool IfUsedThisProcssor( const ProcessorID& _procID ,InputTerminal*_pTerm,std::vector<CString>& _stringVec,const ProcessorID& _entryID)const;
	

};


class INPUTS_TRANSFER MiscStairData : public MiscData
{
public:
    virtual int getType (void) const { return StairProc; }
};

class INPUTS_TRANSFER MiscEscalatorData : public MiscData
{
protected:
	double m_dMovingSpeed;
	int m_nCapacity;	

public:
	MiscEscalatorData(){ m_dMovingSpeed = 1.0; m_nCapacity = 50; }
    virtual int getType (void) const { return EscalatorProc; }
	virtual void readSpecificData (ArctermFile& p_file);
    virtual void writeSpecificData (ArctermFile& p_file) const;
	void SetCapacity(int _nCapacity){ m_nCapacity = _nCapacity; }
	void SetMovingSpeed(double _dMovingSpeed){ m_dMovingSpeed = _dMovingSpeed; }
	double GetCapacity(){ return m_nCapacity; }
	double GetMovingSpeed(){ return m_dMovingSpeed;}	
	
};

class INPUTS_TRANSFER MiscIntegratedStation : public MiscData
{
protected:
	ProcessorIDList m_listStation;
public:
	MiscIntegratedStation(){};
	virtual ~MiscIntegratedStation(){};
	virtual void readSpecificData (ArctermFile& p_file);
    virtual void writeSpecificData (ArctermFile& p_file) const;
	virtual int getType (void) const { return IntegratedStationProc; }
	
	ProcessorIDList* GetStationList(){return &m_listStation;};
};

//Added By Tim In 8/20/2003
class INPUTS_TRANSFER MiscElevatorData:public MiscData
{
protected:
	int m_nCapacity;
	std::vector<BOOL> m_vbStopAtFloor;
	double m_dSpeed[3];
public:
	enum ElevatorSpeedType
	{
		EST_ACCELERATION=0,
			EST_DECELERATION,
			EST_NORMAL,
	};
	MiscElevatorData(){m_nCapacity=20;m_dSpeed[0]=1.0;m_dSpeed[1]=2.0;m_dSpeed[2]=1.0;}
	
    void setCapacity (int maxBags) { m_nCapacity = maxBags; }
    int getCapacity (void) const { return m_nCapacity; }

	void setStopAtFloor(int nIndex,BOOL bValue)	
	{
		ASSERT(nIndex>=0&&(unsigned)nIndex<m_vbStopAtFloor.size());
		m_vbStopAtFloor[nIndex]=bValue;
	}
	BOOL getStopAtFloor(int nIndex)
	{
		ASSERT(nIndex>=0&&(unsigned)nIndex<m_vbStopAtFloor.size());
		return m_vbStopAtFloor[nIndex];
	}
	void clearStopAtFloor()
	{
		m_vbStopAtFloor.clear();
	}
	void addStopAtFloor(BOOL bValue)
	{
		m_vbStopAtFloor.push_back(bValue);
	}
	int getStopAfFloorCount()
	{
		return m_vbStopAtFloor.size();
	}
	void AddStopAtFloorItem(BOOL bValue){m_vbStopAtFloor.push_back(bValue);}
	void setSpeed(ElevatorSpeedType est,double dSpeed){m_dSpeed[est]=dSpeed;}
	double getSpeed(ElevatorSpeedType est){return m_dSpeed[est];}
	virtual int getType(void) const {return Elevator;}
	virtual void readSpecificData (ArctermFile& p_file);

    virtual void writeSpecificData (ArctermFile& p_file) const;
//	{
//		p_file.writeInt (m_nCapacity);
//		p_file.writeFloat(m_dSpeed[0]);		
//		p_file.writeFloat(m_dSpeed[1]);
//		p_file.writeFloat(m_dSpeed[2]);
//		
//	}

	
};

class CPaxTypeCoefficientItem
{
public:
	CPaxTypeCoefficientItem()
	{
		m_nBuyCoefficient = 50;
		m_nPropensityCoefficient = 50;
	}
	~CPaxTypeCoefficientItem(){};

	//member Function
	CMobileElemConstraint getPaxType()const{return m_paxType;}
	void setPaxType(const CMobileElemConstraint& paxType){m_paxType = paxType; }

	int getPropensityCoefficient() const{return m_nPropensityCoefficient;}
	void setPropernsityCoefficient(int nCoefficient){m_nPropensityCoefficient =nCoefficient; }

	int getBuyCoefficient()const{return m_nBuyCoefficient;}
	void setBuyCoefficient(int nCoefficient){m_nBuyCoefficient =nCoefficient; }
protected:

	CMobileElemConstraint m_paxType;
	int m_nPropensityCoefficient;
	int m_nBuyCoefficient;

};


class INPUTS_TRANSFER MiscBillboardData:public MiscData
{
public:
	MiscBillboardData();
	MiscBillboardData(const MiscBillboardData& anothermiscData);
	virtual ~MiscBillboardData();
protected:
	int m_nTimeRemaining;
	int m_nTimeStop;
	ProcessorIDList *m_pBillBoardLinkedProcList;
	std::vector<CPaxTypeCoefficientItem> m_vPaxTypeCoefficient;
	CPaxTypeCoefficientItem m_defaultPaxTypeCoefficient;
private:

public:
	virtual int getType(void) const {return BillboardProcessor;}
	
	virtual void readSpecificData (ArctermFile& p_file);
	virtual void writeSpecificData (ArctermFile& p_file) const;

	int getTimeRemaining() const{ return m_nTimeRemaining;}
	void setTimeRemaining(int timeRemaining){m_nTimeRemaining = timeRemaining;}

	int getTimeStop() const{ return m_nTimeStop;}
	void setTimeStop(int timeStop){m_nTimeStop = timeStop;}
	//double getBillBoardCoefficient() const {return m_dBillBoardCoefficient;}
	//void setBillBoardCoeffcient(double dCoefficient){m_dBillBoardCoefficient= dCoefficient;}

	CPaxTypeCoefficientItem& GetDafaultCoefficient(){return m_defaultPaxTypeCoefficient;}
	
	//linked processor
	ProcessorIDList *GetBillBoardLinkedProcList(){return m_pBillBoardLinkedProcList;}
	void SetBillBoardLinkedProcList(const ProcessorIDList *p_ids) { *m_pBillBoardLinkedProcList = *p_ids; }

	std::vector<CPaxTypeCoefficientItem>& getPaxTypeCoefficient(){return m_vPaxTypeCoefficient;}

	bool FindCoefficient(const CMobileElemConstraint& paxType,int& nPropensityCoefficient,int& nBuyCoefficient );
};

class INPUTS_TRANSFER MiscBridgeConnectorData : public MiscData
{
public:
	MiscBridgeConnectorData();
	MiscBridgeConnectorData(const MiscBridgeConnectorData& anothermiscData);
	~MiscBridgeConnectorData();
protected:
	MiscProcessorIDList* m_pStandLinkedProcList;
public:
	virtual int getType(void) const {return BridgeConnectorProc;}
	virtual void readSpecificData (ArctermFile& p_file);
	virtual void writeSpecificData (ArctermFile& p_file) const;

	MiscProcessorIDList *GetBridgeConnectorLinkedProcList(){return m_pStandLinkedProcList;}
	void SetBridgeConnectorLinkedProcList(const MiscProcessorIDList *p_ids) { *m_pStandLinkedProcList = *p_ids; }
};


class INPUTS_TRANSFER MiscRetailProcData : public MiscData
{

public:
	class PaxSkipCondition
	{
		friend class MiscRetailProcData;
	public:
		PaxSkipCondition()
		{	
			m_bQLimit = true;
			m_bLimitMins = true;

			m_nMins = 45;
			m_nQlenth = 5;
		}
		~PaxSkipCondition(){}

	public:
		const CMobileElemConstraint& getConstraint() const {return m_paxType;}
		void setConstraint(CMobileElemConstraint& paxCons){ m_paxType = paxCons;}

		bool hasTimeLimit() const{return m_bLimitMins;}
		void setTimeLimit(bool bLimit){m_bLimitMins = bLimit;}

		void setMins(int nMins) {m_nMins = nMins;}
		int getMins() const{return m_nMins;}

		bool hasQLimit() const{return m_bQLimit;}
		void setQLimit(bool bLimit){m_bQLimit = bLimit;}

		int getQlenth() const { return m_nQlenth; }
		void setQlenth(int nLen) { m_nQlenth = nLen; }

	protected:
		CMobileElemConstraint	m_paxType;
		bool					m_bLimitMins;
		int						m_nMins;

		bool					m_bQLimit;
		int						m_nQlenth;

	private:
	};

public:
	MiscRetailProcData();
	MiscRetailProcData(const MiscRetailProcData& anothermiscData);
	~MiscRetailProcData();

protected:	
	ProcessorIDList				m_bypassProcList;
	ProcessorIDList				m_checkoutProcList;
	std::vector<PaxSkipCondition *>	m_vConditions;

public:
	ProcessorIDList* getBypassProcList() { return &m_bypassProcList; }
	//void addBypassProcID(ProcessorID& procID) { m_bypassProcID = procID; }

	ProcessorIDList* getCheckoutProcList() { return &m_checkoutProcList; }


	int getConditionCount()
	{
		return static_cast<int>(m_vConditions.size());
	}
	
	PaxSkipCondition *getCondition(int nIndex)
	{
		if( nIndex >= 0 && nIndex < static_cast<int>(m_vConditions.size()) )
			return m_vConditions[nIndex];
		else
			return NULL;
	}

	void addCondition(PaxSkipCondition *pCondition)
	{
		m_vConditions.push_back(pCondition);
	}

	void delCondition(PaxSkipCondition *pCondition)
	{
		std::vector<PaxSkipCondition *>::iterator iter = std::find(m_vConditions.begin(),m_vConditions.end(), pCondition);
		if(m_vConditions.end() != iter)
		{
			m_vConditions.erase(iter);
			delete pCondition;
		}
	}

	bool PaxSkipVacant(const CMobileElemConstraint& type,int nLength,const ElapsedTime& eTime )const;
	bool PaxQueueLenghtVacant(const CMobileElemConstraint& type,int nLength)const;

	//based on passenger type
	void SortSkipCondition();

protected:
	static bool CompareSkipCondition(const PaxSkipCondition*pC1, const PaxSkipCondition* pC2);

public:
	virtual int getType(void) const {return RetailProc;}
	virtual void readSpecificData (ArctermFile& p_file);
	virtual void writeSpecificData (ArctermFile& p_file) const;

};






#endif
