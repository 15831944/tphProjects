// ResourceElement.cpp: implementation of the ResourceElement class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ResourceElement.h"
#include "terminal.h"
#include "results\ResourceElementLog.h"
#include "process.h"
#include "common\states.h"
#include "engine\movevent.h"
#include "../Common/ARCTracker.h"
#include "../Main/TermPlanDoc.h"
#include "../Main/Floor2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ResourceElement::ResourceElement( Terminal* _pTerm, CResourcePool* _pool, int _poolIdx, long _id, double _speed, int _index_in_pool )
			   : MobileElement( _id, 0.0 ), m_pTerm( _pTerm), m_pOwnPool( _pool )
{
	m_strName.Format("%s-%d", _pool->getPoolName(), _index_in_pool );

	m_logEntry.setID( _id );
	m_logEntry.setPoolIdx( _poolIdx );
	m_logEntry.setName( m_strName );
	m_logEntry.setSpeed( (float)_speed );
	m_logEntry.setIndex( _pTerm->resourceLog->getCount() );
	m_logEntry.SetEventLog( _pTerm->m_pResourceEventLog );

	_pTerm->resourceLog->addItem(  m_logEntry );
	
	state = Resource_Stay_In_Base;
	speed = _speed * SCALE_FACTOR;
	
	m_ptDestination = location = _pool->getServiceLocation();

	m_CurrentRequestItem.init();
}

ResourceElement::~ResourceElement()
{

}

// get current service pax
const ResourceRequestItem& ResourceElement::getCurrentRequestItem( void )
{
	return m_CurrentRequestItem;
}

// set current service pax
void ResourceElement::setCurrentRequestItem ( const ResourceRequestItem& _item )
{
	m_CurrentRequestItem = _item;
}

//Event handling routine for all movements.
int ResourceElement::move (ElapsedTime currentTime,bool bNoLog)
{
	PLACE_METHOD_TRACK_STRING();
	switch( state )
	{
	case Birth:
		writeLogEntry( currentTime, false );
		handleBirth( currentTime );
		break;

	case Death:
		writeLogEntry( currentTime, false );
		handleDeath( currentTime );
		break;

	case Resource_Stay_In_Base:
		writeLogEntry( currentTime, false );
		handleStayInBase( currentTime );
		break;

	case Resource_Arrival_Processor:
		writeLogEntry( currentTime, false );
		handleArrivalProcessor( currentTime );
		break;

	case Resource_Leave_Processor:
		writeLogEntry( currentTime, false );
		handleLeaveProcessor( currentTime );
		break;
			
	case Resource_Back_To_Base:
		writeLogEntry( currentTime, false );
		handleBackToBase( currentTime );
		break;

	default:
		//assert(0);
		break;
	}

	return TRUE;
}


// process birth
void ResourceElement::handleBirth( const ElapsedTime& _time )
{
	m_logEntry.setStartTime( _time );
	setState( Resource_Stay_In_Base );
	return;
}

// process death
void ResourceElement::handleDeath( const ElapsedTime& _time )
{
	kill( _time );
	return;
}

// processor stay in bas
void ResourceElement::handleStayInBase( const ElapsedTime& _time )
{
	// do nothing
	return;
}

// processor arrival processor
void ResourceElement::handleArrivalProcessor( const ElapsedTime& _time )
{
	// first a random service time
	ElapsedTime serviceTime( m_CurrentRequestItem.request_servicetime );
	
	// then notice processor release pax
	assert( m_CurrentRequestItem.request_proc && m_CurrentRequestItem.request_mob );
	if (m_pOwnPool->getPoolType() == ConcurrentType)
	{
		m_CurrentRequestItem.request_proc->noticeReleasePax( m_CurrentRequestItem.request_mob, _time + serviceTime );
	}
	
	// write log entry
	writeLogEntry( _time + serviceTime, false );

	// then let the resource back to base
// 	m_CurrentRequestItem.init();
// 	setDestination( m_pOwnPool->getServiceLocation() );
	setState( Resource_Leave_Processor );
	generateEvent( _time + serviceTime,false );
}

