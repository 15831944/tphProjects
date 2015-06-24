#ifndef PROCESSOR_ASSIGNMENT_LIST_H
#define PROCESSOR_ASSIGNMENT_LIST_H

#include "common\template.h"
#include "common\containr.h"
#include "common\elaptime.h"
#include "inputs\MultiMobConstraint.h"
enum ProcAssRelation { Inclusive = 0, Exclusive, Not };


class ProcessorRoster
{
protected:
	//0: closed
	//1: user defined events
	//2: generate from flight schedule events
	int iFlag;						//<<<<<<o>>>>>>
	//if iFlag =1: openTime is absolute time;
	//if iFlag =2: openTime is relative time;
	ElapsedTime openTime;
	ElapsedTime closeTime;

	CMultiMobConstraint assign;		//<<<<<<o>>>>>>
	InputTerminal* m_pInTerm;		//<<<<<<o>>>>>>
	////////////////////////////////////////////////////
	// absXXXXTime use to save the absolute time of a events,
	// need not save the value to dataset
	ElapsedTime absOpenTime;		//<<<<<<o>>>>>>
	ElapsedTime absCloseTime;

	// processor assignment relation
	ProcAssRelation assRelation;

//	int m_bWithTimeConstraint;
	BOOL m_isDaily ;

	int m_ID ;
public:
	BOOL  IsDaily() {return m_isDaily ;} 
	void  IsDaily(BOOL res) { m_isDaily = res ;} 

	int GetID() { return m_ID ;} ;
	void SetID(int _id) { m_ID = _id ;} ;
public:
	ProcessorRoster() 
	{
		m_ID = -1 ; // used for DB 
		iFlag			= 1;
		openTime		= 0l;
		closeTime		= 0l; //24*60*60l;
		m_pInTerm		= NULL;
			
		absOpenTime		= 0l;
		absCloseTime	= 0l; //24*60*60l;

		assRelation		= Inclusive;
		m_isDaily =  TRUE ;
//		m_bWithTimeConstraint = 0;
	}

    ProcessorRoster ( InputTerminal* _pInTerm ) 
	{ 
		m_ID = -1 ; // used for DB 
		iFlag			= 1;
		openTime		= 0l;
		closeTime		= 0l; //24*60*60l;
		m_pInTerm = _pInTerm; 
		assign.initDefault( m_pInTerm ); 

		absOpenTime		= 0l;
		absCloseTime	= 0l; //24*60*60l;

		assRelation		= Inclusive;
		m_isDaily =  TRUE ;
//		m_bWithTimeConstraint = 0;
	}

    ProcessorRoster (const ProcessorRoster& p_assign) 
	{ 
		m_ID = p_assign.m_ID ; // used for DB 
		iFlag		= p_assign.iFlag;
		openTime	= p_assign.openTime;
		closeTime	= p_assign.closeTime;
		assign		= p_assign.assign;
		m_pInTerm   = p_assign.m_pInTerm;

		absOpenTime = p_assign.absOpenTime;
		absCloseTime = p_assign.absCloseTime;

		assRelation = p_assign.assRelation;
		m_isDaily =  p_assign.m_isDaily;
//		m_bWithTimeConstraint = p_assign.m_bWithTimeConstraint;
	}
	ProcessorRoster& operator = ( const ProcessorRoster& p_entry);
    virtual ~ProcessorRoster () {};
	/////////////////////////////////////////////
	// get
	int isOpen (void) const 
	{ 
//		if (m_bWithTimeConstraint)
//		{
			return iFlag;
//		}
//		return true;
	}
	CMultiMobConstraint *getAssignment (void)  { return &assign; }
	ElapsedTime getOpenTime(void)	const { return openTime; }
	ElapsedTime getCloseTime(void)	const { return closeTime;}
	ElapsedTime getAbsOpenTime(void) const{ return absOpenTime;	}
	ElapsedTime getAbsCloseTime(void) const{ return absCloseTime;}
	ProcAssRelation getAssRelation( void ) const { return assRelation;}
//	int getWithTimeConstraint(void) const{return m_bWithTimeConstraint;}

	////////////////////////////////////////////////
	// set
	void setFlag (int p_Flag) { iFlag = p_Flag; }
	int getFlag() { return iFlag ;} ;
	void setAssignment( const CMultiMobConstraint& p_const) { assign = p_const; }
	void setOpenTime(ElapsedTime _time) { openTime = _time;}
	void setCloseTime(ElapsedTime _time) { closeTime = _time;}
	void setAbsOpenTime(ElapsedTime _time) { absOpenTime = _time; }
	void setAbsCloseTime( ElapsedTime _time ) {absCloseTime = _time; }
	void setAssRelation( ProcAssRelation _assRelation ) { assRelation = _assRelation; }
//	void setWithTimeConstraint(int nConstraint){ m_bWithTimeConstraint = nConstraint;}

