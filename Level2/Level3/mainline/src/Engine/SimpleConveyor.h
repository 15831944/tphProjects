// SimpleConveyor.h: interface for the CSimpleConveyor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLECONVEYOR_H__1DFE918E_2913_4C24_A639_AAAD2B5C30D6__INCLUDED_)
#define AFX_SIMPLECONVEYOR_H__1DFE918E_2913_4C24_A639_AAAD2B5C30D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "conveyor.h"

class Person;
class MiscData;
class InputTerminal;

typedef std::pair< Person*, ElapsedTime > ElemTimePair;


class CSimpleConveyor : public Conveyor  
{
public:
	CSimpleConveyor();
	virtual ~CSimpleConveyor();

	enum CONVEYORSTATE 
	{ 
		RUN, 
		STOP
	};

protected:
	// conveyor property
	double	m_dSpeed;		// conveyor's speed
	int		m_iCapacity;	// conveyor's capacity
	CONVEYORSTATE m_state;	// conveyor's state
	//to remember the prvious state of conveyor, if the state changes, it will write the log for the baggage on the conveyor
	CONVEYORSTATE m_preState;	// conveyor's state	
	// logic data 
	ElapsedTime	m_StepTime;	// step time
	int m_iOccapuiedCount;	// the count of all element on the conveyor
	ElemTimePair* m_pSlots;	// conveyor's slots	
	
	CSegmentPath slotsPath;

public:
    //virtual void initSpecificMisc (const MiscData *miscData);

	//	init processor data before engine run
	virtual void InitData( const MiscConveyorData * _pMiscData );

	// check if the conveyor is vacant
	virtual bool isVacant( Person* pPerson  = NULL) const;

	// It is called by Person::processServerArrival
    // The different processor types serve clients in different ways.
	virtual void beginService (Person *aPerson, ElapsedTime curTime);

	// processor conveyorStepItEvent
	virtual void processorStepItEvent( InputTerminal* _pInTerm, const ElapsedTime& time );

	// add person into approaching list
	virtual void addPersonIntoApproachList( Person *_pPerson );

	// if need to wake up the processor
	virtual bool _ifNeedToNotice( void ) const;

	// get the release person' time
	virtual ElapsedTime _getReleasePersonTime( void ) const;
	
	// wake up the processor 
	virtual bool destProcAvailable( const ElapsedTime& time );

	virtual int GetEmptySlotCount( void ) const{return 0;}

	ElemTimePair* GetSlots() const {	return m_pSlots; }
	int GetCapacity() const {	return m_iCapacity; }

	void flushPersonLog(const ElapsedTime& _time);
	void LogDebugInfo();
	
protected:

	// create StepIt event if need
	bool createStepItEventIfNeed( const ElapsedTime& time );
	void writeAdditionLog( CONVEYORSTATE _eState, const ElapsedTime& _time, long _lReason ) ;
		
	// update approaching person
	virtual void updateApproachingPerson( const ElapsedTime& time );

protected:
	// let the conveyor step it
	virtual void stepIt( const ElapsedTime& time );
	
	// start up the conveyor if need
	bool startUpIfNeed( const ElapsedTime& time );

	// write the person's log by processor
	void writePersonLog( Person* _person, std::vector< Point>& ptList, ElapsedTime& time );
	void writePersonLog( Person* _person, const Path& _path, ElapsedTime& time );

	// clear processor data when destructor
	void clearData( void );

	// release the head person
	virtual bool releaseHeadPerson( Person* person, ElapsedTime time );

	virtual void RemoveHeadPerson(const ElapsedTime& time);
	
	// set conveyor state
	void setConveyorState( CONVEYORSTATE _eState, const ElapsedTime& _time , long _lLoad, long _lReason );

	virtual long getLoad()const;

	virtual void removePersonFromOccupiedList(const Person *_pPerson );
	virtual void removePersonFromApproachList(const Person* _pPerson);
};

#endif // !defined(AFX_SIMPLECONVEYOR_H__1DFE918E_2913_4C24_A639_AAAD2B5C30D6__INCLUDED_)
