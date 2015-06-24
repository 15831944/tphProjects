// Loader.h: interface for the CLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADER_H__E50316BC_8B2E_4E0C_A1A3_B9229246ABB0__INCLUDED_)
#define AFX_LOADER_H__E50316BC_8B2E_4E0C_A1A3_B9229246ABB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SimpleConveyor.h"
class Person;

class CLoader : public CSimpleConveyor  
{
public:
	CLoader();
	virtual ~CLoader();
	
private:
	std::vector< HoldingArea* > link_holding;

public:
	virtual void InitData( const MiscConveyorData * _pMiscData );
	// check if the conveyor is vacant
	virtual bool isVacant( Person* pPerson  = NULL) const;
	
	// It is called by Person::processServerArrival
    // The different processor types serve clients in different ways.
	virtual void beginService (Person *aPerson, ElapsedTime curTime);

	// add person into approaching list
	virtual void addPersonIntoApproachList( Person *_pPerson );

	// let the conveyor step it
	virtual void stepIt( const ElapsedTime& time );

	virtual int GetEmptySlotCount() const;

	virtual HoldingArea* GetRandomHoldingArea(void);
	void LogDebugInfo();
private:
	// get empty wait location
	int getNearestEmptyWaitPos( const Person *thePerson ) const;

	// update approaching person
	virtual void updateApproachingPerson( const ElapsedTime& time );

	virtual void getDestinationLocation( Person* aPerson );
	void setPersonWaitLocationNearByLoader( Person* _person );

	// put a person from wait location to it's destination slot
	void putBagToSlot( Person *aPerson, ElapsedTime curTime); 

	// release linkage holding area
	bool releaseLinkHolding( const ElapsedTime& time );

	virtual bool isExceedConveyorCapacity( void ) const;
	virtual void RemoveHeadPerson(const ElapsedTime& time);
protected:
	virtual long getLoad()const;
	
};

#endif // !defined(AFX_LOADER_H__E50316BC_8B2E_4E0C_A1A3_B9229246ABB0__INCLUDED_)
