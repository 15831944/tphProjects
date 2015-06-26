// Scanner.cpp: implementation of the Scanner class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "engine\person.h"
#include "inputs\miscproc.h"
#include "Scanner.h"
using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Scanner::Scanner()
{

}

Scanner::~Scanner()
{

}

void Scanner::InitData( const MiscConveyorData * _pMiscData )
{
// init misc data
	if( _pMiscData )
	{
		m_dSpeed = _pMiscData->GetSpeed();
	}
	else
	{
		m_dSpeed = 300;
	}
	

	m_iCapacity = 1;// capacity should always be equal with 1
	

	// init logic data
	clearData();

	m_state	 = STOP;
	m_approaching.clear();
	m_iOccapuiedCount = 0;

	slotsPath.init( m_location, m_iCapacity );
	m_StepTime = max( ( long) ( slotsPath.getSegLen() / m_dSpeed ), 1l );
	
	//m_iCapacity = slotsPath.getCount();
	m_pSlots = new ElemTimePair[ m_iCapacity ];	
	// init slot
	for( int i=0; i< m_iCapacity; i++  )
	{
		m_pSlots[i].first	= NULL;
		m_pSlots[i].second	= -1l; 
	}
}

// It is called by Person::processServerArrival
// The different processor types serve clients in different ways.
void Scanner::beginService (Person *aPerson, ElapsedTime curTime)
{
	if( m_pSlots[0].first != NULL )
	{
		writeLogEvent (aPerson, curTime, BeginService);
	
		aPerson->flushLog (curTime);

		throw new ARCConveyorSimultaneousServiceError( aPerson->getPersonErrorMsg(), 
			getID()->GetIDString(),_T(""), curTime.printTime() );

	}
	// first write processor's log
	writeLogEvent (aPerson, curTime, BeginService);
	// the write person's log
	std::vector< Point > ptList = slotsPath.getNoFlagPointList( -1, 0 );
	writePersonLog( aPerson, ptList, curTime );

	// determine service time, based on the Person's type
	ElapsedTime _serviceTime = (long) max( getServiceTime( aPerson, curTime ), 0l );
	aPerson->setState( MovingInConveyor );
	aPerson->writeLogEntry( curTime + _serviceTime, false );

	// start up the conveyor if need
	startUpIfNeed( curTime + _serviceTime );	

	// remove from approaching list
	int index = m_approaching.findItem (aPerson);
	assert( index != INT_MAX );
    m_approaching.removeItem (index);
	
	// add the element to the processor
	m_iOccapuiedCount++;
	m_pSlots[0].first = aPerson;
	m_pSlots[0].second = curTime + _serviceTime;
}

