// ElevatorLogEntry.cpp: implementation of the CElevatorLogEntry class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ElevatorLogEntry.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CElevatorLogEntry::CElevatorLogEntry()
:TerminalLogItem<ElevatorDescStruct,ElevatorEventStruct>()
{

}

CElevatorLogEntry::~CElevatorLogEntry()
{

}

void CElevatorLogEntry::SetID( CString _strID )
{
	strcpy( item.id, _strID );
}
	
void CElevatorLogEntry::SetStartTime( const ElapsedTime& _time )
{
	item.startTime = (long)_time;
}

void CElevatorLogEntry::SetEndTime( const ElapsedTime& _time )
{
	item.endTime = (long)_time;
}

ElapsedTime CElevatorLogEntry::GetStartTime() const
{
    ElapsedTime t;
    t.setPrecisely( item.startTime );
    return t;	
}

ElapsedTime CElevatorLogEntry::GetEndTime() const
{
    ElapsedTime t;
    t.setPrecisely( item.endTime );
    return t;	
}
void CElevatorLogEntry::SetIndex( long _lIdx )
{
	item.indexNum = _lIdx;
}
long CElevatorLogEntry::GetIndex()const
{
	return item.indexNum ;
}
void CElevatorLogEntry::echo (ofsstream& p_stream, const CString& _csProjPath) const
{
	assert( m_pOutTerm );
    p_stream	<< item.id << ',' 
				<< GetStartTime() << ',' 
				<< GetEndTime()   << '\n';
}

void CElevatorLogEntry::SetLiftLength( double _dLength )
{
	item.liftLength = _dLength;
}
double CElevatorLogEntry::GetLiftLength()const
{
	return item.liftLength;
}

void CElevatorLogEntry::SetLiftWidth( double _dWidth )
{
	item.liftWidth = _dWidth;
}
double CElevatorLogEntry::GetLiftWidth()const
{
	return item.liftWidth;
}
void CElevatorLogEntry::SetOrientation( double _dOrientation )
{
	item.orientation = _dOrientation;
}
double CElevatorLogEntry::GetOrientation()const
{
	return item.orientation;
}