    void addAssignment (const CMobileElemConstraint& p_const)
        { assign.addConstraint (p_const); }

	//read/write file
    void readAssignment (ArctermFile& p_file, InputTerminal* _pInTerm );
    void writeAssignment (ArctermFile& p_file) const;
	void readObsoleteAssignment(ArctermFile& p_file, InputTerminal* _pInTerm );
	//operator	
   	int operator == (const ProcessorRoster& p_entry) const;
    int operator < (const ProcessorRoster& p_entry) const;
    //    { return absOpenTime < p_entry.absOpenTime; }
	//compute absolute time from flight schedule and open/close time
	int computeAbsoluteTime( InputTerminal* _pTerminal, FlightSchedule* _pFlightSchedule );
    // returns non-zero if the constraint contains p_index at p_level
    int contains (int p_level, int p_index) const
        { return assign.contains (p_level, p_index); }
	bool isDefault();
	bool isEqual(const ProcessorRoster& _entry )const;
};  


//////////////////////////////////////////////////////////////////////////////////
class ProcessorRosterSchedule : public SortedContainer<ProcessorRoster>
{
private:
		int dailycurrent ;
protected:

    int current;
	SortedContainer<ProcessorRoster>  dailyData ;
	//for DB ,
	SortedContainer<ProcessorRoster> m_delProcAss ;
public:
	//this method just remove the _proAss add put it into m_delProcAss , not delete _proAss
	//it should be use this method to remove _proAss after save the data to DB .
	void RemoveProcAssignment(ProcessorRoster* _proAss) ;
     void RemoveAllProcAssignment() ;
	SortedContainer<ProcessorRoster>& GetDelProAssignment() { return m_delProcAss ;} ;
public:
	////
    void AddDailyData( const ProcessorRoster& assigment ,int ndays) ;
	void divideDailyEvents( InputTerminal* _pTerminal);
	void ClearAllDailyData() ;
	void ResetDailyData(int ndays) ;
	void ToNext(){ dailycurrent++ ;} ;
	//////////////////////////////////////////////////////////////////////////

    ProcessorRosterSchedule () : SortedContainer<ProcessorRoster> (8, 0, 8)
        { current = -1; ownsElements(1); dailycurrent =-1;}
    virtual ~ProcessorRosterSchedule () {};

    // returns the current assignment
    ProcessorRoster *getProcRoster() const;

    // returns the next assignment
    ProcessorRoster *getNextProcRoster() const;

    // increments the current assignment to the next one scheduled
    void updateAssignment (void) { current++; }

	void reset(void) { current = -1;}	

    int isOpen() const;

    // returns the assignment scheduled to be in effect at the passed time
    ProcessorRoster *getAssignAtTime (ElapsedTime p_time, InputTerminal* _pInTerm ) const;

    // returns the starting time of the current assignment
    ElapsedTime getCurrentTime() const;

    // returns the multiple constraint of the current assignment
    const CMultiMobConstraint *getCurrentAssignment (void)
        { return (current >= 0)? getProcRoster()->getAssignment(): NULL; }

	// divide an event into an 'opne' event and a 'close' event
	void divideEvents( InputTerminal* _pTerminal );

	// Generate Events for _nDays - 1 days
	void GenerateEntryForMultipleDays( int _nDays,  InputTerminal* _pTerminal  );
	
	// the processor is close or not
	bool processorIsClose();
	
	void ClearAllAssignment();

	bool ifExist( const ProcessorRoster& _assign )const;

//////////////////////////////////////////////////////////////////////////
public:
	ProcessorRoster* GetNearestOpenAssignment(ElapsedTime _CureentTime , const CMobileElemConstraint& _MobileEletype) ;
	BOOL CheckIsOpenAtThisTime(ElapsedTime _currentTime ,ProcessorRoster** _CurrentAssign) ;
	bool IsTimeRangeAvailable(const ElapsedTime& tStart, const ElapsedTime& tEnd, bool bExclusive);
};

class UnmergedAssignments : public UnsortedContainer<ProcessorRoster>
{
protected:
	InputTerminal* m_pInTerm;
public:
	void SetInputTerminal( InputTerminal* _pInTerm ){ m_pInTerm = _pInTerm; }
    UnmergedAssignments () : UnsortedContainer<ProcessorRoster> (8, 0, 8)
        { ownsElements(1); }
    virtual ~UnmergedAssignments () {};

    void split (ProcessorRosterSchedule *p_schedule);
    void merge (ProcessorRosterSchedule *p_schedule);

    void readData (const char *p_filename);
    void writeData (const char *p_filename) const;
};

#endif