ElapsedTime ResourceElement::moveTime(void)const
{
	ElapsedTime t;
	//	if (location.getZ() != destination.getZ() || !location)
	if (!location)
		t = 0l;
	else
	{

		double dLxy = m_ptDestination.distance(location);
		double dLz = 0.0;
		int nFloorFrom = (int)(location.getZ() / SCALE_FACTOR);
		int nFloorTo = (int)(m_ptDestination.getZ() / SCALE_FACTOR);
		double dL = dLxy;
		if( nFloorFrom != nFloorTo )
		{
			CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
			assert( pDoc );
			dLz = pDoc->GetFloorByMode(EnvMode_Terminal).m_vFloors[nFloorFrom]->RealAltitude() - pDoc->GetFloorByMode(EnvMode_Terminal).m_vFloors[nFloorTo]->RealAltitude();
			if( dLz < 0 )
				dLz = -dLz;
			Point pt(dLxy, dLz, 0.0);
			dL = pt.length();
		}

		//		double time = destination.distance(location);
		double time = dL;
		t = (float) (time / (double)getSpeed());
	}
	return t;
}

// processor leave processor
void ResourceElement::handleLeaveProcessor(  const ElapsedTime& _time )
{
	setDestination( m_pOwnPool->getServiceLocation() );
	setState( Resource_Back_To_Base );
	generateEvent( _time + moveTime(),false );
	if (m_pOwnPool->getPoolType() == PostServiceType)
	{
		m_CurrentRequestItem.request_proc->makeAvailable(NULL,_time,false);
	}
	m_CurrentRequestItem.init();
}

// processor back to bas
void ResourceElement::handleBackToBase( const ElapsedTime& _time )
{
	// now the the resource back to base
	setState( Resource_Stay_In_Base );
	m_pOwnPool->resourceBackToBase( this, _time );
}

//Writes the element's current location, state to the log file.
// in:
// _bBackup: if the log is backwards.
void ResourceElement::writeLogEntry (ElapsedTime time, bool _bBackup, bool bOffset/* = true*/ )
{
	ResEventStruct track;
	track.time = (long) time;
	track.state = LOBYTE( LOWORD(state) );
	
	if( location != m_ptDestination )
	{
		Point vector( location, m_ptDestination );
		Point latPoint( vector.perpendicular() );
		latPoint.length( 0.8 * SCALE_FACTOR );
		if(bOffset)
			location = m_ptDestination + latPoint;
		else
			location = m_ptDestination;

		m_ptDestination = location;
	}
	location.getPoint( track.x, track.y, track.z );
	m_logEntry.addEvent( track );
}

//Performs all required functions before element's destruction
void ResourceElement::kill (ElapsedTime killTime)
{
	if (getState() == Death)
	{
		return;
	}
	m_logEntry.setEndTime( killTime );
	flushLog( killTime );
	return;
}

// Dumps all tracks to the log and updates ElementDescription record.
void ResourceElement::flushLog (ElapsedTime currentTime)
{
	setState( Death );
	writeLogEntry( currentTime, false );

	long trackCount = m_logEntry.getCurrentCount();
	
    ResEventStruct *log = NULL;
    m_logEntry.getLog (log);
	
    m_logEntry.setEventList (log, trackCount);
	
	delete []log;
	log = NULL;
	
	assert( m_pTerm );
	m_pTerm->resourceLog->updateItem( m_logEntry,m_logEntry.getIndex() );
    m_logEntry.clearLog();
}

//Generates the next movement event for the receiver based on its current state and adds it to the event list.
void ResourceElement::generateEvent (ElapsedTime eventTime,bool bNoLog)
{
	static short testID = -1;
    MobileElementMovementEvent *aMove = new ResourceElementMovementEvent;
    ElapsedTime destTime = aMove->removeMoveEvent (this);
	
//    if (id == testID)
//        cout << '\r';
	
    aMove->init (this, eventTime,bNoLog);
    aMove->addEvent();
	m_prevEventTime = aMove->getTime();
}